#ifndef JARNGREIPR_IO_XYZ_PARTICLE_HPP
#define JARNGREIPR_IO_XYZ_PARTICLE_HPP
#include <mjolnir/math/Vector.hpp>
#include <utility>
#include <ostream>
#include <istream>
#include <iomanip>
#include <sstream>
#include <string>

namespace jarngreipr
{

template<typename realT>
struct XYZParticle
{
    typedef realT  real_type;
    typedef mjolnir::Vector<real_type, 3> coordinate_type;

    std::string     name;
    coordinate_type position;
};

} // jarngreipr
#endif //JARNGREIPR_IO_XYZ_DATA
