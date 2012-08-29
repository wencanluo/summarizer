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

// Interface that describes a sentence postprocessor, to compress and/or
// discard sentences as they are selected in summaries, in the function
// Summarizer::Summarize().

#ifndef SUMMARIZER_POSTPROCESSOR_H_
#define SUMMARIZER_POSTPROCESSOR_H_

#include "summarizer/distribution.h"
#include "summarizer/document.h"

namespace topicsum {

class SummarizerOptions;

class Postprocessor {
 public:
  Postprocessor() {}
  virtual ~Postprocessor() {}

  // Initializes internal options in the postprocessor. The default
  // implementation does nothing.
  virtual void Init(const SummarizerOptions& options) {}

  // Indicates whether a sentence is good enough to appear in a summary.
  // If the output is 'false', the summarizers should not even consider this
  // sentence.
  virtual bool IsValidSentence(const Sentence& sentence) const = 0;

  // Compresses a sentence removing unused parts of it.
  //
  // If the sentence is not judged good to belong to a summary, it is completely
  // erased. This can be checked by checking if raw_content() is empty or there
  // are zero tokens after Compress is called.
  //
  // Returns true if the sentence was modified in any way.
  virtual bool Compress(Sentence* sentence) const = 0;

 private:
  Postprocessor(const Postprocessor&);
  void operator=(const Postprocessor&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_POSTPROCESSOR_H_
