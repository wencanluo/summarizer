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

#include "summarizer/news_postprocessor.h"

#include "summarizer/logging.h"
#include "summarizer/summarizer.pb.h"
#include "gtest/gtest.h"

namespace topicsum {

TEST(NewsPostprocessorTest, SentencesWithWhQuestionAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("What is the best title");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithQuestionMarkAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("Is that a good move for the company?");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithQuoteAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("\"This is a good move for the company\", he said.");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());

  // This sentence contains a UTF8 double quote.
  s.set_raw_content("The Washington Times\' Dave Boyer: \xe2\x80\x9cObama\'s");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, ShortSentencesAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("This is a good move.");
  s.add_token("this");
  s.add_token("is");
  s.add_token("a");
  s.add_token("good");
  s.add_token("move");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, UsingOptionsShortSentencesAreRemoved) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(2);
  options.mutable_news_postprocessor_options()->set_max_sentence_length(200);
  postprocessor.Init(options);

  // A 5-word sentence is now accepted.
  Sentence s;
  s.set_raw_content("This is a good move.");
  s.add_token("this");
  s.add_token("is");
  s.add_token("a");
  s.add_token("good");
  s.add_token("move");
  EXPECT_FALSE(postprocessor.Compress(&s));
  EXPECT_TRUE(s.has_raw_content());

  // A 1-word sentence is now rejected.
  s.Clear();
  s.set_raw_content("Cool.");
  s.add_token("cool");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, LongSentencesAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence "
                    "This is a very long sentence ");

  for (int i = 0; i < 10; ++i) {
    s.add_token("this");
    s.add_token("is");
    s.add_token("a");
    s.add_token("very");
    s.add_token("long");
    s.add_token("sentence");
  }
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, UsingOptionsLongSentencesAreRemoved) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(1);
  options.mutable_news_postprocessor_options()->set_max_sentence_length(3);
  postprocessor.Init(options);

  // A 5-word sentence is now rejected, it is too long.
  Sentence s;
  s.set_raw_content("This is a good move.");
  s.add_token("this");
  s.add_token("is");
  s.add_token("a");
  s.add_token("good");
  s.add_token("move");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());

  // A 1-word sentence is accepted.
  s.Clear();
  s.set_raw_content("Cool.");
  s.add_token("cool");
  EXPECT_FALSE(postprocessor.Compress(&s));
  EXPECT_TRUE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesInParenthesesAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("(This is a good move.)");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());

  s.set_raw_content("(Read Risky Business: Fed Should Buy More MBS "
                    "After Jackson Hole).");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithTrailingDotsAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("He said that he wanted to come home to...");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithTrailingColonAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("He said that he wanted to come home to:");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesStartingInLowercaseAreRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("won its fourth straight Emmy, and its only "
                    "award Sunday night, for outstanding drama.");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SomeSentencesStartingInLowercaseAreNotRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("iPhone is a good phone.");
  EXPECT_FALSE(postprocessor.Compress(&s));
  EXPECT_EQ("iPhone is a good phone.", s.raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithFirstPersonSingularPronounAreRemoved) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(0);
  postprocessor.Init(options);

  Sentence s;
  s.set_raw_content("I think so");
  s.add_token("i");
  s.add_token("think");
  s.add_token("so");
  s.add_pos_tag("PRON");
  s.add_pos_tag("VERB");
  s.add_pos_tag("ADV");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithFirstPersonPluralPronounAreRemoved) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(0);
  postprocessor.Init(options);

  Sentence s;
  s.set_raw_content("We think so");
  s.add_token("we");
  s.add_token("think");
  s.add_token("so");
  s.add_pos_tag("PRON");
  s.add_pos_tag("VERB");
  s.add_pos_tag("ADV");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithNoVerbAreRemoved) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(0);
  postprocessor.Init(options);

  Sentence s;
  s.set_raw_content("You so happy yeah");
  s.add_token("you");
  s.add_token("so");
  s.add_token("happy");
  s.add_token("yeah");
  s.add_pos_tag("PRON");
  s.add_pos_tag("ADV");
  s.add_pos_tag("ADJ");
  s.add_pos_tag("ADV");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_FALSE(s.has_raw_content());
}

TEST(NewsPostprocessorTest, SentencesWithNoPosTagsAreNotFiltered) {
  NewsPostprocessor postprocessor;
  SummarizerOptions options;
  options.mutable_news_postprocessor_options()->set_min_sentence_length(0);
  options.mutable_news_postprocessor_options()->set_max_sentence_length(200);
  postprocessor.Init(options);

  Sentence s;
  s.set_raw_content("You so happy yeah");
  s.add_token("you");
  s.add_token("so");
  s.add_token("happy");
  s.add_token("yeah");
  EXPECT_FALSE(postprocessor.Compress(&s));
  EXPECT_EQ("You so happy yeah", s.raw_content());
}

TEST(NewsPostprocessorTest, ParenthesisAtBeginningIsRemoved) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("(This is a good move.) We all agree");
  EXPECT_TRUE(postprocessor.Compress(&s));
  EXPECT_TRUE(s.has_raw_content());
  EXPECT_EQ("We all agree", s.raw_content());
}

TEST(NewsPostprocessorTest, GoodSentencesAreUnchanged) {
  NewsPostprocessor postprocessor;
  Sentence s;
  s.set_raw_content("This is a good move for the company.");
  EXPECT_FALSE(postprocessor.Compress(&s));
  EXPECT_TRUE(s.has_raw_content());
}

}  // namespace topicsum

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
