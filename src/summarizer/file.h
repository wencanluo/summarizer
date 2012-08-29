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

#ifndef SUMMARIZER_COMMON_FILE_H_
#define SUMMARIZER_COMMON_FILE_H_

#include <string>

namespace topicsum {

using std::string;

class File {
 public:
  // Read an entire file to a string.  Return true if successful, false
  // otherwise.
  static bool ReadFileToString(const string& name, string* output);

  // Same as above, but crash on failure
  static void ReadFileToStringOrDie(const string& name, string* output);

 protected:
  File(const File&);
  void operator=(const File&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_COMMON_FILE_H_
