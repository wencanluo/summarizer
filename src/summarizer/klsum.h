// Copyright 2012 Google Inc. All Rights Reserved.
// Author: jydelort@google.com (Jean-Yves Delort)
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

// This summarizer uses the KL divergence to build a summary.
//
// The details of the KLSum algorithm can be found in:
//   http://www.aria42.com/pubs/naacl09-topical.pdf
//
// This is a slightly modified version: instead of KL(collection||summary)
// we use KL(summary||collection).

#ifndef SUMMARIZER_KLSUM_H_
#define SUMMARIZER_KLSUM_H_

#include "summarizer/distribution.h"
#include "summarizer/document.h"
#include "summarizer/postprocessor.h"
#include "summarizer/scoped_ptr.h"
#include "summarizer/sum.h"

namespace topicsum {

struct VectorDistribution {
 public:
  explicit VectorDistribution(int vocabulary_size)
      : distribution(new double[vocabulary_size]),
        length(vocabulary_size),
        sentence(NULL),
        document(NULL) {
    for (int i = 0; i < length; ++i) distribution[i] = 0;
  }

  ~VectorDistribution() {
    delete[] distribution;
  }

  // Adds values of other vector to current vector and returns the sum of values
  // in updated vector.
  double AddVector(const VectorDistribution& other) {
    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
      distribution[i] += other.distribution[i];
      sum += distribution[i];
    }
    return sum;
  }

  // Substracts values of other vector to current vector and returns the sum of
  // values in updated vector.
  double SubstractVector(const VectorDistribution& other) {
    double sum = 0.0;
    for (int i = 0; i < length; ++i) {
      distribution[i] -= other.distribution[i];
      sum += distribution[i];
    }
    return sum;
  }

  // Distribution of words in a topic, or in a summary.
  double* distribution;

  // Number of unique tokens in the vocabulary.
  int length;

  // This field is only used for the collection language model, not for sentence
  // or summary LMs. It indicates the number of words for which the associated
  // value is not zero. For the other vectors it is not even calculated so it
  // should not be used.
  int num_non_zero;

  // This field is only used for the collection language model. It contains a
  // weight threshold over which we have the top words in the distribution.
  double top_words_threshold;

  // Sentence and document for which this vector corresponds. This is only
  // used for the sentences in the original documents, not for the collection
  // nor for the summary language models.
  const Sentence* sentence;
  const Document* document;
};

class KLSum : public Summarizer {
 public:
  // KLSum minimizes an objective function, kMaxCost defines a large positive
  // value that can be used to rule out sentences (or any other scored unit)
  // that do not match a constraint by assigning a large cost as their score.
  // For example, if we want to impose that a sentence contains the query entity
  // we can assign to that sentence this value.
  static const float kMaxCost;
  KLSum() : coll_distri_(NULL),
            output_debug_(NULL),
            redundancy_removal_(true),
            postprocessor_(NULL) {}

  virtual ~KLSum() {}

  bool Init(const DocumentCollection& collection,
            const SummarizerOptions& options);

  bool Summarize(const SummaryOptions& options,
                 Document* summary);

  // Returns whether a collection distribution has already been initialized.
  bool HasCollectionDistribution() {
    return coll_distri_.get() != NULL;
  }

  // Returns the collection distribution. It is necessary to call
  // HasCollectionDistribution before to verify that it has been created.
  const Distribution& GetCollectionDistribution() {
    return *coll_distri_;
  }

 protected:
  // Calculates the token distribution of the collection provided
  // as an input to the Summarize function. The function is automatically
  // called by the Summarize.
  // Returns false if an error happened.
  virtual bool CalculateCollectionDistribution(const DocumentCollection& coll);

  // Collection distribution for the collection that is being summarized.
  scoped_ptr<Distribution> coll_distri_;

  string* GetDebugString() { return &debug_; }

  DocumentCollection collection_;

 private:
  // Summarize by selecting one by one the sentences that will be chosen for the
  // summary. Two different strategies are implemented by this function:
  // greedily growing the summary one sentence at a time, or simply ranking the
  // sentences once based on their divergence with the collection distribution.
  virtual bool SummarizeIncremental(
      const DocumentCollection& collection,
      const SummaryOptions& options,
      KLSumOptions::OptimizationStrategy strategy,
      Document* summary);

  // If there is a postprocessor, invokes it on the sentence before adding it to
  // the summary. Returns a pointer to the added summary sentence if a sentence
  // was added to the summary, NULL if the postprocessor recommended to filter
  // it out.
  virtual Sentence* PostprocessAndAddSentence(const Sentence& new_sentence,
                                              double score,
                                              const Document& source_document,
                                              Document* summary);

  // Debug information that will be appended to the summary.
  string debug_;

  // Pointer to the output debug information in the summary, in case it has
  // to be generated.
  string* output_debug_;

  bool redundancy_removal_;
  bool sentence_position_;

  scoped_ptr<Postprocessor> postprocessor_;

  KLSumOptions::OptimizationStrategy strategy_;
  float summary_weight_;
  float prior_weight_;

  KLSum(const KLSum&);
  void operator=(const KLSum&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_KLSUM_H_
