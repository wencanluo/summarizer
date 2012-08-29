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

#include "summarizer/sum.h"

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "summarizer/logging.h"
#include "summarizer/types.h"

namespace topicsum {

using std::getline;
using std::vector;
using std::istringstream;

void SummarizerUtils::ParseStringVectorAsDoubleVector(
    const string& str_values,
    scoped_array<double>* values,
    int* num_values) {
  vector<string> str_vals;
  istringstream ss(str_values);
  string str_val;

  while (getline(ss, str_val, ',')) {
    str_vals.push_back(str_val);
  }

  values->reset(new double[str_vals.size()]);
  for (uint i = 0; i < str_vals.size(); i++) {
    double d;
    char* endptr;
    const char* str = str_vals[i].c_str();

    d = strtod(str, &endptr);
    if (endptr != str) {
      while (isspace(*endptr)) ++endptr;
    }

    CHECK(*str != '\0' && *endptr == '\0')
        << "Non-numeric value '" << str_vals[i] << "' could not be parsed "
        << "from '" << str_values << "'";
    (*values)[i] = d;
  }
  *num_values = str_vals.size();
}

}  // namespace topicsum
