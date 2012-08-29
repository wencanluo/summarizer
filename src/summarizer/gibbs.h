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

// Abstract class of a Gibbs Sampler.
//
// The training algorithm is implemented in this class, deriving
// classes only have to implement the main functions of the algorithm.

#ifndef SUMMARIZER_GIBBS_H_
#define SUMMARIZER_GIBBS_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "summarizer/scoped_ptr.h"

namespace topicsum {

using std::pair;
using std::string;
using std::vector;

class GibbsSampler {
 public:
  // Initializes Sampler for training. The user needs to specify:
  // - the number of burnin iterations during training
  // - the number of iterations during training
  // - the lag between iterations before recalculating the model parameters
  // - a flag indicating if the model likelihood should be calculated
  //   and recorded after each training iteration.
  //
  // Generally it is nice to calculate the model likelihood after each
  // iteration. It allow us to see how the likelihood improves by iterating
  // and to see when the likelihood seems to have converged.
  // However, it also implies a significant overload, since not only the
  // likelihood needs to be calculated but also the model parameters.
  GibbsSampler(int iterations, int burnin, int lag, bool track_likelihood)
      : iterations_(iterations),
        burnin_(burnin),
        lag_(lag),
        track_likelihood_(track_likelihood),
        iteration_no_(-1),
        sum_count_(0)
        {}

  // Destroys the GibbSampler.
  virtual ~GibbsSampler() {}

  // Trains the sampler tracking (or not) the model likelihood after
  // each iteration.
  void Train();

  // Returns a reference to a vector with the recorded model likelihoods.
  // The result is a pair <iteration_no, likelihood>
  const vector<pair<int, double> >& saved_likelihoods() const;

  // Prints information on the sampler and the currently trained model
  virtual string PrintSamplerInfo();

  // Prints information on the sampler and the model at the current interation
  virtual string PrintSamplingIterationInfo();

  // Prints current parameters.
  virtual string PrintParams() { return ""; }

 protected:
  // Initializes the Sampler.
  virtual void Init() = 0;

  // Does the initial assignment, it may be random or based
  // on some knowledge about the data.
  virtual void DoInitialAssignment() = 0;

  // Implements what typically needs to be done during one
  // iteration, i.e. decrementing the counters, calculating the conditional
  // probability, sampling a new value and incrementing the counters.
  virtual void DoIteration() = 0;

  // Estimates the model parameters for the current sample values and
  // adds them up to previous estimations.
  virtual void CalculateParams() = 0;

  // Multiplies model parameters to given value.
  virtual void MultiplyParams(double multiplier) = 0;

  // Calculates and returns the likelihood for the current model parameters.
  virtual double CalculateModelLikelihood() const = 0;

 private:
  // Number of iterations to perform.
  int iterations_;

  // Number of burning iterations to perform before sampling.
  int burnin_;

  // Lag between iterations to avoid choosing correlated samples when
  // calculating the model parameters.
  int lag_;

  // If this value is true, the sampler will calculate the model likelihood
  // every time the model parameters have been re-estimated.
  bool track_likelihood_;

  // Current iteration number. The number includes "normal" and "burnin"
  // iterations.
  int iteration_no_;

  // The total number of times the model parameters have been estimated
  // (ie the number of times CalculateParams has been called). It depends on the
  // lag, the number of burnin iterations and the number of iterations.
  int sum_count_;

  // Stores model likelihoods.
  vector<pair<int, double> > saved_likelihoods_;

  GibbsSampler(const GibbsSampler&);
  void operator=(const GibbsSampler&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_GIBBS_H_
