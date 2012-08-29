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

// Base classes containing helper functions to manipulate sentences, documents
// and collections as well as convenient iterators for sentence and document
// containers.

#ifndef SUMMARIZER_DOCUMENT_H_
#define SUMMARIZER_DOCUMENT_H_

#include <string>
#include <vector>

#include "google/protobuf/repeated_field.h"
#include "summarizer/distribution.h"
#include "summarizer/document.pb.h"
#include "summarizer/hash.h"
#include "summarizer/logging.h"

namespace topicsum {

using std::vector;

typedef google::protobuf::internal::RepeatedPtrIterator<Sentence>DocumentSentenceIterator;

typedef google::protobuf::internal::RepeatedPtrIterator<const Sentence>
    ConstDocumentSentenceIterator;

typedef google::protobuf::internal::RepeatedPtrIterator<Document>
    CollectionDocumentIterator;

typedef google::protobuf::internal::RepeatedPtrIterator<const Document>
    ConstCollectionDocumentIterator;

// The class provides helper functions to create Sentences.
class SentenceUtils {
 public:
  // Generates a string representation of a tokenized sentence.
  static string ToString(const Sentence& sentence);

  // Generates a sentence from a vector containing its tokens.
  static Sentence FromTokens(const vector<string>& tokens);

  // Generates a sentence from two vectors, one containing the tokens
  // and the second one containing part-of-speech tags. The two vectors
  // are required to have the same number of elements.
  static Sentence FromTokensAndTags(const vector<string>& tokens,
                                    const vector<string>& pos_tags);

  // Adds the frequency distribution of terms in a sentence to distri.
  // This is an update function as distri is not reset before adding the terms
  // of the sentence.
  static void AddTermFrequency(const Sentence& sent,
                               Distribution* distri);

  // Adds the frequency distribution of terms in a set of sentences accessible
  // via an iterator to distri.
  // This is an update function as distri is not reset before adding the terms
  // of the sentences.
  template<typename SentenceIterator>
  static inline void AddTermFrequency(const SentenceIterator& sent_iter_begin,
                                      const SentenceIterator& sent_iter_end,
                                      Distribution* distri) {
    CHECK(distri);

    // Count words in all sentences.
    for (SentenceIterator iter = sent_iter_begin;
         iter != sent_iter_end;
         ++iter) {
      const Sentence& sent = *iter;
      AddTermFrequency(sent, distri);
    }
  }

};

// The class provides helper functions to create Documents.
class DocumentUtils {
 public:
  // Adds a sentence to a document. Returns a mutable pointer to the newly added
  // sentence.
  static Sentence* AddSentence(const Sentence& sentence, Document* doc);

  // Truncates sentences of a document to have no more than num_tokens
  // consecutive tokens.
  // The raw content of the remaining sentences is not modified.
  static void FitToSize(int num_tokens, Document* doc);

  // Generates a string representation of a document.
  static string ToString(const Document& doc);

  // Checks whether a document has already reached the length limit allowed.
  static bool ReachesLengthLimit(int length_limit,
                                 int length_unit,
                                 const Document& doc);

  // Adds the frequency distribution of terms in a document to distri.
  // This is an update function as distri is not reset before adding the terms
  // of the sentences of the document.
  static void AddTermFrequency(const Document& doc,
                               Distribution* distri);
};

// The class provides helper functions to create Collections.
class CollectionUtils {
 public:
  static void AddDocument(const Document& doc,
                          DocumentCollection* collection);

  // Adds the frequency distribution of terms in a collection to distri.
  // This is an update function as distri is not reset before adding the terms
  // of the documents of the collection.
  static void AddTermFrequency(const DocumentCollection& col,
                               Distribution* distri);
};

// The class provides an iterator over tokens in a collection of sentences.
template <typename T>
class TokenIterator {
 public:
  // Creates an empty ConstTokenIterator.
  TokenIterator(T begin, T end)
      : begin_(begin),
        end_(end),
        tok_index_(0) {
    while (begin_->token_size() == 0 && !Done())
      ++begin_;
  }

