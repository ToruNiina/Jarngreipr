#ifndef JARNGREIPR_MODEL_3SPN2_HPP
#define JARNGREIPR_MODEL_3SPN2_HPP
#include <jarngreipr/model/CGBead.hpp>
#include <jarngreipr/model/CGModelGenerator.hpp>
#include <jarngreipr/io/log.hpp>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <string>

namespace jarngreipr
{

template<typename realT>
class ThreeSPN2Base final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Base(std::size_t idx, container_type atoms, std::string name)
        : base_type(idx, std::move(atoms), std::move(name))
    {
        // TODO
    }
    ~ThreeSPN2Base() override = default;

    ThreeSPN2Base(const ThreeSPN2Base&) = default;
    ThreeSPN2Base(ThreeSPN2Base&&)      = default;
    ThreeSPN2Base& operator=(const ThreeSPN2Base&) = default;
    ThreeSPN2Base& operator=(ThreeSPN2Base&&)      = default;

    std::string kind() const override {return "3SPN2Base";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class ThreeSPN2Sugar final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Sugar(std::size_t idx, container_type atoms, std::string name)
        : base_type(idx, std::move(atoms), std::move(name))
    {
        // TODO
    }
    ~ThreeSPN2Sugar() override = default;

    ThreeSPN2Sugar(const ThreeSPN2Sugar&) = default;
    ThreeSPN2Sugar(ThreeSPN2Sugar&&)      = default;
    ThreeSPN2Sugar& operator=(const ThreeSPN2Sugar&) = default;
    ThreeSPN2Sugar& operator=(ThreeSPN2Sugar&&)      = default;

    std::string kind() const override {return "3SPN2Sugar";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

template<typename realT>
class ThreeSPN2Phosphate final : public CGBead<realT>
{
  public:
    typedef CGBead<realT> base_type;
    typedef typename base_type::real_type       real_type;
    typedef typename base_type::coordinate_type coordinate_type;
    typedef typename base_type::atom_type       atom_type;
    typedef typename base_type::container_type  container_type;

  public:

    ThreeSPN2Phosphate(std::size_t idx, container_type atoms, std::string name)
        : base_type(idx, std::move(atoms), std::move(name))
    {
        // TODO
    }
    ~ThreeSPN2Phosphate() override = default;

    ThreeSPN2Phosphate(const ThreeSPN2Phosphate&) = default;
    ThreeSPN2Phosphate(ThreeSPN2Phosphate&&)      = default;
    ThreeSPN2Phosphate& operator=(const ThreeSPN2Phosphate&) = default;
    ThreeSPN2Phosphate& operator=(ThreeSPN2Phosphate&&)      = default;

    std::string kind() const override {return "3SPN2Phosphate";}

    coordinate_type position() const override {return this->position_;}

  private:

    coordinate_type position_;
};

} // jarngreipr
#endif /*JARNGREIPR_CARBON_ALPHA*/
