// #include <jarngreipr/forcefield/ClementiGo.hpp>
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
    const auto input  = toml::parse(fname);

    // output files and units tables
    {
        std::cout << "[files.output]\n";
        std::cout << toml::find(input, "files", "output") << std::endl;
        std::cout << "[units]\n";
        std::cout << toml::find(input, "units")           << std::endl;
    }

    // TODO be aware of paths
    const auto params = toml::parse("parameter/parameters.toml");

    // get `path.pdb`. if not exists, return "./"
    const auto pdb_path = [&]() -> std::string {
        try
        {
            auto path = toml::find<std::string>(input, "files", "path", "pdb");
            if(path.back() != '/') {path += '/';}
            return path;
        }
        catch(...)
        {
            return std::string("./");
        }
    }();

    // -----------------------------------------------------------------------
    // construct groups of Coarse-Grained chains

    // TODO: consider multiple systems ...
    const auto system = toml::find(input, "systems").as_array().front();

    std::size_t offset = 0;
    std::vector<CGGroup<double>> groups;
    for(const auto& kv : system.as_table())
    {
        // special keys. skip them.
        if(kv.first == "boundary_shape" || kv.first == "attributes") {continue;}

        // other stuffs are the definitions of groups.
        CGGroup<double> group(kv.first);
        const auto& group_def = kv.second;

        // read a reference file
        PDBReader<double> reader(pdb_path + toml::find<std::string>(group_def, "reference"));

        // TODO assuming CarbonAlpha...
        CarbonAlphaGenerator<double> model_generator;

        for(auto&& chain_id : toml::find<std::vector<std::string>>(group_def, "chain"))
        {
            if(chain_id.size() != 1)
            {
                std::cerr << "chain should be defined by a character" << std::endl;
                return 1;
            }
            const auto chain = reader.read_chain(chain_id.front());
            group.push_back(model_generator.generate(chain, offset));
            offset += group.back().size();
        }
        groups.push_back(std::move(group));
    }

    // ========================================================================

    std::random_device rng;
    std::cout << "[simulator]\n";
    std::cout << "type                  = \"MolecularDynamics\"\n";
    std::cout << "boundary_type         = \"Unlimited\"\n";
    std::cout << "precision             = \"double\"\n";
    std::cout << "delta_t               = 0.1\n";
    std::cout << "total_step            = 1000_000\n";
    std::cout << "save_step             =    1_000\n";
    std::cout << "integrator.type       = \"BAOABLangevin\"\n";
    std::cout << "integrator.seed       = " << rng() << '\n';
    std::cout << "integrator.parameters = [\n";
    {
        const auto& mass = toml::find(params, "mass");
        std::size_t num_total_bead = 0;
        for(const auto& group : groups)
        {
            for(const auto& chain : group)
            {
                num_total_bead += chain.size();
            }
        }

        const auto width = std::to_string(num_total_bead).size();
        for(const auto& group : groups)
        {
            for(const auto& chain : group)
            {
                for(const auto& bead : chain)
                {
                    const auto m = toml::find<double>(mass, bead->name());
                    // TODO assuming default CA parameter...
                    std::cout << "{index = " << std::setw(width) << bead->index()
                              << ", gamma = " << 168.7 * 0.005 / m << "},\n";
                }
            }
        }
        std::cout << "]\n";
    }

    // ========================================================================
    // generate system
    //
    // TODO consider multiple systems...
    {
        using value_type = toml::basic_value<toml::preserve_comments, std::map>;
        using table_type = typename value_type::table_type;
        using array_type = typename value_type::array_type;

        // consider units
        const double kBT = 300.0 * 1.986231313e-3;

        value_type sys = table_type{
            {"attributes",     toml::find(system, "attributes")    },
            {"boundary_shape", toml::find(system, "boundary_shape")},
            {"particles",      array_type{}}
        };
        std::mt19937 mt(123456789);
        const auto& mass = toml::find(params, "mass");

        auto& ps = sys.as_table().at("particles").as_array();
        for(const auto& group : groups)
        {
            for(const auto& chain : group)
            {
                bool is_front = true;
                for(const auto& bead : chain)
                {
                    const auto m = toml::find<double>(mass, bead->name());
                    std::normal_distribution<double>
                        maxwell_boltzmann(0.0, std::sqrt(kBT / m));

                    const auto& p = bead->position();
                    const std::array<double, 3> v{
                        {maxwell_boltzmann(mt), maxwell_boltzmann(mt), maxwell_boltzmann(mt)}
                    };
                    value_type particle = table_type{
                        {"mass", m},
                        {"position", toml::value{p[0], p[1], p[2]}},
                        {"velocity", toml::value{v[0], v[1], v[2]}},
                        {"name",     bead->name()},
                        {"group",    group.name()}
                    };
                    if(is_front)
                    {
                        is_front = false;
                        particle.comments().push_back(std::string(" chain ") +
                                chain.name() + " in group \"" + group.name() +
                                "\"");
                    }
                    ps.push_back(std::move(particle));
                }
            }
        }
        write_system(std::cout, sys);
    }

    // ========================================================================
    // generate forcefield parameters
    //
    // TODO

    const auto aicg2p_params = toml::parse("parameter/AICG2+.toml");
    const auto exv_params    = toml::parse("parameter/ExcludedVolume.toml");

    const std::vector<std::size_t> flex{/* flexible region*/};
    toml::basic_value<toml::preserve_comments, std::map> ff;

    // -----------------------------------------------------------------------
    // local

    AICG2Plus<double> aicg(aicg2p_params);
    for(const auto& group : groups)
    {
        aicg.generate(ff, group);
    }

    // -----------------------------------------------------------------------
    // global

    ExcludedVolume<double> exv(exv_params);

    for(const auto& group : groups)
    {
        exv. generate(ff, group);
        aicg.generate(ff, group, group);
    }

    write_forcefield(std::cout, ff);

    return 0;
}
