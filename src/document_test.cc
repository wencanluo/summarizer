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

#include "summarizer/document.h"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "summarizer/distribution.h"
#include "summarizer/logging.h"
#include "summarizer/scoped_ptr.h"
#include "summarizer/summarizer.pb.h"

static char kStrA[] = "  Today is a beautiful day ";
static char kStrB[] = " But tomorrow will be wonderful ";

static char kPosA[] = "NN VBZ DT JJ NN";

namespace topicsum {

using std::istringstream;

// This unit-test class builds a test collection.
// It is declared a friend of Document so that it can create documents
// directly without calling DocumentBuilder.
class CollectionTest : public testing::Test {
 protected:
  // Creates a collection with two documents: A and B
  // - A contains 2 sentences
  // - B contains 1 sentence
  virtual void SetUp() {
    test_collection_.reset(new DocumentCollection);
    vector<string> tokens;
    Document document;
    string token;

    // Build Document 1 and add it to the collection.
    istringstream iss(kStrA);
    while (iss >> token) {
      tokens.push_back(token);
    }
    iss.clear();

    DocumentUtils::AddSentence(SentenceUtils::FromTokens(tokens), &document);
    tokens.clear();

    iss.str(kStrB);
    while (iss >> token) {
      tokens.push_back(token);
    }
    iss.clear();

    DocumentUtils::AddSentence(SentenceUtils::FromTokens(tokens), &document);
    CollectionUtils::AddDocument(document, test_collection_.get());

    // Build Document 2 and add it to the collection. Add part-of-speech tags
    // to this document.
    tokens.clear();
    vector<string> pos_tags;

    iss.str(kStrA);
    while (iss >> token) {
      tokens.push_back(token);
    }
    iss.clear();

    iss.str(kPosA);
    string pos_tag;
    while (iss >> pos_tag) {
      pos_tags.push_back(pos_tag);
    }
    iss.clear();

    document.Clear();
    DocumentUtils::AddSentence(
        SentenceUtils::FromTokensAndTags(tokens, pos_tags),
        &document);
    CollectionUtils::AddDocument(document, test_collection_.get());
  }

  virtual void TearDown() { }

  scoped_ptr<DocumentCollection> test_collection_;
};

TEST_F(CollectionTest, CollectionTest) {
  EXPECT_EQ(2, test_collection_->document_size());
  EXPECT_EQ(3, test_collection_->num_sentences());

  const Document& doc = test_collection_->document(0);
  EXPECT_EQ(2, doc.sentence_size());
  EXPECT_EQ(10, doc.num_tokens());
}

TEST_F(CollectionTest, SentenceIterator) {
  SentenceIterator<CollectionDocumentIterator> sent_iter(
      test_collection_->mutable_document()->begin(),
      test_collection_->mutable_document()->end());

  const Sentence* sentence = sent_iter.CurrentValue();
  EXPECT_EQ("Today is a beautiful day",
            SentenceUtils::ToString(*sentence));
  EXPECT_EQ(0, sentence->pos_tag_size());

  sent_iter.Next();
  sentence = sent_iter.CurrentValue();
  EXPECT_EQ("But tomorrow will be wonderful",
            SentenceUtils::ToString(*sentence));
  EXPECT_EQ(0, sentence->pos_tag_size());

  sent_iter.Next();
  sentence = sent_iter.CurrentValue();
  EXPECT_EQ("Today is a beautiful day",
            SentenceUtils::ToString(*sentence));
  ASSERT_EQ(5, sentence->pos_tag_size());
  EXPECT_EQ("NN", sentence->pos_tag(0));
  EXPECT_EQ("VBZ", sentence->pos_tag(1));
  EXPECT_EQ("DT", sentence->pos_tag(2));
  EXPECT_EQ("JJ", sentence->pos_tag(3));
  EXPECT_EQ("NN", sentence->pos_tag(4));

  sent_iter.Next();
  EXPECT_EQ(true, sent_iter.Done());
}

TEST_F(CollectionTest, FitToSize) {
  Document doc = test_collection_->document(0);
  DocumentUtils::FitToSize(3, &doc);

  EXPECT_EQ("Today is a [...]", DocumentUtils::ToString(doc));

  doc.Clear();

  doc = test_collection_->document(0);
  DocumentUtils::FitToSize(8, &doc);
  EXPECT_EQ(8, doc.num_tokens());
}

TEST_F(CollectionTest, ReachesLengthLimitWithChars) {
  Document doc = test_collection_->document(0);
  string expected_doc = "Today is a beautiful day\nBut tomorrow will be "
      "wonderful";
  int expected_length = expected_doc.length();

  EXPECT_EQ(expected_doc, DocumentUtils::ToString(doc));
  EXPECT_FALSE(DocumentUtils::ReachesLengthLimit(expected_length + 1,
                                                 SummaryLength::CHARACTER,
                                                 doc));
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(expected_length,
                                                SummaryLength::CHARACTER,
                                                doc));
}