  // Destroys the iterator.
  virtual ~TokenIterator() {}

  // Returns a the current value pointed by the iterator.
  string* CurrentValue() const {
    CHECK(!Done());
    return begin_->mutable_token(tok_index_);
  }

  const T& CurrentSentence() const {
    CHECK(!Done());
    return begin_;
  }

  // Checks if the iterator has reached the end.
  bool Done() const {
    return (begin_ == end_);
  }

  // Moves to the next token.
  void Next() {
    CHECK(!Done());
    CHECK_LT(tok_index_, begin_->token_size());

    if (tok_index_ < begin_->token_size() - 1) {
      ++tok_index_;
    } else {
      // Move to the first token in next sentence with at least one token.
      tok_index_ = 0;
      do {
        ++begin_;
      } while (!Done() && tok_index_ == begin_->token_size());
    }
  }

 protected:
  T begin_;
  T end_;
  int tok_index_;
};

// The class provides a constant iterator over tokens in a container of
// sentences.
template <typename T>
class ConstTokenIterator : public TokenIterator<T> {
 public:
  // Creates an empty ConstSentenceIterator.
  ConstTokenIterator(T begin, T end) : TokenIterator<T>(begin, end) {}

  // Returns a the current value pointed by the iterator.
  const string& CurrentValue() const {
    CHECK(!this->Done());
    return this->begin_->token(this->tok_index_);
  }
};


// The class provides an iterator over sentences in a container of documents.
template <typename T>
class SentenceIterator {
 public:
  // Creates an empty SentenceIterator.
  SentenceIterator(T begin,
                   T end)
      : begin_(begin), end_(end), sentence_index_(0) {
    while (begin_->sentence_size() == 0 && !Done())
      ++begin_;
  }

  // Destroys the iterator.
  virtual ~SentenceIterator() {}

  // Returns a the current value pointed by the iterator.
  Sentence* CurrentValue() const {
    CHECK(!Done());
    return begin_->mutable_sentence(sentence_index_);
  }

  // Returns the document that contains the current sentence.
  const T& CurrentDocument() const {
    return begin_;
  }

  // Checks if the iterator has reached the end.
  bool Done() const {
    return (begin_ == end_);
  }

  // Moves to the next sentence.
  void Next() {
    CHECK(!Done());
    CHECK_LT(sentence_index_, begin_->sentence_size());

    if (sentence_index_ < begin_->sentence_size() - 1) {
      ++sentence_index_;
    } else {
      // Move to the first sentence in next document with at least one sentence.
      sentence_index_ = 0;
      do {
        ++begin_;
      } while (!Done() && sentence_index_ == begin_->sentence_size());
    }
  }

  // Removes the current sentence, and moves to the next one.
  void Erase() {
    CHECK(!Done());
    CHECK_LT(sentence_index_, begin_->sentence_size());

    // Remove the current sentence.
    for (int i = sentence_index_; i < begin_->sentence_size() - 1; ++i)
      begin_->mutable_sentence()->SwapElements(i, i + 1);
    begin_->mutable_sentence()->RemoveLast();

    // If it was the last sentence, advance to the next document.
    if (sentence_index_ == begin_->sentence_size()) {
      sentence_index_ = 0;
      do {
        ++begin_;
      } while (!Done() && sentence_index_ == begin_->sentence_size());
    }
  }

 protected:
  T begin_;
  T end_;
  int sentence_index_;
};

// The class provides a constant iterator over sentences in a container of
// documents.
template <typename T>
class ConstSentenceIterator : public SentenceIterator<T> {
 public:
  // Creates an empty ConstSentenceIterator.
  ConstSentenceIterator(T begin, T end) : SentenceIterator<T>(begin, end) {}

  // Returns a the current value pointed by the iterator.
  const Sentence* CurrentValue() const {
    CHECK(!this->Done());
    return &(this->begin_->sentence(this->sentence_index_));
  }
};

}  // namespace topicsum

#endif  // SUMMARIZER_DOCUMENT_H_
