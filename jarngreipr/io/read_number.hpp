#ifndef JARNGREIPR_READ_NUMBER_HPP
#define JARNGREIPR_READ_NUMBER_HPP
#include <jarngreipr/io/write_error.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>

// use `get_substr` and `read_number` in the following way.
// ```cpp
// std::string line = "hello, world!";
// std::string sub1 = get_substr(line, 7, 5); // sub1 become "world"
// std::string sub2 = get_substr(line, 7, 5, at_line(10));
// std::string sub3 = get_substr(line, 7, 5, "while reading a string");
// std::string sub4 = get_substr(line, 7, 5, "while reading a string", at_line(10));
//
// std::string num = "the answer is 42";
// int a1 = read_number<int>(num, 14, 2);
// int a2 = read_number<int>(num, 14, 2, at_line(10));
// int a3 = read_number<int>(num, 14, 2, "six by nine,");
// int a4 = read_number<int>(num, 14, 2, "six by nine,", at_line(10));
// ```
// and when failed you will get an error message like...
// (with line number and extra error message)
// ```
// error: while reading a string, expected number, but got
// 10 | foobar
//    |    ^^^
// ```

namespace jarngreipr
{
namespace detail
{

template<typename T>
T read_number_impl(const std::string& str);

template<>
double read_number_impl<double>(const std::string& s)
{
    return std::stod(s);
}
template<>
float read_number_impl<float>(const std::string& s)
{
    return std::stof(s);
}
template<>
int read_number_impl<int>(const std::string& s)
{
    return std::stoi(s);
}
template<>
long read_number_impl<long  >(const std::string& s)
{
    return std::stol(s);
}
template<>
long long read_number_impl<long long>(const std::string& s)
{
    return std::stoll(s);
}
template<>
unsigned long read_number_impl<unsigned long>(const std::string& s)
{
    return std::stoul(s);
}
template<>
unsigned long long read_number_impl<unsigned long long>(const std::string& s)
{
    return std::stoull(s);
}
} // detail

template<typename charT, typename traits, typename Alloc>
inline charT
get_char_at(const std::basic_string<charT, traits, Alloc>& str,
            const std::size_t index,
            const at_line line_number = at_line{0})
{
    try
    {
        return str.at(index);
    }
    catch(const std::out_of_range& err)
    {
        write_error(std::cerr, "character at ", index,
                    " cannot be extracted from the following string");
        write_underline(std::cerr, str, index, 1, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
}

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
        write_error(std::cerr, error_message, "character at ", index,
                    " cannot be extracted from the following string");
        write_underline(std::cerr, str, index, 1, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
}

template<typename charT, typename traits, typename Alloc>
inline std::basic_string<charT, traits, Alloc>
get_substr(const std::basic_string<charT, traits, Alloc>& str,
           const std::size_t begin, const std::size_t len,
           const at_line line_number = at_line{0})
{
    try
    {
        return str.substr(begin, len);
    }
    catch(const std::out_of_range& err)
    {
        write_error(std::cerr, "range [", begin, ", ", begin + len,
                    ") cannot be extracted from the following string");
        write_underline(std::cerr, str, begin, len, '^', line_number);
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
        write_underline(std::cerr, str, begin, len, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
}



template<typename T>
T read_number(const std::string& str,
              const std::size_t begin, const std::size_t len,
              const at_line line_number = at_line{0})
{
    static_assert(std::is_arithmetic<T>::value, "");
    try
    {
        return detail::read_number_impl<T>(
                get_substr(str, begin, len, line_number));
    }
    catch(const std::invalid_argument& err)
    {
        write_error    (std::cerr, "expected number, but got");
        write_underline(std::cerr, str, begin, len, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
    catch(const std::out_of_range& err)
    {
        write_error    (std::cerr, "invalid number appeared");
        write_underline(std::cerr, str, begin, len, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
}
// with extra error message
template<typename T>
T read_number(const std::string& str,
              const std::size_t begin, const std::size_t len,
              const std::string error_message,
              const at_line line_number = at_line{0})
{
    static_assert(std::is_arithmetic<T>::value, "");
    try
    {
        return detail::read_number_impl<T>(
                get_substr(str, begin, len, error_message, line_number));
    }
    catch(const std::invalid_argument& err)
    {
        write_error    (std::cerr, error_message, " expected number, but got");
        write_underline(std::cerr, str, begin, len, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
    catch(const std::out_of_range& err)
    {
        write_error    (std::cerr, error_message, " invalid number appeared");
        write_underline(std::cerr, str, begin, len, '^', line_number);
        std::exit(EXIT_FAILURE);
    }
}



} // jarngreipr
#endif// JARNGREIPR_READ_NUMBER_HPP
