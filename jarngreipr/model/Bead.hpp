#ifndef JARNGREIPR_MODEL_BEAD
#define JARNGREIPR_MODEL_BEAD
#include <jarngreipr/pdb/PDBAtom.hpp>
#include <vector>

namespace jarngreipr
{

template<typename realT>
class Bead
{
  public:
    typedef realT  real_type;
    typedef PDBAtom<real_type> atom_type;
    typedef typename atom_type::coordinate_type coordinate_type;
    typedef std::vector<atom_type> container_type;

  public:

    Bead(std::size_t index, container_type atoms, std::string name)
        : index_(index), name_(std::move(name)), atoms_(std::move(atoms))
    {}
    virtual ~Bead() = default;

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
#endif /* JARNGREIPR_BEAD */
