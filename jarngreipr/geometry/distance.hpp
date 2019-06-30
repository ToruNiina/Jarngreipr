#ifndef JARNGREIPR_GEOMETRY_DISTANCE
#define JARNGREIPR_GEOMETRY_DISTANCE
#include <mjolnir/math/math.hpp>

namespace jarngreipr
{

template<typename realT>
inline realT distance_sq(const mjolnir::math::Vector<realT, 3>& lhs,
                         const mjolnir::math::Vector<realT, 3>& rhs) noexcept
{
    return mjolnir::math::length_sq(lhs - rhs);
}

template<typename realT>
inline realT distance(const mjolnir::math::Vector<realT, 3>& lhs,
                      const mjolnir::math::Vector<realT, 3>& rhs) noexcept
{
    return mjolnir::math::length(lhs - rhs);
}

} // jarngreipr
#endif /* JARNGREIPR_GEOMETRY_DISTANCE */
