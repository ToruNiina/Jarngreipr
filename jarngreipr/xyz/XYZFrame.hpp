#ifndef JARNGREIPR_XYZ_FRAME_HPP
#define JARNGREIPR_XYZ_FRAME_HPP
#include <jarngreipr/xyz/XYZParticle.hpp>
#include <vector>

namespace jarngreipr
{

template<typename realT>
struct XYZFrame
{
    typedef realT              real_type;
    typedef XYZParticle<realT> particle_type;

    std::string                comment;
    std::vector<particle_type> lines;
};

} // jarngreipr
#endif // JARNGREIPR_XYZ_FRAME_HPP
