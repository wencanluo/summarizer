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

// Include summarizer header.
#include "summarizer/summarizer.h"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

using namespace std;
using namespace topicsum;

int main(int argc, char** argv) {
  // Read article.
  ifstream article_file("testdata/article_14319162.xml");
  if (!article_file) {
    cerr << "File not found\n";
    return 1;
  }

  // Read file content (STL magic).
  string article((istreambuf_iterator<char>(article_file)),
                 istreambuf_iterator<char>());

  // Create options for summarizer.
  SummarizerOptions summarizer_options;

  // Create TopicSum options.
  TopicSumOptions* topicsum_options =
      summarizer_options.mutable_topicsum_options();
  // (Optional) Hyper-parameters for each topic distribution.
  // Default: "0.1,1,1"
  topicsum_options->set_lambda("0.1,1,1");
  // (Optional) Hyper-parameters for distribution of topics in each sentence.
  // Default: "1,5,10"
  topicsum_options->set_gamma("1,5,10");

  // Create KLSum options.
  KLSumOptions* klsumoptions = summarizer_options.mutable_klsum_options();
  // (Optional) If true, discard redundant sentences.
  // Default: false
  klsumoptions->set_redundancy_removal(false);
  // (Optional) Weight of a sentence score from a summarizer.
  // Default: 1.0
  klsumoptions->set_summary_weight(0.5);
  // (Optional) Weight of a prior score of a sentence.
  // Default: 0.0
  klsumoptions->set_prior_weight(0.5);

  // Create Gibbs sampling options.
  GibbsSamplingOptions* gibbsoptions =
      topicsum_options->mutable_gibbs_sampling_options();
  // (Optional) Number of sampling iterations.
  // Default: 500
  gibbsoptions->set_iterations(500);
  // (Optional) Number of burnin iterations.
  // Default: 0
  gibbsoptions->set_burnin(0);
  // (Optional) Lag before updating model parameters.
  // Default: 10
  gibbsoptions->set_lag(100);
  // (Optional) Track likelihood at each iteration?
  // Default: false
  gibbsoptions->set_track_likelihood(true);

  // (Optional) Set postprocessor options.
  NewsPostProcessorOptions* postprocess_options =
      summarizer_options.mutable_news_postprocessor_options();
  // (Optional) Minimum length allowed to valid sentences (measured in number of
  // words).
  // Default: 10
  postprocess_options->set_min_sentence_length(5);
  // (Optional) Maximum length allowed to valid sentences (measured in number of
  // words).
  // Default: 30
  postprocess_options->set_max_sentence_length(30);


  // Add one document to collection.
  Article* document = summarizer_options.add_article();
  // Set the content of the article.
  document->set_content(article);
  // Add scores for sentences. Scores must be in same order as sentences in
  // the document.
  const int kNumberOfSentences = 17;
  for (int i = 0; i < kNumberOfSentences; i++) {
    // Prior score of a sentence.
    document->add_score(0.5);
  }

  // Create summary options.
  SummaryOptions summary_options;

  // Set the expected length of the summary.
  SummaryLength* sum_length = summary_options.mutable_length();
  // Set desired unit (SENTENCE or TOKEN).
  sum_length->set_unit(SummaryLength::SENTENCE);
  // Set the expected length of the summary.
  sum_length->set_length(2);

  // Initialize summarizer with collection and options. Return true on success.
  TopicSummarizer summarizer;
  if(!summarizer.Init(summarizer_options)) {
    // Print last error.
    cerr << summarizer.last_error_message();
    return 1;
  }

  // Sumarize the collection with desired options. Return true on success.
  string summary;
  if (!summarizer.Summarize(summary_options, &summary)) {
    // Print last error.
    cerr << summarizer.last_error_message();
    return 1;
  }

  // Print the summary.
  cout << summary << endl;

  return 0;
}
