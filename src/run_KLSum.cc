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
  string path = DATADIR "testdata/singledoc";

  // Read collection.
  DocumentCollection c;
  fstream in(path.c_str(), ios::in | ios::binary);
  CHECK(c.ParseFromIstream(&in));

  // Create the summarizer.
  scoped_ptr<KLSum> summarizer(new KLSum());

  // Create the options.
  SummarizerOptions options;

  KLSumOptions* klsumoptions = options.mutable_klsum_options();
  klsumoptions->set_optimization_strategy(KLSumOptions::SENTENCE_RANKING);
  klsumoptions->set_redundancy_removal(true);

  SummaryOptions sum_options;
  SummaryLength* length = sum_options.mutable_length();
  length->set_length(4);
  length->set_unit(SummaryLength::SENTENCE);
  
  // Summarize the collection: the limit is set very high, so it should
  // include all the sentences in the original document.
  Document summary;
  summarizer->Init(c, options);
  summarizer->Summarize(sum_options, &summary);

  for (int i = 0; i < summary.sentence_size(); ++i) {
	cout << summary.sentence(i).raw_content() << endl;
  }

  return 0;
}
