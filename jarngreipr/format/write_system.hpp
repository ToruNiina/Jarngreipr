#ifndef JARNGREIPR_WRITE_SYSTEM_HPP
#define JARNGREIPR_WRITE_SYSTEM_HPP
#include <jarngreipr/format/toml_serializer.hpp>
#include <jarngreipr/format/write_number.hpp>
#include <iomanip>

namespace jarngreipr
{

template<typename charT, typename traits, typename Comment,
         template<typename...> class Map, template<typename...> class Array>
std::basic_ostream<charT, traits>&
write_system(std::basic_ostream<charT, traits>& os,
             const toml::basic_value<Comment, Map, Array>& sys)
{
    using value_type = toml::basic_value<Comment, Map, Array>;

    if(!sys.comments().empty()) {os << sys.comments();}
    os << "[[systems]]\n";

    inline_formatted_serializer<value_type> inline_serializer("%d", "%9.4f");

    if(toml::find(sys, "boundary_shape").as_table().empty())
    {
        os << "boundary_shape = {} # unlimited\n";
    }
    else
    {
        for(const auto& kv : toml::find(sys, "boundary_shape").as_table())
        {
            assert(kv.second.comments().empty());
            os << "boundary_shape." << toml::format_key(kv.first)
               << " = " << toml::visit(inline_serializer, kv.second) << '\n';
        }
    }

    if(sys.as_table().count("attributes") == 1)
    {
        for(const auto& kv : toml::find(sys, "attributes").as_table())
        {
            assert(kv.second.comments().empty());
            os << "attributes." << toml::format_key(kv.first)
               << " = " << toml::visit(inline_serializer, kv.second) << '\n';
        }
    }

    os << "particles = [ # {{{\n";
    for(const auto& particle : toml::find(sys, "particles").as_array())
    {
        const auto m = toml::find<double>(particle, "mass");
        const auto p = toml::find<std::array<double, 3>>(particle, "position");
        const auto n = toml::find(particle, "name");
        const auto g = toml::find(particle, "group");

        if(!particle.comments().empty())
        {
            os << particle.comments();
        }
        os << "{m = "     << format_number("%8.3f", m);
        os << ", pos = "  << format_number("[%9.4f,%9.4f,%9.4f]", p[0], p[1], p[2]);
        os << ", name = " << n << ", group = " << g << "},\n";
    }
    os << "] # }}}\n";
    return os;
}

} // jarngreipr
#endif //JARNGREIPR_WRITE_PARTICLES_HPP
