// Copyright 2012 Google Inc. All Rights Reserved.
// Author: jydelort@google.com (Jean-Yves Delort)
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

#include "summarizer/klsum.h"

#include <fstream>

#include "summarizer/document.h"
#include "gtest/gtest.h"

namespace topicsum {

using std::fstream;
using std::ios;
using std::string;

TEST(KLSum, Summarize) {
  string path = DATADIR "testdata/singledoc";

  // Read collection.
  DocumentCollection collection;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(collection.ParseFromIstream(&in));

  // Create the summarizer.
  scoped_ptr<KLSum> summarizer(new KLSum());
  ASSERT_TRUE(summarizer.get() != NULL);

  // Create the options.
  SummarizerOptions options;

  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
  klsumoptions->set_redundancy_removal(false);

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(50);
  length->set_unit(SummaryLength::TOKEN);
  sum_options.set_generate_debug(true);

  // Summarize first document of the collection.
  Document summary;

  summarizer->Init(collection, options);
  summarizer->Summarize(sum_options, &summary);
  DocumentUtils::FitToSize(50, &summary);

  LOG(INFO) << summary.DebugString();
  EXPECT_EQ(
      "the u s mint said it is seeking offers on 3,701,000 lbs of electrolytic "
      "copper and 629,000 lbs of electrolytic cut nickel cathodes or "
      "briquettes that it intends to purchase\ntechnigen platinum corp said "
      "it initial results of a 13-hole drilling program on its r m "
      "nicel platinum property in [...]",
      DocumentUtils::ToString(summary));

  // Check that the filename of the source document has been propagated
  // to the summary sentences.
  double expected_scores[] = {6.9548811912536621, 5.3816475868225098};
  ASSERT_EQ(ARRAYSIZE(expected_scores), summary.sentence_size());
  for (int i = 0; i < summary.sentence_size(); ++i) {
    EXPECT_DOUBLE_EQ(expected_scores[i], summary.sentence(i).score());
  }

  // Check that the debug output has been generated.
  EXPECT_FALSE(summary.debug_string().empty());
  EXPECT_NE(summary.debug_string().find("Collection distribution"),
            string::npos);
}

TEST(KLSum, SummarizerWithPostprocessor) {
  string path = DATADIR "testdata/singledoc";

  // Read collection
  DocumentCollection c;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(c.ParseFromIstream(&in));

  // Create the summarizer
  KLSum summarizer;

  // Create the options.
  SummarizerOptions options;

  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
  klsumoptions->set_redundancy_removal(true);
  klsumoptions->set_postprocessor_name("TestPostprocessor");

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(50);
  length->set_unit(SummaryLength::TOKEN);
  sum_options.set_generate_debug(true);

  // Summarize collection.
  Document summary;
  summarizer.Init(c, options);
  summarizer.Summarize(sum_options, &summary);
  DocumentUtils::FitToSize(50, &summary);

  LOG(INFO) << summary.DebugString();

  // There should not be any zero digit in the generated summary.
  string summary_raw_content = DocumentUtils::ToString(summary);
  EXPECT_EQ(string::npos, summary_raw_content.find('0'));

  // Check in the debug information that at least one sentence was removed.
  EXPECT_NE(string::npos, summary.debug_string().find("Removed sentence"));
}

TEST(KLSum, SummarizeVeryLong) {
  string path = DATADIR "testdata/singledoc";

  // Read collection.
  DocumentCollection c;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(c.ParseFromIstream(&in));

  // Create the summarizer.
  scoped_ptr<KLSum> summarizer(new KLSum());
  ASSERT_TRUE(summarizer.get() != NULL);

  // Create the options.
  SummarizerOptions options;

  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
  klsumoptions->set_redundancy_removal(false);

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(100000);
  length->set_unit(SummaryLength::TOKEN);
  sum_options.set_generate_debug(true);

  // Summarize the collection: the limit is set very high, so it should
  // include all the sentences in the original document.
  Document summary;
  summarizer->Init(c, options);
  summarizer->Summarize(sum_options, &summary);

  EXPECT_EQ(377, summary.num_tokens());
}

TEST(KLSum, RankSentencesVeryLongSummary) {
  string path = DATADIR "testdata/singledoc";

  // Read collection.
  DocumentCollection c;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(c.ParseFromIstream(&in));

  // Create the summarizer.
  scoped_ptr<KLSum> summarizer(new KLSum());
  ASSERT_TRUE(summarizer.get() != NULL);

  // Create the options.
  SummarizerOptions options;
  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::SENTENCE_RANKING);
  klsumoptions->set_redundancy_removal(false);

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(100000);
  length->set_unit(SummaryLength::TOKEN);

  // Summarize the collection: the limit is set very high, so it should
  // include all the sentences in the original document.
  Document summary;
  summarizer->Init(c, options);
  summarizer->Summarize(sum_options, &summary);

  EXPECT_EQ(377, summary.num_tokens());
  double expected_scores[] = {6.95,
                              7.07,
                              7.52,
                              7.60,
                              7.72,
                              7.87,
                              7.92,
                              8.39,
                              8.46,
                              8.57,
                              8.57,
                              8.65,
                              8.80,
                              8.93,
                              9.55,
                              10.40};
  ASSERT_EQ(ARRAYSIZE(expected_scores), summary.sentence_size());
  for (int i = 0; i < summary.sentence_size(); ++i) {
    EXPECT_NEAR(expected_scores[i], summary.sentence(i).score(), 0.01);
  }
}

TEST(KLSum, RankSentencesShortSummary) {
  string path = DATADIR "testdata/singledoc";

  // Read collection.
  DocumentCollection c;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(c.ParseFromIstream(&in));

  // Create the summarizer.
  scoped_ptr<KLSum> summarizer(new KLSum());
  ASSERT_TRUE(summarizer.get() != NULL);

  // Create the options.
  SummarizerOptions options;

  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::SENTENCE_RANKING);
  klsumoptions->set_redundancy_removal(false);

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(2);
  length->set_unit(SummaryLength::SENTENCE);
  sum_options.set_generate_debug(true);

  // Summarize the collection: the limit is set very high, so it should
  // include all the sentences in the original document.
  Document summary;
  summarizer->Init(c, options);
  summarizer->Summarize(sum_options, &summary);

  EXPECT_EQ(74, summary.num_tokens());
  double expected_scores[] = {6.95,
                              7.07};
  ASSERT_EQ(ARRAYSIZE(expected_scores), summary.sentence_size());
  for (int i = 0; i < summary.sentence_size(); ++i) {
    EXPECT_NEAR(expected_scores[i], summary.sentence(i).score(), 0.01);
  }
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  // Reset seed. Necessary to be able to reproduce the experiments.
  srandom(0);

  return RUN_ALL_TESTS();
}
