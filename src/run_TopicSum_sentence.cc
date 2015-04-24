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
#include "summarizer/xml_parser.h"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

#include <stdio.h>

using namespace std;
using namespace topicsum;

int main(int argc, char** argv) {

	string input = argv[1];
	string output = argv[2];

	SummarizerOptions options_;
	SummaryOptions sum_options_;
	DocumentCollection collection_;

	//string input = argv[1];
    //string output = argv[2];
	srandom(0);

	string path = input;
	ifstream article_file(path.c_str());
	if (!article_file) {
	cerr << "File not found\n";
	return 1;
	}

	// Read file content (STL magic).
	string article((istreambuf_iterator<char>(article_file)),
				 istreambuf_iterator<char>());

	XmlParser xml_parser;
	Document* doc = collection_.add_document();
	// Parse document from xml.
	if (!xml_parser.ParseDocument(article, doc)) {
	  return false;
	}

	options_.Clear();

	// TopicSum options
	TopicSumOptions* topicsum_options = options_.mutable_topicsum_options();
	topicsum_options->set_lambda("0.1,1,1");
	topicsum_options->set_gamma("1,5,10");

	// KLSum options
	KLSumOptions* klsumoptions = options_.mutable_klsum_options();
	klsumoptions->set_optimization_strategy(KLSumOptions::GREEDY_OPTIMIZATION);
	klsumoptions->set_redundancy_removal(true);

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
	length->set_length(4);
	length->set_unit(SummaryLength::SENTENCE);

	TopicSum topicsum;
	topicsum.Init(collection_, options_);

	// Summarize collection.
	Document summary;
	topicsum.Summarize(sum_options_, &summary);

	freopen(output.c_str(), "w", stdout);
	for (int i = 0; i < summary.sentence_size(); ++i) {
		cout << summary.sentence(i).raw_content() << endl;
	}
	fclose (stdout);

	return 0;
}
