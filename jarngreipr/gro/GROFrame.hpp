#ifndef JARNGREIPR_GRO_FRAME_HPP
#define JARNGREIPR_GRO_FRAME_HPP
#include <jarngreipr/gro/GROLine.hpp>
#include <mjolnir/math/Vector.hpp>

namespace jarngreipr
{

template<typename realT>
struct GROFrame
{
    using real_type = realT;
    using line_type = GROLine<realT>;
    using coordinate_type = typename line_type::coordinate_type;

    std::string            comment;
    std::vector<line_type> lines;
    realT                  time;
    coordinate_type        box;
};

} // jarngreipr
#endif // JARNGREIPR_GRO_FRAME_HPP
