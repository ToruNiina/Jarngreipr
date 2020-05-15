#ifndef JARNGREIPR_FORCEFIELD_GO_CONTACT_HPP
#define JARNGREIPR_FORCEFIELD_GO_CONTACT_HPP
#include <extlib/toml/toml.hpp>
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <jarngreipr/geometry/distance.hpp>
#include <jarngreipr/util/log.hpp>
#include <iterator>
#include <iostream>
#include <vector>

namespace jarngreipr
{

template<typename realT>
class GoContact final : public ForceFieldGenerator<realT>
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
    explicit GoContact(const toml::basic_value<Comment, Map, Array>& para)
        : coef_contact_     (toml::find<real_type>(para, "coef_contact")),
          contact_threshold_(toml::find<real_type>(para, "contact_threshold"))
    {}
    ~GoContact() override = default;

    // generate intra-chain contacts.
    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& group) const override
    {
        using value_type = toml::basic_value<toml::preserve_comments, std::map>;
        using array_type = value_type::array_type;
        using table_type = value_type::table_type;

        if(out.is_uninitialized())
        {
            out = table_type{};
        }
        if(!out.contains("local"))
        {
            out["local"] = array_type{};
        }

        const auto th2 = this->contact_threshold_ * this->contact_threshold_;

        auto& params = find_or_push_table(out.at("local"), value_type{
            {"interaction", "BondLength"},
            {"potential",   "GoContact"},
            {"topology",    "contact"},
            {"parameters",  array_type{}}
        }, /* the keys that should be equivalent = */ {
            "interaction", "potential", "topology"
        }).as_table().at("parameters").as_array();

        for(std::size_t chain_i = 0; chain_i < group.size(); ++chain_i)
        {
            const auto& chain1 = group.at(chain_i);

            for(std::size_t chain_j=chain_i+1; chain_j<group.size(); ++chain_j)
            {
                const auto& chain2 = group.at(chain_j);

                log::info("generating Go Contact parameters between ", chain1.name(),
                          " and ", chain2.name(), " with coefficient ", this->coef_contact_, ".\n");

                bool is_first = true; // gen comment about chain name
                for(const auto& bead1 : chain1)
                {
                    if(is_in_flexible_region(bead1)) {continue;}

                    for(const auto& bead2 : chain2)
                    {
                        if(is_in_flexible_region(bead2)) {continue;}

                        if(this->min_distance_sq(bead1, bead2) < th2)
                        {
                            const auto i1 = bead1->index();
                            const auto i2 = bead2->index();

                            const auto nat_dist =
                                distance(bead1->position(), bead2->position());

                            value_type para = table_type{
                                {"indices", value_type{i1, i2}  },
                                {"v0"     , nat_dist            },
                                {"k"      , -this->coef_contact_}
                            };
                            if(is_first)
                            {
                                para.comments().push_back(std::string(" Go "
                                    "Contact Potential between chain ") +
                                    chain1.name() + " and " + chain2.name());
                                is_first = false;
                            }
                            params.push_back(std::move(para));
                        }
                    }
                }
            }
        }
        return out;
    }

    // generate inter-chain contacts.
    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const std::vector<std::reference_wrapper<const group_type>>& gs
             ) const override
    {
        using value_type = toml::basic_value<toml::preserve_comments, std::map>;
        using array_type = value_type::array_type;
        using table_type = value_type::table_type;

        log::debug("generating inter-chain AICG2+ parameters\n");
        for(const auto& g : gs)
        {
            log::debug("- ", g.get().name(), "\n");
        }

        if(out.is_uninitialized())
        {
            out = table_type{};
        }
        if(!out.contains("local"))
        {
            out["local"] = array_type{};
        }

        const auto th2 = this->contact_threshold_ * this->contact_threshold_;

        auto& params = find_or_push_table(out.at("local"), value_type{
            {"interaction", "BondLength"},
            {"potential",   "GoContact"},
            {"topology",    "contact"},
            {"parameters",  array_type{}}
        }, /* the keys that should be equivalent = */ {
            "interaction", "potential", "topology"
        }).as_table().at("parameters").as_array();

        std::vector<std::pair<std::string, std::string>> combinations;

        for(std::size_t i=0; i<gs.size(); ++i)
        {
            const auto& lhs = gs.at(i).get();
            for(std::size_t j=i+1; j<gs.size(); ++j)
            {
                const auto& rhs = gs.at(j).get();
                for(const auto& chain1 : lhs)
                {
                    for(const auto& chain2 : rhs)
                    {
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

                        log::info("generating Go Contact parameters between chain ",
                                  chain1.name(), " and ", chain2.name(), " using coefficient ",
                                  this->coef_contact_, ".\n");

                        bool is_first = true;
                        for(const auto& bead1 : chain1)
                        {
                            for(const auto& bead2 : chain2)
                            {
                                if(this->min_distance_sq(bead1, bead2) < th2)
                                {
                                    const auto i1 = bead1->index();
                                    const auto i2 = bead2->index();

                                    // if one of the bead is flexible region, continue.
                                    if(is_in_flexible_region(bead1) || is_in_flexible_region(bead2))
                                    {
                                        continue;
                                    }

                                    value_type para = table_type{
                                        {"indices", value_type{i1, i2}},
                                        {"v0"     , distance(bead1->position(), bead2->position())},
                                        {"k"      , -coef_contact_}
                                    };
                                    if(is_first)
                                    {
                                        para.comments().push_back(" Go Contact Potential "
                                            "between chain " + chain1.name() + " and chain " + chain2.name());
                                        is_first = false;
                                    }
                                    params.push_back(std::move(para));
                                }
                            }
                        }
                    }
                }
            }
        }
        return out;
    }

    bool check_beads_kind(const chain_type& chain) const override {return true;}

  private:

    bool is_in_flexible_region(const bead_ptr& bead) const
    {
        return bead->has_attribute("flexible_regions");
    }
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

    real_type coef_contact_;
    real_type contact_threshold_;
};

} // jarngreipr
#endif// JARNGREIPR_FORCEFIELD_GO_HPP
