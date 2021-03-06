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

// Interface that describes a summarizer (see summarizers/*)

#ifndef SUMMARIZER_SUM_H_
#define SUMMARIZER_SUM_H_

#include "summarizer/distribution.h"
#include "summarizer/scoped_ptr.h"
#include "summarizer/summarizer.pb.h"
#include "summarizer/types.h"

namespace topicsum {

class Summarizer {
 public:
  // Destroys the summarizer.
  virtual ~Summarizer() {}

  // Summarizes a collection of documents according to stores
  // the result in a document.
  // Returns false if the collection could not be summarized.
  virtual bool Summarize(const SummaryOptions& options,
                         Document* summary) = 0;

  // Initializes the summarizer with given options.
  // Returns false if the summarizer could not be initialized.
  virtual bool Init(const DocumentCollection& collection,
                    const SummarizerOptions& options) { return true; }

  // Return the last error message generated by the summarizer
  // during initialization or summarization.
  const string& last_error_message() const { return last_error_message_; }

 protected:
  // Creates an empty summarizer.
  Summarizer() {}

  string last_error_message_;

 private:
  Summarizer(const Summarizer&);
  void operator=(const Summarizer&);
};

class SummarizerUtils {
 public:
  // Parse a string representing a vector of double values and creates
  // an array of doubles containing the values. Puts the number
  // of parsed values in num_values.
  static void ParseStringVectorAsDoubleVector(const string& str_values,
                                              scoped_array<double>* values,
                                              int* num_values);

};

}  // namespace topicsum

#endif  // SUMMARIZER_SUM_H_
