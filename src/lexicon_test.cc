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

#include "summarizer/lexicon.h"

#include "gtest/gtest.h"
#include <string>
#include <vector>

#include "summarizer/logging.h"

namespace topicsum {

TEST(Lexicon, TestAll) {
  Lexicon lexicon;
  lexicon.add_token("toto");
  lexicon.add_token("le");
  lexicon.add_token("heros");

  EXPECT_EQ("heros", lexicon.id2token(2));
  EXPECT_EQ(1, lexicon.token2id("le"));

  vector<string> additional_tokens;
  additional_tokens.push_back("la");
  additional_tokens.push_back("vie");
  additional_tokens.push_back("de");
  additional_tokens.push_back("toto");

  lexicon.add_tokens(additional_tokens);
  EXPECT_EQ("vie", lexicon.id2token(4));
  EXPECT_EQ(0, lexicon.token2id("toto"));
  EXPECT_EQ(6, lexicon.size());
}

}  // namespace topicsum

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
