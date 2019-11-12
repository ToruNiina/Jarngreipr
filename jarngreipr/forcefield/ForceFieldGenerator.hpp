#ifndef JARNGREIPR_FORCEFIELD_GENERATOR
#define JARNGREIPR_FORCEFIELD_GENERATOR
#include <jarngreipr/model/CGGroup.hpp>
#include <extlib/toml/toml.hpp>
#include <memory>
#include <map>

namespace jarngreipr
{

template<typename realT>
class ForceFieldGenerator
{
  public:
    using real_type  = realT;
    using bead_type  = CGBead<real_type>;
    using chain_type = CGChain<real_type>;
    using group_type = CGGroup<real_type>;
    using atom_type  = typename bead_type::atom_type;

  public:
    virtual ~ForceFieldGenerator() = default;

    //!@brief generate forcefield parameter values
    virtual toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& group) const = 0;

    //!@brief generate inter-chain parameters if it's defined.
    virtual toml::basic_value<toml::preserve_comments, std::map>&
    generate(toml::basic_value<toml::preserve_comments, std::map>& out,
             const group_type& lhs, const group_type& rhs) const = 0;

    //!@brief if chain contains invalid bead, return false.
    virtual bool check_beads_kind(const chain_type& chain) const = 0;
};

// It is inefficient to define multiple LocalForceFiled having the same
// combination of interaction and potential.
//
// This comparator finds a table that has the same combination as the reference
// table value. It is helpful to merge two (semantically the same) tables.
//
template<typename Value>
struct toml_table_comparator
{
    using value_type = Value; // toml::basic_value<...>

    value_type reference;
    std::vector<std::string> keys; // keys to compare

    toml_table_comparator(value_type r, std::vector<std::string> ks)
        : reference(std::move(r)), keys(std::move(ks))
    {
        if(!r.is_table())
        {
            throw std::invalid_argument("reference toml value is not a table");
        }
    }

    bool operator()(const value_type& other) noexcept
    {
        // check "interaction", "potential" and "topology".
        try
        {
            const auto& t1 = reference.as_table();
            const auto& t2 = other.as_table();
            for(const auto& key : keys)
            {
                if(t1.at(key) != t2.at(key))
                {
                    return false;
                }
            }
            return true;
        }
        catch(...) {return false;}
    };
};

template<typename Comment, template<typename...> class Map,
         template<typename...> class Array>
toml_table_comparator<toml::basic_value<Comment, Map, Array>>
make_table_comparator(const toml::basic_value<Comment, Map, Array>& tab,
                      std::vector<std::string> keys)
{
    return toml_table_comparator<toml::basic_value<Comment, Map, Array>>(
            tab, std::move(keys));
}

// search a table.
// If a table that is equivalent to`src` exists in a table `dst`,
// return a reference to it.
// Otherwise, push `src` and return a reference to the newly created one.
template<typename Comment, template<typename...> class Map,
         template<typename...> class Array>
toml::basic_value<Comment, Map, Array>&
find_or_push_table(toml::basic_value<Comment, Map, Array>& dst,
                   const toml::basic_value<Comment, Map, Array>& src,
                   std::vector<std::string> keys)
{
    const auto cmp = make_table_comparator(src, std::move(keys));
    auto& dst_array = dst.as_array();
    const auto found = std::find_if(dst_array.begin(), dst_array.end(), cmp);
    if(dst_array.end() == found)
    {
        dst_array.push_back(src);
        return dst_array.back();
    }
    return *found;
}

} // mjolnir
#endif// JARNGREIPR_FORCEFIELD_GENERATOR
