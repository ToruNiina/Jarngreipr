#ifndef JARNGREIPR_SOURCE_LOCATION_HPP
#define JARNGREIPR_SOURCE_LOCATION_HPP
#include <mjolnir/util/color.hpp>
#include <string>
#include <cstdint>

namespace jarngreipr
{

struct source_location
{
    // range-begin starts from 0
    source_location(std::string filename,    std::string line,
                    std::size_t range_begin, std::size_t range_length,
                    std::size_t line_num)
        : line_(line_num), column_(range_begin), range_(range_length),
          file_name_(std::move(filename)), line_str_(std::string(line))
    {}

    source_location() = default;
    ~source_location() = default;
    source_location(source_location const&) = default;
    source_location(source_location &&)     = default;
    source_location& operator=(source_location const&) = default;
    source_location& operator=(source_location &&)     = default;

    std::uint32_t line()   const noexcept {return line_;}
    std::uint32_t column() const noexcept {return column_;}
    std::uint32_t range()  const noexcept {return range_;}

    std::string const& file_name() const noexcept {return file_name_;}
    std::string const& line_str()  const noexcept {return line_str_;}

  private:

    std::uint32_t line_;
    std::uint32_t column_;
    std::uint32_t range_;
    std::string   file_name_;
    std::string   line_str_;
};

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const source_location& src)
{
    const auto line_num_str   = std::to_string(src.line());
    const auto line_num_width = line_num_str.size();

    // something like the following;
    // ```
    // ---> example.dat
    //    |
    // 10 | invalid line
    //    |         ^^^^
    // ```

    os << "---> " << src.file_name() << '\n';
    for(std::size_t i=0; i<line_num_width; ++i){os << ' ';}
    os << " | \n";
    os << line_num_str << " | " << src.line_str() << '\n';
    for(std::size_t i=0; i<line_num_width; ++i){os << ' ';}
    os << " | ";
    for(std::size_t i=0; i<src.column();++i){os << ' ';}
    os << mjolnir::io::red;
    for(std::size_t i=0; i<src.range(); ++i){os << '^';}
    os << mjolnir::io::nocolor;
    return os;
}

} // jarngreipr
#endif//JARNGREIPR_SOURCE_LOCATION_HPP
