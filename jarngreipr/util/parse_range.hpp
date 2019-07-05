#ifndef JARNGREIPR_PARSE_RANGE_HPP
#define JARNGREIPR_PARSE_RANGE_HPP
#include <jarngreipr/io/log.hpp>
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
std::vector<integerT> parse_range(std::string str)
{
    while(str.front() == ' ') {str.erase(str.begin());}
    while(str.back()  == ' ') {str.pop_back();}

    std::regex syntax(R"((\(|\[)((\+|-)?\d+),\s*((\+|-)?\d+)(\)|\]))");
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

} // jarngriepr
#endif// JARNGREIPR_PARSE_RANGE_HPP
