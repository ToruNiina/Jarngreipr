#ifndef JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
#define JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
#include <jarngreipr/forcefield/ForceFieldGenerator.hpp>
#include <extlib/toml/toml.hpp>

namespace jarngreipr
{

template<typename realT>
class ExcludedVolume final : public ForceFieldGenerator<realT>
{
  public:
    using base_type = ForceFieldGenerator<realT>;
    using real_type  = typename base_type::real_type;
    using bead_type  = typename base_type::bead_type;
    using chain_type = typename base_type::chain_type;
    using group_type = typename base_type::group_type;

  public:

    template<typename Comment, template<typename...> class Map,
             template<typename...> class Array>
    explicit ExcludedVolume(const toml::basic_value<Comment, Map, Array>& para)
        : epsilon_(toml::find<decltype(epsilon_)>(para, "epsilon")),
          radii_  (toml::find<decltype(radii_)  >(para, "radii"))
    {}
    ~ExcludedVolume() override = default;

    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& chains) const override;

    toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const std::vector<std::reference_wrapper<const group_type>>& gs
             ) const override;

    bool check_beads_kind(const chain_type&) const override
    {
        return true;
    }

  private:

    real_type epsilon_;
    std::map<std::string, real_type> radii_;
};

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
ExcludedVolume<realT>::generate(
        toml::basic_value<toml::preserve_comments, std::map>& ff_,
        const group_type& chains) const
{
    throw std::runtime_error("ExcludedVolume is global-only potential");
//     using value_type = toml::basic_value<toml::preserve_comments, std::map>;
//     using array_type = value_type::array_type;
//     using table_type = value_type::table_type;
//
//     if(ff_.is_uninitialized())
//     {
//         ff_ = table_type{};
//     }
//
//     table_type& ff = ff_.as_table();
//     if(ff.count("global") == 0)
//     {
//         ff["global"] = array_type{};
//     }
//
//     toml::basic_value<toml::preserve_comments, std::map> exv{
//         {"interaction", "Pair"          },
//         {"potential"  , "ExcludedVolume"},
//         {"ignore", table_type{
//                 {"particles_within", table_type{{"bond", 3}, {"contact", 1}}},
//                 {"molecule", "Nothing"}
//             }
//         },
//         {"spatial_partition", table_type{
//                 {"type", "CellList"}, {"margin", 0.5}
//             }
//         },
//         {"epsilon", this->epsilon_},
//         {"parameters",  array_type{}}
//     };
//
//     auto& params = find_or_push_table(ff.at("global"), exv,
//         /* the keys that should be equivalent = */ {
//             "interaction", "potential", "ignore", "spatial_partition", "epsilon"
//         }).as_table().at("parameters").as_array();
//
//     for(const auto& chain : chains)
//     {
//         for(const auto& bead : chain)
//         {
//             table_type para;
//             para["index"]  = bead->index();
//             para["radius"] = this->radii_.at(bead->name());
//             params.push_back(std::move(para));
//         }
//     }
//     return ff_;
}

template<typename realT>
toml::basic_value<toml::preserve_comments, std::map>&
ExcludedVolume<realT>::generate(
    toml::basic_value<toml::preserve_comments, std::map>& ff_,
    const std::vector<std::reference_wrapper<const group_type>>& groups) const
{
    using value_type = toml::basic_value<toml::preserve_comments, std::map>;
    using array_type = value_type::array_type;
    using table_type = value_type::table_type;

    if(ff_.is_uninitialized())
    {
        ff_ = table_type{};
    }

    table_type& ff = ff_.as_table();
    if(ff.count("global") == 0)
    {
        ff["global"] = array_type{};
    }

    table_type exv{
        {"interaction", "Pair"          },
        {"potential"  , "ExcludedVolume"},
        // TODO: input ignore particles
        {"ignore", table_type{
                {"particles_within", table_type{{"bond", 3}, {"contact", 1}}},
            }
        },
        {"spatial_partition", table_type{
                {"type", "CellList"}, {"margin", 0.5}
            }
        },
        {"epsilon", this->epsilon_}
    };

    array_type params;
    for(const auto& group : groups)
    {
        for(const auto& chain : group.get())
        {
            for(const auto& bead : chain)
            {
                table_type para;
                para["index"]  = bead->index();
                para["radius"] = this->radii_.at(bead->name());
                params.push_back(std::move(para));
            }
        }
    }
    exv["parameters"] = std::move(params);

    ff.at("global").as_array().push_back(std::move(exv));
    return ff_;
}

} // jarngreipr
#endif// JARNGREIPR_FORCEFIELD_EXCLUDED_VOLUME
