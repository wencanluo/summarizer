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

#include "summarizer/topicsum.h"

#include <fstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "summarizer/distribution.h"
#include "summarizer/distribution.pb.h"
#include "summarizer/document.h"
#include "summarizer/document.pb.h"
#include "summarizer/file.h"
#include "summarizer/logging.h"
#include "summarizer/topicsumgibbs.h"

namespace topicsum {

using std::fstream;
using std::ios;

class TopicSumTest : public testing::Test {
 public:
  virtual void SetUp() {
    // Even though the input of that unit-test is only one collection, we need
    // to store the results in a vector of DocumentCollections as TopicSum input
    // is usually a vector of DocumentCollections. For consistency with other
    // summarization techniques we use the same input. However the Gibbs sampler
    // has been tested with multiple collections (see topicsumgibbs_test.cc).

    // Reset seed. Necessary to be able to reproduce the experiments.
    srandom(0);

    // Read collection
    string path = DATADIR "testdata/singledoc";
    fstream in(path.c_str(), ios::in | ios::binary);
    CHECK(collection_.ParseFromIstream(&in));

    // Create the options.
    options_.Clear();

    // TopicSum options
    TopicSumOptions* topicsum_options = options_.mutable_topicsum_options();
    topicsum_options->set_lambda("0.1,1,1");
    topicsum_options->set_gamma("1,5,10");

    // KLSum options
    KLSumOptions* klsumoptions = options_.mutable_klsum_options();
    klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
    klsumoptions->set_redundancy_removal(false);

    // Gibbs options
    GibbsSamplingOptions* gibbsoptions =
        topicsum_options->mutable_gibbs_sampling_options();
    gibbsoptions->set_iterations(500);
    gibbsoptions->set_burnin(0);
    gibbsoptions->set_lag(100);
    gibbsoptions->set_track_likelihood(true);

    //Create summary options
    sum_options_.Clear();
    SummaryLength* length = sum_options_.mutable_length();
    length->set_length(50);
    length->set_unit(SummaryLength::TOKEN);
    sum_options_.set_generate_debug(true);
  }

 protected:
  SummarizerOptions options_;
  SummaryOptions sum_options_;
  DocumentCollection collection_;
};

TEST_F(TopicSumTest, Summarize) {
  // Create and initialize the summarizer.
  TopicSum topicsum;
  CHECK(topicsum.Init(collection_, options_));

  // Summarize collection.
  Document summary;
  CHECK(topicsum.Summarize(sum_options_, &summary));

  EXPECT_EQ(
      "technigen platinum corp said it initial results of a 13-hole "
      "drilling program on its r m nicel platinum property in rouyn-noranda "
      "quebec indicate \"extensive\" near-surface zones \"highly\" enriched "
      "in gold platinum and palladium were found in rocks on the periphery of "
      "a sulphide deposit\nsumitomo metal mining co said it will raise its "
      "monthly nickel output to around 1,750 tonnes from april 1 from 1,650 "
      "now because of increased domestic demand mainly from stainless steel "
      "makers",
      DocumentUtils::ToString(summary));

  LOG(INFO) << summary.debug_string();

  // Check that the debug output has been generated. 'the' is the word
  // with the highest probability, as there is just one collection and
  // the background cannot be properly identified.
  EXPECT_FALSE(summary.debug_string().empty());
  EXPECT_NE(summary.debug_string().find("Collection distribution"),
            string::npos);
  EXPECT_NE(summary.debug_string().find("it\t0.0082"), string::npos);
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
