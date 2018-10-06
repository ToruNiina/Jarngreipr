#include <jarngreipr/forcefield/ClementiGo.hpp>
#include <jarngreipr/forcefield/AICG2Plus.hpp>
#include <jarngreipr/forcefield/ExcludedVolume.hpp>
#include <jarngreipr/io/write_forcefield.hpp>
#include <jarngreipr/io/write_system.hpp>
#include <jarngreipr/model/CarbonAlpha.hpp>
#include <jarngreipr/pdb/PDBReader.hpp>
#include <algorithm>
#include <random>
#include <map>

int main(int argc, char **argv)
{
    using namespace jarngreipr;
    if(argc < 2)
    {
        std::cerr << "Usage: jarngreipr [file.toml]" << std::endl;
        return 1;
    }
    const std::string fname(argv[1]);
//     const auto input = toml::parse(fname);
//
//     const auto general = mjolnir::toml_value_at(input, "general", "<root>"
//             ).cast<toml::value_t::Table>();
//     const std::uint32_t seed = toml::get<std::size_t>(
//             mjolnir::toml_value_at(general, "seed", "general"));

    PDBReader<double> reader(fname);
    const auto chain = reader.read_next_chain();
    CarbonAlphaGenerator<double> model_generator;
    std::vector<CGChain<double>> ca;
    ca.push_back(model_generator.generate(chain, 0));

    // ========================================================================

    const toml::Table params = toml::parse("parameter/parameters.toml");

    toml::Table sys;
    const double kB = 1.986231313e-3;
    const double T  = 300.0;
    sys["attributes"] = toml::value({{"temperature", T}});
    sys["boundary"]   = toml::Table();
    toml::Array ps;

    std::mt19937 mt(123456789);
    const auto& mass = params.at("mass").cast<toml::value_t::Table>();
    for(const auto& chain : ca)
    {
        for(const auto& bead : chain)
        {
            const double m = toml::get<double>(mass.at(bead->name()));
            const mjolnir::Vector<double, 3> p = bead->position();
            std::normal_distribution<double> distro(0., std::sqrt(kB * T / m));
            const mjolnir::Vector<double, 3> v(distro(mt), distro(mt), distro(mt));
            toml::Table tab;
            tab["mass"] = m;
            tab["position"] = toml::Array{p[0], p[1], p[2]};
            tab["velocity"] = toml::Array{v[0], v[1], v[2]};
            ps.push_back(tab);
        }
    }
    sys["particles"] = std::move(ps);
    write_system(std::cout, sys);

    // ========================================================================

    std::random_device rng;
    std::cout << "[simulator]\n";
    std::cout << "type       = \"Molecular Dynamics\"\n";
    std::cout << "scheme     = \"Underdamped Langevin\"\n";
    std::cout << "seed       = " << rng() << '\n';
    std::cout << "delta_t    = " << 0.1 << '\n';
    std::cout << "total_step = " << 100000 << '\n';
    std::cout << "save_step  = " << 100 << '\n';
    std::cout << "parameters = [\n";
    for(const auto& chain : ca)
    {
        for(std::size_t i=0; i<chain.size(); ++i)
        {
            std::cout << "{index = " << std::setw(std::to_string(chain.size()).length())
                      << i << ", gamma = " << 168.7 * 0.005 << "},\n";
        }
    }
    std::cout << "]\n";

    // ========================================================================

    const toml::Table aicg2p_params = toml::parse("parameter/AICG2+.toml");
    const toml::Table exv_params    = toml::parse("parameter/ExcludedVolume.toml");

    toml::Table ff;

    std::vector<std::size_t> flex{/* flexible region*/};

    AICG2Plus<double> aicg(aicg2p_params, flex);
    aicg.generate(ff, ca);

    ExcludedVolume<double> exv(exv_params);
    exv.generate(ff, ca);

    write_forcefield(std::cout, ff);
    return 0;
}
