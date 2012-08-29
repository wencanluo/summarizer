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

#include "summarizer/topicsumgibbs.h"

#include "gtest/gtest.h"
#include "summarizer/file.h"
#include "summarizer/logging.h"
#include "summarizer/xml_parser.h"

namespace topicsum {

namespace {
void BuildToyCollections(vector<const DocumentCollection*>* cols) {
  XmlParser xml_parser;
  string file;

  string p1 = DATADIR "testdata/toycol1/";
  string p2 = DATADIR "testdata/toycol2/";

  DocumentCollection* col1 = new DocumentCollection();
  col1->set_id(p1);
  DocumentCollection* col2 = new DocumentCollection();
  col2->set_id(p2);

  file = "";
  File::ReadFileToStringOrDie(p1 + "d1.xml", &file);
  xml_parser.ParseDocument(file, col1->add_document());

  file = "";
  File::ReadFileToStringOrDie(p1 + "d2.xml", &file);
  xml_parser.ParseDocument(file, col1->add_document());

  file = "";
  File::ReadFileToStringOrDie(p2 + "d1.xml", &file);
  xml_parser.ParseDocument(file, col2->add_document());

  file = "";
  File::ReadFileToStringOrDie(p2 + "d2.xml", &file);
  xml_parser.ParseDocument(file, col2->add_document());

  file = "";
  File::ReadFileToStringOrDie(p2 + "d3.xml", &file);
  xml_parser.ParseDocument(file, col2->add_document());

  LOG(INFO) << col1->document(0).sentence_size();

  cols->push_back(col1);
  cols->push_back(col2);
}

void BuildRealCollections(vector<const DocumentCollection*>* cols) {
  vector<string> paths;
  XmlParser xml_parser;

  string p1 = DATADIR "testdata/realcol1/";
  string p2 = DATADIR "testdata/realcol2/";
  string p3 = DATADIR "testdata/realcol3/";
  paths.push_back(p1);
  paths.push_back(p2);
  paths.push_back(p3);

  string file;
  for (vector<string>::iterator it = paths.begin();
       it != paths.end();
       ++it) {
    DocumentCollection* col = new DocumentCollection();

    file = "";
    File::ReadFileToStringOrDie(*it + "d1.xml", &file);
    xml_parser.ParseDocument(file, col->add_document());

    file = "";
    File::ReadFileToStringOrDie(*it + "d2.xml", &file);
    xml_parser.ParseDocument(file, col->add_document());

    file = "";
    File::ReadFileToStringOrDie(*it + "d3.xml", &file);
    xml_parser.ParseDocument(file, col->add_document());

    file = "";
    File::ReadFileToStringOrDie(*it + "d4.xml", &file);
    xml_parser.ParseDocument(file, col->add_document());

    cols->push_back(col);
  }
}
}  // unnamed namespace

class ToyCollectionsTest : public testing::Test {
 protected:
  virtual void Reset() {
    topicsum_.reset(NULL);
    for (vector<const DocumentCollection*>::iterator it = cols_.begin();
         it != cols_.end();
         ++it) {
      delete *it;
    }
    cols_.clear();
  }

  virtual void TrainToy(int num_iterations, Distribution* background) {
    BuildToyCollections(&cols_);

    // Important notes on the parameters: The toy collection is built so as to
    // capture in one topic all the stopwords ("apple, "and", "the"), in one
    // collection topic (all "iphone", "ipad", "company") and in another
    // collection topic (all "fruit", "banana").
    //
    // To get this result, gamma[2] is set to 0, meaning that a word cannot come
    // from a document topic. Both the relative and absolute values of the
    // lambda and gamma parameters need to be considered.  Looking at the
    // equations given in the documentation is extremely important to understand
    // that.

    // If the absolute value of lambda is close to zero, then, not having the
    // word already assigned in the given topic significantly reduces its
    // probability to be assigned to that topic.

    // The relative values are also important, thus (gamma[COL] / gamma[BKG])
    // should be equal to the average proportion in each sentence of COL vs BKG
    // to achieve better results. And the sum of all gammas should be equal to
    // the average size of a sentence.

    // Reset seed. Necessary to be able to reproduce the experiments.
    srandom(0);

    int burnin = 0;
    int lag = 100;
    bool track_likelihood = true;

    double lambda[3] = {.1, .1, .1};
    double gamma[3] = {2, 3, 0};
    topicsum_.reset(new TopicSumGibbsSampler(num_iterations,
                                             burnin,
                                             lag,
                                             track_likelihood,
                                             lambda,
                                             gamma,
                                             cols_,
                                             background));

    topicsum_->Train();
  }

