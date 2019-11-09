#ifndef JARNGREIPR_MODEL_CGBEAD_HPP
#define JARNGREIPR_MODEL_CGBEAD_HPP
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <vector>
#include <string>
#include <map>

namespace jarngreipr
{

template<typename realT>
class CGBead
{
  public:
    using real_type = realT;
    using atom_type = PDBAtom<real_type>;
    using coordinate_type = typename atom_type::coordinate_type;
    using container_type  = std::vector<atom_type>;

  public:

    CGBead(std::size_t index, real_type mass, container_type atoms, std::string name)
        : index_(index), mass_(mass), name_(std::move(name)), atoms_(std::move(atoms))
    {}
    virtual ~CGBead() = default;

    virtual coordinate_type position() const = 0;
    virtual std::string kind() const = 0;

    container_type const& atoms() const noexcept {return atoms_;}
    std::string    const& name()  const noexcept {return name_;}
    std::size_t    const& index() const noexcept {return index_;}
    real_type      const& mass()  const noexcept {return mass_;}

    bool has_attribute(const std::string& key) const {return attr_.count(key) == 1;}
    std::string const& attribute(const std::string& key) const {return attr_.at(key);}
    std::string&       attribute(const std::string& key)       {return attr_[key];}

  protected:

    std::size_t     index_;
    real_type       mass_;
    std::string     name_;
    container_type  atoms_;
    std::map<std::string, std::string> attr_;
};

}//jarngreipr
#endif /* JARNGREIPR_MODEL_CGBEAD_HPP */
