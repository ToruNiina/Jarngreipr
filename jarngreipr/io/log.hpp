#ifndef JARNGREIPR_IO_LOG_HPP
#define JARNGREIPR_IO_LOG_HPP
#include <mjolnir/util/color.hpp>
#include <memory>
#include <utility>
#include <memory>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdint>

namespace jarngreipr
{

enum class log_level : std::uint8_t
{
    debug = 0,
    info  = 1,
    warn  = 2,
    error = 3
};

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const log_level lv)
{
    using mjolnir::io::red;
    using mjolnir::io::yellow;
    using mjolnir::io::green;
    using mjolnir::io::cyan;
    using mjolnir::io::nocolor;

    switch(lv)
    {
        case log_level::error:
        {
            os << '[' << red    << "error" << nocolor << "] ";
            break;
        }
        case log_level::warn:
        {
            os << '[' << yellow << "warn"  << nocolor << "] ";
            break;
        }
        case log_level::info:
        {
            os << '[' << green  << "info"  << nocolor << "] ";
            break;
        }
        case log_level::debug:
        {
            os << '[' << cyan   << "debug" << nocolor << "] ";
            break;
        }
        default:
        {
            std::cerr << '[' << red << "error" << nocolor << ']'
                      << " invlaid log level appeared: "
                      << static_cast<int>(static_cast<std::uint8_t>(lv))
                      << std::endl;
            std::terminate();
        }
    }
    return os;
}

namespace detail
{

template<typename Level = log_level>
struct basic_logger
{
    static std::unique_ptr<std::ofstream> file;

    static bool is_activated(const Level lv)
    {
        const auto found = filter.find(lv);
        if(found == filter.end()) {return true;}
        return found->second;
    }

    static void   activate(const Level lv) {filter[lv] = true;}
    static void inactivate(const Level lv) {filter[lv] = false;}

    static void open(const std::string& filename)
    {
        file.reset(new std::ofstream(filename));
        return ;
    }

  private:

    static std::map<Level, bool> filter;
};

template<typename Level>
std::map<Level, bool> basic_logger<Level>::filter;
template<typename Level>
std::unique_ptr<std::ofstream> basic_logger<Level>::file;

template<typename charT, typename traits>
void log_output(std::basic_ostream<charT, traits>&)
{
    return;
}

// to output arbitral number of arguments...
template<typename charT, typename traits, typename T, typename ... Ts>
void log_output(std::basic_ostream<charT, traits>& os, T&& v, Ts&& ... args)
{
    if(basic_logger<log_level>::file)
    {
        (*basic_logger<log_level>::file) << v;
    }
    os << std::forward<T>(v);
    log_output(os, std::forward<Ts>(args)...);
    return;
}

} // detail

using logger = detail::basic_logger<log_level>;

template<typename ... Ts>
void log(log_level lv, Ts&& ... args)
{
    if(logger::is_activated(lv))
    {
        detail::log_output(std::cerr, lv, std::forward<Ts>(args)...);
    }
    return ;
}

} // jarngreipr
#endif // JARNGREIPR_IO_LOG_HPP
