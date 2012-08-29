// Copyright 2012 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "summarizer/sum.h"

#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "summarizer/logging.h"
#include "summarizer/scoped_ptr.h"

namespace topicsum {

TEST(TestParseStringVectorAsDoubleVector, ParseGoodString) {
  string str_values = "1,2 , 13,  4  ";
  scoped_array<double> values;
  int num_values;
  SummarizerUtils::ParseStringVectorAsDoubleVector(str_values,
                                                   &values,
                                                   &num_values);

  EXPECT_EQ(4, num_values);
  EXPECT_EQ(1, values[0]);
  EXPECT_EQ(2, values[1]);
  EXPECT_EQ(13, values[2]);
  EXPECT_EQ(4, values[3]);
}

TEST(TestParseStringVectorAsDoubleVector, ParseBadString) {
  string str_values = "1,2 , C,  4  ";
  scoped_array<double> values;
  int num_values;
  EXPECT_DEATH(
      SummarizerUtils::ParseStringVectorAsDoubleVector(
          str_values,
          &values,
          &num_values),
          "Non-numeric value ' C' could not be parsed from '1,2 , C,  4  '");
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
