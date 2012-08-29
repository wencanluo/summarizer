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

#include "summarizer/sampling.h"

#include "gtest/gtest.h"
#include "summarizer/logging.h"

namespace topicsum {

static const int kNumberTrials = 10000;

TEST(Sampling, SampleMulti) {
  double distri[5] = {0.1, 0.1, 0.1, 0.1, 0.5};
  int sampled[5] = {0};
  for (int i = 0; i < kNumberTrials; i++) {
    int sample = SampleMulti(distri, 5);
    sampled[sample]++;
  }

  for (int i = 0; i < 5; i++) {
    int min_ok = static_cast<int>(distri[i] * kNumberTrials / 2);
    EXPECT_LE(min_ok, sampled[i]);
  }
}

TEST(Sampling, SampleUniform) {
  int sampled[10] = {0};
  for (int i = 0; i < kNumberTrials; i++) {
    int sample = SampleUniform(10);
    sampled[sample]++;
  }

  int min_ok = static_cast<int>(0.1 *kNumberTrials / 2);
  for (int i = 0; i < 10; i++) {
    EXPECT_LE(min_ok, sampled[i]);
  }
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
