// #include <jarngreipr/forcefield/ClementiGo.hpp>
#include <jarngreipr/forcefield/AICG2Plus.hpp>
#include <jarngreipr/forcefield/ExcludedVolume.hpp>
#include <jarngreipr/forcefield/DebyeHuckel.hpp>
#include <jarngreipr/format/write_forcefield.hpp>
#include <jarngreipr/format/write_system.hpp>
#include <jarngreipr/model/CarbonAlpha.hpp>
#include <jarngreipr/model/ThreeSPN2.hpp>
#include <jarngreipr/pdb/PDBReader.hpp>
#include <jarngreipr/util/parse_range.hpp>
#include <algorithm>
#include <random>
#include <map>

// map of attribute name -> {map of chain ID -> pair of {indices, parameters}}
template<typename Com, template<typename ...> class Tab,
         template<typename ...> class Arr>
std::map<std::string,     // attribute name ->
    std::map<std::string, // chain ID ->
        std::vector<std::pair<std::int64_t, std::string>> // {indices, parameter}
        >
    >
read_attributes(const toml::basic_value<Com, Tab, Arr>& group)
{
    using namespace jarngreipr;

    std::map<std::string, std::map<std::string,
             std::vector<std::pair<std::int64_t, std::string>>>> attributes;
    for(const auto& kv : group.as_table())
    {
        const auto& key = kv.first;
        if(key == "reference" || key == "initial" || key == "model" || key == "chain")
        {
            continue; // these are special keys, not an additional attribute.
        }
        std::map<std::string, std::vector<std::pair<std::int64_t, std::string>>>
            regions;
        for(const auto& v : kv.second.as_array())
        {
            const auto chainID  = toml::find<std::string>(v, "chain");
            if(regions.count(chainID) == 0)
            {
                regions[chainID] = {};
            }
            const auto residues = parse_range<std::int64_t>(
                    toml::find<std::string>(v, "residues"));
            const auto attr_value = toml::find_or<std::string>(
                    v, "value", std::string(""));

            log(log_level::info, "residue from ", residues.front(), " to ",
                residues.back(), " has an attribute ", key, '\n');

            for(const auto& res : residues)
            {
                regions.at(chainID).emplace_back(res, attr_value);
            }
        }

        // overlap check
        for(auto& region : regions)
        {
            std::sort(region.second.begin(), region.second.end(),
                [](const std::pair<std::int64_t, std::string>& lhs,
                   const std::pair<std::int64_t, std::string>& rhs) -> bool {
                    return lhs.first < rhs.first;
                });
            const auto overlapped = std::adjacent_find(
                region.second.begin(), region.second.end(),
                [](const std::pair<std::int64_t, std::string>& lhs,
                   const std::pair<std::int64_t, std::string>& rhs) -> bool {
                    return lhs.first == rhs.first;
                });
            if(overlapped != region.second.end())
            {
                log(log_level::warn, "attribute regions overlap each other: ",
                        overlapped->first, " specified twice\n");
            }
        }
        attributes[key] = regions;
    }
    return attributes;
}

std::unique_ptr<jarngreipr::ForceFieldGenerator<double>>
setup_forcefield_generator(const std::string& forcefield,
                           const std::string& parameter_file)
{
    using namespace jarngreipr;
    if(forcefield == "AICG2+")
    {
        return std::unique_ptr<ForceFieldGenerator<double>>(
            new AICG2Plus<double>(toml::parse(parameter_file)));
    }
    else
    {
        log(log_level::error, "unknown forcefield specified: ", forcefield, '\n');
        log(log_level::error, "- \"AICG2+\" is for AICG2+");
        std::terminate();
    }
}

std::unique_ptr<jarngreipr::CGModelGeneratorBase<double>>
setup_model_generator(const std::string& model, const toml::value& params)
{
    using namespace jarngreipr;
    if(model == "CarbonAlpha")
    {
        return std::unique_ptr<CGModelGeneratorBase<double>>(
            new CarbonAlphaGenerator<double>(params));
    }
    else if(model == "3SPN2")
    {
        return std::unique_ptr<CGModelGeneratorBase<double>>(
            new ThreeSPN2Generator<double>(params));
    }
    else
    {
        log(log_level::error, "unknown model specified: ", model, '\n');
        log(log_level::error, "- \"CarbonAlpha\" is for AICG2+");
        log(log_level::error, "- \"3SPN2\" is for 3SPN.2 and 3SPN.2C");
        std::terminate();
    }
}

std::string read_input_filename(int argc, char **argv)
{
    using namespace jarngreipr;
    std::vector<std::string> opts;
    for(int i=1; i<argc; ++i)
    {
        opts.push_back(std::string(argv[i]));
    }

    // default settings
    logger::inactivate(log_level::debug);
    logger::activate(log_level::info);
    logger::activate(log_level::warn);
    logger::activate(log_level::error);

    std::string fname;
    for(const auto& opt : opts)
    {
        if(opt == "--debug")
        {
            logger::activate(log_level::debug);
        }
        else if(5 < opt.size() && opt.substr(opt.size()-5, 5) == ".toml")
        {
            fname = opt;
        }
        else
        {
            log(log_level::warn, "unknown option appeared. ignore\"", opt, "\"\n");
        }
    }
    return fname;
}

