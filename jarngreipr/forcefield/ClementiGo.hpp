#ifndef JARNGREIPR_CLEMENTI_GO
#define JARNGREIPR_CLEMENTI_GO
#include <extlib/toml/toml.hpp>
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <jarngreipr/geometry/distance.hpp>
#include <jarngreipr/geometry/angle.hpp>
#include <jarngreipr/geometry/dihedral.hpp>
#include <iterator>
#include <iostream>
#include <vector>

namespace jarngreipr
{

template<typename realT>
class ClementiGo final : public ForceFieldGenerator<realT>
{
  public:
    using base_type  = ForceFieldGenerator<realT>;
    using real_type  = typename base_type::real_type;
    using bead_type  = typename base_type::bead_type;
    using chain_type = typename base_type::chain_type;
    using group_type = typename base_type::group_type;
    using bead_ptr   = typename chain_type::bead_ptr;

  public:

    template<typename Comment, template<typename...> class Map,
             template<typename...> class Array>
    ClementiGo(const toml::basic_value<Comment, Map, Array>& para)
        : coef_bond_        (toml::find<real_type>(para, "coef_bond")),
          coef_angle_       (toml::find<real_type>(para, "coef_angle")),
          coef_dihedral_1_  (toml::find<real_type>(para, "coef_dihedral_1")),
          coef_dihedral_3_  (toml::find<real_type>(para, "coef_dihedral_3")),
          coef_contact_     (toml::find<real_type>(para, "coef_contact")),
          contact_threshold_(toml::find<real_type>(para, "contact_threshold"))
    {}
    ~ClementiGo() override = default;

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

    real_type min_distance_sq(const bead_ptr& bead1, const bead_ptr& bead2) const
    {
        real_type min_dist_sq = std::numeric_limits<real_type>::max();
        for(const auto& atom1 : remove_hydrogens(bead1->atoms()))
        {
            for(const auto& atom2 : remove_hydrogens(bead2->atoms()))
            {
                const auto dsq = distance_sq(atom1.position, atom2.position);
                min_dist_sq = std::min(dsq, min_dist_sq);
            }
        }
        return min_dist_sq;
    }

  private:

    real_type coef_bond_;
    real_type coef_angle_;
    real_type coef_dihedral_1_;
    real_type coef_dihedral_3_;
    real_type coef_contact_;
    real_type contact_threshold_;
};

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
ClementiGo<realT>::generate(
        toml::basic_value<toml::preserve_comments, std::map>& out,
        const group_type& chains) const
{
    using value_type = toml::basic_value<toml::preserve_comments, std::map>;
    using table_type = typename value_type::table_type;
    using array_type = typename value_type::array_type;

    if(!this->check_beads_kind(chain))
    {
        std::cerr << "ClementiGo: stop generating forcefield..." << std::endl;
        return ;
    }
    if(ff.is_uninitialized())
    {
        ff = table_type{};
    }
    if(ff.count("local") == 0)
    {
        ff["local"] = toml::array_type{};
    }

    for(const auto& chain : chains)
    {
        /* bond-length */ {
            table_type bond_length{
                {"interaction", "BondLength"},
                {"potential",   "Harmonic"},
                {"topology",    "bond"}
            };

            array_type params;
            for(std::size_t i=1, sz = chain.size(); i<sz; ++i)
            {
                const auto& bead1 = chain.at(i-1);
                const auto& bead2 = chain.at(i);
                const auto  i1    = bead1->index();
                const auto  i2    = bead2->index();
                table_type para;
                para["indices"] = toml::value{i1, i2};
                para["v0"     ] = distance(bead1->position(), bead2->position());
                para["k"      ] = coef_bond_;
                params.push_back(std::move(para));
            }
            bond_length["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(bond_length));
        }
        /* bond-angle */{
            table_type bond_length{
                {"interaction", "BondAngle"},
                {"potential",   "Harmonic"},
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

                const auto nat_angle = angle(
                    bead1->position(), bead2->position(), bead3->position());

                table_type para;
                para["indices"] = toml::value{i1, i2, i3};
                para["v0"     ] = nat_angle;
                para["k"      ] = coef_angle_;
                params.push_back(std::move(para));
            }
            bond_angle["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(bond_angle));
        }
        /* dihedral-angle */{
            table_type dihd_angle{
                {"interaction", "DihedralAngle"},
                {"potential",   "ClementiDihedral"},
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

                const auto nat_dihd = dihedral_angle(
                        bead1->position(), bead2->position(),
                        bead3->position(), bead4->position());

                table_type para;
                para["indices"] = toml::value{i1, i2, i3, i4};
                para["eq"     ] = nat_dihd;
                para["k1"     ] = coef_dihedral_1_;
                para["k3"     ] = coef_dihedral_3_;
                params.push_back(std::move(para));
            }
            dihd_angle["parameters"] = std::move(params);
            ff["local"].as_array().push_back(std::move(dihd_angle));
        }

        /* intra-chain-go-contacts */{
            const real_type th2 = contact_threshold_ * contact_threshold_;

            table_type go_contact {
                {"interaction", "BondLength"},
                {"potential",   "Go1012Contact"},
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

                        table_type para;
                        para["indices"] = toml::value{i1, i2};
                        para["eq"     ] = distance(bead1->position(), bead2->position());
                        para["k"      ] = coef_contact_;
                        params.push_back(std::move(para));
                    }
                }
            }
            go_contact["parameters"]  = std::move(params);
            ff["local"].as_array().push_back(std::move(go_contact));
        }
    }
    return;
}

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
ClementiGo<realT>::generate(
        toml::basic_value<toml::preserve_comments, std::map>& out,
        const group_type& lhs, const group_type& rhs) const
{
    const real_type th2 = contact_threshold_ * contact_threshold_;

    table_type go_contact {
        {"interaction", "BondLength"},
        {"potential"  , "Go1012Contact"},
        {"topology"   , "contact"}
    };

    std::vector<std::pair<std::string, std::string>> combinations;
    combinations.reserve(lhs.size() * rhs.size() / 2);

    array_type params;
    for(const auto& chain1 : lhs)
    {
        for(const auto& chain2 : rhs)
        {
            if(chain1.name() == chain2.name()){continue;}
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

                        table_type para;
                        para["indices"] = toml::value{i1, i2};
                        para["eq"     ] = distance(bead1->position(), bead2->position());
                        para["k"      ] = coef_contact_;
                        params.push_back(std::move(para));
                    }
                }
            }
        }
    }
    go_contact["parameters"]  = std::move(params);
    ff["local"].as_array().push_back(std::move(go_contact));

    return;
}

template<typename realT>
bool ClementiGo<realT>::check_beads_kind(const chain_type& chain) const
{
    for(const auto& bead : chain)
    {
        if(bead->kind() != "CarbonAlpha")
        {
            std::cerr << "ClementiGo: invalid coarse-grained bead kind: "
                      << bead->kind() << '\n';
            std::cerr << "it allows only CarbonAlpha beads.\n";
            return false;
        }
    }
    return true;
}

}//jarngreipr
#endif /* JARNGREIPR_CLEMENTI_GO */
