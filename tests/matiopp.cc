// Copyright 2019 Florian Eich <florian@bmc-labs.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "catch.h"

#include "mat.h"


TEST_CASE("functionality of mat file container", "[mat_file_container]") {
  SECTION("constructors") {
    REQUIRE_NOTHROW(bmc::mat{"TESTDATA-sane.mat"});
    REQUIRE_NOTHROW(
      bmc::mat{"TESTDATA-sane.mat",
               R"((L\d+_|^)(?!Team_)Pmanifold|AnaThrott|AnaTPS|RPM)"});

    REQUIRE_THROWS_WITH(bmc::mat{"nonexistent.mat"}, "file does not exist");

    // extra parentheses around constructor call in the following statement
    // because catch2 macro fails without
    REQUIRE_THROWS_WITH((bmc::mat{"TESTDATA-sane.mat", R"(nonexistent)"}),
                        "no channels matched");

    REQUIRE_THROWS_WITH(bmc::mat{"TESTDATA-cardinality_exception.mat"},
                        "columns differ in size");
  }

  bmc::mat matfile{"TESTDATA-sane.mat",
                   R"((?<!team_)pmanifold|anathrott|rpm)"};

  SECTION("data access") {
    auto                     keys = matfile.keys();
    std::vector<std::string> testkeys{
      "L001_Team_AnaThrott", "L001_Pmanifold", "L001_Team_RPM"};
    CHECK(keys == testkeys);
    CHECK(keys == matfile.channels());

    CHECK(matfile.at("L001_Pmanifold") == matfile["L001_Pmanifold"]);
    CHECK(matfile.at("L001_Pmanifold") == matfile.channel("L001_Pmanifold"));

    CHECK(matfile.find("pmanif")->second == matfile.at("L001_Pmanifold"));
  }

  SECTION("iterators") {
    CHECK(matfile.begin()->second == matfile.at("L001_Team_AnaThrott"));
    CHECK(matfile.cbegin()->second == matfile.at("L001_Team_AnaThrott"));
    CHECK(matfile.end() == matfile.find("nonexistent"));
    CHECK(matfile.cend() == matfile.find("nonexistent"));
  }

  SECTION("sizes") {
    CHECK_FALSE(matfile.empty());
    CHECK(matfile.size() == 14359);
    CHECK(matfile.number_of_channels() == 3);
  }

  SECTION("equality") {
    bmc::mat rhs{"TESTDATA-sane.mat", R"((?<!team_)pmanifold|anathrott|rpm)"};
    CHECK(matfile == rhs);
    CHECK_FALSE(matfile != rhs);
  }
}