int main(int argc, char **argv)
{
    using namespace jarngreipr;

    if(argc < 2)
    {
        log(log_level::error, "Usage: jarngreipr input.toml\n");
        return 1;
    }

    const std::string fname = read_input_filename(argc, argv);
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
    std::map<std::string, CGGroup<double>> groups;
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

        const auto model_generator = setup_model_generator(
                toml::find<std::string>(group_def, "model"),
                toml::find(mass_params, "mass"));

        // --------------------------------------------------------------------
        // Extract chains to be coarse-grained. All of the following are valid.
        //  1. proteins.chain = "A"
        //  2. proteins.chain = "A:D"
        //  3. proteins.chain = ["A", "B"]
        //  4. proteins.chain = ["A:B", "E:F", "H"]
        std::vector<std::string> chain_ids;
        if(group_def.at("chain").is_string())
        {
            chain_ids = jarngreipr::parse_chain_range(
                    toml::find<std::string>(group_def, "chain"));
        }
        else if(group_def.at("chain").is_array())
        {
            for(const auto& chain_def : toml::find<std::vector<std::string>>(group_def, "chain"))
            {
                for(const auto& chain_id : jarngreipr::parse_chain_range(chain_def))
                {
                    chain_ids.push_back(chain_id);
                }
            }
        }

        // -------------------------------------------------------------------
        // Coarse-Graining

        const auto attributes = read_attributes(group_def);
        for(const auto& chain_id : chain_ids)
        {
            if(chain_id.size() != 1)
            {
                log(log_level::error, "chain ID should be 1 letter -> ", chain_id, '\n');
                return 1;
            }
            log(log_level::info, "reading chain ", chain_id, " of group ",
                                 kv.first, '\n');

            const auto chain = reader.read_chain(chain_id.front());
            auto cg_chain = model_generator->generate(chain, offset);

            for(const auto& attribute : attributes)
            {
                const auto& attr_name = attribute.first;
                if(attribute.second.count(chain_id) != 0)
                {
                    const auto& regions  = attribute.second.at(chain_id);
                    for(auto& cg_bead : cg_chain)
                    {
                        const auto resID = cg_bead->atoms().front().residue_id;
                        const auto found = std::find_if(
                            regions.begin(), regions.end(),
                            [=](const std::pair<std::int64_t, std::string>& x){
                                return x.first == resID;
                            });
                        if(found != regions.end())
                        {
                            log(log_level::debug, "bead residue idx = ", resID,
                                " attribute name = ", attr_name,
                                " attribute value = ", found->second, '\n');
                            cg_bead->attribute(attr_name) = found->second;
                        }
                    }
                }
            }
            group.push_back(cg_chain);
            offset += group.back().size();
        }
        groups[kv.first] = std::move(group);
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
        for(const auto& kv : groups)
        {
            const auto& group = kv.second;
            for(const auto& chain : group)
            {
                num_total_bead += chain.size();
            }
        }

        const auto width = std::to_string(num_total_bead).size();
        for(const auto& kv : groups)
        {
            const auto& group = kv.second;
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
        for(const auto& kv : groups)
        {
            const auto& group = kv.second;
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

    toml::basic_value<toml::preserve_comments, std::map> ff;
    const auto forcefield = toml::find(input, "forcefields").as_array().front();

    // -----------------------------------------------------------------------
    log(log_level::info, "generating local forcefield ...\n");

    for(const auto& local : toml::find(forcefield, "local").as_array())
    {
        const auto ff_name   = toml::find<std::string>(local, "forcefield");
        const auto para_file = toml::find_or<std::string>(
                local, "parameter_file", "parameter/" + ff_name + ".toml");

        const auto ffgen = setup_forcefield_generator(ff_name, para_file);

        for(auto gname : toml::find<std::vector<std::string>>(local, "groups"))
        {
            const auto& group = groups.at(gname);
            ffgen->generate(ff, group);
        }
    }

    // -----------------------------------------------------------------------
    log(log_level::info, "generating global forcefield ...\n");

    const auto aicg2p_params = toml::parse("parameter/AICG2+.toml");
    const auto exv_params    = toml::parse("parameter/ExcludedVolume.toml");
    const auto ele_params    = toml::parse("parameter/ElectroStatic.toml");

    AICG2Plus<double> aicg(aicg2p_params);
    ExcludedVolume<double> exv(exv_params);
    DebyeHuckel<double> ele(ele_params);

    for(const auto& global : toml::find(forcefield, "global").as_array())
    {
        for(auto gname : toml::find<std::vector<std::string>>(global, "groups"))
        {
            const auto& group = groups.at(gname);
            exv. generate(ff, group);
            ele. generate(ff, group);
            aicg.generate(ff, group, group);
        }
    }

    log(log_level::info, "writing forcefields\n");
    write_forcefield(std::cout, ff);

    return 0;
}
