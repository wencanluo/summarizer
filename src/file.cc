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

#include "file.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace topicsum {

using std::cerr;

bool File::ReadFileToString(const string& name, string* output) {
  char buffer[1024];
  FILE* file = fopen(name.c_str(), "rb");
  if (file == NULL) return false;

  while (true) {
    size_t n = fread(buffer, 1, sizeof(buffer), file);
    if (n <= 0) break;
    output->append(buffer, n);
  }

  int error = ferror(file);
  if (fclose(file) != 0) return false;
  return error == 0;
}

void File::ReadFileToStringOrDie(const string& name, string* output) {
  if (!ReadFileToString(name, output)) {
    cerr << "Could not read: " << name;
    abort();
  }
}

}  // namespace topicsum
