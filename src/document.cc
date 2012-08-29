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

#include "summarizer/document.h"

#include <vector>

#include "summarizer/distribution.h"
#include "summarizer/logging.h"
#include "summarizer/summarizer.pb.h"
#include "summarizer/types.h"

namespace topicsum {

using std::vector;

// =======================================================================
// SentenceUtils
// =======================================================================

string SentenceUtils::ToString(const Sentence& sentence) {
  string str;

  for (int i = 0; i < sentence.token_size(); i++) {
    if (i != 0) {
      str += " ";
    }
    str += sentence.token(i);
  }

  return str;
}

Sentence SentenceUtils::FromTokens(const vector<string>& tokens) {
  Sentence s;
  for (uint i = 0; i < tokens.size(); ++i) {
    s.add_token(tokens[i]);
  }
  return s;
}

Sentence SentenceUtils::FromTokensAndTags(const vector<string>& tokens,
                                          const vector<string>& pos_tags) {
  Sentence s;
  for (uint i = 0; i < tokens.size(); ++i)
    s.add_token(tokens[i]);
  for (uint i = 0; i < pos_tags.size(); ++i)
    s.add_pos_tag(pos_tags[i]);

  CHECK(s.token_size() == s.pos_tag_size())
      << "Sentence created with a different number of "
      << "tokens and tags.";

  return s;
}

void SentenceUtils::AddTermFrequency(const Sentence& sent,
                                     Distribution* distri) {
  CHECK(distri != NULL);

  // Count words in the sentence.
  for (int w = 0; w < sent.token_size(); w++) {
    const string& token = sent.token(w);
    Distribution::iterator iter = distri->find(token);
    if (iter == distri->end())
      distri->insert(pair<string, double>(token, 1));
    else
      iter->second += 1;
  }
}

// =======================================================================
// DocumentUtils
// =======================================================================

Sentence* DocumentUtils::AddSentence(const Sentence& sentence, Document* doc) {
  Sentence* copy = doc->add_sentence();
  copy->CopyFrom(sentence);
  doc->set_num_tokens(doc->num_tokens() + sentence.token_size());
  return copy;
}

string DocumentUtils::ToString(const Document& doc) {
  string str;
  if (doc.sentence_size() > 0) {
    int i;
    for (i = 0; i < doc.sentence_size() - 1; i++) {
      str += SentenceUtils::ToString(doc.sentence(i)) + "\n";
    }
    str += SentenceUtils::ToString(doc.sentence(i));
  }
  return str;
}

void DocumentUtils::FitToSize(int num_tokens, Document* doc) {
  if (doc->num_tokens() > num_tokens) {
    int token_count = 0;
    int i = 0;
    // Find last sentence to keep.
    for (; i < doc->sentence_size(); i++) {
      token_count += doc->sentence(i).token_size();
      if (token_count > num_tokens)
        break;
    }

    // Truncate last sentence to keep.
    Sentence* last_sentence = doc->mutable_sentence(i);
    int trunc_size = last_sentence->token_size() - (token_count - num_tokens);

    while (last_sentence->token_size() > trunc_size)
      last_sentence->mutable_token()->RemoveLast();

    // Add marker to show that sentence has been truncated.
    last_sentence->add_token("[...]");

    // Delete remaining sentences (if any).
    while (doc->sentence_size() > i+1)
      doc->mutable_sentence()->RemoveLast();

    // Update num_tokens.
    doc->set_num_tokens(num_tokens);
  }
}

bool DocumentUtils::ReachesLengthLimit(int length_limit,
                                       int length_unit,
                                       const Document& doc) {
  if (length_unit == SummaryLength::TOKEN) {
    return doc.num_tokens() >= length_limit;
  } else if (length_unit == SummaryLength::CHARACTER) {
    int document_length = DocumentUtils::ToString(doc).length();
    return document_length >= length_limit;
  } else if (length_unit == SummaryLength::SENTENCE) {
    return doc.sentence_size() >= length_limit;
  } else {
    LOG(FATAL) << "Unsupported length unit: " << length_unit;
    return false;
  }
}

void DocumentUtils::AddTermFrequency(const Document& doc,
                                     Distribution* distri) {
  CHECK(distri != NULL);

  SentenceUtils::AddTermFrequency(doc.sentence().begin(),
                                  doc.sentence().end(),
                                  distri);
}

// =======================================================================
// CollectionUtils
// =======================================================================

void CollectionUtils::AddDocument(const Document& doc,
                                  DocumentCollection* collection) {
  Document* copy = collection->add_document();
  copy->CopyFrom(doc);
  collection->set_num_sentences(collection->num_sentences()
                                + doc.sentence_size());
}

void CollectionUtils::AddTermFrequency(const DocumentCollection& col,
                                       Distribution* distri) {
  for (ConstCollectionDocumentIterator iter = col.document().begin();
       iter != col.document().end();
       ++iter) {
    const Document& doc = *iter;
    DocumentUtils::AddTermFrequency(doc, distri);
  }
}
}  // namespace topicsum
