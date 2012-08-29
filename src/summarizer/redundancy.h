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

// A class used for detecting and removing redundancy from summaries.

#ifndef SUMMARIZER_REDUNDANCY_H_
#define SUMMARIZER_REDUNDANCY_H_

#include "summarizer/klsum.h"

namespace topicsum {

// Trivial implementation of redundancy detection. Given the words that
// have already selected for the summary, the words that are highly relevant
// in the collection model, and a new sentence, the sentence is considered
// redundant it if either:
// - Does not add to the summary enough new collection words.
// - The percentage of sentence words that are new collection words in the
//   summary is less than a certain percentage of the sentence.
bool IsRedundant(const VectorDistribution& summary,
                 const VectorDistribution& collection,
                 const VectorDistribution& new_sentence,
                 string* debug);

}  // namespace topicsum

#endif  // SUMMARIZER_REDUNDANCY_H_
