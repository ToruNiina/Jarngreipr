#ifndef JARNGREIPR_GET_SUBSTR_HPP
#define JARNGREIPR_GET_SUBSTR_HPP
#include <jarngreipr/io/source_location.hpp>
#include <jarngreipr/io/log.hpp>
#include <string>
#include <cstdlib>

namespace jarngreipr
{

inline char get_char_at(const source_location& src)
{
    try
    {
        return src.line_str().at(src.column());
    }
    catch(const std::out_of_range& err)
    {
        log(log_level::error, "couldn't get a character", src, "here");
        std::terminate();
    }
}
inline char get_char_at(source_location& src, const std::size_t index)
{
    src.column() = index;
    return get_char_at(src);
}

inline std::string get_substr(const source_location& src)
{
    try
    {
        return src.line_str().substr(src.column(), src.range());
    }
    catch(const std::out_of_range& err)
    {
        log(log_level::error, "couldn't get a sub-string", src, "here");
        std::terminate();
    }
}
inline std::string
get_substr(source_location& src, std::size_t first, std::size_t length)
{
    src.column() = first;
    src.range()  = length;
    return get_substr(src);
}


} // jarngreipr
#endif// JARNGREIPR_GET_SUBSTR_HPP
