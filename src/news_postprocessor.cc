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

#include "summarizer/news_postprocessor.h"

#include <cctype>
#include <string>

#include "summarizer/summarizer.pb.h"


// Sentences with strictly less than this number of words will be discarded.
const static int kNewsPostprocessorMinSentenceLength = 10;

// Sentences with strictly more than this number of words will be discarded.
const static int kNewsPostprocessorMaxSentenceLength = 30;

namespace topicsum {

NewsPostprocessor::NewsPostprocessor() {
  min_sentence_length_ = kNewsPostprocessorMinSentenceLength;
  max_sentence_length_ = kNewsPostprocessorMaxSentenceLength;
}

void NewsPostprocessor::Init(const SummarizerOptions& options) {
  if (!options.has_news_postprocessor_options()) return;
  const NewsPostProcessorOptions& news_options =
      options.news_postprocessor_options();

  if (news_options.has_min_sentence_length())
    min_sentence_length_ = news_options.min_sentence_length();

  if (news_options.has_max_sentence_length())
    max_sentence_length_ = news_options.max_sentence_length();
}

bool NewsPostprocessor::IsValidSentence(const Sentence& sentence) const {
  const string& raw_content = sentence.raw_content();
  int length = raw_content.length();
  if (length == 0) return false;

  // Discard Wh-questions (What, Where, Who,...) and How
  LOG(INFO) << raw_content.substr(2);
  if (raw_content.substr(0, 2) == "Wh" ||
      raw_content.substr(0, 3) == "How" ) {
    return false;
  }

  // If the sentence contains a question mark, remove it completely.
  if (raw_content.find('?') != string::npos) return false;

  // If the sentence ends with a colon, remove it.
  if (raw_content[length - 1] == ':') return false;

  // If the sentence contains a double quote, remove it completely.
  if (raw_content.find('"') != string::npos ||
      raw_content.find("\xe2\x80\x9c") != string::npos ||
      raw_content.find("\xe2\x80\x9d") != string::npos)
    return false;

  // If the sentence was tokenized (token_size > 0) and it contains very few
  // words, remove it completely.
  if (sentence.token_size() > 0 && sentence.token_size() < min_sentence_length_)
    return false;

  if (sentence.token_size() > max_sentence_length_)
    return false;

  // If the whole sentence is between parentheses, it is usually not relevant.
  if (length > 2 &&
      raw_content[0] == '(' &&
      ((raw_content[length - 1] == ')') ||
       (raw_content[length - 2] == ')' && raw_content[length - 1] == '.')))
    return false;

  if (length > 2 &&
      raw_content[0] == '[' &&
      ((raw_content[length - 1] == ']') ||
       (raw_content[length - 2] == ']' && raw_content[length - 1] == '.')))
    return false;

  // Sentences ending in ... are discarded.
  if (length > 3 &&
      raw_content.substr(raw_content.length() - 3) == "...")
    return false;

  // If the raw content starts with a lowercase, and the word has no uppercase,
  // skip this sentence as well (e.g. a sentence starting with 'iPhone' is fine,
  // but a sentence starting with 'but' is not).
  if (raw_content.length() > 1 && !isupper(raw_content[0])) {
    for (int i = 1; i < raw_content.length(); ++i) {
      if (isupper(raw_content[i])) break;
      if (raw_content[i] == ' ') return false;
    }
  }

  // Filters on part-of-speech tags.
  // TODO(ealfonseca): use a single tagset in these rules, preferably the one
  // used by the universal part-of-speech tagger.
  if (sentence.pos_tag_size() > 0 &&
      sentence.pos_tag_size() == sentence.token_size()) {
    // Sentences with first-person pronouns are discarded.
    for (int i = 0; i < sentence.token_size(); ++i) {
      if (sentence.pos_tag(i) == "PRON" || sentence.pos_tag(i) == "PRP") {
        const string& token = sentence.token(i);
        if (token == "i" || token == "we") return false;
      }
    }

    // Sentences with no verbs are removed.
    bool has_verb = false;
    for (int i = 0; i < sentence.pos_tag_size(); ++i) {
      const string& tag = sentence.pos_tag(i);
      if (tag == "VERB" || tag == "VBP" || tag == "VBZ" || tag == "VBD") {
        has_verb = true;
        break;
      }
    }
    if (!has_verb) return false;
  }

  return true;
}

bool NewsPostprocessor::Compress(Sentence* sentence) const {
  const string& raw_content = sentence->raw_content();
  int length = raw_content.length();

  bool scrap = !IsValidSentence(*sentence);
  if (scrap) {
    sentence->Clear();
    return true;
  }

  // If the sentence starts with a portion between parentheses, remove it.
  if (length > 2 && raw_content[0] == '(') {
    int position = raw_content.find(')');
    string cleaned = raw_content.substr(position + 1);
    int start = 0;
    int end = cleaned.length()-1;
    while (start < cleaned.length() && isspace(cleaned[start])) start++;
    while (end >= 0 && isspace(cleaned[end])) end--;
    if (start > end) {
      sentence->Clear();
    } else {
      cleaned = cleaned.substr(start, end - start + 1);
      sentence->set_raw_content(cleaned);
    }
    return true;
  }

  return false;
}

}  // namespace topicsum
