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
  ifstream article_file("src/testdata/article_14319162.xml");
  if (!article_file) {
    cerr << "File not found\n";
    return 1;
  }

  // Read file content (STL magic).
  string article((istreambuf_iterator<char>(article_file)),
                 istreambuf_iterator<char>());

  // Create options for summarizer.
  SummarizerOptions summarizer_options;

  // Create KLSum options.
  KLSumOptions* klsumoptions = summarizer_options.mutable_klsum_options();
  // (Optional) Weight of a sentence score from a summarizer.
  // Default: 1.0
  klsumoptions->set_summary_weight(0.5);
  // (Optional) Weight of a prior score of a sentence.
  // Default: 0.0
  klsumoptions->set_prior_weight(0.5);

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
