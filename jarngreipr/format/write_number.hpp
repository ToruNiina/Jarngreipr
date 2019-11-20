#ifndef JARNGREIPR_WRITE_NUMBER_HPP
#define JARNGREIPR_WRITE_NUMBER_HPP
#include <vector>
#include <string>
#include <ostream>
#include <cstdio>

namespace jarngreipr
{

template<typename ... Ts>
std::string format_number(const char* fmt, const Ts& ... xs)
{
    const std::size_t N = std::snprintf(nullptr, 0, fmt, xs ...);
    std::vector<char> buf(N + 1);
    std::snprintf(buf.data(), buf.size(), fmt, xs...);
    return std::string(buf.data());
}

template<typename ... Ts>
std::ostream& write_number(std::ostream& os, const char* fmt, const Ts& ... xs)
{
    os << format_number(fmt, xs...);
    return os;
}

}
#endif// JARNGREIPR_WRITE_NUMBER_HPP
