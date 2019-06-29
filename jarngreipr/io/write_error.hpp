#ifndef JARNGREIPR_WRITE_ERROR_HPP
#define JARNGREIPR_WRITE_ERROR_HPP
#include <mjolnir/util/color.hpp>
#include <utility>
#include <string>
#include <ostream>
#include <cstdlib>

namespace jarngreipr
{

namespace detail
{

template<typename charT, typename traits>
void write_error_impl(std::basic_ostream<charT, traits>&)
{
    return;
}

// to output arbitral number of arguments...
template<typename charT, typename traits, typename T, typename ... Ts>
void write_error_impl(std::basic_ostream<charT, traits>& os, T&& v, Ts&& ... args)
{
    os << std::forward<T>(v);
    return write_error_impl(os, std::forward<Ts>(args)...);
}
} // detail

// to make the error message format uniform.
template<typename ... Ts>
void write_error(std::ostream& os, Ts&& ... args)
{
    static_assert(sizeof...(Ts) > 0, "write_error requires at least one value");

    os << mjolnir::io::red << "error: " << mjolnir::io::nocolor;
    detail::write_error_impl(os, std::forward<Ts>(args)...);
    os << '\n';
    return;
}

// a wrapper struct that prevents (implicit) conversion from ints to line_number
struct at_line
{
    explicit at_line(std::size_t v): value(v){}

    at_line(): value(0){}
    ~at_line() = default;
    at_line(const at_line&) = default;
    at_line(at_line&&)      = default;
    at_line& operator=(const at_line&) = default;
    at_line& operator=(at_line&&)      = default;

    std::size_t value;
};

inline std::ostream&
write_underline(std::ostream& os, const std::string& line,
        const std::size_t begin, const std::size_t length,
        const at_line line_number = at_line{0},
        const char c = '^')
{
    if(line_number.value == 0)
    {
        os << " | " << line << '\n';
        os << " | ";
        for(std::size_t i=0; i<begin;  ++i){os << ' ';}
        os << mjolnir::io::red;
        for(std::size_t i=0; i<length; ++i){os << c;}
        os << mjolnir::io::nocolor;
        os << '\n';
        return ;
    }

    const auto line_num_str   = std::to_string(line_number.value);
    const auto line_num_width = line_num_str.size();

    os << line_num_str << " | " << line << '\n';
    for(std::size_t i=0; i<line_num_width; ++i){os << ' ';}
    os << " | ";
    for(std::size_t i=0; i<begin;  ++i){os << ' ';}
    os << mjolnir::io::red;
    for(std::size_t i=0; i<length; ++i){os << c;}
    os << mjolnir::io::nocolor;
    os << '\n';
    return;
}

} // jarngreipr
#endif // JARNGREIPR_WRITE_ERROR_HPP
