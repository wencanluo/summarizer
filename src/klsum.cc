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

#include "summarizer/klsum.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <set>
#include <sstream>

#include "summarizer/distribution.h"
#include "summarizer/document.h"
#include "summarizer/lexicon.h"
#include "summarizer/news_postprocessor.h"
#include "summarizer/redundancy.h"
#include "summarizer/scoped_ptr.h"
#include "summarizer/test_postprocessor.h"

namespace topicsum {

using std::ostringstream;
using std::nth_element;
using std::make_pair;

static const float kKLSmoothingValue = 0.0000001;

const float KLSum::kMaxCost = std::numeric_limits<float>::max();

namespace {

double CalculateKLDivergence(const VectorDistribution& distriA,
                             const VectorDistribution& distriB,
                             double smoothing_factor) {
  if (smoothing_factor < 0)
    smoothing_factor = kKLSmoothingValue;

  double kl = 0;
  for (int i = 0; i < distriA.length; ++i) {
    double valA = distriA.distribution[i];
    if (valA == 0) continue;

    double valB = distriB.distribution[i];

    // Apply smoothing factor.
    valB = (valB + smoothing_factor) /
        (1 + distriA.length  * smoothing_factor);

    // Update divergence.
    kl += valA * log(valA / valB);
  }

  return kl;
}

}  // unnamed namespace

bool KLSum::Init(const DocumentCollection& collection,
                 const SummarizerOptions& options) {
  collection_ = collection;

  if (options.klsum_options().has_redundancy_removal() &&
      options.klsum_options().redundancy_removal())
    redundancy_removal_ = true;
  else
    redundancy_removal_ = false;

  if (options.klsum_options().has_sentence_position() &&
      options.klsum_options().sentence_position())
    sentence_position_ = true;
  else
    sentence_position_ = false;

  if (options.klsum_options().has_postprocessor_name()) {
    string postprocessor_name = options.klsum_options().postprocessor_name();
    if (postprocessor_name == "NewsPostprocessor") {
      postprocessor_.reset(new NewsPostprocessor());
      postprocessor_->Init(options);
    } else if (postprocessor_name == "TestPostprocessor") {
      postprocessor_.reset(new TestPostprocessor());
      postprocessor_->Init(options);
    } else {
      LOG(ERROR) << "Unknown postprocessor.";
    }
  }

  strategy_ = options.klsum_options().has_optimization_strategy() ?
      options.klsum_options().optimization_strategy() :
      KLSumOptions::GREEDY_OPTIMIZATION;

  summary_weight_ = options.klsum_options().summary_weight();
  prior_weight_ = options.klsum_options().prior_weight();

  return true;
}

bool KLSum::CalculateCollectionDistribution(const DocumentCollection& coll) {
  coll_distri_.reset(new Distribution);

  CollectionUtils::AddTermFrequency(coll, coll_distri_.get());
  NormalizeDistribution(coll_distri_.get());
  return true;
}

bool KLSum::Summarize(const SummaryOptions& options,
                      Document* summary) {
  CHECK(summary != NULL);
  if (!options.has_length()) {
    last_error_message_ = "SummarizerOptions does not contain summary length";
    return false;
  }
  if (options.length().unit() != SummaryLength::TOKEN &&
      options.length().unit() != SummaryLength::SENTENCE) {
    ostringstream oss;
    oss << "This summarizer only supports TOKEN and SENTENCE as "
        << "summary length unit but you required "
        << options.length().unit();
    last_error_message_ = oss.str();
    return false;
  }

  summary->Clear();

  // If we are to generate debug, keep a pointer to the output debug
  // and append the information that has been generated so far.
  if (options.has_generate_debug() && options.generate_debug()) {
    output_debug_ = summary->mutable_debug_string();
    output_debug_->append(debug_);
  }

  // Calculate distribution of words in the collection.
  if (!CalculateCollectionDistribution(collection_)) {
    last_error_message_.append(
        "\nError computing the collection distribution.");
    return false;
  }

  if (output_debug_) {
    // Add the collection distribution to the debug output.
    output_debug_->append("\nCollection distribution:\n");
    output_debug_->append(PrintDistribution(*coll_distri_, 4));
  }

  return SummarizeIncremental(collection_, options, strategy_, summary);
}

struct SecondValueGreater {
  bool operator() (const pair<const Sentence*, double>& a,
                   const pair<const Sentence*, double>& b) {
    return a.second < b.second;
  }
};

namespace {

// Decides how many words from the language model to retain for
// checking whether a sentence is relevant and for redundancy removal.
// This value is currently set to the length limit of the summary.
// For lengths expressed in sentences, for the moment, a sentence
// is hardcoded to contain on average 15 words. This is not set as a flag
// as it is a very specific setting with limited effect.
int NumTopTopicModelWords(int length_limit,
                          int length_unit) {
  if (length_unit == SummaryLength::TOKEN) {
    return length_limit;
  } else if (length_unit == SummaryLength::SENTENCE) {
    return length_limit * 15;
  } else {
    LOG(FATAL) << "Unsupported length unit: " << length_unit;
  }
}

struct OrderBySecond {
  template <class P>
  bool operator()(const P& a, const P& b) const {
    return a.second < b.second;
  }
};

}  // anonymous namespace

// The greedy approach suggested in Haghighi's NAACL'09 paper consists in
// iteratively adding sentences minimizing the KL divergence.
bool KLSum::SummarizeIncremental(const DocumentCollection& collection,
                                 const SummaryOptions& options,
                                 KLSumOptions::OptimizationStrategy strategy,
                                 Document* summary) {
  int length_limit = options.length().length();
  int length_unit = options.length().unit();

  // Generate a lexicon for the collection.
  Lexicon lexicon;
  for (int d = 0; d < collection.document_size(); d++) {
    const Document& doc = collection.document(d);
    for (int s = 0; s < doc.sentence_size(); s++) {
      const Sentence& sent = doc.sentence(s);
      for (int w = 0; w < sent.token_size(); w++) {
        const string& word = sent.token(w);
        lexicon.add_token(word);
      }
    }
  }

  // For all the sentences in the collection, represent them as
  // VectorDistributions.
  vector<VectorDistribution*> sentences;
  vector<int> doc_positions;
  vector<int> prior_scores;
  int num_tokens = lexicon.size();
  for (int d = 0; d < collection.document_size(); d++) {
    const Document& doc = collection.document(d);
    for (int s = 0; s < doc.sentence_size(); s++) {
      const Sentence& sent = doc.sentence(s);

      // If the sentence will not add any information, we skip it.
      if (sent.token_size() == 0) continue;

      if (postprocessor_.get() != NULL &&
          !postprocessor_->IsValidSentence(sent))
        continue;

      VectorDistribution* sentence_distribution =
          new VectorDistribution(num_tokens);
      sentence_distribution->sentence = &sent;
      sentence_distribution->document = &doc;
      for (int w = 0; w < sent.token_size(); w++) {
        const string& word = sent.token(w);
        int id = lexicon.token2id(word);
        sentence_distribution->distribution[id]++;
      }
      sentences.push_back(sentence_distribution);
      prior_scores.push_back(sent.prior_score());

      doc_positions.push_back(s+1);
    }
  }

  // Generate the VectorDistribution for the collection.
  VectorDistribution collection_distribution(num_tokens);
  vector<double> scores;
  scores.reserve(coll_distri_->size());
  double sum_collection = 0;
  for (Distribution::const_iterator iter = coll_distri_->begin();
       iter != coll_distri_->end();
       ++iter) {
    if (iter->second > 0.0) {
      int id = lexicon.token2id(iter->first);
      // Check that the collection token exists in the test collection.
      if (id != -1) {
        collection_distribution.distribution[id] = iter->second;
        scores.push_back(iter->second);
        sum_collection += iter->second;
      }
    }
  }

  // If the collection distribution has no non-zero values, then we return.
  if (sum_collection == 0) {
    summary->mutable_debug_string()->append(
        "Collection Distribution is empty.\n");
    return false;
  }

  // Normalize the collection_distribution.
  if (sum_collection != 1) {
    for (int k = 0; k < num_tokens; ++k)
      collection_distribution.distribution[k] /= sum_collection;
  }

  // The redundancy removal library checks how many of the top words in the
  // collection distribution have so far appeared in the summary. The
  // number of top collection words to use for redundancy removal is computed
  // here. The longer the summary, the more collection words are allowed
  // to appear in it.
  int top_words_threshold_position =
      scores.size() - NumTopTopicModelWords(length_limit, length_unit);
  if (top_words_threshold_position < 0) top_words_threshold_position = 0;
  nth_element(scores.begin(),
              scores.begin() + top_words_threshold_position,
              scores.end());
  collection_distribution.top_words_threshold =
      scores[top_words_threshold_position];

  // Create an empty VectorDistribution for the summary.
  VectorDistribution summary_distribution(num_tokens);

  while (!DocumentUtils::ReachesLengthLimit(length_limit,
                                            length_unit,
                                            *summary)) {
    int current_best_sentence = -1;
    double current_best_kl = -1;

    // To store the normalized summary vector.
    VectorDistribution normalized(num_tokens);

    vector<pair<const VectorDistribution*, double> > scored_sentences;
    for (uint i = 0; i < sentences.size(); ++i) {
      // If the sentence has been already selected for the summary, it has
      // been set to NULL so that we do not select it twice.
      if (sentences[i] == NULL) continue;

      // Calculate and normalize the summary distribution.
      double sum = summary_distribution.AddVector(*sentences[i]);
      for (int k = 0; k < num_tokens; ++k) {
        normalized.distribution[k] = summary_distribution.distribution[k] / sum;
      }

      // Calculate the divergence.
      double kl = CalculateKLDivergence(collection_distribution,
                                        normalized, -1);

      if (sentence_position_)
        kl = kl * (1 + log(doc_positions[i]));

      kl = summary_weight_ * kl + prior_weight_ * prior_scores[i];

      // Check if this is the best value so far.
      if ((current_best_kl == -1) || (kl < current_best_kl)) {
        current_best_sentence = i;
        current_best_kl = kl;
      }
      scored_sentences.push_back(make_pair(sentences[i], kl));

      // Substract term frequency from distribution to restore the summary.
      summary_distribution.SubstractVector(*sentences[i]);
    }

    // If the caller is just expecting a ranking of sentences instead of the
    // greedy approach, sort them and *return* them without doing any more
    // iterations.
    if (strategy == KLSumOptions::SENTENCE_RANKING) {
      sort(scored_sentences.begin(), scored_sentences.end(),
           OrderBySecond());
      for (uint i = 0; i < scored_sentences.size(); ++i) {
        PostprocessAndAddSentence(*(scored_sentences[i].first->sentence),
                                  scored_sentences[i].second,
                                  *(scored_sentences[i].first->document),
                                  summary);
        if (DocumentUtils::ReachesLengthLimit(length_limit,
                                              length_unit,
                                              *summary))
          break;
      }

      for (vector<VectorDistribution*>::iterator it = sentences.begin();
           it != sentences.end();
           ++it) {
        delete *it;
      }

      return true;
    }

    // This condition means that all the sentences in the input collection have
    // been added to the summary. In this case, stop growing the summary.
    if (current_best_sentence == -1) break;

    // Check whether this sentence is redundant.
    string redundancy_debug;
    if (redundancy_removal_ && IsRedundant(summary_distribution,
                                           collection_distribution,
                                           *sentences[current_best_sentence],
                                           &redundancy_debug)) {
      if (output_debug_) {
        output_debug_->append("\nRemoved redundant sentence: ");
        output_debug_->append(
            sentences[current_best_sentence]->sentence->raw_content());
        output_debug_->append(" (" + redundancy_debug + ")");
      }
    } else {
      Sentence* new_summary_sentence = PostprocessAndAddSentence(
          *(sentences[current_best_sentence]->sentence),
          current_best_kl,
          *sentences[current_best_sentence]->document,
          summary);
      if (new_summary_sentence != NULL) {
        // Update the summary distribution with the sentence that has just been
        // added. Note that even if the postprocessor removed some tokens, they
        // are still added to the distribution as it is hard to know here which
        // ones were removed.
        summary_distribution.AddVector(*sentences[current_best_sentence]);
        if (output_debug_) {
          ostringstream oss;
          oss << "\nAdded sentence: "
              << sentences[current_best_sentence]->sentence->raw_content()
              << "(" << current_best_kl << ", " << redundancy_debug << ")" ;
          output_debug_->append(oss.str());
        }
      }
    }

    // Remove the sentence that has been selected from the list of candidates.
    delete sentences[current_best_sentence];
    sentences[current_best_sentence] = NULL;
  }

  for (vector<VectorDistribution*>::iterator it = sentences.begin();
        it != sentences.end();
        ++it) {
    delete *it;
  }

  return true;
}

Sentence* KLSum::PostprocessAndAddSentence(const Sentence& added_sentence,
                                           double score,
                                           const Document& source_document,
                                           Document* summary) {
  // Try to post-process the sentence, to clean it up, compress it or
  // filter it out if it is judged not good enough in terms of quality.
  Sentence postprocessed;
  bool sentence_was_postprocessed = false;
  if (postprocessor_.get() != NULL) {
    postprocessed.CopyFrom(added_sentence);
    sentence_was_postprocessed = postprocessor_->Compress(&postprocessed);
  }

  // Unless the sentence was filtered out by the post-processor, add it to
  // the summary.
  Sentence* new_summary_sentence = NULL;
  if (!sentence_was_postprocessed) {
    new_summary_sentence = DocumentUtils::AddSentence(added_sentence, summary);
  } else if (!postprocessed.raw_content().empty()) {
    new_summary_sentence = DocumentUtils::AddSentence(postprocessed, summary);
  } else if (output_debug_) {
    output_debug_->append("\nRemoved sentence: " + added_sentence.raw_content());
  }

  if (new_summary_sentence != NULL) {
    new_summary_sentence->set_score(score);
    if (source_document.has_id())
      new_summary_sentence->set_source_document(source_document.id());
  }

  return new_summary_sentence;
}

}  // namespace topicsum
