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

// A lexicon is a dictionnary of words linking tokens to ids.

#ifndef SUMMARIZER_LEXICON_H_
#define SUMMARIZER_LEXICON_H_

#include <string>
#include <vector>
#include "summarizer/hash.h"

namespace topicsum {

using std::vector;
using std::string;

class Lexicon {
 public:
  // Creates an empty lexicon.
  Lexicon();

  // Destroys a lexicon.
  ~Lexicon();

  // Verifies if a token exists in the lexicon.
  bool has_token(const string& token) const;

  // Adds a new token to the lexicon and returns its id. If the token already
  // exists, only returns its id.
  int add_token(const string& token);

  // Adds several tokens to the lexicon.
  void add_tokens(const vector<string>& tokens);

  // Returns the number of tokens in the lexicon.
  int size() const;

  // Returns the id of the given token or -1 if it is not in the lexicon.
  int token2id(const string& token) const;

  // Returns a reference to the token of the given id. The id must be valid
  // before the call.
  const string& id2token(int id) const;

 private:
  hash_map<string, int> token2id_;
  vector<string> id2token_;
  int size_;

  Lexicon(const Lexicon&);
  void operator=(const Lexicon&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_LEXICON_H_
