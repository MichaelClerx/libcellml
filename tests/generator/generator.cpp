/*
Copyright libCellML Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "gtest/gtest.h"

#include <libcellml>

TEST(Generator, emptyModel) {
    const std::string e = "";

    libcellml::Generator generator;
    libcellml::ModelPtr model = std::make_shared<libcellml::Model>();

    const std::string a = generator.generateCode(model);

    EXPECT_EQ(e, a);

    EXPECT_EQ(size_t(0), generator.constantCount());
    EXPECT_EQ(size_t(0), generator.stateCount());
    EXPECT_EQ(size_t(0), generator.rateCount());
    EXPECT_EQ(size_t(0), generator.algebraicCount());
}
