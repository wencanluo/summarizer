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

// Simple postprocessor to be used in unittests, that removes every sentence
// containing a zero digit.

#ifndef SUMMARIZER_TEST_POSTPROCESSOR_H_
#define SUMMARIZER_TEST_POSTPROCESSOR_H_

#include "summarizer/document.pb.h"
#include "summarizer/postprocessor.h"

namespace topicsum {

class TestPostprocessor : public Postprocessor {
 public:
  TestPostprocessor() {}
  virtual ~TestPostprocessor() {}

  virtual bool IsValidSentence(const Sentence& sentence) const { return true; }
  virtual bool Compress(Sentence* sentence) const;

 private:
  TestPostprocessor(const TestPostprocessor&);
  void operator=(const TestPostprocessor&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_TEST_POSTPROCESSOR_H_
