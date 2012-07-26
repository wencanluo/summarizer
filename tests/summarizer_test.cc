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

#include "summarizer.h"

#include "gtest/gtest.h"

#include "xml_parser.h"
#include "file.h"

namespace topicsum {

class SummarizerTest : public ::testing::Test {
 protected:
  SummarizerTest() {
    File::ReadFileToStringOrDie(
        "tests/testdata/article_14178836.xml",
        &article_14178836_);
    File::ReadFileToStringOrDie(
        "tests/testdata/article_14319162.xml",
        &article_14319162_);
  }

  string article_14178836_;
  string article_14319162_;
};

TEST_F(SummarizerTest, TestArticle1) {
  // Create options for summarizer.
  SummarizerOptions sum_options;

  // Create collection containing only one article.
  vector<string> collection;
  collection.push_back(article_14319162_);

  // Initialize summarizer with training collection and options.
  Summarizer sum;
  sum.Init(collection, sum_options);

  // Set desired summary length.
  SummaryLength* sum_length = sum_options.mutable_summary_length();
  sum_length->set_unit(SummaryLength::SENTENCE);
  sum_length->set_length(2);

  // Sumarize collection with desired options.
  Query query;
  string summary;
  sum.Summarize(collection, sum_options, query, &summary);

  // Compare returned summary.
  EXPECT_EQ(
      "Windows 7 Leaked To The Internet Microsoft's next operating system is "
      "available from torrent sites. ", summary);
}

TEST_F(SummarizerTest, TestArticle2) {
  // Create options for summarizer.
  SummarizerOptions sum_options;

  // Create collection containing only one article.
  vector<string> collection;
  collection.push_back(article_14178836_);

  // Set desired summary length.
  SummaryLength* sum_length = sum_options.mutable_summary_length();
  sum_length->set_unit(SummaryLength::TOKEN);
  sum_length->set_length(70);

  // Initialize summarizer with training collection and options.
  Summarizer sum;
  sum.Init(collection, sum_options);

  // Sumarize collection with desired options.
  Query query;
  string summary;
  sum.Summarize(collection, sum_options, query, &summary);

  // Compare returned summary.
  EXPECT_EQ(
      "Israeli strikes hit Gaza, at least 230 killed GAZA CITY, Gaza Strip "
      "(AP) -- Israeli warplanes sent more than 100 tons of bombs crashing "
      "down on key security installations in Hamas-ruled Gaza on Saturday, "
      "killing more than 200 Palestinians at the launch of an open-ended "
      "campaign mean to stop rocket and mortar attacks that have traumatized "
      "southern Israel. ", summary);
}

}  // namespace topicsum

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
