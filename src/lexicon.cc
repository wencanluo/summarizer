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

#include "summarizer/lexicon.h"

#include <utility>

#include "summarizer/logging.h"

namespace topicsum {

Lexicon::Lexicon() : size_(0) {
  id2token_.clear();
  token2id_.clear();
}

Lexicon::~Lexicon() {
}

bool Lexicon::has_token(const string& token) const {
  return (token2id_.find(token) != token2id_.end());
}

int Lexicon::add_token(const string& token) {
  int id = -1;
  hash_map<string, int>::const_iterator iter = token2id_.find(token);
  // if the token is not in the lexicon, adds it, otherwise returns its
  // current id.
  if (iter == token2id_.end()) {
    id = size_;
    token2id_.insert(pair<string, int>(token, id));
    id2token_.push_back(token);
    size_++;
  } else {
    id = iter->second;
  }
  return id;
}

void Lexicon::add_tokens(const vector<string>& tokens) {
  for (vector<string>::const_iterator iter = tokens.begin();
       iter != tokens.end(); ++iter)
    add_token(*iter);
}

int Lexicon::size() const {
  return size_;
}

int Lexicon::token2id(const string& token) const {
  hash_map<string, int>::const_iterator it = token2id_.find(token);
  if (it == token2id_.end())
    return -1;
  else
    return it->second;
}

const string& Lexicon::id2token(int id) const {
  // It is an error if the token does not exist.
  CHECK_LT(id, size_);
  return id2token_[id];
}

}  // namespace topicsum
