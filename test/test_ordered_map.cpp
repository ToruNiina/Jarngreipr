#define BOOST_TEST_MODULE "test_ordered_map"
#include <boost/test/included/unit_test.hpp>
#include <jarngreipr/util/ordered_map.hpp>

BOOST_AUTO_TEST_CASE(test_ordered_map_construction)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map;

        BOOST_TEST(map.empty());
        BOOST_TEST(map.size() == 0u);
    }
    {
        std::vector<std::pair<std::string, int>> v = {
            {"answer", 42}, {"reason", 54}
        };
        ordered_map<std::string, int> map(v.begin(), v.end());

        BOOST_TEST(!map.empty());
        BOOST_TEST(map.at("answer") == 42);
        BOOST_TEST(map.at("reason") == 54);
        BOOST_TEST(map["answer"]    == 42);
        BOOST_TEST(map["reason"]    == 54);
        BOOST_TEST_REQUIRE(map.size() == 2u);
        BOOST_TEST(map.begin()->second            == 42);
        BOOST_TEST(std::next(map.begin())->second == 54);
    }

    {
        std::vector<std::pair<std::string, int>> v = {
            {"reason", 54}, {"answer", 42}
        };
        ordered_map<std::string, int> map(v.begin(), v.end());

        BOOST_TEST(!map.empty());
        BOOST_TEST(map.at("answer") == 42);
        BOOST_TEST(map.at("reason") == 54);
        BOOST_TEST(map["answer"]    == 42);
        BOOST_TEST(map["reason"]    == 54);
        BOOST_TEST_REQUIRE(map.size() == 2u);
        BOOST_TEST(map.begin()->second            == 54);
        BOOST_TEST(std::next(map.begin())->second == 42);
    }

    {
        ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST(!map.empty());
        BOOST_TEST(map.at("answer") == 42);
        BOOST_TEST(map.at("reason") == 54);
        BOOST_TEST(map["answer"]    == 42);
        BOOST_TEST(map["reason"]    == 54);
        BOOST_TEST_REQUIRE(map.size() == 2u);
        BOOST_TEST(map.begin()->second            == 42);
        BOOST_TEST(std::next(map.begin())->second == 54);
    }
    {
        ordered_map<std::string, int> map{
            {"reason", 54}, {"answer", 42}
        };

        BOOST_TEST(!map.empty());
        BOOST_TEST(map.at("answer") == 42);
        BOOST_TEST(map.at("reason") == 54);
        BOOST_TEST(map["answer"]    == 42);
        BOOST_TEST(map["reason"]    == 54);
        BOOST_TEST_REQUIRE(map.size() == 2u);
        BOOST_TEST(map.begin()->second            == 54);
        BOOST_TEST(std::next(map.begin())->second == 42);
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_access_operator)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map;
        BOOST_TEST_REQUIRE(map.empty());

        BOOST_CHECK_THROW(map.at("answer"), std::out_of_range);
        BOOST_CHECK_THROW(map.at("reason"), std::out_of_range);

        map["answer"] = 42;
        map["reason"] = 54;

        BOOST_TEST(!map.empty());
        BOOST_TEST(map.at("answer") == 42);
        BOOST_TEST(map.at("reason") == 54);
        BOOST_TEST(map["answer"]    == 42);
        BOOST_TEST(map["reason"]    == 54);
        BOOST_TEST_REQUIRE(map.size() == 2u);
        BOOST_TEST(map.begin()->second            == 42);
        BOOST_TEST(std::next(map.begin())->second == 54);
    }
}


