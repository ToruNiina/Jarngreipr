#define BOOST_TEST_MODULE "test_parse_range"
#include <boost/test/included/unit_test.hpp>
#include <jarngreipr/util/parse_range.hpp>

std::string operator"" _str(const char* c, std::size_t l)
{
    return std::string(c);
}

BOOST_AUTO_TEST_CASE(test_parse_range_simple)
{
    {
        const auto range = jarngreipr::parse_range("[1,10]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[1,  10]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[1, 10)"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(1, 10)"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(1, 10]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }

    {
        const auto range = jarngreipr::parse_range("[ 1,10 ]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[  1,  10 ]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[ 1, 10 )"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( 1, 10 )"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( 1, 10 ]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_parse_range_signs)
{
    {
        const auto range = jarngreipr::parse_range("[+1, +10]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[+1, +10)"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(+1, +10)"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(+1, +10]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[-10, -1]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i-10); // from -10 to -1
        }
    }
    {
        const auto range = jarngreipr::parse_range("[-10, -1)"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i-10);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(-10, -1)"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i-9);
        }
    }
    {
        const auto range = jarngreipr::parse_range("(-10, -1]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i-9);
        }
    }


    {
        const auto range = jarngreipr::parse_range("[ +1, +10 ]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[ +1, +10 )"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+1);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( +1, +10 )"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( +1, +10 ]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i+2);
        }
    }
    {
        const auto range = jarngreipr::parse_range("[ -10, -1 ]"_str);
        BOOST_TEST(range.size() == 10u);
        for(std::int64_t i=0; i<10; ++i)
        {
            BOOST_TEST(range.at(i) == i-10); // from -10 to -1
        }
    }
    {
        const auto range = jarngreipr::parse_range("[ -10, -1 )"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i-10);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( -10, -1 )"_str);
        BOOST_TEST(range.size() == 8u);
        for(std::int64_t i=0; i<8; ++i)
        {
            BOOST_TEST(range.at(i) == i-9);
        }
    }
    {
        const auto range = jarngreipr::parse_range("( -10, -1 ]"_str);
        BOOST_TEST(range.size() == 9u);
        for(std::int64_t i=0; i<9; ++i)
        {
            BOOST_TEST(range.at(i) == i-9);
        }
    }
}

BOOST_AUTO_TEST_CASE(test_parse_range_chains)
{
    {
        const auto range = jarngreipr::parse_chain_range("A:B"_str);
        BOOST_TEST(range.size() == 2u);
        BOOST_TEST(range.at(0) == "A");
        BOOST_TEST(range.at(1) == "B");
    }
    {
        const auto range = jarngreipr::parse_chain_range("C:F"_str);
        BOOST_TEST(range.size() == 4u);
        BOOST_TEST(range.at(0) == "C");
        BOOST_TEST(range.at(1) == "D");
        BOOST_TEST(range.at(2) == "E");
        BOOST_TEST(range.at(3) == "F");
    }
}
