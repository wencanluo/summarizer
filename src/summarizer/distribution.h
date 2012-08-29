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

// Functions and structures for handling distributions.
//
// Functions and structures for handling distributions both probabilistic and
// frequentist. Also provides some functions to calculate term distributions
// such as Term-Frequency, Document-Frequency over sentences, documents and
// collections.

#ifndef SUMMARIZER_DISTRIBUTION_H_
#define SUMMARIZER_DISTRIBUTION_H_

#include <iterator>
#include <string>

#include "summarizer/hash.h"

namespace topicsum {

class DistributionProto;

// A convenient alias to represent a distribution over a set of labels.
// This structure can support both frequency distributions,
// probability distributions, etc.
typedef hash_map<string, double> Distribution;

// Normalizes the distribution to make sure that sum of its values equals to 1.
void NormalizeDistribution(Distribution* distri);

// Returns a string representing a distribution. The precision indicates the
// precision of the probability values (the number of decimal digits).
string PrintDistribution(const Distribution& distri, int precision);

// Converts the distribution to a protocol buffer for easy serialization.
// The entries will be added sorted by value.
void ToProto(const Distribution& distri, DistributionProto* proto);

// Converts the distribution to a protocol buffer for easy serialization.
// The entries will be added sorted by value.
void FromProto(const DistributionProto& proto, Distribution* distri);

}  // namespace topicsum

#endif  // SUMMARIZER_DISTRIBUTION_H_