BOOST_AUTO_TEST_CASE(test_ordered_map_comparison)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map1;
        ordered_map<std::string, int> map2;

        BOOST_TEST(map1.empty());
        BOOST_TEST(map1.size() == 0u);
        BOOST_TEST(map2.empty());
        BOOST_TEST(map2.size() == 0u);

        const bool eq = map1 == map2;
        const bool ne = map1 != map2;
        BOOST_TEST(eq);
        BOOST_TEST(!ne);
    }
    {
        ordered_map<std::string, int> map1{
            {"answer", 42}, {"reason", 54}
        };
        ordered_map<std::string, int> map2{
            {"reason", 54}, {"answer", 42}
        };

        BOOST_TEST(map1.size() == 2u);
        BOOST_TEST(map2.size() == 2u);

        const bool eq = map1 == map2;
        const bool ne = map1 != map2;
        BOOST_TEST(!eq);
        BOOST_TEST(ne);
    }
    {
        ordered_map<std::string, int> map1{
            {"reason", 54}, {"answer", 42}
        };
        ordered_map<std::string, int> map2{
            {"reason", 54}, {"answer", 42}
        };

        BOOST_TEST(map1.size() == 2u);
        BOOST_TEST(map2.size() == 2u);

        const bool eq = map1 == map2;
        const bool ne = map1 != map2;
        BOOST_TEST(eq);
        BOOST_TEST(!ne);
    }
    {
        ordered_map<std::string, int> map1{
            {"answer", 42}, {"reason", 54}
        };
        ordered_map<std::string, int> map2{
            {"answer", 42}
        };

        BOOST_TEST(map1.size() == 2u);
        BOOST_TEST(map2.size() == 1u);

        const bool eq = map1 == map2;
        const bool ne = map1 != map2;
        BOOST_TEST(!eq);
        BOOST_TEST(ne);
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_clear)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST(!map.empty());

        map.clear();

        BOOST_TEST(map.empty());
        BOOST_TEST(map.size() == 0u);
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_count_contains)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST(map.count("answer") == 1u);
        BOOST_TEST(map.contains("answer"));
        BOOST_TEST(map.count("reason") == 1u);
        BOOST_TEST(map.contains("reason"));

        BOOST_TEST(map.count("life") == 0u);
        BOOST_TEST(map.count("universe") == 0u);
        BOOST_TEST(map.count("everything") == 0u);
        BOOST_TEST(!map.contains("life"));
        BOOST_TEST(!map.contains("universe"));
        BOOST_TEST(!map.contains("everything"));
    }
    {
        const ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST(map.count("answer") == 1u);
        BOOST_TEST(map.contains("answer"));
        BOOST_TEST(map.count("reason") == 1u);
        BOOST_TEST(map.contains("reason"));

        BOOST_TEST(map.count("life") == 0u);
        BOOST_TEST(map.count("universe") == 0u);
        BOOST_TEST(map.count("everything") == 0u);
        BOOST_TEST(!map.contains("life"));
        BOOST_TEST(!map.contains("universe"));
        BOOST_TEST(!map.contains("everything"));
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_find)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));
    }

    {
        const ordered_map<std::string, int> map{
            {"answer", 42}, {"reason", 54}
        };

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_insert)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map;

        BOOST_TEST_REQUIRE(map.empty());

        const auto a = map.insert(std::make_pair("answer", 42));
        BOOST_TEST((a.first != map.end()));
        BOOST_TEST(a.second);

        const auto reason = std::make_pair("reason", 54);
        const auto r = map.insert(reason);
        BOOST_TEST((r.first != map.end()));
        BOOST_TEST(r.second);

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));

        const auto found = map.find("answer");
        const auto a_ = map.insert(std::make_pair("answer", 42));
        BOOST_TEST((a_.first == found));
        BOOST_TEST(!a_.second);
    }

    {
        ordered_map<std::string, int> map;

        BOOST_TEST_REQUIRE(map.empty());

        std::vector<std::pair<std::string, int>> v = {
            {"answer", 42}, {"reason", 54}
        };
        map.insert(v.begin(), v.end());

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));

        const auto found = map.find("answer");
        const auto a = map.insert(std::make_pair("answer", 42));
        BOOST_TEST((a.first == found));
        BOOST_TEST(!a.second);
    }

    {
        ordered_map<std::string, int> map;

        BOOST_TEST_REQUIRE(map.empty());

        map.insert({ {"answer", 42}, {"reason", 54} });

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));

        const auto found = map.find("answer");
        const auto a = map.insert(std::make_pair("answer", 42));
        BOOST_TEST((a.first == found));
        BOOST_TEST(!a.second);
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_emplace)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map;

        BOOST_TEST_REQUIRE(map.empty());

        const auto a = map.emplace("answer", 42);
        BOOST_TEST((a.first != map.end()));
        BOOST_TEST(a.second);

        const auto reason_k = "reason";
        const auto reason_v = 54;
        const auto r = map.emplace(reason_k, reason_v);
        BOOST_TEST((r.first != map.end()));
        BOOST_TEST(r.second);

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));

        const auto found = map.find("answer");
        const auto a_ = map.emplace("answer", 42);
        BOOST_TEST((a_.first == found));
        BOOST_TEST(!a_.second);
    }

    {
        ordered_map<std::string, int> map;

        BOOST_TEST_REQUIRE(map.empty());

        const auto a = map.try_emplace("answer", 42);
        BOOST_TEST((a.first != map.end()));
        BOOST_TEST(a.second);

        const auto reason_k = "reason";
        const auto reason_v = 54;
        const auto r = map.try_emplace(reason_k, reason_v);
        BOOST_TEST((r.first != map.end()));
        BOOST_TEST(r.second);

        BOOST_TEST_REQUIRE((map.find("answer") != map.end()));
        BOOST_TEST(map.find("answer")->first  == "answer");
        BOOST_TEST(map.find("answer")->second == 42);

        BOOST_TEST_REQUIRE((map.find("reason") != map.end()));
        BOOST_TEST(map.find("reason")->first  == "reason");
        BOOST_TEST(map.find("reason")->second == 54);

        BOOST_TEST((map.find("ultimate") == map.end()));

        const auto found = map.find("answer");
        const auto a_ = map.try_emplace("answer", 42);
        BOOST_TEST((a_.first == found));
        BOOST_TEST(!a_.second);
    }
}

BOOST_AUTO_TEST_CASE(test_ordered_map_erase)
{
    using jarngreipr::ordered_map;
    {
        ordered_map<std::string, int> map{ {"answer", 42}, {"reason", 54} };

        BOOST_TEST_REQUIRE(map.size() == 2u);
        {
            const auto erased = map.erase("answer");
            BOOST_TEST(erased == 1u);
            BOOST_TEST(map.size() == 1u);
        }
        {
            const auto erased = map.erase("life");
            BOOST_TEST(erased == 0u);
            BOOST_TEST(map.size() == 1u);
        }
    }
    {
        ordered_map<std::string, int> map{ {"answer", 42}, {"reason", 54} };

        BOOST_TEST_REQUIRE(map.size() == 2u);
        {
            map.erase(map.find("answer"));
            BOOST_TEST(map.size() == 1u);
        }
    }

    {
        ordered_map<std::string, int> map{ {"answer", 42}, {"reason", 54} };

        BOOST_TEST_REQUIRE(map.size() == 2u);
        {
            map.erase(map.begin());
            BOOST_TEST(map.size() == 1u);
            BOOST_TEST(map.at("reason") == 54);
        }
    }
    {
        ordered_map<std::string, int> map{ {"answer", 42}, {"reason", 54} };

        BOOST_TEST_REQUIRE(map.size() == 2u);
        {
            map.erase(map.begin(), map.end());
            BOOST_TEST(map.size() == 0u);
            BOOST_TEST(map.empty());
        }
    }
}
