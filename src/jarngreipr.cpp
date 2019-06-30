// #include <jarngreipr/forcefield/ClementiGo.hpp>
#include <jarngreipr/forcefield/AICG2Plus.hpp>
#include <jarngreipr/forcefield/ExcludedVolume.hpp>
#include <jarngreipr/io/write_forcefield.hpp>
#include <jarngreipr/io/write_system.hpp>
#include <jarngreipr/model/CarbonAlpha.hpp>
#include <jarngreipr/pdb/PDBReader.hpp>
#include <jarngreipr/gro/GROWriter.hpp>
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
    const auto chain = reader.read_chain('A');
    CarbonAlphaGenerator<double> model_generator;
    std::vector<CGChain<double>> ca;
    ca.push_back(model_generator.generate(chain, 0));

    const auto params = toml::parse("parameter/parameters.toml");

    // ========================================================================

    std::random_device rng;
    std::cout << "[simulator]\n";
    std::cout << "type          = \"MolecularDynamics\"\n";
    std::cout << "boundary_type = \"Unlimited\"\n";
    std::cout << "precision     = \"double\"\n";
    std::cout << "delta_t       = 0.1\n";
    std::cout << "total_step    = 1000_000\n";
    std::cout << "save_step     =    1_000\n";
    std::cout << "integrator.type       = \"BAOABLangevin\"\n";
    std::cout << "integrator.seed       = " << rng() << '\n';
    std::cout << "integrator.parameters = [\n";
    for(const auto& chain : ca)
    {
        for(std::size_t i=0; i<chain.size(); ++i)
        {
            std::cout << "{index = "
                      << std::setw(std::to_string(chain.size()).length())
                      << i << ", gamma = " << 168.7 * 0.005 << "},\n";
        }
    }
    std::cout << "]\n";

    // ========================================================================
    {
        const double kBT = 300.0 * 1.986231313e-3;
        toml::table sys{
            {"attributes", toml::table{{"temperature", 300.0}}},
            {"boundary_shape", toml::table{}}
        };
        std::mt19937 mt(123456789);
        const auto& mass = toml::find(params, "mass");

        toml::array ps;
        for(const auto& chain : ca)
        {
            for(const auto& bead : chain)
            {
                const auto m = toml::find<double>(mass, bead->name());
                std::normal_distribution<double>
                    maxwell_boltzmann(0.0, std::sqrt(kBT / m));

                const auto& p = bead->position();
                const std::array<double, 3> v{
                    {maxwell_boltzmann(mt), maxwell_boltzmann(mt), maxwell_boltzmann(mt)}
                };
                toml::table tab {
                    {"mass", m},
                    {"position", toml::value{p[0], p[1], p[2]}},
                    {"velocity", toml::value{v[0], v[1], v[2]}},
                    {"name",     "CA"},
                    {"group",    "none"}
                };
                ps.push_back(std::move(tab));
            }
        }
        sys["particles"] = std::move(ps);
        write_system(std::cout, toml::value(sys));
    }
    // ========================================================================

    const auto aicg2p_params = toml::parse("parameter/AICG2+.toml");
    const auto exv_params    = toml::parse("parameter/ExcludedVolume.toml");

    const std::vector<std::size_t> flex{/* flexible region*/};
    toml::basic_value<toml::preserve_comments, std::map> ff;

    AICG2Plus<double> aicg(aicg2p_params, flex);
    aicg.generate(ff, ca);

    ExcludedVolume<double> exv(exv_params);
    exv.generate(ff, ca);

    write_forcefield(std::cout, ff);

    return 0;
}
