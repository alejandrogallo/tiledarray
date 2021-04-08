/*
* This file is a part of TiledArray.
* Copyright (C) 2021 Virginia Tech
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "tiledarray.h"
#include "TiledArray/util/index.h"
#include "unit_test_config.h"
#include <vector>

BOOST_AUTO_TEST_SUITE(ta_index)

using namespace TiledArray;

BOOST_AUTO_TEST_CASE(index) {

        // Test == operator
	BOOST_CHECK(Index("i") == Index{{"i"}});
	BOOST_CHECK(!(Index("i") == Index{{"j"}}));

        //bool conversion testing
	BOOST_CHECK(Index("i"));
	BOOST_CHECK(!(Index()));

        //Iterator testing
        struct iterator_test_tuple {
          std::string ref_str;
          std::vector<std::string> iter;
        };
        std::vector<iterator_test_tuple> iterator_tests{
            {"i,j,k,l", {"i", "j", "k", "l"}},
            { "alpha,omega", {"alpha", "omega"}},
            {"i", {"i"}},
            {"", {}}
        };
        for (const auto iterator_test : iterator_tests) {
          Index index_from_str(iterator_test.ref_str);
          auto it = iterator_test.iter.begin();
          BOOST_CHECK(std::equal(index_from_str.begin(), index_from_str.end(), it)); // check iterator
          BOOST_CHECK( index_from_str == iterator_test.ref_str); // check string conversion
          BOOST_CHECK(index_from_str.size() == iterator_test.iter.size()); // check size operator
          for (size_t i = 0; i != index_from_str.size(); ++i) {
            BOOST_CHECK(index_from_str[i] == iterator_test.iter[i]); // check access operator
          }
        }

        struct test_tuple {
          std::string lhs, rhs;
          std::string and_result, or_result, plus_result, minus_result, xor_result;
        };
        std::vector<test_tuple> operator_tests {
            {"i,j", "j,l", "j", "i,j,l", "i,j,j,l", "i", "i,l"},
            {"i,j", "k,l", "", "i,j,k,l", "i,j,k,l", "i,j", "i,j,k,l"},
            {"i,j", "j,i", "i,j", "i,j", "i,j,j,i", "", ""},
            {"i", "i", "i", "i", "i,i", "", ""},
            {"alpha,beta,omega", "", "", "alpha,beta,omega", "alpha,beta,omega", "alpha,beta,omega", "alpha,beta,omega"},
            {"", "gamma,sigma,epsilon", "", "gamma,sigma,epsilon", "gamma,sigma,epsilon", "", "gamma,sigma,epsilon"},
            {"", "", "", "", "", "", ""},
            {"i", "j", "", "i,j", "i,j", "i", "i,j"}
        };
        for (const auto test : operator_tests) {
          Index lhs(test.lhs);
          Index rhs = test.rhs;
          BOOST_CHECK((lhs & rhs) == test.and_result);
          BOOST_CHECK((lhs | rhs) == test.or_result);
          BOOST_CHECK((lhs + rhs) == test.plus_result);
          BOOST_CHECK((lhs - rhs) == test.minus_result);
          BOOST_CHECK((lhs ^ rhs) == test.xor_result);
        }

        struct indexMap_test_tuple {
          IndexMap<Index, std::vector<std::string>> lhs;
          IndexMap<Index, std::vector<std::string>> rhs;
          IndexMap<Index, std::vector<std::string>> or_result;
        };
        std::vector<indexMap_test_tuple> indexMap_tests {
            {IndexMap<Index, std::vector<std::string>>(Index("i,j,k"),std::vector<std::string> {"eye", "jay", "kay"}), IndexMap<Index, std::vector<std::string>>("a,b,c",std::vector<std::string> {"aye", "bee", "see"}),
                IndexMap<Index, std::vector<std::string>>(Index("i,j,k,a,b,c"), std::vector<std::string>{"eye", "jay", "kay", "aye", "bee", "see"})}
        };
        for (const auto test : indexMap_tests) {
          IndexMap lhs(test.lhs);
          IndexMap rhs(test.rhs);
          BOOST_CHECK((lhs | rhs) == test.or_result);
        }

        IndexMap<std::string, std::string> m(Index("i,j,k"),std::vector<std::string> {"eye", "jay", "kay"});

        IndexMap<std::string, std::string> p("a,b,c",std::vector<std::string> {"aye", "bee", "see"});

        BOOST_CHECK((m | p) == (IndexMap<std::string, std::string>(Index("i,j,k,a,b,c"), std::vector<std::string>{"eye", "jay", "kay", "aye", "bee", "see"})));

        //const auto shuffle1 = IndexShuffle(Index({"i", "j", "k", "l"}), Index({"i", "k", "j", "l"}));
        //auto shuffled = shuffle1()"0,2,1,3";

	//BOOST_CHECK(Shuffle(Index({"i", "j", "k", "l"}), Index({"n", "i", "j"})));
	//BOOST_CHECK(Shuffle(Index({"i", "j", "k", "l"}), Index({"i", "k", "j", "l"}))("0,2,1,3") == Shuffle(Index({"a", "b", "c", "d"}), Index({"a", "c", "b", "d"})));
	//BOOST_CHECK(Shuffle(Index({"i", "j", "k", "l"}), Index({"i", "k", "j", "l"}))(std::vector<std::string>{"0", "2", "1", "3"}) == std::vector<std::string>{"0", "1", "2", "3"}); // not working

}

BOOST_AUTO_TEST_SUITE_END()
