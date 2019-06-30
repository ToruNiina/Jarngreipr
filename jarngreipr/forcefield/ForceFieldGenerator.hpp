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
struct local_forcefield_table_comparator
{
    using value_type = Value;

    value_type ref;

    local_forcefield_table_comparator(value_type r)
        : ref(std::move(r))
    {
        if(!r.is_table())
        {
            throw std::invalid_argument("reference toml value is not a table");
        }
    }

    bool operator()(const value_type& tgt) noexcept
    {
        // check "interaction", "potential" and "topology".

        // check only the content of a string. toml::values are the same
        // only if both comment and the value is the same. But here we can
        // ignore the comments.
        try
        {
            const auto& lt = ref.as_table();
            const auto& rt = tgt.as_table();
            if(lt.count("interaction"))
            {
                if(lt.at("interaction").as_string().str !=
                   rt.at("interaction").as_string().str)
                {
                    return false;
                }
            }
            if(lt.count("potential"))
            {
                if(lt.at("potential").as_string().str !=
                   rt.at("potential").as_string().str)
                {
                    return false;
                }
            }
            if(lt.count("topology"))
            {
                if(lt.at("topology").as_string().str !=
                   rt.at("topology").as_string().str)
                {
                    return false;
                }
            }
            return true;
        }
        catch(...) {return false;}
    };
};

} // mjolnir
#endif// JARNGREIPR_FORCEFIELD_GENERATOR
