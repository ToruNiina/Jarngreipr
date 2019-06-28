#ifndef JARNGREIPR_GET_SUBSTR_HPP
#define JARNGREIPR_GET_SUBSTR_HPP
#include <jarngreipr/io/write_error.hpp>
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
            const std::size_t index,
            const std::string error_message,
            const at_line line_number = at_line{0})
{
    try
    {
        return str.at(index);
    }
    catch(const std::out_of_range& err)
    {
        write_error(std::cerr, error_message, " character at ", index,
                    " cannot be extracted from the following string");
        write_underline(std::cerr, str, index, 1, line_number);
        std::exit(EXIT_FAILURE);
    }
}

// with extra error message
template<typename charT, typename traits, typename Alloc>
inline std::basic_string<charT, traits, Alloc>
get_substr(const std::basic_string<charT, traits, Alloc>& str,
           const std::size_t begin, const std::size_t len,
           const std::string error_message,
           const at_line line_number = at_line{0})
{
    try
    {
        return str.substr(begin, len);
    }
    catch(const std::out_of_range& err)
    {
        write_error(std::cerr, error_message, "range [", begin, ", ", begin+len,
                    ") cannot be extracted from the following string");
        write_underline(std::cerr, str, begin, len, line_number);
        std::exit(EXIT_FAILURE);
    }
}
} // jarngreipr
#endif// JARNGREIPR_GET_SUBSTR_HPP
