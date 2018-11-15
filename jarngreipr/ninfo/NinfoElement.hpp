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

namespace detail
{

template<char ... args>
struct char_sequence
{
    using value_type = char;
    static constexpr std::size_t size() noexcept {return sizeof ... (args);}
};

template<typename T> struct char_sequence_string_generator;

template<char ... args>
struct char_sequence_string_generator<char_sequence<args...>>
{
    static const char* generate() noexcept
    {
        constexpr std::size_t len = char_sequence<args...>::size()+1;
        static constexpr char retval[len] = {args..., '\0'};
        return retval;
    }
};

// n3559
using ninfo_bond_prefix      = char_sequence<'b','o','n','d'>;
using ninfo_angl_prefix      = char_sequence<'a','n','g','l'>;
using ninfo_dihd_prefix      = char_sequence<'d','i','h','d'>;
using ninfo_aicg13_prefix    = char_sequence<'a','i','c','g','1','3'>;
using ninfo_aicg14_prefix    = char_sequence<'a','i','c','g','1','4'>;
using ninfo_aicgdih_prefix   = char_sequence<'a','i','c','g','d','i','h'>;
using ninfo_contact_prefix   = char_sequence<'c','o','n','t','a','c','t'>;
using ninfo_basepair_prefix  = char_sequence<'b','a','s','e','p','a','i','r'>;
using ninfo_basestack_prefix = char_sequence<'b','a','s','e','s','t','a','c','k'>;

} // detail

template<typename realT, std::size_t N_particles, std::size_t N_coefs, typename prefixT>
struct NinfoElement
{
    using real_type   = realT;
    using prefix_type = prefixT;
    static char const* prefix;

    std::size_t                          id;
    std::array<std::size_t, 2>           units;
    std::array<std::size_t, N_particles> imps;
    std::array<std::size_t, N_particles> impuns;
    std::array<real_type, N_coefs>       coefs;
    std::string suffix; // like "p-p"
};

template<typename realT, std::size_t N_p, std::size_t N_c, typename prefixT>
char const* NinfoElement<realT, N_p, N_c, prefixT>::prefix =
    detail::char_sequence_string_generator<prefixT>::generate();

template<typename realT>
using NinfoBond      = NinfoElement<realT, 2, 4, detail::ninfo_bond_prefix>;
template<typename realT>
using NinfoAngl      = NinfoElement<realT, 3, 4, detail::ninfo_angl_prefix>;
template<typename realT>
using NinfoDihd      = NinfoElement<realT, 4, 5, detail::ninfo_dihd_prefix>;
template<typename realT>
using NinfoAicg13    = NinfoElement<realT, 3, 5, detail::ninfo_aicg13_prefix>;
template<typename realT>
using NinfoAicg14    = NinfoElement<realT, 4, 5, detail::ninfo_aicg14_prefix>;
template<typename realT>
using NinfoAicgdih   = NinfoElement<realT, 4, 5, detail::ninfo_aicgdih_prefix>;
template<typename realT>
using NinfoContact   = NinfoElement<realT, 2, 4, detail::ninfo_contact_prefix>;
template<typename realT>
using NinfoBasePair  = NinfoElement<realT, 2, 4, detail::ninfo_basepair_prefix>;
template<typename realT>
using NinfoBaseStack = NinfoElement<realT, 2, 4, detail::ninfo_basestack_prefix>;

} // jarngreipr
#endif// JARNGREIPR_NINFO_ELEMENT_HPP