TEST_F(CollectionTest, ReachesLengthLimitWithSentences) {
  Document doc;
  Sentence sentence;

  EXPECT_FALSE(DocumentUtils::ReachesLengthLimit(1,
                                                 SummaryLength::SENTENCE,
                                                 doc));

  DocumentUtils::AddSentence(sentence, &doc);
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(1,
                                                SummaryLength::SENTENCE,
                                                doc));
  EXPECT_FALSE(DocumentUtils::ReachesLengthLimit(2,
                                                 SummaryLength::SENTENCE,
                                                 doc));

  DocumentUtils::AddSentence(sentence, &doc);
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(1,
                                                SummaryLength::SENTENCE,
                                                doc));
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(2,
                                                SummaryLength::SENTENCE,
                                                doc));
}

TEST_F(CollectionTest, ReachesLengthLimitWithTokens) {
  Document doc;
  EXPECT_FALSE(DocumentUtils::ReachesLengthLimit(1,
                                                 SummaryLength::TOKEN,
                                                 doc));

  Sentence s;
  s.add_token("the");
  s.add_token("summary");
  DocumentUtils::AddSentence(s, &doc);
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(1,
                                                SummaryLength::TOKEN,
                                                doc));
  EXPECT_TRUE(DocumentUtils::ReachesLengthLimit(2,
                                                SummaryLength::TOKEN,
                                                doc));
  EXPECT_FALSE(DocumentUtils::ReachesLengthLimit(3,
                                                 SummaryLength::TOKEN,
                                                 doc));
}

TEST(Frequencies, AddTermFrequency) {
  Distribution distri;

  vector<string> token_a;
  token_a.push_back("A");
  token_a.push_back("B");
  Sentence sentence_a(SentenceUtils::FromTokens(token_a));

  vector<string> token_b;
  token_b.push_back("A");
  token_b.push_back("B");
  token_b.push_back("C");
  token_b.push_back("C");
  Sentence sentence_b(SentenceUtils::FromTokens(token_b));

  // Test term frequency in a sentence
  SentenceUtils::AddTermFrequency(sentence_b, &distri);
  EXPECT_DOUBLE_EQ(1, distri["A"]);
  EXPECT_DOUBLE_EQ(1, distri["B"]);
  EXPECT_DOUBLE_EQ(2, distri["C"]);
  distri.clear();

  // Test term frequency in a document
  Document doc;
  DocumentUtils::AddSentence(sentence_a, &doc);
  DocumentUtils::AddSentence(sentence_b, &doc);

  DocumentUtils::AddTermFrequency(doc, &distri);
  EXPECT_DOUBLE_EQ(2, distri["A"]);
  EXPECT_DOUBLE_EQ(2, distri["B"]);
  EXPECT_DOUBLE_EQ(2, distri["C"]);
  distri.clear();

  // Test term frequency in a collection
  DocumentCollection collection;
  CollectionUtils::AddDocument(doc, &collection);
  CollectionUtils::AddTermFrequency(collection, &distri);
  EXPECT_DOUBLE_EQ(2, distri["A"]);
  EXPECT_DOUBLE_EQ(2, distri["B"]);
  EXPECT_DOUBLE_EQ(2, distri["C"]);
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
