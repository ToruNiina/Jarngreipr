#ifndef JARNGREIPR_PARSE_RANGE_HPP
#define JARNGREIPR_PARSE_RANGE_HPP
#include <jarngreipr/util/log.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <regex>
#include <numeric>
#include <cassert>

namespace jarngreipr
{

//
// [1,5] -> {1,2,3,4,5}
// [1,5) -> {1,2,3,4}
// (1,5) -> {2,3,4}
// (1,5] -> {2,3,4,5}
//
template<typename integerT = std::int64_t>
typename std::enable_if<std::is_integral<integerT>::value, std::vector<integerT>
    >::type parse_range(std::string str)
{
    while(str.front() == ' ') {str.erase(str.begin());}
    while(str.back()  == ' ') {str.pop_back();}

    std::regex syntax(R"((\(|\[)\s*((\+|-)?\d+),\s*((\+|-)?\d+)\s*(\)|\]))");
    std::smatch sm;
    if(!std::regex_match(str, sm, syntax))
    {
        log(log_level::error, "syntax error in range \"", str, "\"\n");
        log(log_level::error, "expected like: \"[1,10]\", \"[10, 20)\"\n");
        return {};
    }
    const auto front = static_cast<integerT>(std::stoll(sm.str(2)));
    const auto back  = static_cast<integerT>(std::stoll(sm.str(4)));
    assert(front <= back);

    const bool contains_front = (str.front() == '[');
    const bool contains_back  = (str.back() == ']');

    std::vector<integerT> ints(back - front + 1);
    std::iota(ints.begin(), ints.end(), front);

    if(!contains_front) {ints.erase(ints.begin());}
    if(!contains_back)  {ints.pop_back();}

    return ints;
}

//
// "A:D" -> ["A", "B", "C", "D"]
//
inline std::vector<std::string> parse_chain_range(std::string str)
{
    while(str.front() == ' ') {str.erase(str.begin());}
    while(str.back()  == ' ') {str.pop_back();}

    std::regex syntax(R"((\s*)([A-Z])(\s*:\s*)([A-Z])(\s*))");
    std::smatch sm;
    if(!std::regex_match(str, sm, syntax))
    {
        if(str.size() == 1u && std::isupper(str.at(0)) != 0)
        {
            return std::vector<std::string>{str};
        }
        log(log_level::error, "syntax error in range \"", str, "\"\n");
        log(log_level::error, "expected like: \"A:D\", \"C:F\"\n");
        return {};
    }
    const auto front = sm.str(2);
    const auto back  = sm.str(4);
    assert(front.size() == 1u);
    assert(back .size() == 1u);

    std::vector<std::string> chains;
    //XXX here it assumes that the character encoding is ASCII or UTF-8
    for(char c = front.at(0); c <= back.at(0); ++c)
    {
        chains.emplace_back(1, c);
    }
    return chains;
}

} // jarngriepr
#endif// JARNGREIPR_PARSE_RANGE_HPP
