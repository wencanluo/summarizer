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

// Implementats of the TopicSum summarization techniq (NAACL2009, Haghighi)
//
// TopicSum differs from KLSum by using Gibbs Sampling to learn the Collection
// Topic which is used as an estimate of the collection distribution.

#ifndef SUMMARIZER_TOPICSUM_H_
#define SUMMARIZER_TOPICSUM_H_

#include <string>
#include <vector>

#include "summarizer/klsum.h"
#include "summarizer/scoped_ptr.h"
#include "summarizer/topicsumgibbs.h"

namespace topicsum {

class DocumentCollection;
class SummarizerOptions;

class TopicSum : public KLSum {
 public:
  TopicSum() : KLSum() {}
  virtual ~TopicSum() {}

  // Initializes the summarizer with given options.
  bool Init(const DocumentCollection& collection,
            const SummarizerOptions& options);

  // Calculates the distribution of tokens of the collection. The function is
  // called by other summarizers that wrap TopicSum, such as KLSum and
  // DoubleTopicSum. The collection to summarize needs to be among the training
  // collections.
  virtual bool CalculateCollectionDistribution(const DocumentCollection& col);

 private:
  scoped_ptr<TopicSumGibbsSampler> gibbs_sampler_;

  TopicSum(const TopicSum&);
  void operator=(const TopicSum&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_TOPICSUM_H_