  vector<const DocumentCollection*> cols_;
  scoped_ptr<TopicSumGibbsSampler> topicsum_;
};

TEST_F(ToyCollectionsTest, ToyCollectionsTraining) {
  TrainToy(1000, NULL  /* no predefined background */);
  LOG(INFO) << "Likelihoods of the model after each iteration:";
  for (int i = 0; i < topicsum_->saved_likelihoods().size(); i++) {
    LOG(INFO) << "Iteration "<< topicsum_->saved_likelihoods()[i].first
              << ": " << topicsum_->saved_likelihoods()[i].second;
  }

  EXPECT_EQ(0, topicsum_->saved_likelihoods()[0].first);
  EXPECT_NEAR(-51.329371535815405,
              topicsum_->saved_likelihoods()[0].second, 0.000001);
  Reset();
}

TEST_F(ToyCollectionsTest, GetCollectionDistribution) {
  TrainToy(1000, NULL  /* no predefined background */);
  Distribution distri;
  // Check that the collection distributions should contain the
  // expected words. 'apple' appears in all the documents so it
  // should be considered background, not a collection word.
  topicsum_->GetCollectionDistribution(0, &distri);
  EXPECT_LT(0.1, distri["iphone"]);
  EXPECT_LT(0.1, distri["ipad"]);
  EXPECT_LT(0.1, distri["company"]);
  EXPECT_GT(0.1, distri["apple"]);
  distri.clear();
  topicsum_->GetCollectionDistribution(1, &distri);
  EXPECT_LT(0.1, distri["banana"]);
  EXPECT_LT(0.1, distri["fruit"]);
  EXPECT_GT(0.1, distri["apple"]);
  Reset();
}

TEST_F(ToyCollectionsTest, GetPosteriorDistribution) {
  TrainToy(1000, NULL  /* no predefined background */);
  Distribution distri;

  // Check that the posterior distributions are as expected.

  // iphone should only belong to the first collection.
  topicsum_->GetPosteriorDistribution("iphone", &distri);
  EXPECT_EQ(0, distri["background"]);
  EXPECT_EQ(0, distri["document"]);
  for (Distribution::const_iterator cit = distri.begin();
       cit != distri.end(); ++cit) {
    if (cit->first[cit->first.size() - 1] == '1') {
      EXPECT_EQ(1.0, cit->second);
    } else if (cit->first[cit->first.size() - 1] == '2') {
      EXPECT_EQ(0.0, cit->second);
    }
  }
  distri.clear();

  // banana should only belong to the second collection.
  topicsum_->GetPosteriorDistribution("banana", &distri);
  EXPECT_EQ(0, distri["background"]);
  EXPECT_EQ(0, distri["document"]);
  for (Distribution::const_iterator cit = distri.begin();
       cit != distri.end(); ++cit) {
    if (cit->first[cit->first.size() - 1] == '1') {
      EXPECT_EQ(0.0, cit->second);
    } else if (cit->first[cit->first.size() - 1] == '2') {
      EXPECT_EQ(1.0, cit->second);
    }
  }
  distri.clear();

  // apple should belong to the two collections
  topicsum_->GetPosteriorDistribution("apple", &distri);
  EXPECT_EQ(0, distri["background"]);
  EXPECT_EQ(0, distri["document"]);
  for (Distribution::const_iterator cit = distri.begin();
       cit != distri.end(); ++cit) {
    if (cit->first[cit->first.size() - 1] == '1') {
      EXPECT_NEAR(0.5, cit->second, 0.2);
    } else if (cit->first[cit->first.size() - 1] == '2') {
      EXPECT_NEAR(0.5, cit->second, 0.2);
    }
  }

  // notfound is not found in the vocabulary.
  distri.clear();
  EXPECT_FALSE(topicsum_->GetPosteriorDistribution("notfound", &distri));

  Reset();
}

TEST_F(ToyCollectionsTest, GetCollectionDistributionFixedBackground) {
  // Setup a fixed background distribution that contains only
  // 'the' and 'and' (not 'apple'). See whether 'apple' is now
  // assigned to the collection distributions.
  Distribution background;
  background["the"] = 0.5;
  background["and"] = 0.5;
  TrainToy(1000, &background);
  LOG(INFO) << topicsum_->PrintSamplingIterationInfo();
  // Check the words from the two collections. 'apple' is now forced
  // not to belong to the background, so it has to be assigned to
  // both collection distributions with a high score.
  Distribution distri;
  topicsum_->GetCollectionDistribution(0, &distri);
  EXPECT_LT(0.2, distri["apple"]);
  EXPECT_LT(0.1, distri["iphone"]);
  EXPECT_LT(0.1, distri["ipad"]);
  EXPECT_LT(0.1, distri["company"]);
  distri.clear();
  topicsum_->GetCollectionDistribution(1, &distri);
  EXPECT_LT(0.2, distri["apple"]);
  EXPECT_LT(0.1, distri["banana"]);
  EXPECT_LT(0.1, distri["fruit"]);
  Reset();
}

TEST_F(ToyCollectionsTest, GetPosteriorDistributionFixedBackground) {
  Distribution background;
  background["the"] = 0.5;
  background["and"] = 0.5;
  TrainToy(1000, &background);
  Distribution distri;

  // Check that the posterior distributions are as expected.
  // 'the' should have all probability assigned to the background now.
  topicsum_->GetPosteriorDistribution("the", &distri);
  EXPECT_EQ(1.0, distri["background"]);
  EXPECT_EQ(0, distri["document"]);
  Reset();
}

TEST_F(ToyCollectionsTest, GetBackgroundDistribution100000Iterations) {
  TrainToy(100000, NULL  /* no predefined background */);
  Distribution distri;
  topicsum_->GetBackgroundDistribution(&distri);
  EXPECT_NEAR(0.14, distri["and"], 0.01);
  EXPECT_NEAR(0.25, distri["the"], 0.01);
  EXPECT_NEAR(0.46, distri["apple"], 0.01);
  Reset();
}

TEST_F(ToyCollectionsTest, GetBackgroundDistribution200000Iterations) {
  TrainToy(200000, NULL  /* no predefined background */);
  Distribution distri;
  topicsum_->GetBackgroundDistribution(&distri);
  EXPECT_NEAR(0.14, distri["and"], 0.01);
  EXPECT_NEAR(0.25, distri["the"], 0.01);
  EXPECT_NEAR(0.46, distri["apple"], 0.01);
  Reset();
}

TEST(TopicSumGibbsSampler, RealCollectionsTraining) {
  vector<const DocumentCollection*> cols;
  BuildRealCollections(&cols);

  int iterations = 4000;
  int burnin = 1000;
  int lag = 100;
  bool track_likelihood = true;

  // Unlike with the toy collection example, there is not much thinking
  // regarding the parameters here. We just use the exact same ones as those
  // given in the original paper by Haghighi in NAACL 2009.

  // Reset seed. Necessary to be able to reproduce the experiments.
  srandom(0);

  double lambda[3] = {0.1, 1, 1};
  double gamma[3] = {1, 5, 10};

  {
    TopicSumGibbsSampler topicsum(iterations,
                                  burnin,
                                  lag,
                                  track_likelihood,
                                  lambda,
                                  gamma,
                                  cols,
                                  NULL);
    topicsum.Train();

    LOG(INFO) << "Likelihoods of the model after each iteration:";
    for (int i = 0; i < topicsum.saved_likelihoods().size(); i++) {
      LOG(INFO) << "Iteration "<< topicsum.saved_likelihoods()[i].first
                << ": " << topicsum.saved_likelihoods()[i].second;
    }

    EXPECT_EQ(1100, topicsum.saved_likelihoods()[0].first);
    EXPECT_NEAR(-6963.279241539145,
                topicsum.saved_likelihoods()[0].second, 0.000001);
  }

  for (vector<const DocumentCollection*>::iterator it = cols.begin();
        it != cols.end();
        ++it) {
    delete *it;
  }
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
