#ifndef JARNGREIPR_FORCEFIELD_AICG2_PLUS_H
#define JARNGREIPR_FORCEFIELD_AICG2_PLUS_H
#include <extlib/toml/toml.hpp>
#include <mjolnir/util/color.hpp>
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <jarngreipr/geometry/distance.hpp>
#include <jarngreipr/geometry/angle.hpp>
#include <jarngreipr/geometry/dihedral.hpp>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <limits>

namespace jarngreipr
{

template<typename realT>
class AICG2Plus final : public ForceFieldGenerator<realT>
{
  public:
    using base_type  = ForceFieldGenerator<realT>;
    using real_type  = typename base_type::real_type;
    using bead_type  = typename base_type::bead_type;
    using chain_type = typename base_type::chain_type;
    using group_type = typename base_type::group_type;
    using atom_type  = typename base_type::atom_type;
    using bead_ptr   = typename chain_type::bead_ptr;

  public:

    template<typename Comment, template<typename...> class Map,
             template<typename...> class Array>
    AICG2Plus(const toml::basic_value<Comment, Map, Array>& para,
              const std::vector<std::size_t>& flex)
      : e_min_(toml::find<real_type>(para, "ecut_up_aicg2")),
        e_max_(toml::find<real_type>(para, "ecut_low_aicg2")),
        // cut-offs
        go_contact_threshold_(toml::find<real_type>(para, "go_contact_threshold")),
        atom_contact_cutoff_ (toml::find<real_type>(para, "atom_contact_cutoff")),
        hydrogen_bond_cutoff_(toml::find<real_type>(para, "hydrogen_bond_cutoff")),
        salt_bridge_cutoff_  (toml::find<real_type>(para, "salt_bridge_cutoff")),
        // base coefficients
        coef_13_   (toml::find<real_type>(para, "caicg2plus_13")),
        coef_14_   (toml::find<real_type>(para, "caicg2plus_14")),
        coef_go_   (toml::find<real_type>(para, "caicg2plus_nloc")),
        cbd_aicg2_ (toml::find<real_type>(para, "cbd_aicg2")),
        wid_aicg13_(toml::find<real_type>(para, "wid_aicg13")),
        wid_dih_   (toml::find<real_type>(para, "wid_dih")),
        // parameters for contact strength
        bb_hydrogen_bond_  (toml::find<real_type>(para, "contact_energy_coefficients", "backbone_hydrogen_bond")),
        bb_donor_acceptor_ (toml::find<real_type>(para, "contact_energy_coefficients", "backbone_donor_acceptor")),
        bb_carbon_contact_ (toml::find<real_type>(para, "contact_energy_coefficients", "backbone_carbon_contact")),
        bb_other_contact_  (toml::find<real_type>(para, "contact_energy_coefficients", "backbone_contact")),
        ss_hydrogen_bond_  (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_hydrogen_bond")),
        ss_donor_acceptor_ (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_donor_acceptor")),
        ss_salty_bridge_   (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_salty_bridge")),
        ss_carbon_contact_ (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_carbon_contact")),
        ss_charge_contact_ (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_charge_contact")),
        ss_other_contact_  (toml::find<real_type>(para, "contact_energy_coefficients", "sidechain_contact")),
        bs_hydrogen_bond_  (toml::find<real_type>(para, "contact_energy_coefficients", "heterogeneous_hydrogen_bond")),
        bs_donor_acceptor_ (toml::find<real_type>(para, "contact_energy_coefficients", "heterogeneous_donor_acceptor")),
        bs_carbon_contact_ (toml::find<real_type>(para, "contact_energy_coefficients", "heterogeneous_carbon_contact")),
        bs_charge_contact_ (toml::find<real_type>(para, "contact_energy_coefficients", "heterogeneous_charge_contact")),
        bs_other_contact_  (toml::find<real_type>(para, "contact_energy_coefficients", "heterogeneous_contact")),
        long_range_contact_(toml::find<real_type>(para, "contact_energy_coefficients", "long_range_contact")),
        offset_            (toml::find<real_type>(para, "contact_energy_coefficients", "offset")),
        // flexible local stuff
        k_angle_      (toml::find<real_type>(para, "flexible_local", "k_angle")),
        k_dihedral_   (toml::find<real_type>(para, "flexible_local", "k_dihedral")),
        angle_x_      (toml::find<decltype(angle_x_      )>(para, "flexible_local", "angle_x")),
        angle_y_1_    (toml::find<decltype(angle_y_1_    )>(para, "flexible_local", "angle_term1")),
        angle_y_2_    (toml::find<decltype(angle_y_2_    )>(para, "flexible_local", "angle_term2")),
        dihedral_term_(toml::find<decltype(dihedral_term_)>(para, "flexible_local", "dihedral_term")),
        // list of index of beads in flexible regions
        flexible_beads_(flex)
    {
        // to use binary_search, we need to sort it.
        std::sort(this->flexible_beads_.begin(), this->flexible_beads_.end());

        if(go_contact_threshold_ <= atom_contact_cutoff_)
        {
            std::cerr
                << '[' << mjolnir::io::yellow << "warning" << mjolnir::io::nocolor
                << "] go contact threshold is shorter than atom contact cutoff."
                << std::endl;
        }
    }
    ~AICG2Plus() override = default;

    // generate local parameters, not inter-chain contacts
    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& chains) const override;

    // generate inter-chain contacts.
    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& lhs, const group_type& rhs) const override;

    bool check_beads_kind(const chain_type& chain) const override;

  private:

    bool is_in_flexible_region(const std::size_t bead_idx) const
    {
        return std::binary_search(this->flexible_beads_.begin(),
                                  this->flexible_beads_.end(), bead_idx);
    }

    bool is_backbone(const atom_type& atom) const
    {
        return (atom.atom_name == " N  " || atom.atom_name == " C  " ||
                atom.atom_name == " O  " || atom.atom_name == " OXT" ||
                atom.atom_name == " CA ");
    }
    bool is_sidechain(const atom_type& atom) const
    {
        return !is_backbone(atom) &&
            atom.atom_name.at(0) != 'H' && atom.atom_name.at(1) != 'H';
    }

    bool is_donor(const atom_type& atom) const
    {
        return (atom.atom_name.at(1) == 'N' ||
                (atom.residue_name == "SER" && atom.atom_name == " OG ") ||
                (atom.residue_name == "THR" && atom.atom_name == " OG1") ||
                (atom.residue_name == "TYR" && atom.atom_name == " OH ") ||
                (atom.residue_name == "CYS" && atom.atom_name.at(1) == 'S'));
    }
    bool is_acceptor(const atom_type& atom) const
    {
        return (atom.atom_name.at(1) == 'O' || atom.atom_name.at(1) == 'S');
    }
    bool is_cation(const atom_type& atom) const
    {
	    return ((atom.residue_name == "ARG" && atom.atom_name == " NH1") ||
                (atom.residue_name == "ARG" && atom.atom_name == " NH2") ||
                (atom.residue_name == "LYS" && atom.atom_name == " NZ "));
    }
    bool is_anion(const atom_type& atom) const
    {
		return ((atom.residue_name == "GLU" && atom.atom_name == " OE1") ||
                (atom.residue_name == "GLU" && atom.atom_name == " OE2") ||
                (atom.residue_name == "ASP" && atom.atom_name == " OD1") ||
                (atom.residue_name == "ASP" && atom.atom_name == " OD2"));
    }
    bool is_carbon(const atom_type& atom) const
    {
        return (atom.atom_name.at(1) == 'C');
    }

    bool is_donor_acceptor_pair(const atom_type& lhs, const atom_type& rhs) const
    {
        return (is_acceptor(lhs) && is_donor(rhs)) ||
               (is_acceptor(rhs) && is_donor(lhs));
    }
    bool is_cation_anion_pair(const atom_type& lhs, const atom_type& rhs) const
    {
        return (is_cation(lhs) && is_anion(rhs)) ||
               (is_cation(rhs) && is_anion(lhs));
    }

    real_type calc_contact_coef(const bead_ptr& bead1, const bead_ptr& bead2) const;

    real_type min_distance_sq(const std::shared_ptr<bead_type>& bead1,
                              const std::shared_ptr<bead_type>& bead2) const
    {
        real_type min_dist = std::numeric_limits<real_type>::max();
        for(const auto& atom1 : bead1->atoms())
        {
            if(atom1.element == " H") {continue;}
            for(const auto& atom2 : bead2->atoms())
            {
                if(atom2.element == " H") {continue;}
                const real_type dist = distance_sq(atom1.position, atom2.position);
                if(dist < min_dist) {min_dist = dist;}
            }
        }
        return min_dist;
    }

    real_type limit_energy(const real_type val) const
    {
        if(val >= this->e_max_) {return e_max_;}
        if(val <= this->e_min_) {return e_min_;}
        return val;
    }

  private:

    real_type e_min_;
    real_type e_max_;

    real_type go_contact_threshold_;
    real_type atom_contact_cutoff_;
    real_type hydrogen_bond_cutoff_;
    real_type salt_bridge_cutoff_;

    real_type coef_13_;
    real_type coef_14_;
    real_type coef_go_;
    real_type cbd_aicg2_;
    real_type wid_aicg13_;
    real_type wid_dih_;

    real_type bb_hydrogen_bond_;
    real_type bb_donor_acceptor_;
    real_type bb_carbon_contact_;
    real_type bb_other_contact_;
    real_type ss_hydrogen_bond_;
    real_type ss_donor_acceptor_;
    real_type ss_salty_bridge_;
    real_type ss_carbon_contact_;
    real_type ss_charge_contact_;
    real_type ss_other_contact_;
    real_type bs_hydrogen_bond_;
    real_type bs_donor_acceptor_;
    real_type bs_carbon_contact_;
    real_type bs_charge_contact_;
    real_type bs_other_contact_;
    real_type long_range_contact_;
    real_type offset_;

    real_type k_angle_;
    real_type k_dihedral_;

    std::array<real_type, 10>                        angle_x_;
    std::map<std::string, std::array<real_type, 10>> angle_y_1_;
    std::map<std::string, std::array<real_type, 10>> angle_y_2_;
    std::map<std::string, std::array<real_type, 7>>  dihedral_term_;

    std::vector<std::size_t> flexible_beads_;
};

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
AICG2Plus<realT>::generate(
        toml::basic_value<toml::preserve_comments, std::map>& ff_,
        const group_type& chains) const
{
    using value_type = toml::basic_value<toml::preserve_comments, std::map>;
    using array_type = value_type::array_type;
    using table_type = value_type::table_type;

    if(ff_.is_uninitialized())
    {
        ff_ = table_type{};
    }

    table_type& ff = ff_.as_table();
    if(ff.count("local") == 0)
    {
        ff["local"] = array_type{};
    }
    array_type& ff_tables = ff.at("local").as_array();

    for(const auto& chain : chains)
    {
        if(!this->check_beads_kind(chain))
        {
            std::cerr
                << '[' << mjolnir::io::red << "error" << mjolnir::io::nocolor
                << "] AICG2+: Invalid Bead Kind. stop parameter generation"
                << std::endl;
            return ff_;
        }

        // --------------------------------------------------------------------
        // generate bond length interaction
        {
            // It is inefficient to define multiple LocalForceFiled having the
            // same combination of interaction and potential.
            // So here, first search a table that defines the same forcefield.
            // If it exists, push new parameters to the found one. Otherwise,
            // add a new table and push to it.

            // make sure that the tablel does have bondlength+harmonic forcefield
            value_type bond_length(toml::table{
                {"interaction", "BondLength"},
                {"potential",   "Harmonic"},
                {"topology",    "bond"},
                {"parameters",  array_type{}}
            });
            const auto table_finder =
                local_forcefield_table_comparator<value_type>(bond_length);

            if(ff_tables.end() == std::find_if(
                        ff_tables.begin(), ff_tables.end(), table_finder))
            {
                ff_tables.push_back(std::move(bond_length));
            }

            // append new parameters to bondlength+harmonic forcefield

            auto& params = std::find_if(
                    ff_tables.begin(), ff_tables.end(), table_finder
                )->as_table().at("parameters").as_array();
            params.reserve(params.size() + chain.size());

            for(std::size_t i=1, sz = chain.size(); i<sz; ++i)
            {
                const auto& bead1 = chain.at(i-1);
                const auto& bead2 = chain.at(i);
                const auto  i1    = bead1->index();
                const auto  i2    = bead2->index();
                const auto  dist  = distance(bead1->position(), bead2->position());

                value_type para = toml::table{};
                para.as_table()["indices"] = array_type{i1, i2};
                para.as_table()["v0"     ] = dist;
                para.as_table()["k"      ] = this->cbd_aicg2_;

                if(i == 1)
                {
                    para.comments().push_back(std::string(
                            " AICG2+ BondLength for chain ") + chain.name());
                }
                params.push_back(std::move(para));
            }
        }
        /* bond-angle */{
            table_type bond_angle{
                {"interaction", "BondLength"},
                {"potential",   "Gaussian"},
                {"topology",    "none"}
            };

            array_type params;
            for(std::size_t i=2, sz = chain.size(); i<sz; ++i)
            {
                const auto& bead1 = chain.at(i-2);
                const auto& bead2 = chain.at(i-1);
                const auto& bead3 = chain.at(i);
                const auto  i1    = bead1->index();
                const auto  i2    = bead2->index();
                const auto  i3    = bead3->index();

                // if the beads contains flexible region, remove 1-3 contact.
                if(is_in_flexible_region(i1) || is_in_flexible_region(i2) ||
                   is_in_flexible_region(i3))
                {
                    continue;
                }

                table_type para;
                para["indices"] = value_type{i1, i3};
                para["v0"     ] = distance(bead1->position(), bead3->position());
                para["sigma"  ] = this->wid_aicg13_;
                para["k"      ] = this->coef_13_ * this->calc_contact_coef(bead1, bead3);
                params.push_back(std::move(para));
            }
            bond_angle["parameters"] = std::move(params);
            ff.at("local").as_array().push_back(std::move(bond_angle));
        }
        /* flexible-local-angle */{
            table_type flp_angle{
                {"interaction", "BondAngle"},
                {"potential",   "FlexibleLocalAngle"},
                {"topology",    "none"}
            };

            table_type env;
            const std::string y1_prefix("y1_");
            const std::string y2_prefix("y2_");
            {
                for(const auto& y1 : this->angle_y_1_)
                {
                    env[y1_prefix + y1.first] = y1.second;
                }
            }
            {
                for(const auto& y2 : this->angle_y_2_)
                {
                    env[y2_prefix + y2.first] = y2.second;
                }
            }

            flp_angle["env"] = std::move(env);

            array_type params;
            for(std::size_t i=2, sz = chain.size(); i<sz; ++i)
            {
                const auto& bead1 = chain.at(i-2);
                const auto& bead2 = chain.at(i-1);
                const auto& bead3 = chain.at(i);
                const auto  i1    = bead1->index();
                const auto  i2    = bead2->index();
                const auto  i3    = bead3->index();

                table_type para;
                para["indices"] = value_type{i1, i2, i3};
                para["k"      ] = this->k_angle_;
                para["y"      ] = y1_prefix + bead2->name();
                para["d2y"    ] = y2_prefix + bead2->name();
                params.push_back(std::move(para));
            }
            flp_angle["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(flp_angle));
        }
        /* dihedral-angle */{
            table_type dihd_angle{
                {"interaction", "DihedralAngle"},
                {"potential",   "Gaussian"},
                {"topology",    "none"}
            };

            array_type params;
            for(std::size_t i=3, sz = chain.size(); i<sz; ++i)
            {
                const auto& bead1 = chain.at(i-3);
                const auto& bead2 = chain.at(i-2);
                const auto& bead3 = chain.at(i-1);
                const auto& bead4 = chain.at(i);
                const auto  i1    = bead1->index();
                const auto  i2    = bead2->index();
                const auto  i3    = bead3->index();
                const auto  i4    = bead4->index();

                // if the beads contains flexible region, remove 1-4 contact.
                if(is_in_flexible_region(i1) || is_in_flexible_region(i2) ||
                   is_in_flexible_region(i3) || is_in_flexible_region(i4))
                {
                    continue;
                }

                table_type para;
                para["indices"] = value_type{i1, i2, i3, i4};
                para["v0"     ] = dihedral_angle(
                                    bead1->position(), bead2->position(),
                                    bead3->position(), bead4->position());
                para["sigma"  ] = this->wid_dih_;
                para["k"      ] = this->coef_14_ * this->calc_contact_coef(bead1, bead4);
                params.push_back(std::move(para));
            }
            dihd_angle["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(dihd_angle));
        }
        /* flexible-dihedral-angle */{
            table_type dihd_angle{
                {"interaction", "DihedralAngle"},
                {"potential"  , "FlexibleLocalDihedral"},
                {"topology"   , "none"}
            };

            table_type env;
            for(const auto& dih : this->dihedral_term_)
            {
                env[dih.first] = dih.second;
            }
            dihd_angle["env"] = std::move(env);

            array_type params;
            for(std::size_t i=0, sz = chain.size() - 3; i<sz; ++i)
            {
                const auto& bead1 = chain.at(i);
                const auto& bead2 = chain.at(i+1);
                const auto& bead3 = chain.at(i+2);
                const auto& bead4 = chain.at(i+3);
                const std::size_t i1 = bead1->index();
                const std::size_t i2 = bead2->index();
                const std::size_t i3 = bead3->index();
                const std::size_t i4 = bead4->index();

                // like "ALA-PHE" or something like that
                const std::string separator("-");

                table_type para;
                para["indices"] = value_type{i1, i2, i3, i4};
                para["k"      ] = this->k_dihedral_;
                para["coef"   ] = bead2->name() + separator + bead3->name();
                params.push_back(std::move(para));
            }
            dihd_angle["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(dihd_angle));
        }

        const real_type th2 = this->go_contact_threshold_ *
                              this->go_contact_threshold_;
        /* intra-chain-go-contacts */
        if(4 < chain.size()) // if chain has <4 atoms, no contact would be formed
        {
            table_type go_contact{
                {"interaction", "BondLength"},
                {"potential",   "GoContact"},
                {"topology",    "contact"}
            };

            array_type params;
            for(std::size_t i=0, sz_i = chain.size()-4; i<sz_i; ++i)
            {
                for(std::size_t j=i+4, sz_j = chain.size(); j<sz_j; ++j)
                {
                    if(this->min_distance_sq(chain.at(i), chain.at(j)) < th2)
                    {
                        const auto& bead1 = chain.at(i);
                        const auto& bead2 = chain.at(j);
                        const auto  i1    = bead1->index();
                        const auto  i2    = bead2->index();

                        // if one of the bead is flexible region, continue.
                        if(is_in_flexible_region(i1) || is_in_flexible_region(i2))
                        {
                            continue;
                        }

                        table_type para;
                        para["indices"] = value_type{i1, i2};
                        para["v0"     ] = distance(bead1->position(), bead2->position());
                        para["k"      ] = -this->coef_go_ * calc_contact_coef(bead1, bead2);
                        params.push_back(std::move(para));
                    }
                }
            }
            go_contact["parameters"]  = std::move(params);
            ff["local"].as_array().push_back(std::move(go_contact));
        }
    }
    return ff_;
}

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
AICG2Plus<realT>::generate(
        toml::basic_value<toml::preserve_comments, std::map>& ff_,
        const group_type& lhs, const group_type& rhs) const
{
    using value_type = toml::basic_value<toml::preserve_comments, std::map>;
    using array_type = value_type::array_type;
    using table_type = value_type::table_type;

    table_type& ff = ff_.as_table();
    if(ff.count("local") == 0)
    {
        ff["local"] = array_type{};
    }

    const auto th2 = this->go_contact_threshold_ * this->go_contact_threshold_;

    table_type go_contact{
        {"interaction", "BondLength"},
        {"potential",   "GoContact"},
        {"topology",    "contact"}
    };

    std::vector<std::pair<std::string, std::string>> combinations;
    combinations.reserve(1 + lhs.size() * rhs.size() / 2);

    array_type params;
    for(const auto& chain1 : lhs)
    {
        for(const auto& chain2 : rhs)
        {
            // intra chain. skip
            if(chain1.name() == chain2.name())
            {
                continue;
            }
            // combination already found
            if(std::find_if(combinations.begin(), combinations.end(),
                [&](const std::pair<std::string, std::string>& c){
                    return (c.first == chain2.name() && c.second == chain1.name()) ||
                           (c.first == chain1.name() && c.second == chain2.name());
                }) != combinations.end())
            {
                continue;
            }
            combinations.push_back(std::make_pair(chain1.name(), chain2.name()));

            for(const auto& bead1 : chain1)
            {
                for(const auto& bead2 : chain2)
                {
                    if(this->min_distance_sq(bead1, bead2) < th2)
                    {
                        const auto i1 = bead1->index();
                        const auto i2 = bead2->index();

                        // if one of the bead is flexible region, continue.
                        if(is_in_flexible_region(i1) || is_in_flexible_region(i2))
                        {
                            continue;
                        }

                        table_type para;
                        para["indices"] = value_type{i1, i2};
                        para["v0"     ] = distance(bead1->position(), bead2->position());
                        para["k"      ] = -coef_go_ * this->calc_contact_coef(bead1, bead2);
                        params.push_back(std::move(para));
                    }
                }
            }
        }
    }
    go_contact["parameters"] = std::move(params);
    ff["local"].as_array().push_back(std::move(go_contact));

    return ff_;
}

template<typename realT>
typename AICG2Plus<realT>::real_type
AICG2Plus<realT>::calc_contact_coef(
        const bead_ptr& bead1, const bead_ptr& bead2) const
{
    std::size_t num_bb_hb = 0; // backbone-backbone hydrogen bond
    std::size_t num_bb_da = 0; // backbone-backbone donor-acceptor
    std::size_t num_bb_cx = 0; // backbone-backbone carbon-X contact
    std::size_t num_bb_oc = 0; // backbone-backbone other contact

    std::size_t num_ss_hb = 0; // sidechain-sidechain hydrogen bond
    std::size_t num_ss_sb = 0; // sidechain-sidechain salty bridge
    std::size_t num_ss_da = 0; // sidechain-sidechain donor-acceptor
    std::size_t num_ss_cc = 0; // sidechain-sidechain charge contact
    std::size_t num_ss_cx = 0; // sidechain-sidechain carbon-X contact
    std::size_t num_ss_oc = 0; // sidechain-sidechain other contact

    std::size_t num_bs_hb = 0; // backbone-sidechain hydrogen bond
    std::size_t num_bs_da = 0; // backbone-sidechain donor-acceptor
    std::size_t num_bs_cc = 0; // backbone-sidechain charge contact
    std::size_t num_bs_cx = 0; // backbone-sidechain carbon-X contact
    std::size_t num_bs_oc = 0; // backbone-sidechain other contact

    std::int32_t num_short = 0; // short range contact
    std::int32_t num_long  = 0; // long range contact

    for(const auto& atom1 : bead1->atoms())
    {
        for(const auto& atom2 : bead2->atoms())
        {
            const auto dist = distance(atom1.position, atom2.position);

            if(dist < go_contact_threshold_)
            {
                num_long += 1;
            }
            if(atom_contact_cutoff_ <= dist)
            {
                // out of cutoff range. no contact between atom1 and atom2.
                continue;
            }
            // it is a short range contact (dist < atom_contact_cutoff).
            num_short += 1;

            if(is_backbone(atom1) && is_backbone(atom2))
            {
                if(is_donor_acceptor_pair(atom1, atom2))
                {
                    if(dist < hydrogen_bond_cutoff_)
                    {
                        num_bb_hb += 1;
                    }
                    else // no hydrogen bond, just donor-acceptor
                    {
                        num_bb_da += 1;
                    }
                }
                else if(is_carbon(atom1) || is_carbon(atom2))
                {
                    num_bb_cx += 1;
                }
                else // other atomic contacts
                {
                    num_bb_oc += 1;
                }
            }
            else if(is_sidechain(atom1) && is_sidechain(atom2))
            {
                if(is_donor_acceptor_pair(atom1, atom2))
                {
                    if(is_cation_anion_pair(atom1, atom2))
                    {
                        if(dist < salt_bridge_cutoff_)
                        {
                            num_ss_sb += 1;
                        }
                        else // not a salt bridge, charge contact.
                        {
                            num_ss_cc += 1;
                        }
                    }
                    else if(dist < hydrogen_bond_cutoff_)
                    {
                        num_ss_hb += 1;
                    }
                    else if(is_cation(atom1) || is_anion(atom2) ||
                            is_cation(atom2) || is_anion(atom1))
                    {
                        // one of the two has a charge, charge contact.
                        num_ss_cc += 1;
                    }
                    else // just a donor-acceptor.
                    {
                        num_ss_da += 1;
                    }
                }
                else if(is_cation(atom1) || is_anion(atom2) ||
                        is_cation(atom2) || is_anion(atom1))
                {
                    num_ss_cc += 1; // charge contact.
                }
                else if(is_carbon(atom1) || is_carbon(atom2))
                {
                    num_ss_cx += 1; // carbon-X
                }
                else // other atomic contacts
                {
                    num_ss_oc += 1;
                }
            }
            else // backbone-sidechain contact
            {
                if(is_donor_acceptor_pair(atom1, atom2))
                {
                    if(dist < hydrogen_bond_cutoff_)
                    {
                        num_bs_hb += 1; // hydrogen bond formed.
                    }
                    else if(is_cation(atom1) || is_anion(atom2) ||
                            is_cation(atom2) || is_anion(atom1))
                    {
                        num_bs_cc += 1; // one of two has a charge.
                    }
                    else
                    {
                        num_bs_da += 1; // just donor-acceptor.
                    }
                }
                else if(is_cation(atom1) || is_anion(atom2) ||
                        is_cation(atom2) || is_anion(atom1))
                {
                    num_bs_cc += 1; // one of two has a charge.
                }
                else if(is_carbon(atom1) || is_carbon(atom2))
                {
                    num_bs_cx += 1; // one of two is a carbon
                }
                else // other kind of atomic contacts...
                {
                    num_bs_oc += 1;
                }
            }
        } // atom2
    } // atom1

    // XXX pair of residues cannot form more than one salty bridge.
    // set the number of salty bridge to 1, and the rests are counted as
    // sidechain-sidechain charge contact(backbone never forms salty bridge).
    if(num_ss_sb > 1)
    {
        num_ss_cc += (num_ss_sb - 1);
        num_ss_sb = 1;
    }

    if(this->atom_contact_cutoff_ < this->go_contact_threshold_)
    {
        assert(num_long >= num_short);
        num_long -= num_short;
    }
    else
    {
        num_long = 0;
    }

    // calculate weighted sum.
    const real_type e_tot = this->offset_     +
        this->bb_hydrogen_bond_   * num_bb_hb +
        this->bb_donor_acceptor_  * num_bb_da +
        this->bb_carbon_contact_  * num_bb_cx +
        this->bb_other_contact_   * num_bb_oc +
        this->ss_hydrogen_bond_   * num_ss_hb +
        this->ss_donor_acceptor_  * num_ss_da +
        this->ss_salty_bridge_    * num_ss_sb +
        this->ss_carbon_contact_  * num_ss_cx +
        this->ss_charge_contact_  * num_ss_cc +
        this->ss_other_contact_   * num_ss_oc +
        this->bs_hydrogen_bond_   * num_bs_hb +
        this->bs_donor_acceptor_  * num_bs_da +
        this->bs_carbon_contact_  * num_bs_cx +
        this->bs_charge_contact_  * num_bs_cc +
        this->bs_other_contact_   * num_bs_oc +
        this->long_range_contact_ * num_long;

    return this->limit_energy(e_tot);
}

template<typename realT>
bool AICG2Plus<realT>::check_beads_kind(const chain_type& chain) const
{
    for(const auto& bead : chain)
    {
        if(bead->kind() != "CarbonAlpha")
        {
            std::cerr << "AICG2Plus: invalid coarse-grained bead kind: "
                      << bead->kind() << '\n';
            std::cerr << "it allows only CarbonAlpha beads.\n";
            return false;
        }
    }
    return true;
}

}//jarngreipr
#endif /* JARNGREIPR_FORCEFIELD_AICG2_PLUS */
