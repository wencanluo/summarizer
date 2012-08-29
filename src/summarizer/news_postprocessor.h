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

// Sentence post-processor to be applied to Google News summaries.

#ifndef SUMMARIZER_NEWS_POSTPROCESSOR_H_
#define SUMMARIZER_NEWS_POSTPROCESSOR_H_

#include "summarizer/document.pb.h"
#include "summarizer/postprocessor.h"

namespace topicsum {

class NewsPostprocessor : public Postprocessor {
 public:
  NewsPostprocessor();
  virtual ~NewsPostprocessor() {}

  // Reads from the options the conditions to consider some sentences as
  // invalid.
  virtual void Init(const SummarizerOptions& options);

  virtual bool IsValidSentence(const Sentence& sentence) const;
  virtual bool Compress(Sentence* sentence) const;

 private:
  int min_sentence_length_;
  int max_sentence_length_;

  NewsPostprocessor(const NewsPostprocessor&);
  void operator=(const NewsPostprocessor&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_NEWS_POSTPROCESSOR_H_
