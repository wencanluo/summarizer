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

#include "summarizer/distribution.h"

#include <algorithm>
#include <iomanip>
#include <list>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "summarizer/distribution.pb.h"
#include "summarizer/logging.h"

namespace topicsum {

using std::fixed;
using std::ostringstream;
using std::pair;
using std::setprecision;
using std::sort;
using std::string;
using std::vector;

struct SecondValueGreater {
  bool operator() (const pair<string, double>& a,
                   const pair<string, double>& b) {
    return a.second > b.second;
  }
};

string PrintDistribution(const Distribution& distri, int precision) {
  ostringstream oss;

  // Set precision.
  oss << fixed << setprecision(precision);

  int num_labels = distri.size();
  int label_count = 0;

  // Sort keys based on values (Descending order).
  std::vector<pair<string, double> > myvec(distri.begin(), distri.end());
  sort(myvec.begin(), myvec.end(), SecondValueGreater());

  // Print all labels with their associated probabilities.
  for (vector<pair<string, double> >::const_iterator iter = myvec.begin();
       iter != myvec.end();
       ++iter) {
    const string& w = iter->first;
    double val = iter->second;

    // Round the value to precision.
    val = round(val * pow(10, precision)) / pow(10, precision);

    // Break if rounded value is equal to 0, as they are sorted and no
    // further entry will have a non-zero rounded value.
    if (val == 0)
      break;

    oss << w << "\t" << val;

    // Add newline if there are more lines to come.
    if (++label_count < num_labels)
      oss << "\n";
  }

  return oss.str();
}

void ToProto(const Distribution& distri, DistributionProto* proto) {
  // Sort keys based on values (Descending order).
  vector<pair<string, double> > myvec(distri.begin(), distri.end());
  sort(myvec.begin(), myvec.end(), SecondValueGreater());

  for (vector<pair<string, double> >::const_iterator iter = myvec.begin();
       iter != myvec.end();
       ++iter) {
    DistributionEntry* entry = proto->add_entry();
    entry->set_word(iter->first);
    entry->set_value(iter->second);
  }
}

void FromProto(const DistributionProto& proto, Distribution* distri) {
  distri->clear();
  for (int i = 0; i < proto.entry_size(); ++i) {
    const DistributionEntry& entry = proto.entry(i);
    distri->insert(make_pair(entry.word(), entry.value()));
  }
}

void NormalizeDistribution(Distribution* distri) {
  CHECK(distri != NULL);

  double sum = 0;
  for (Distribution::const_iterator iter = distri->begin();
       iter != distri->end();
       ++iter) {
    sum += iter->second;
  }

  if ((sum > 0) && (sum != 1)) {
    for (Distribution::iterator iter = distri->begin();
         iter != distri->end();
         ++iter) {
      iter->second = iter->second / sum;
    }
  }
}

}  // namespace topicsum
