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

// Functions for generating random values for various types of distributions.
//
// Some of these functions depend on the random function in <math.h>, but do not
// set the seed themselves. So srandom should be called before calling the
// functions.

#ifndef SUMMARIZER_SAMPLING_H_
#define SUMMARIZER_SAMPLING_H_

namespace topicsum {

// Returns a value between 0..K-1, randomly sampled with uniform probability.
int SampleUniform(int K);

// Returns a value between 0..distri_size-1, sampled from the multinomial
// distribution distri.
int SampleMulti(const double* distri, int distri_size);

}  // namespace topicsum

#endif  // SUMMARIZER_SAMPLING_H_
