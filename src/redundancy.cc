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

#include "summarizer/redundancy.h"

#include <string>

#include "summarizer/document.h"
#include "summarizer/klsum.h"

// Minimum number of new collection words in a sentence to consider it
// non-redundant.
static const int kMinNumNewCollectionWords = 1;

// Minimum proportion of new collection words in a sentence to consider it
// non-redundant.
static const double kMinProportionNewCollectionWordsInSentence = 0.25;

// Minimum proportion of new collection words in a sentence, with respect to the
// total number of collection words in the sentence, to consider it
// non-redundant.
static const double kMinProportionCollectionWordsInSentenceThatAreNew = 0.67;

namespace topicsum {

bool IsRedundant(const VectorDistribution& summary,
                 const VectorDistribution& collection,
                 const VectorDistribution& new_sentence,
                 string *debug) {
  int num_words_in_sentence = 0;
  int num_collection_words_in_sentence = 0;
  int num_new_collection_words_in_sentence = 0;

  for (int i = 0; i < collection.length; ++i) {
    if (new_sentence.distribution[i] != 0) {
      ++num_words_in_sentence;
      if (collection.distribution[i] > collection.top_words_threshold) {
        ++num_collection_words_in_sentence;
        if (summary.distribution[i] == 0)
          ++num_new_collection_words_in_sentence;
      }
    }
  }

  // Filter based on some thresholds.
  if (num_new_collection_words_in_sentence <
      kMinNumNewCollectionWords) return true;

  if ((num_new_collection_words_in_sentence /
       static_cast<double>(num_words_in_sentence)) <
      kMinProportionNewCollectionWordsInSentence) return true;

  if ((num_new_collection_words_in_sentence /
       static_cast<double>(num_collection_words_in_sentence)) <
      kMinProportionCollectionWordsInSentenceThatAreNew)
    return true;

  return false;
}

}  // namespace topicsum
