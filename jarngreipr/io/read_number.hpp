#ifndef JARNGREIPR_READ_NUMBER_HPP
#define JARNGREIPR_READ_NUMBER_HPP
#include <jarngreipr/io/write_error.hpp>
#include <jarngreipr/io/get_substr.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>

// use and `read_number` in the following way.
//
// ```cpp
// std::string num = "the answer is 42";
// int a1 = read_number<int>(num, 14, 2);
// int a2 = read_number<int>(num, 14, 2, at_line(10));
// int a3 = read_number<int>(num, 14, 2, "while reading a file: ");
// int a4 = read_number<int>(num, 14, 2, "while reading a file: ", at_line(10));
// ```
//
// and when it failed you will get an error message like...
// (with line number and extra error message)
//
// ```
// error: while reading a string, expected number, but got
// 10 | foobar
//    |    ^^^
// ```

namespace jarngreipr
{
namespace detail
{
// ----------------------------------------------------------------------------
// floating point numbers
template<typename T>
typename std::enable_if<std::is_same<T, double>::value, double>::type
read_number_impl(const std::string& s)
{
    return std::stod(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, float>::value, float>::type
read_number_impl(const std::string& s)
{
    return std::stof(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, long double>::value, long double>::type
read_number_impl(const std::string& s)
{
    return std::stold(s);
}
// ----------------------------------------------------------------------------
// integers supported by standard library
template<typename T>
typename std::enable_if<std::is_same<T, int>::value, T>::type
read_number_impl(const std::string& s)
{
    return std::stoi(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, long>::value, T>::type
read_number_impl(const std::string& s)
{
    return std::stol(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, long long>::value, T>::type
read_number_impl(const std::string& s)
{
    return std::stoll(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, unsigned long>::value, T>::type
read_number_impl(const std::string& s)
{
    return std::stoul(s);
}
template<typename T>
typename std::enable_if<std::is_same<T, unsigned long long>::value, T>::type
read_number_impl(const std::string& s)
{
    return std::stoull(s);
}
// ----------------------------------------------------------------------------
// other integers (require conversion) ...
template<typename T>
typename std::enable_if<
    std::is_signed<T>::value     && std::is_integral<T>::value    &&
    !std::is_same<T, int>::value && !std::is_same<T, long>::value &&
    !std::is_same<T, long long>::value, T>::type
read_number_impl(const std::string& s)
{
    return static_cast<T>(std::stoll(s));
}
template<typename T>
typename std::enable_if<
    std::is_unsigned<T>::value             && std::is_integral<T>::value &&
    !std::is_same<T, unsigned long>::value &&
    !std::is_same<T, unsigned long long>::value, T>::type
read_number_impl(const std::string& s)
{
    return static_cast<T>(std::stoull(s));
}
} // detail

template<typename T>
T read_number(const std::string& str,
              const std::size_t begin, const std::size_t len,
              const std::string error_prefix = "",
              const at_line line_number = at_line{0})
{
    static_assert(std::is_arithmetic<T>::value, "");
    try
    {
        return detail::read_number_impl<T>(
                get_substr(str, begin, len, error_prefix, line_number));
    }
    catch(const std::invalid_argument& err)
    {
        write_error    (std::cerr, error_prefix, "expected number, but got");
        write_underline(std::cerr, str, begin, len, line_number);
        std::exit(EXIT_FAILURE);
    }
    catch(const std::out_of_range& err)
    {
        write_error    (std::cerr, error_prefix, "invalid number appeared");
        write_underline(std::cerr, str, begin, len, line_number);
        std::exit(EXIT_FAILURE);
    }
    catch(const std::exception& err)
    {
        write_error    (std::cerr, error_prefix, "unknown error appeared: ",
                                   err.what());
        write_underline(std::cerr, str, begin, len, line_number);
        std::exit(EXIT_FAILURE);
    }
}

} // jarngreipr
#endif// JARNGREIPR_READ_NUMBER_HPP
