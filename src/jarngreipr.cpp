// #include <jarngreipr/forcefield/ClementiGo.hpp>
#include <jarngreipr/forcefield/AICG2Plus.hpp>
#include <jarngreipr/forcefield/ExcludedVolume.hpp>
#include <jarngreipr/forcefield/ElectroStatic.hpp>
#include <jarngreipr/io/write_forcefield.hpp>
#include <jarngreipr/io/write_system.hpp>
#include <jarngreipr/model/CarbonAlpha.hpp>
#include <jarngreipr/model/ThreeSPN2.hpp>
#include <jarngreipr/pdb/PDBReader.hpp>
#include <jarngreipr/util/parse_range.hpp>
#include <algorithm>
#include <random>
#include <map>

template<typename Com, template<typename ...> class Tab,
         template<typename ...> class Arr>
std::map<std::string, std::vector<std::int64_t>>
read_flexible_regions(const toml::basic_value<Com, Tab, Arr>& group)
{
    using namespace jarngreipr;
    std::map<std::string, std::vector<std::int64_t>> regions;
    if(group.as_table().count("flexible_regions") == 0)
    {
        log(log_level::info, "no flexible region defined in this group\n");
        return regions;
    }

    const auto& flexible_regions = toml::find(group, "flexible_regions");
    for(const auto& region : flexible_regions.as_array())
    {
        const auto chainID = toml::find<std::string>(region, "chain");
        if(regions.count(chainID) == 0)
        {
            regions[chainID] = {};
        }
        const auto residues = jarngreipr::parse_range<std::int64_t>(
                toml::find<std::string>(region, "residues"));

        log(log_level::info, "residue from ", residues.front(), " to ",
            residues.back(), " are the flexible region of chain ", chainID, '\n');

        std::copy(residues.begin(), residues.end(),
                  std::back_inserter(regions.at(chainID)));
    }
    for(auto& kv : regions)
    {
        std::sort(kv.second.begin(), kv.second.end());
    }
    return regions;
}

int main(int argc, char **argv)
{
    using namespace jarngreipr;
    if(argc < 2)
    {
        log(log_level::error, "Usage: jarngreipr [file.toml]\n");
        return 1;
    }
    const std::string fname(argv[1]);
    const auto input  = toml::parse<toml::discard_comments, std::map>(fname);

    // output files and units tables
    {
        std::cout << "[files.output]\n";
        std::cout << toml::find(input, "files", "output") << std::endl;
        std::cout << "[units]\n";
        std::cout << toml::find(input, "units")           << std::endl;
    }

    // TODO be aware of paths
    const auto mass_params = toml::parse("parameter/mass.toml");

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

        log(log_level::info, "reading group ", kv.first, "\n");

        // other stuffs are the definitions of groups.
        CGGroup<double> group(kv.first);
        const auto& group_def = kv.second;

        // read a reference file
        PDBReader<double> reader(pdb_path + toml::find<std::string>(group_def, "reference"));

        // TODO assuming CarbonAlpha here...
        CarbonAlphaGenerator<double> model_generator(toml::find(mass_params, "mass"));

        const auto flexible_regions = read_flexible_regions(group_def);

        for(auto&& chain_id : toml::find<std::vector<std::string>>(group_def, "chain"))
        {
            if(chain_id.size() != 1)
            {
                log(log_level::error, "chain ID should be 1 letter -> ", chain_id, '\n');
                return 1;
            }
            log(log_level::info, "reading chain ", chain_id, " of group ",
                                 kv.first, '\n');

            const auto chain = reader.read_chain(chain_id.front());
            auto cg_chain = model_generator.generate(chain, offset);

            if(flexible_regions.count(chain_id) != 0)
            {
                const auto& flex = flexible_regions.at(chain_id);
                for(auto& cg_bead : cg_chain)
                {
                    const auto resID = cg_bead->atoms().front().residue_id;
                    if(std::binary_search(flex.begin(), flex.end(), resID))
                    {
                        cg_bead->attribute("is_flexible") = std::string("true");
                    }
                    else
                    {
                        cg_bead->attribute("is_flexible") = std::string("false");
                    }
                }
            }
            group.push_back(cg_chain);
            offset += group.back().size();
        }
        groups.push_back(std::move(group));
    }
    log(log_level::info, "systems are coarse-grained\n");

    // ========================================================================

    std::random_device rng;
    std::cout << "[simulator]\n";
    std::cout << "type                  = \"MolecularDynamics\"\n";
    std::cout << "boundary_type         = \"Unlimited\"\n";
    std::cout << "precision             = \"double\"\n";
    std::cout << "delta_t               = 0.1\n";
    std::cout << "total_step            = 1000_000\n";
    std::cout << "save_step             =    1_000\n";
    std::cout << "seed                  = " << rng() << '\n';
    std::cout << "integrator.type       = \"BAOABLangevin\"\n";
    std::cout << "integrator.parameters = [\n";
    {
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
                    std::cout << "{index = " << std::setw(width) << bead->index()
                              << ", gamma = " << 168.7 * 0.005 / bead->mass() << "},\n";
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

        value_type sys = table_type{
            {"attributes",     toml::find(system, "attributes")    },
            {"boundary_shape", toml::find(system, "boundary_shape")},
            {"particles",      array_type{}}
        };
        std::mt19937 mt(123456789);

        auto& ps = sys.as_table().at("particles").as_array();
        for(const auto& group : groups)
        {
            for(const auto& chain : group)
            {
                bool is_front = true;
                for(const auto& bead : chain)
                {
                    const auto& p = bead->position();

                    value_type particle = table_type{
                        {"mass",     bead->mass()},
                        {"position", toml::value{p[0], p[1], p[2]}},
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

        log(log_level::info, "[[systems]] written\n");
    }

    // ========================================================================
    // generate forcefield parameters
    //
    // TODO

    const auto aicg2p_params = toml::parse("parameter/AICG2+.toml");
    const auto exv_params    = toml::parse("parameter/ExcludedVolume.toml");
    const auto ele_params    = toml::parse("parameter/ElectroStatic.toml");

    toml::basic_value<toml::preserve_comments, std::map> ff;

    // -----------------------------------------------------------------------
    // local

    log(log_level::info, "generating local forcefield ...\n");

    AICG2Plus<double> aicg(aicg2p_params);
    for(const auto& group : groups)
    {
        aicg.generate(ff, group);
    }
    log(log_level::info, "local forcefield generated\n");

    // -----------------------------------------------------------------------
    // global

    log(log_level::info, "generating global forcefield ...\n");

    ExcludedVolume<double> exv(exv_params);
    ElectroStatic<double> ele(ele_params);

    for(const auto& group : groups)
    {
        exv. generate(ff, group);
        ele. generate(ff, group);
        aicg.generate(ff, group, group);
    }
    log(log_level::info, "global forcefield generated\n");

    write_forcefield(std::cout, ff);

    return 0;
}
