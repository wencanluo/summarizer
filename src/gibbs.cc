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

#include "summarizer/gibbs.h"

#include <sstream>

namespace topicsum {

using std::make_pair;
using std::ostringstream;

const vector<pair<int, double> >& GibbsSampler::saved_likelihoods() const {
  return saved_likelihoods_;
}

string GibbsSampler::PrintSamplerInfo() {
  ostringstream oss;
  oss << "Sampler configuration:\n";
  oss << "\tNumber of iterations: " << iterations_ << "\n";
  oss << "\tNumber of burnin iterations: " << burnin_ << "\n";
  return oss.str();
}

string GibbsSampler::PrintSamplingIterationInfo() {
  ostringstream oss;
  oss << "Iteration info:\n";
  oss << "Iteration number: " << iteration_no_ << "\n";
  return oss.str();
}

void GibbsSampler::Train() {
  // Initialize data structures (counters...).
  Init();

  // Sampling initialization.
  iteration_no_ = -1;
  DoInitialAssignment();

  // Core sampling algorithm.
  bool burnin = (burnin_ > 0 ? true : false);

  iteration_no_ = 0;
  while (iteration_no_ < (burnin_ + iterations_)) {
    if (burnin && iteration_no_ > burnin_)
      burnin = false;

      DoIteration();

    // Do not update estimates in burnin model and wait lag iterations
    // before estimating from the last sample.
    if (!burnin && (!lag_ || iteration_no_ % lag_ == 0)) {
      // Update model parameters.
      CalculateParams();
      sum_count_++;

      if (track_likelihood_) {
        // Calculate model likelihood. We need to average over the sum of
        // model parameters to get true estimates to use when calculating
        // the likelihood. After using the estimates, the original sums
        // are restored. We can loose a bit of precision here.
        MultiplyParams(1/static_cast<double>(sum_count_));
        double likelihood = CalculateModelLikelihood();

        MultiplyParams(sum_count_);
        saved_likelihoods_.push_back(make_pair(iteration_no_, likelihood));
       }
    }

    iteration_no_++;
  }

  // Average parameters to obtain final estimates.
  if (sum_count_ > 0)
    MultiplyParams(1/static_cast<double>(sum_count_));
}

}  // namespace topicsum
