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

#include "summarizer/sampling.h"

#include <math.h>
#include <stdlib.h>

#include "summarizer/scoped_ptr.h"

namespace topicsum {

int SampleMulti(const double* distri, int distri_size) {
  double sum = 0;
  int klass = -1;
  double x = random() / static_cast<double>(RAND_MAX);

  for (int i = 0; i < distri_size; i++) {
    if (x <= sum + distri[i]) {
      klass = i;
      break;
    } else {
      sum += distri[i];
    }
  }
  if (klass == -1)
    klass = distri_size - 1;

  return klass;
}

int SampleUniform(int K) {
  double u = 1 / static_cast<double>(K);
  double x = random() / static_cast<double>(RAND_MAX);
  int sampled = static_cast<int>(x / u);
  return (sampled < K ? sampled : K-1);
}

}  // namespace topicsum
