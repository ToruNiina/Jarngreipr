#ifndef JARNGREIPR_TOML_SERIALIZER_HPP
#define JARNGREIPR_TOML_SERIALIZER_HPP
#include <extlib/toml/toml.hpp>
#include <jarngreipr/format/write_number.hpp>
#include <sstream>
#include <string>

//
// special formatter for toml values
//
namespace jarngreipr
{

template<typename Value>
struct inline_formatted_serializer
{
    using value_type = Value;
    using table_type = typename Value::table_type;
    using array_type = typename Value::array_type;

    inline_formatted_serializer(
        const std::string& fmt_int, const std::string& fmt_float)
        : fmt_int_(fmt_int), fmt_float_(fmt_float)
    {}

    std::string operator()(const toml::integer& x)
    {
        return format_number(fmt_int_.data(), x);
    }
    std::string operator()(const toml::floating& x)
    {
        return format_number(fmt_float_.data(), x);
    }
    std::string operator()(const array_type& x)
    {
        if(x.empty()) {return "[]";}
        std::ostringstream oss;
        oss << '[';
        bool is_front = true;
        for(const auto& v: x)
        {
            if(!is_front) {oss << ", ";}
            oss << toml::visit(*this, v);
            is_front = false;
        }
        oss << ']';
        return oss.str();
    }
    std::string operator()(const table_type& x)
    {
        std::ostringstream oss;
        if(x.empty()) {return "{}";}

        oss << '{';
        bool is_front = true;
        for(const auto& kv: x)
        {
            if(!is_front) {oss << ", ";}
            oss << kv.first << " = " << toml::visit(*this, kv.second);
            is_front = false;
        }
        oss << '}';
        return oss.str();
    }
    template<typename T, typename std::enable_if<
        !std::is_same<T, toml::floating>::value &&
        !std::is_same<T, toml::integer>::value  &&
        !std::is_same<T, table_type>::value     &&
        !std::is_same<T, array_type>::value,
        std::nullptr_t>::type = nullptr>
    std::string operator()(const T& x) noexcept
    {
        std::ostringstream oss;
        oss << x;
        return oss.str();
    }

  private:

    std::string fmt_int_;
    std::string fmt_float_;
};

} // jarngreipr
#endif
