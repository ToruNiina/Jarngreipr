#ifndef JARNGREIPR_NINFO_ELEMENT_HPP
#define JARNGREIPR_NINFO_ELEMENT_HPP
#include <ostream>
#include <istream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

namespace jarngreipr
{

enum class NinfoKind
{
    bond,
    angl,
    dihd,
    aicg13,
    aicg14,
    aicgdih,
    contact,
    basepair,
    basestack,
    pdpwm,
};

template<NinfoKind kind>
struct ninfo_prefix;
template<>
struct ninfo_prefix<NinfoKind::bond>
{static constexpr auto prefix = "bond";};
template<>
struct ninfo_prefix<NinfoKind::angl>
{static constexpr auto prefix = "angl";};
template<>
struct ninfo_prefix<NinfoKind::dihd>
{static constexpr auto prefix = "dihd";};
template<>
struct ninfo_prefix<NinfoKind::aicg13>
{static constexpr auto prefix = "aicg13";};
template<>
struct ninfo_prefix<NinfoKind::aicg14>
{static constexpr auto prefix = "aicg14";};
template<>
struct ninfo_prefix<NinfoKind::aicgdih>
{static constexpr auto prefix = "aicgdih";};
template<>
struct ninfo_prefix<NinfoKind::contact>
{static constexpr auto prefix = "contact";};
template<>
struct ninfo_prefix<NinfoKind::basepair>
{static constexpr auto prefix = "basepair";};
template<>
struct ninfo_prefix<NinfoKind::basestack>
{static constexpr auto prefix = "basestack";};
template<>
struct ninfo_prefix<NinfoKind::pdpwm>
{static constexpr auto prefix = "pdpwm";};

template<typename realT, std::size_t N_units, std::size_t N_particles,
         std::size_t N_coefs, NinfoKind Kind>
struct NinfoElement
{
    using real_type = realT;
    static constexpr auto        kind   = Kind;
    static constexpr const char* prefix = ninfo_prefix<Kind>::prefix;

    std::size_t                          id;
    std::array<std::size_t, N_units>     units;
    std::array<std::size_t, N_particles> imps;
    std::array<std::size_t, N_particles> impuns;
    std::array<real_type, N_coefs>       coefs;
    std::string suffix; // like "p-p"
};

template<typename realT, std::size_t N_u, std::size_t N_p, std::size_t N_c,
         NinfoKind kind>
constexpr const char* NinfoElement<realT, N_u, N_p, N_c, kind>::prefix;

template<typename T, std::size_t Nu, std::size_t Np, std::size_t Nc, NinfoKind kind>
bool operator==(const NinfoElement<T, Nu, Np, Nc, kind>& lhs,
                const NinfoElement<T, Nu, Np, Nc, kind>& rhs) noexcept
{
    return lhs.id     == rhs.id     &&
           lhs.units  == rhs.units  &&
           lhs.imps   == rhs.imps   &&
           lhs.impuns == rhs.impuns &&
           lhs.coefs  == rhs.coefs  &&
           lhs.suffix == rhs.suffix;
}

template<typename T, std::size_t Nu, std::size_t Np, std::size_t Nc, NinfoKind kind>
bool operator!=(const NinfoElement<T, Nu, Np, Nc, kind>& lhs,
                const NinfoElement<T, Nu, Np, Nc, kind>& rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename realT>
using NinfoBond      = NinfoElement<realT, 2, 2,  4, NinfoKind::bond>;
template<typename realT>
using NinfoAngl      = NinfoElement<realT, 2, 3,  4, NinfoKind::angl>;
template<typename realT>
using NinfoDihd      = NinfoElement<realT, 2, 4,  5, NinfoKind::dihd>;
template<typename realT>
using NinfoAicg13    = NinfoElement<realT, 2, 3,  5, NinfoKind::aicg13>;
template<typename realT>
using NinfoAicg14    = NinfoElement<realT, 2, 4,  5, NinfoKind::aicg14>;
template<typename realT>
using NinfoAicgdih   = NinfoElement<realT, 2, 4,  5, NinfoKind::aicgdih>;
template<typename realT>
using NinfoContact   = NinfoElement<realT, 2, 2,  4, NinfoKind::contact>;
template<typename realT>
using NinfoBasePair  = NinfoElement<realT, 2, 2,  4, NinfoKind::basepair>;
template<typename realT>
using NinfoBaseStack = NinfoElement<realT, 2, 2,  4, NinfoKind::basestack>;
template<typename realT>
using NinfoPDPWM     = NinfoElement<realT, 1, 1, 10, NinfoKind::pdpwm>;

// XXX: BE CAREFUL. In the format of PDPWM, the second column group (normally imp)
//      has nil value, filled with 999 and the third column group (normally impun)
//      represents imp.

template<NinfoKind kind, typename realT>
struct ninfo_type_of;
template<typename realT>
struct ninfo_type_of<NinfoKind::bond,      realT> {using type = NinfoBond<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::angl,      realT> {using type = NinfoAngl<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::dihd,      realT> {using type = NinfoDihd<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::aicg13,    realT> {using type = NinfoAicg13<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::aicg14,    realT> {using type = NinfoAicg14<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::aicgdih,   realT> {using type = NinfoAicgdih<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::contact,   realT> {using type = NinfoContact<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::basepair,  realT> {using type = NinfoBasePair<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::basestack, realT> {using type = NinfoBaseStack<realT>;};
template<typename realT>
struct ninfo_type_of<NinfoKind::pdpwm,     realT> {using type = NinfoPDPWM<realT>;};

} // jarngreipr
#endif// JARNGREIPR_NINFO_ELEMENT_HPP
