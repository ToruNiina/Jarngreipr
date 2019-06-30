#ifndef JARNGREIPR_MODEL_CGBEAD_HPP
#define JARNGREIPR_MODEL_CGBEAD_HPP
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <vector>

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

    CGBead(std::size_t index, container_type atoms, std::string name)
        : index_(index), name_(std::move(name)), atoms_(std::move(atoms))
    {}
    virtual ~CGBead() = default;

    virtual coordinate_type position() const = 0;
    virtual std::string attribute(const std::string& attr_name) const = 0;
    virtual std::string kind() const = 0;

    container_type const& atoms() const noexcept {return atoms_;}
    std::string    const& name()  const noexcept {return name_;}
    std::size_t    const& index() const noexcept {return index_;}

  protected:

    std::size_t     index_;
    std::string     name_;
    container_type  atoms_;
};

}//jarngreipr
#endif /* JARNGREIPR_MODEL_CGBEAD_HPP */
