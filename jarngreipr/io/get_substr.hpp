#ifndef JARNGREIPR_GET_SUBSTR_HPP
#define JARNGREIPR_GET_SUBSTR_HPP
#include <jarngreipr/io/source_location.hpp>
#include <jarngreipr/io/log.hpp>
#include <iostream>
#include <string>
#include <cstdlib>

// use `get_substr` and `get_char_at` in the following way.
// std::string line = "hello, world!";
// std::string sub1 = get_substr(line, 7, 5); // sub1 become "world"
// std::string sub2 = get_substr(line, 7, 5, at_line(10));
// std::string sub3 = get_substr(line, 7, 5, "while reading a string");
// std::string sub4 = get_substr(line, 7, 5, "while reading a string", at_line(10));

namespace jarngreipr
{

template<typename charT, typename traits, typename Alloc>
inline charT
get_char_at(const std::basic_string<charT, traits, Alloc>& str,
            const std::size_t index, source_location src)
{
    try
    {
        return str.at(index);
    }
    catch(const std::out_of_range& err)
    {
        src.column() = index;
        src.range()  = 1;
        log(log_level::error, "couldn't get a character", src, "here");
        std::terminate();
    }
}

// with extra error message
template<typename charT, typename traits, typename Alloc>
inline std::basic_string<charT, traits, Alloc>
get_substr(const std::basic_string<charT, traits, Alloc>& str,
           const std::size_t begin, const std::size_t len,
           source_location src)
{
    try
    {
        return str.substr(begin, len);
    }
    catch(const std::out_of_range& err)
    {
        src.column() = begin;
        src.range()  = len;
        log(log_level::error, "couldn't get a sub-string", src, "here");
        std::terminate();
    }
}
} // jarngreipr
#endif// JARNGREIPR_GET_SUBSTR_HPP
