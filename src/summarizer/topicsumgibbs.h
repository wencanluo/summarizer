// Copyright 2011 Google Inc. All Rights Reserved.
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

// Implements a Gibbs sampling algorithm to estimate parameters of TopicSum.
//
// TopicSum is a summarization technique based on topic-models. This class
// provides an implementation of a Gibbs sampler to estimate the parameters of
// the model. Details of the model can be found in the original paper:
// Exploring Content Models for Multi-Document Summarization, NAACL 2009.
// (link to the paper: http://www.aria42.com/pubs/naacl09-topical.pdf)

#ifndef SUMMARIZER_TOPICSUMGIBBS_H_
#define SUMMARIZER_TOPICSUMGIBBS_H_

#include <map>
#include <string>
#include <vector>

#include "summarizer/distribution.h"
#include "summarizer/document.pb.h"
#include "summarizer/gibbs.h"
#include "summarizer/lexicon.h"
#include "summarizer/scoped_ptr.h"

namespace topicsum {

class TopicSumGibbsSampler : public GibbsSampler {
 public:
  // Creates a TopicSumGibbsSampler.
  // The TopicSum specific parameters are:
  // - lambda: hyper-parameters for each topic distribution
  // - gamma: hyper-parameters for distribution of topics in each sentence
  // - collections: collections used for training the sampler.
  // - background: optional argument. If not NULL, it contains the
  //               background distribution and it will not change
  //               during sampling.
  TopicSumGibbsSampler(int iterations,
                       int burnin,
                       int lag,
                       bool track_likelihood,
                       double lambda[3],
                       double gamma[3],
                       const vector<const DocumentCollection*>& collections,
                       const Distribution* background);

  // Destroys the TopicSumGibbsSampler.
  virtual ~TopicSumGibbsSampler() { Reset(); }

  // Returns the probability distribution of words estimated for the topic of
  // the collection colid. The result is stored in wd.
  void GetCollectionDistribution(int colid,
                                 Distribution* wd) const;

  // Returns the posterior probability distribution of words in collections,
  // i.e. the probability of belong to a specific collection given that we
  // have seen the word.
  //
  // Returns false if the term is not in the vocabulary.
  bool GetPosteriorDistribution(string term,
                                Distribution* cd) const;

  // Returns the probability distributions of words estimated in the BKG topic.
  void GetBackgroundDistribution(Distribution* wd) const;

  // Returns the topic assignments for all the words.
  int**** GetTopicAssignments() { return lastZ_; }

  const Lexicon& GetLexicon() { return *lexicon_.get(); }

  // Prints information on the sampler and the currently trained model.
  virtual string PrintSamplerInfo();

  // Prints information on the sampler and the model at the current interation.
  virtual string PrintSamplingIterationInfo();

 protected:
  // Initializes the sampler which includes initializing some counters and
  // allocating memory to store 1) the counters used by the samplers, 2) the
  // sampled values and the estimate parameters.
  virtual void Init();

  // This function does the initial assignment, it may be random or based
  // on some knowledge about the data.
  virtual void DoInitialAssignment();

  // This function implements what typically needs to be done during one
  // iteration, i.e. decrementing the counters, calculating the conditional
  // probability, sampling a new value and incrementing the counters.
  virtual void DoIteration();

  // Estimate the model parameters based on the last sampled values and the
  // counters.
  virtual void CalculateParams();

  // Multiplies model parameters to the given value.
  // TODO(jydelort): this is repeated in dualsumgibb. Move this to the base
  // class?
  virtual void MultiplyParams(double multiplier);

  // Calculates and returns the likelihood based on the current values of the
  // estimated parameters.
  virtual double CalculateModelLikelihood() const;

 private:
  // Destroys data structures created on the heap used by the sampler.
  void Reset();

  // Calculates the likelihood of sentence identified by a triplet
  // [colid, docid, sentid].
  double CalculateSentenceLikelihood(int colid, int docid, int sentid) const;


  // Implements the Increment function used in the Gibbs Sampling algorithm.
  void IncrementCounters(int colid,
                         int docid,
                         int sentid,
                         int wordid,
                         int topic);

  // Implements the Decrement function used in the Gibbs Sampling algorithm.
  void DecrementCounters(int colid,
                         int docid,
                         int sentid,
                         int wordid,
                         int topic);

  // Generates the distribution (P(z_i=j|z_{-i}, w))_j and stores it in distri.
  void GenerateConditionalDistribution(int colid,
                                       int docid,
                                       int sentid,
                                       int wordid,
                                       double* distri);

 private:
  // The following are the hyperparameters for the algorithm. In
  // TopicSum there are several Dirichlet distributions: the topic assignment
  // for sentences, gamma, (which may come from the background distribition,
  // the collection or the document), and the word assignment for the different
  // topics (the BG, Collection and Document distribution, each of which has
  // one hyperparameter), corresponding to lambda.

  // Total number of possible topics per sentence
  // (Note: this differs from the actual number of topics which is 1+C_+D_)
  static const int K_ = 3;

  // Hyper-parameter for topic (0:BKG, 1:COL, 2: DOC).
  double lambda_[3];

  // Sum of all lambda hyper-parameters.
  double lambda_sum_;

  // Hyper-parameter for sentences (0:BKG, 1:COL, 2: DOC).
  double gamma_[3];

  // Sum of all gamma hyper-parameters.
  double gamma_sum_;


  // Total number of collections.
  int C_;

  // Total number of documents.
  int D_;

  // Total number of sentences.
  int S_;

  // Vocabulary size (unique tokens).
  int W_;

  // Total number of words (non-unique tokens).
  int AW_;

  // Size of each sentence in each document and collection.
  int*** NS_;

  // Frequency distribution of words in the BKG topic.
  int* NWB_;

  // Frequency distribution of words in each COL topic.
  int** NWC_;

  // Frequency distribution of words in each DOC topic.
  int*** NWD_;

  // Number of words in the BKG topic.
  int NB_;

  // Numbers of words in each COL topic.
  int* NC_;

  // Numbers of words in each DOC topic.
  int** ND_;

  // Frequency distribution of words in each topic for each sentence.
  // COLID -> DOCID -> SENTID -> TOPIC PROPORTION
  int**** NZS_;

  // Probability distribution of words in the BKG topic.
  double* phi_B_;

  // Probability distribution of words in each COL topic.
  double** phi_C_;

  // Probability distribution of words in each DOC topic.
  double*** phi_D_;

  // Probability distribution of topics for each sentence.
  double**** psi_;

  // Dictionnary of words in the training collections.
  scoped_ptr<Lexicon> lexicon_;

  // Training collections.
  vector<const DocumentCollection*> training_cols_;

  // Last sampled assignment.
  int**** lastZ_;

  // If not null, a fixed background distribution.
  const Distribution* background_;

  TopicSumGibbsSampler(const TopicSumGibbsSampler&);
  void operator=(const TopicSumGibbsSampler&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_TOPICSUMGIBBS_H_
