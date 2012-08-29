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

#include "summarizer/topicsum.h"

#include <sstream>

#include "summarizer/sum.h"
#include "summarizer/topicsumgibbs.h"

namespace topicsum {

using std::ostringstream;

bool TopicSum::Init(const DocumentCollection& collection,
                    const SummarizerOptions& options) {
  collection_ = collection;

  // Init KLSum
  if (!KLSum::Init(collection_, options)) {
    return false;
  }

  scoped_array<double> lambda;
  scoped_array<double> gamma;
  int lambda_size = 0;
  int gamma_size = 0;

  SummarizerUtils::ParseStringVectorAsDoubleVector(
      options.topicsum_options().lambda(), &lambda, &lambda_size);

  if (!lambda_size == 3) {
    last_error_message_ = "Hyper-parameter lambda expects exactly 3 values";
    return false;
  }

  SummarizerUtils::ParseStringVectorAsDoubleVector(
      options.topicsum_options().gamma(), &gamma, &gamma_size);

  if (!gamma_size == 3) {
    last_error_message_ = "Hyper-parameter gamma expects exactly 3 values";
    return false;
  }

  // Create the sampler.
  const GibbsSamplingOptions& gibbs_options =
      options.topicsum_options().gibbs_sampling_options();

  vector<const DocumentCollection*> training_cols;
  training_cols.push_back(&collection_);

  gibbs_sampler_.reset(
      new TopicSumGibbsSampler(gibbs_options.iterations(),
                               gibbs_options.burnin(),
                               gibbs_options.lag(),
                               gibbs_options.track_likelihood(),
                               lambda.get(),
                               gamma.get(),
                               training_cols,
                               NULL));

  // Run the sampler.
  gibbs_sampler_.get()->Train();

  return true;
}

bool TopicSum::CalculateCollectionDistribution(
    const DocumentCollection& collection) {
  // Get the collection distribution from the collection.
  coll_distri_.reset(new Distribution);
  gibbs_sampler_->GetCollectionDistribution(0, coll_distri_.get());
  return true;
}

}  // namespace topicsum
