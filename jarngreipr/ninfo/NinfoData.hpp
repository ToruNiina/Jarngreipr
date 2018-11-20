#ifndef JARNGREIPR_NINFO_DATA_HPP
#define JARNGREIPR_NINFO_DATA_HPP
#include <jarngreipr/ninfo/NinfoElement.hpp>
#include <vector>

namespace jarngreipr
{

template<typename realT>
struct NinfoData
{
    using real_type = realT;
    // std::variant or inheritance, later...
    std::vector<NinfoBond     <real_type>> bonds;
    std::vector<NinfoAngl     <real_type>> angls;
    std::vector<NinfoDihd     <real_type>> dihds;
    std::vector<NinfoAicg13   <real_type>> aicg13s;
    std::vector<NinfoAicg14   <real_type>> aicg14s;
    std::vector<NinfoAicgdih  <real_type>> aicgdihs;
    std::vector<NinfoContact  <real_type>> contacts;
    std::vector<NinfoBasePair <real_type>> basepairs;
    std::vector<NinfoBaseStack<real_type>> basestacks;
    std::vector<NinfoPDPWM    <real_type>> pdpwms;
};

namespace detail
{

template<NinfoKind kind, typename realT>
struct block_getter_impl;

#define JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(kind, valname)\
    template<typename realT>\
    struct block_getter_impl<NinfoKind::kind, realT>\
    {\
        using real_type = realT;\
        using ninfo_type = typename ninfo_type_of<NinfoKind::kind, realT>::type;\
        static std::vector<ninfo_type> const&\
        invoke(NinfoData<realT> const& nd) noexcept {return nd.valname;}\
        static std::vector<ninfo_type>&\
        invoke(NinfoData<realT>&       nd) noexcept {return nd.valname;}\
    };

JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(bond     , bonds     )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(angl     , angls     )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(dihd     , dihds     )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(aicg13   , aicg13s   )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(aicg14   , aicg14s   )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(aicgdih  , aicgdihs  )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(contact  , contacts  )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(basepair , basepairs )
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(basestack, basestacks)
JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR(pdpwm    , pdpwms    )

#undef JARNGREIPR_NINFO_BLOCK_GETTER_IMPL_GENERATOR
} // detail

template<NinfoKind kind, typename realT>
inline std::vector<typename ninfo_type_of<kind, realT>::type> const&
get_block(NinfoData<realT> const& nd) noexcept
{
    return detail::block_getter_impl<kind, realT>::invoke(nd);
}
template<NinfoKind kind, typename realT>
inline std::vector<typename ninfo_type_of<kind, realT>::type>&
get_block(NinfoData<realT>& nd) noexcept
{
    return detail::block_getter_impl<kind, realT>::invoke(nd);
}

// TODO operators for concat/split this?

} // jarngreipr
#endif // JARNGREIPR_NINFO_DATA_HPP
