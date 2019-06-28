#ifndef JARNGREIPR_WRITE_SYSTEM_HPP
#define JARNGREIPR_WRITE_SYSTEM_HPP
#include <extlib/toml/toml.hpp>
#include <iomanip>

namespace jarngreipr
{

template<typename charT, typename traits, typename Comment,
         template<typename...> class Map, template<typename...> class Array>
std::basic_ostream<charT, traits>&
write_system(std::basic_ostream<charT, traits>& os,
             const toml::basic_value<Comment, Map, Array>& sys)
{
    if(!sys.comments().empty())
    {
        // TODO later (toml11-v3.0.0), this can be refactored into
        //      `os << sys.comments();`
        for(const auto& c : sys.comments())
        {
            os << '#' << c << '\n';
        }
    }
    os << "[[systems]]\n";

    if(toml::find(sys, "boundary_shape").as_table().empty())
    {
        os << "boundary_shape = {} # unlimited\n";
        std::cerr << "boundary is unlimited" << std::endl;
    }
    else
    {
        for(const auto& kv : toml::find(sys, "boundary_shape").as_table())
        {
            assert(kv.second.comments().empty());
            // TODO later (toml11 v3.0.0), use toml::format_key(kv.first) here
            os << "boundary_shape." << kv.first << " = " << kv.second << '\n';
        }
        std::cerr << "boundary is periodic" << std::endl;
    }

    if(sys.as_table().count("attributes") == 1)
    {
        for(const auto& kv : toml::find(sys, "attributes").as_table())
        {
            assert(kv.second.comments().empty());
            // TODO later (toml11 v3.0.0), use toml::format_key(kv.first) here
            os << "attributes." << kv.first << " = " << kv.second << '\n';
        }
    }

    os << "particles = [\n";
    for(const auto& particle : toml::find(sys, "particles").as_array())
    {
        const auto m = toml::find<double>(particle, "mass");
        const auto p = toml::find<std::array<double, 3>>(particle, "position");
        const auto v = toml::find<std::array<double, 3>>(particle, "velocity");
        const auto n = toml::find_or<std::string>(particle, "name",  "CA");
        const auto g = toml::find_or<std::string>(particle, "group", "none");

        os << "{m=" << std::setw(7) << std::fixed << std::right << m;
        os << ",pos=["
           << std::setw(9) << std::fixed << std::right << p[0] << ','
           << std::setw(9) << std::fixed << std::right << p[1] << ','
           << std::setw(9) << std::fixed << std::right << p[2];
        os << "],vel=["
           << std::setw(9) << std::fixed << std::right << v[0] << ','
           << std::setw(9) << std::fixed << std::right << v[1] << ','
           << std::setw(9) << std::fixed << std::right << v[2];
        os << "], name = \"" << n << "\", group = \"" << g << "\"},\n";
    }
    os << "]\n";
    return os;
}

} // jarngreipr
#endif //JARNGREIPR_WRITE_PARTICLES_HPP
