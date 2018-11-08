#ifndef JARNGREPIR_GENINP_HPP
#define JARNGREPIR_GENINP_HPP
#include <extlib/toml/toml.hpp>
#include <mjolnir/util/get_toml_value.hpp>
#include <mjolnir/core/RandomNumberGenerator.hpp>
#include <jarngreipr/io/split_chain_ids.hpp>
#include <jarngreipr/io/PDBReader.hpp>
#include <jarngreipr/io/PDBWriter.hpp>
#include <jarngreipr/io/write_as_xyz.hpp>
#include <jarngreipr/model/CGChain.hpp>
#include <jarngreipr/model/make_coarse_grained.hpp>
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <jarngreipr/forcefield/ClementiGo.hpp>
#include <jarngreipr/geninp/apply_coarse_grained_models.hpp>
#include <jarngreipr/geninp/read_coarse_grained_models.hpp>
#include <jarngreipr/geninp/read_initial_structures.hpp>
#include <jarngreipr/geninp/read_reference_structures.hpp>

namespace mjolnir
{

template<typename coordT>
int geninp(int argc, char **argv)
{
    using mjolnir::operator"" _str;
    typedef PDBChain<coordT> pdb_chain_type;
    typedef CGChain<coordT>  cg_chain_type;

    const auto input_data = toml::parse(std::string(argv[1]));

    /* prepairing parameters */
    const auto general   = toml::get<toml::Table>(
            toml_value_at(input_data, "general", "<root>"));
    const auto file_name = toml::get<std::string>(
            toml_value_at(general, "file_prefix", "<root>"));
    std::random_device devrand;
    std::mt19937 mt(devrand());

    /* output general information */{
        std::cout << "[general]\n";
        std::cout << "file_name   = \""
                  << toml::get<std::string>(general.at("file_prefix")) << '"' << '\n';
        std::cout << "output_path = \""
                  << toml::get_or(general, "output_path", "./"_str) << '"' << '\n';
        std::cout << "precision = \""
                  << toml::get_or(general, "precision", "double"_str) << '"' << '\n';
        std::cout << "boundary  = \""
                  << toml::get_or(general, "boundary", "Unlimited"_str) << '"' << '\n';
        std::cout << "thread    = " << std::boolalpha
                  << toml::get_or(general, "thread", false) << '\n';
        std::cout << "GPU       = " << std::boolalpha
                  << toml::get_or(general, "GPU",    false) << '\n';
    }

    const auto parameters =
        toml::parse(toml::get<std::string>(general.at("parameters")));
    const auto& phys =
        toml::get<toml::Table>(parameters.at("physical_constants"));

    /* output parameters */ {
        std::cout << "[parameters]\n";
        std::cout << "kB = "     << std::setprecision(10) << toml::get<double>(phys.at("kB")) << '\n';
        std::cout << "NA = "     << std::setprecision(10) << toml::get<double>(phys.at("NA")) << '\n';
        std::cout << "e  = "     << std::setprecision(10) << toml::get<double>(phys.at("e"))  << '\n';
        std::cout << "\"ε0\" = " << std::setprecision(10) << toml::get<double>(phys.at("ε0")) << '\n';
    }
    std::cout << std::setprecision(6);

    const auto& mass =
        toml::get<toml::Table>(parameters.at("mass"));
    const auto& stokes_radius =
        toml::get<toml::Table>(parameters.at("stokes_radius"));

    const auto system_config =
        toml::get<std::vector<toml::Table>>(input_data.at("systems"));

    /* generating coarse-grained structures */
    const auto structure_config =
        toml::get<std::vector<toml::Table>>(input_data.at("structures"));

    // vector<map<char, PDBChain<coord>>>
    // in most cases, the size of vector is one.
    const auto ref_pdbs = read_reference_structures<coordT>(structure_config);
    const auto ini_pdbs = read_initial_structures<coordT>(structure_config);
    const auto models   = read_coarse_grained_models(structure_config);

    // apply coarse-grained model to input pdbs
    const auto ref_cgss = mjolnir::apply_coarse_grained_models(ref_pdbs, models);
    const auto ini_cgss = mjolnir::apply_coarse_grained_models(ini_pdbs, models);

    // output system setting and cg structure
    assert(system_config.size() == ini_cgss.size());
    for(std::size_t i=0; i<system_config.size(); ++i)
    {
        std::cout << "[[systems]]\n";
        // TODO
        const auto& sysconf = system_config.at(i);
        const auto T = toml::get<double>(sysconf.at("temperature"));
        std::cout << "temperature    = " << std::fixed << T << '\n';
        std::cout << "ionic_strength = " << std::fixed
                  << toml::get<double>(sysconf.at("ionic_strength")) << '\n';
        std::cout << "boundary = {";
        for(const auto& kv : toml::get<toml::Table>(sysconf.at("boundary")))
        {
            if(kv.first == "type")
            {
                std::cout << kv.first << "=\"" << toml::get<std::string>(kv.second) << '\"';
            }
            else
            {
                const auto& crd = toml::get<std::array<double, 3>>(kv.second);
                std::cout << kv.first << "=[" << crd[0] << ',' << crd[1] << ','
                          << crd[2] << ']';
            }
        }
        std::cout << "}\n";

        std::cout << "particles = [\n";
        const auto& chain_table = ini_cgss.at(i);
        for(const auto& id_chain_pair : chain_table)
        {
            std::cerr << "chain ID = " << id_chain_pair.first;
            std::cerr << " has " << id_chain_pair.second.size() << " beads." << std::endl;
            for(std::size_t j=0; j<id_chain_pair.second.size(); ++j)
            {
                const auto& bead = id_chain_pair.second.at(j);
                const auto kB = toml::get<double>(phys.at("kB"));
                const auto m  = toml::get<double>(mass.at(bead->name()));
                const auto p  = bead->position();
                std::normal_distribution<double> mxw_blz(0.0, std::sqrt(kB*T/m));
                std::cout << "{mass = " << m
                          <<  ", position = [" << p[0] << ',' << p[1] << ',' << p[2]
                          << "], velocity = [" << mxw_blz(mt) << ','
                          << mxw_blz(mt) << ',' << mxw_blz(mt) << "]},\n";
            }
        }
        std::cout << "]\n";
    }

    const std::unique_ptr<mjolnir::IntraChainForceFieldGenerator<coordT>>
        ffgen = mjolnir::make_unique<mjolnir::ClementiGo<coordT>>();

    for(const auto& chains : ref_cgss)
    {
        std::cout << "[[forcefields]]\n";
        for(const auto& idch : chains)
        {
            const auto& ch = idch.second;
            const auto connect = ffgen->generate(std::cout, ch);
        }
    }

    const auto sim = toml::get<toml::Table>(input_data.at("simulator"));
    const auto sim_type   = toml::get<std::string>(
            toml_value_at(sim, "type", "[simulator]"));
    const auto sim_scheme = toml::get<std::string>(
            toml_value_at(sim, "scheme", "[simulator]"));
    const auto sim_dt     = toml::get<double>(
            toml_value_at(sim, "delta_t", "[simulator]"));
    const auto sim_t_tot  = toml::get<std::size_t>(
            toml_value_at(sim, "total_step", "[simulator]"));
    const auto sim_t_sav  = toml::get<std::size_t>(
            toml_value_at(sim, "save_step", "[simulator]"));

    std::cout << "[simulator]\n";
    std::cout << "type       = \"" << sim_type   << "\"\n";
    std::cout << "scheme     = \"" << sim_scheme << "\"\n";
    std::cout << "delta_t    = " << sim_dt     << '\n';
    std::cout << "total_step = " << sim_t_tot  << '\n';
    std::cout << "save_step  = " << sim_t_sav  << '\n';
    std::cout << "seed       = " << mt()       << '\n';

    //XXX ...
    if(sim_scheme == "Underdamped Langevin")
    {
        std::cout << "parameters = [\n";
        const auto& chain_table = ini_cgss.front();
        for(const auto& id_chain_pair : chain_table)
        {
            for(std::size_t i=0; i<id_chain_pair.second.size(); ++i)
            {
                const auto& bead = id_chain_pair.second.at(i);
                const auto m  = toml::get<double>(mass.at(bead->name()));
                const auto a  = toml::get<double>(stokes_radius.at(bead->name()));
                const auto mu = toml::get<double>(phys.at("μ"));
                const auto gamma = 6.0 * 3.14159265358979 * mu * a / m;
                std::cout << "{index = " << i << ", gamma = " << gamma << "},\n";
            }
        }
        std::cout << "]\n";
    }
    std::cout << std::flush;

    return 0;
}

}// mjolnir
#endif// JARNGREPIR_GENINP_HPP
