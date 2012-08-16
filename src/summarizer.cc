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

#include "summarizer/summarizer.pb.h"
#include "summarizer/document.pb.h"
#include "summarizer/xml_parser.h"

namespace topicsum {

bool Summarizer::Init(const SummarizerOptions& options) {
  // Check that there is at least one document in the collection.
  if (!options.article_size()) {
    last_error_message_ = "Collection must contain at least one document.";
    return false;
  }

  vector<float> scores;
  for (int i = 0; i < options.article_size(); i++) {
    articles_.push_back(options.article(i).content());

    scores.clear();
    for (int j = 0; j < options.article(i).score_size(); j++) {
      scores.push_back(options.article(i).score(j));
    }
    scores_.push_back(scores);
  }

  return true;
}

bool Summarizer::Summarize(const SummaryOptions& options,
                           string* summary) {
  Document article;

  // Check that summary length is specified
  if (!options.has_length()) {
    last_error_message_ = "SummarizerOptions does not contain summary length.";
    return false;
  }

  // Check that summary length is in correct units. Only tokens and sentences
  // are supported.
  if (options.length().unit() != SummaryLength::TOKEN &&
      options.length().unit() != SummaryLength::SENTENCE) {
    last_error_message_ = "This summarizer only supports TOKEN and SENTENCE "
                          "as summary length unit.";
    return false;
  }

  // Parse the document from xml.
  XmlParser xml_parser;
  if (!xml_parser.ParseDocument(articles_[0], &article)) {
    last_error_message_ = "Error occured while parsing article.";
    return false;
  }

  // Create summary.
  // Just takes sentences from the beginning of the article.
  int used_units = 0;
  int limit = options.length().length();
  int sentence_i = 0;
  while (sentence_i < article.sentence_size()) {
    int sentence_units;

    // Compute how many units will current sentence take
    if (options.length().unit() == SummaryLength::TOKEN) {
      sentence_units = article.sentence(sentence_i).token_size();
    } else if (options.length().unit() == SummaryLength::SENTENCE) {
      sentence_units = 1;
    }

    // Add sentence to summary if it fits or end.
    if (used_units + sentence_units <= limit) {
      used_units += sentence_units;
      summary->append(article.sentence(sentence_i).raw_content() + " ");
    } else {
      break;
    }

    sentence_i++;
  }

  return true;
}

}  // namespace topicsum
