#ifndef JARNGREIPR_UTIL_LOG_HPP
#define JARNGREIPR_UTIL_LOG_HPP
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

namespace log
{
enum class level : std::uint8_t
{
    debug = 0,
    info  = 1,
    warn  = 2,
    error = 3
};

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const level lv)
{
    using mjolnir::io::red;
    using mjolnir::io::yellow;
    using mjolnir::io::green;
    using mjolnir::io::cyan;
    using mjolnir::io::nocolor;

    switch(lv)
    {
        case level::error:
        {
            os << '[' << red    << "error" << nocolor << "] ";
            break;
        }
        case level::warn:
        {
            os << '[' << yellow << "warn"  << nocolor << "] ";
            break;
        }
        case level::info:
        {
            os << '[' << green  << "info"  << nocolor << "] ";
            break;
        }
        case level::debug:
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

template<typename Level = level>
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
    if(basic_logger<level>::file)
    {
        (*basic_logger<level>::file) << v;
    }
    os << std::forward<T>(v);
    log_output(os, std::forward<Ts>(args)...);
    return;
}

using logger = basic_logger<level>;

template<typename ... Ts>
void error(Ts&& ... args)
{
    if(logger::is_activated(level::error))
    {
        log_output(std::cerr, level::error, std::forward<Ts>(args)...);
    }
    return ;
}

template<typename ... Ts>
void warn(Ts&& ... args)
{
    if(logger::is_activated(level::warn))
    {
        log_output(std::cerr, level::warn, std::forward<Ts>(args)...);
    }
    return ;
}
template<typename ... Ts>
void info(Ts&& ... args)
{
    if(logger::is_activated(level::info))
    {
        log_output(std::cerr, level::info, std::forward<Ts>(args)...);
    }
    return ;
}
template<typename ... Ts>
void debug(Ts&& ... args)
{
    if(logger::is_activated(level::debug))
    {
        log_output(std::cerr, level::debug, std::forward<Ts>(args)...);
    }
    return ;
}
} // log

} // jarngreipr
#endif // JARNGREIPR_UTIL_LOG_HPP
