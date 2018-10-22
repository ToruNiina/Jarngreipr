#ifndef JARNGREIPR_READ_NUMBER_HPP
#define JARNGREIPR_READ_NUMBER_HPP
#include <stdexcept>
#include <string>

#include <iostream>
#include <cstdlib>
#include <cassert>

namespace jarngreipr
{

template<typename T>
T read_number_impl(const std::string& str);

template<>
double read_number_impl<double>(const std::string& s) {return std::stod(s);}
template<>
float  read_number_impl<float >(const std::string& s) {return std::stof(s);}
template<>
int    read_number_impl<int   >(const std::string& s) {return std::stoi(s);}
template<>
long   read_number_impl<long  >(const std::string& s) {return std::stol(s);}
template<>
long long read_number_impl<long long>(const std::string& s)
{return std::stoll(s);}
template<>
unsigned long read_number_impl<unsigned long>(const std::string& s)
{return std::stoul(s);}
template<>
unsigned long long read_number_impl<unsigned long long>(const std::string& s)
{return std::stoull(s);}

template<typename charT, typename traits, typename Alloc>
inline std::basic_string<charT, traits, Alloc>
get_substr(const std::basic_string<charT, traits, Alloc>& str,
           std::size_t begin, std::size_t end,
           std::string error_message = std::string())
{
    try
    {
        return str.substr(begin, end);
    }
    catch(const std::out_of_range& err)
    {
        if(!error_message.empty())
        {
            std::cerr << "-- " << error_message << '\n';
        }
        std::cerr << "-- range [" << begin << ", " << end
                  << ") cannot be extracted from the following string\n";
        std::cerr << ">> " << str << '\n';
        std::exit(EXIT_FAILURE);
    }
}

template<typename T>
T read_number(const std::string& str, std::size_t begin, std::size_t end,
              std::string error_message = std::string())
{
    static_assert(std::is_arithmetic<T>::value, "");
    assert(begin < end);

    try
    {
        return read_number_impl<T>(get_substr(str, begin, end));
    }
    catch(const std::invalid_argument& err)
    {
        if(!error_message.empty())
        {
            std::cerr << "-- " << error_message << '\n';
        }
        std::cerr << "-- expected number, but got\n";
        std::cerr << ">> " << str << '\n';
        std::cerr << "   " << std::string(begin, ' ')
                  << std::string(end - begin, '^') << '\n';
        std::exit(EXIT_FAILURE);
    }
    catch(const std::out_of_range& err)
    {
        if(!error_message.empty())
        {
            std::cerr << "-- " << error_message << '\n';
        }
        std::cerr << "-- invalid number appeared\n";
        std::cerr << ">> " << str << '\n';
        std::cerr << "   " << std::string(begin, ' ')
                  << std::string(end - begin, '^') << '\n';
        std::exit(EXIT_FAILURE);
    }
}

} // jarngreipr
#endif// JARNGREIPR_READ_NUMBER_HPP
