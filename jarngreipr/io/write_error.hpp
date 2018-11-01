#ifndef JARNGREIPR_WRITE_ERROR_HPP
#define JARNGREIPR_WRITE_ERROR_HPP
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

// to output arbitral number of arguments
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

    os << "error: ";
    detail::write_error_impl(os, std::forward<Ts>(args)...);
    os << '\n';
    return;
}

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

inline void write_underline(std::ostream& os, const std::string& line,
        const std::size_t begin, const std::size_t length, const char c = '^',
        const at_line line_number = at_line{0})
{
    if(line_number.value == 0)
    {
        os << line << '\n';
        for(std::size_t i=0; i<begin;  ++i){os << ' ';}
        for(std::size_t i=0; i<length; ++i){os << c;}
        os << '\n';
    }
    else
    {
        const std::size_t wid = std::to_string(line_number.value).size();

        os << line_number.value << " | " << line << '\n';
        for(std::size_t i=0; i<wid;    ++i){os << ' ';}
        os << " | ";
        for(std::size_t i=0; i<begin;  ++i){os << ' ';}
        for(std::size_t i=0; i<length; ++i){os << c;}
        os << '\n';
    }
    return;
}

} // jarngreipr
#endif // JARNGREIPR_WRITE_ERROR_HPP
