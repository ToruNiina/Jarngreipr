#ifndef JARNGREIPR_READ_NUMBER_HPP
#define JARNGREIPR_READ_NUMBER_HPP
#include <jarngreipr/util/get_substr.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>

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
T read_number(const source_location& src)
{
    static_assert(std::is_arithmetic<T>::value, "");
    try
    {
        return detail::read_number_impl<T>(get_substr(src));
    }
    catch(const std::invalid_argument& err)
    {
        log(log_level::error, "read_number: invalid number format", src, "here");
        std::terminate();
    }
    catch(const std::out_of_range& err)
    {
        log(log_level::error, "read_number: invalid number format", src, "here");
        std::terminate();
    }
    catch(const std::exception& err)
    {
        log(log_level::error, "read_number: unknown error appeared", src, "here");
        std::terminate();
    }
}

template<typename T>
T read_number(source_location& src, std::size_t first, std::size_t length)
{
    src.column() = first;
    src.range()  = length;
    return read_number<T>(src);
}

} // jarngreipr
#endif// JARNGREIPR_READ_NUMBER_HPP
