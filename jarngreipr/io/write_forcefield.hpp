#ifndef JARNGREIPR_WRITE_FORCEFIELD_HPP
#define JARNGREIPR_WRITE_FORCEFIELD_HPP
#include <extlib/toml/toml.hpp>
#include <jarngreipr/io/write_error.hpp>
#include <algorithm>
#include <ostream>

namespace jarngreipr
{

//
// Without considering the readability, this function is not needed because
// toml11 has a serializer. But to prettify the output, some sorting and
// extra formatting stuff is needed.
//
template<typename charT, typename traits, typename Comment,
         template<typename...> class Map, template<typename...> class Array>
std::basic_ostream<charT, traits>&
write_local_forcefield(std::basic_ostream<charT, traits>& os,
                       const toml::basic_value<Comment, Map, Array>& ff)
{
    if(!ff.comment().empty())
    {
        // TODO later (toml11-v3.0.0), this can be refactored into
        //      `os << local.comment();`
        for(const auto& c : ff.comment())
        {
            os << '#' << c << '\n';
        }
    }
    os << "[[forcefields.local]]\n";

    // ========================================================================
    // output miscellaneous stuff in the predefined order

    assert(toml::find(ff, "interaction").comment().empty());
    os << "interaction = " << toml::find(ff, "interaction") << '\n';
    if(ff.as_table().count("potential") == 1)
    {
        assert(toml::find(ff, "potential").comment().empty());
        os << "potential   = " << toml::find(ff, "potential") << '\n';
    }
    if(ff.as_table().count("topology") == 1)
    {
        assert(toml::find(ff, "topology").comment().empty());
        os << "topology    = " << toml::find(ff, "topology") << '\n';
    }

    if(ff.as_table().count("env") == 1)
    {
        for(const auto& kv : toml::find(ff, "env").as_table())
        {
            assert(kv.second.comment().empty());
            // TODO later (toml11 v3.0.0), use toml::format_key(kv.first) here
            os << "env." << kv.first << " = " << kv.second << '\n';
        }
    }

    // output other potential-specific paramters, if exists
    for(const auto& kv : ff.as_table())
    {
        const auto& key = kv.first;
        if(key == "potential" || key == "interaction" || key == "topology" ||
           key == "env"       || key == "parameters")
        {
            // these keys has special meaning, so output in a different way
            continue;
        }
        assert(kv.second.comment().empty());
        os << key << " = " << kv.second;
    }

    // ========================================================================
    // output `parameters` field in an array-of-inline-tables way.

    // ------------------------------------------------------------------------
    // find the maximum index and calculate the max width to write them

    std::size_t max_index = 0;
    for(const auto& p : toml::find(ff, "parameters").as_array())
    {
        if(p.as_table().count("indices") == 0)
        {
            write_error(os, "`parameters` does not has `indices` field");
        }
        const auto idxs = toml::find<std::vector<std::size_t>>(p, "indices");
        max_index = std::max(max_index, *std::max_element(idxs.begin(), idxs.end()));
    }
    const auto idx_width = std::to_string(max_index).size();

    // ------------------------------------------------------------------------
    // collect and sort keys in a table it to output them in a fixed order

    std::vector<std::string> keys;
    for(const auto& kv : toml::find(ff, "parameters").as_array().front())
    {
        if(kv.first != "indices") {keys.push_back(kv.first);}
    }
    std::sort(keys.begin(), keys.end());

    os << "parameters = [\n";
    for(const auto& p : toml::find(ff, "parameters").as_array())
    {
        // write comment if exists
        if(!p.comment().empty())
        {
            // TODO later (toml11-v3.0.0), this can be refactored into
            //      `os << local.comment();`
            for(const auto& c : p.comment())
            {
                os << '#' << c << '\n';
            }
        }

        // write indices first
        os << "{indices = [";
        {
            const auto idxs = toml::find<std::vector<std::size_t>>(p, "indices");
            for(auto iter = idxs.begin(); iter != idxs.end(); ++iter)
            {
                if(iter != idxs.begin()) {os << ',';}
                os << std::setw(idx_width) << *iter;
            }
        }
        os << ']';

        // write other keys in the fixed order
        for(const auto& key : keys)
        {
            assert(toml::find(p, key).comment().empty());
            os << ", " << key << " = " << toml::find(p, key);
        }
        os << "},\n";
    }
    os << "]\n";
    return os;
}

template<typename charT, typename traits, typename Comment,
         template<typename...> class Map, template<typename...> class Array>
std::basic_ostream<charT, traits>&
write_global_forcefield(std::basic_ostream<charT, traits>& os,
                        const toml::basic_value<Comment, Map, Array>& ff)
{
    if(!ff.comment().empty())
    {
        for(const auto& c : ff.comment())
        {
            os << '#' << c << '\n';
        }
    }
    os << "[[forcefields.global]]\n";

    // ========================================================================
    // output interaction, potential, ignore, spatial_partition, and env first

    assert(toml::find(ff, "interaction").comment().empty());
    os << "interaction = " << toml::find(ff, "interaction") << '\n';

    assert(toml::find(ff, "potential").comment().empty());
    os << "potential   = " << toml::find(ff, "potential") << '\n';

    for(const auto& kv : toml::find(ff, "ignore").as_table())
    {
        assert(kv.second.comment().empty());
        // TODO toml::format_key(kv.first)
        os << "ignore." << kv.first << " = " << kv.second << '\n';
    }

    for(const auto& kv : toml::find(ff, "spatial_partition").as_table())
    {
        assert(kv.second.comment().empty());
        // TODO toml::format_key(kv.first)
        os << "spatial_partition." << kv.first << " = " << kv.second << '\n';
    }

    if(ff.as_table().count("env") == 1)
    {
        for(const auto& kv : toml::find(ff, "env").as_table())
        {
            assert(kv.second.comment().empty());
            // TODO toml::format_key(kv.first)
            os << "env." << kv.first << " = " << kv.second << '\n';
        }
    }

    // ------------------------------------------------------------------------
    // output other potential-specific paramters, if exists
    for(const auto& kv : ff.as_table())
    {
        const auto& key = kv.first;
        if(key == "potential" || key == "interaction" || key == "ignore" ||
           key == "spatial_partition" || key == "env" || key == "parameters")
        {
            // these keys has special meaning, so output in a different way
            continue;
        }
        assert(kv.second.comment().empty());
        // TODO toml::format_key(kv.first)
        os << key << " = " << kv.second;
    }

    // ========================================================================
    // output parameters = [{...}, ...]

    // ------------------------------------------------------------------------
    // find the maximum index and calculate the max width to write them

    std::size_t max_index = 0;
    for(const auto& p : toml::find(ff, "parameters").as_array())
    {
        if(p.as_table().count("index") == 0)
        {
            write_error(os, "`parameters` does not has `index` field");
        }
        max_index = std::max(max_index, toml::find<std::size_t>(p, "index"));
    }
    const auto idx_width = std::to_string(max_index).size();

    // ------------------------------------------------------------------------
    // collect and sort keys in a table it to output them in a fixed order

    std::vector<std::string> keys;
    for(const auto& kv : toml::find(ff, "parameters").as_array().front())
    {
        if(kv.first != "indices") {keys.push_back(kv.first);}
    }
    std::sort(keys.begin(), keys.end());

    // ------------------------------------------------------------------------
    // output parameters

    os << "parameters = [\n";
    for(const auto& p : toml::find(ff, "parameters").as_array())
    {
        os << "{index = "
           << std::setw(idx_width) << toml::find<std::size_t>(p, "index");

        for(const auto& key : keys)
        {
            assert(toml::find(p, key).comments().empty());
            os << ", " << key << " = " << toml::find(p, key);
        }
        os << "},\n";
    }
    os << "]\n";
    return os;
}

template<typename charT, typename traits, typename Comment,
         template<typename...> class Map, template<typename...> class Array>
std::basic_ostream<charT, traits>&
write_forcefield(std::basic_ostream<charT, traits>& os,
                 const toml::basic_value<Comment, Map, Array>& ff)
{
    os << "[[forcefields]]\n";
    if(ff.as_table().count("local") == 1)
    {
        for(const auto& local : ff.as_table().at("local").as_array())
        {
            write_local_forcefield(os, local);
        }
    }
    if(ff.as_table().count("global") == 1)
    {
        for(const auto& global : ff.as_table().at("global").as_array())
        {
            write_global_forcefield(os, global);
        }
    }
    return os;
}

} // jarngreipr
#endif // JARNGREIPR_WRITE_FORCEFIELD_HPP
