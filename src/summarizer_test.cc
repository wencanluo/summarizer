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

#include "summarizer/summarizer.h"

#include "gtest/gtest.h"
#include "summarizer/file.h"
#include "summarizer/xml_parser.h"

namespace topicsum {

class SummarizerTest : public ::testing::Test {
 protected:
  SummarizerTest() {
    File::ReadFileToStringOrDie(
        DATADIR "testdata/article_14319162.xml",
        &article_14319162_);

    TopicSumOptions* topicsum_options =
        summarizer_options_.mutable_topicsum_options();
    topicsum_options->set_lambda("0.1,1,1");
    topicsum_options->set_gamma("1,5,10");

    KLSumOptions* klsumoptions = summarizer_options_.mutable_klsum_options();
    klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
    klsumoptions->set_redundancy_removal(false);

    GibbsSamplingOptions* gibbsoptions =
        topicsum_options->mutable_gibbs_sampling_options();
    gibbsoptions->set_iterations(500);
    gibbsoptions->set_burnin(0);
    gibbsoptions->set_lag(100);
    gibbsoptions->set_track_likelihood(true);

    Article *article = summarizer_options_.add_article();
    article->set_content(article_14319162_);
  }

  string article_14319162_;

  SummarizerOptions summarizer_options_;
};

TEST_F(SummarizerTest, TestArticle1) {
  SummaryOptions sum_options;

  SummaryLength* sum_length = sum_options.mutable_length();
  sum_length->set_unit(SummaryLength::SENTENCE);
  sum_length->set_length(2);

  Summarizer sum;
  sum.Init(summarizer_options_);

  string summary;
  sum.Summarize(sum_options, &summary);

  EXPECT_EQ(
      "Windows 7 Leaked To The Internet Microsoft's next operating system is "
      "available from torrent sites. ", summary);
}

}  // namespace topicsum

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
