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

#include "summarizer/document.pb.h"
#include "summarizer/summarizer.pb.h"
#include "summarizer/topicsum.h"
#include "summarizer/xml_parser.h"

namespace topicsum {

bool TopicSummarizer::Init(const SummarizerOptions& options) {
  XmlParser xml_parser;
  DocumentCollection collection;

  for (int i = 0; i < options.article_size(); i++) {
    Document* doc = collection.add_document();
    // Parse document from xml.
    if (!xml_parser.ParseDocument(options.article(i).content(), doc)) {
      last_error_message_ = "Error while parsing xml document";
      return false;
    }

    // Check number of prior scores.
    if (doc->sentence_size() != options.article(i).score_size()) {
      last_error_message_ = "Wrong number of prior scores.";
      return false;
    }

    // Add prior scores to sentences.
    for (int j = 0; j < options.article(i).score_size(); j++) {
      doc->mutable_sentence(j)->set_prior_score(options.article(i).score(j));
    }
  }

  bool result = summarizer.Init(collection, options);
  last_error_message_ = summarizer.last_error_message();
  return result;
}

bool TopicSummarizer::Summarize(const SummaryOptions& options,
                                string* summary) {
  Document sum;
  if (summarizer.Summarize(options, &sum)) {
    for (int i = 0; i < sum.sentence_size() - 1; i++) {
      summary->append(sum.sentence(i).raw_content());
      summary->append(" ");
    }
    summary->append(sum.sentence(sum.sentence_size() - 1).raw_content());
  } else {
    last_error_message_ = sum.debug_string();
    return false;
  }

  return true;
}

}  // namespace topicsum
