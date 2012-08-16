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

#include "summarizer/xml_parser.h"

#include <stdio.h>

#include "gtest/gtest.h"
#include "summarizer/file.h"

namespace topicsum {

class XmlParserTest : public ::testing::Test {
 protected:
  XmlParserTest() {
    XmlParser xml_parser;
    string file;

    File::ReadFileToStringOrDie(
        DATADIR "testdata/article_14319162.xml", &file);
    xml_parser.ParseDocument(file, &article_14319162_);
  }

  Document article_14178836_;
  Document article_14319162_;
};

TEST_F(XmlParserTest, TestArticle2) {
  EXPECT_EQ("14319162", article_14319162_.id());
  EXPECT_EQ("Windows 7 Leaked To The Internet",
                  article_14319162_.title());
  EXPECT_EQ(17, article_14319162_.sentence_size());
  EXPECT_EQ(396, article_14319162_.num_tokens());
  EXPECT_EQ(6, article_14319162_.sentence(0).token_size());
  EXPECT_EQ(11, article_14319162_.sentence(1).token_size());
  EXPECT_EQ("A", article_14319162_.sentence(2).token(0));
  EXPECT_EQ("DT", article_14319162_.sentence(2).pos_tag(0));
  EXPECT_EQ("trial", article_14319162_.sentence(2).token(1));
  EXPECT_EQ("NN", article_14319162_.sentence(2).pos_tag(1));
}

}  // namespace


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
