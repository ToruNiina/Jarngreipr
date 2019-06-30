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

} // mjolnir
#endif// JARNGREIPR_FORCEFIELD_GENERATOR
