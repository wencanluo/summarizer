lib_LIBRARIES = libsummarizer.a
noinst_PROGRAMS = example example_simple run_KLSum_sentence run_KLSum_word run_TopicSum_sentence run_TopicSum_word

libsummarizer_a_CXXFLAGS = @SUM_CFLAGS@

libsummarizer_a_SOURCES =               \
  distribution.cc                       \
  document.cc                           \
  file.cc                               \
  gibbs.cc                              \
  klsum.cc                              \
  lexicon.cc                            \
  logging.cc                            \
  news_postprocessor.cc                 \
  redundancy.cc                         \
  sampling.cc                           \
  sum.cc                                \
  summarizer.cc                         \
  summarizer/distribution.h             \
  summarizer/distribution.pb.cc         \
  summarizer/distribution.pb.h          \
  summarizer/document.h                 \
  summarizer/document.pb.cc             \
  summarizer/document.pb.h              \
  summarizer/file.h                     \
  summarizer/gibbs.h                    \
  summarizer/hash.h                     \
  summarizer/klsum.h                    \
  summarizer/lexicon.h                  \
  summarizer/logging.h                  \
  summarizer/news_postprocessor.h       \
  summarizer/postprocessor.h            \
  summarizer/redundancy.h               \
  summarizer/sampling.h                 \
  summarizer/scoped_ptr.h               \
  summarizer/sum.h                      \
  summarizer/summarizer.h               \
  summarizer/summarizer.pb.cc           \
  summarizer/summarizer.pb.h            \
  summarizer/test_postprocessor.h       \
  summarizer/topicsumgibbs.h            \
  summarizer/topicsum.h                 \
  summarizer/types.h                    \
  summarizer/xml_parser.h               \
  test_postprocessor.cc                 \
  topicsum.cc                           \
  topicsumgibbs.cc                      \
  xml_parser.cc

pkginclude_HEADERS =                    \
  config.h                              \
  summarizer/distribution.h             \
  summarizer/distribution.pb.h          \
  summarizer/document.h                 \
  summarizer/document.pb.h              \
  summarizer/file.h                     \
  summarizer/gibbs.h                    \
  summarizer/hash.h                     \
  summarizer/klsum.h                    \
  summarizer/lexicon.h                  \
  summarizer/logging.h                  \
  summarizer/news_postprocessor.h       \
  summarizer/postprocessor.h            \
  summarizer/redundancy.h               \
  summarizer/sampling.h                 \
  summarizer/scoped_ptr.h               \
  summarizer/sum.h                      \
  summarizer/summarizer.h               \
  summarizer/summarizer.pb.h            \
  summarizer/test_postprocessor.h       \
  summarizer/topicsumgibbs.h            \
  summarizer/topicsum.h                 \
  summarizer/types.h                    \
  summarizer/xml_parser.h

TESTS =                                 \
  document_test                         \
  klsum_test                            \
  lexicon_test                          \
  news_postprocessor_test               \
  sampling_test                         \
  sum_test                              \
  test_postprocessor_test               \
  topicsumgibbs_test                    \
  topicsum_test                         \
  xml_parser_test

check_PROGRAMS =                        \
  document_test                         \
  klsum_test                            \
  lexicon_test                          \
  news_postprocessor_test               \
  sampling_test                         \
  summarizer_test                       \
  sum_test                              \
  test_postprocessor_test               \
  topicsumgibbs_test                    \
  topicsum_test                         \
  xml_parser_test

EXTRA_DIST =                            \
  testdata/article_14319162.xml         \
  testdata/realcol1/d1.xml              \
  testdata/realcol1/d2.xml              \
  testdata/realcol1/d3.xml              \
  testdata/realcol1/d4.xml              \
  testdata/realcol2/d1.xml              \
  testdata/realcol2/d2.xml              \
  testdata/realcol2/d3.xml              \
  testdata/realcol2/d4.xml              \
  testdata/realcol3/d1.xml              \
  testdata/realcol3/d2.xml              \
  testdata/realcol3/d3.xml              \
  testdata/realcol3/d4.xml              \
  testdata/singledoc                    \
  testdata/toycol1/d1.xml               \
  testdata/toycol1/d2.xml               \
  testdata/toycol2/d1.xml               \
  testdata/toycol2/d2.xml               \
  testdata/toycol2/d3.xml

AM_CPPFLAGS = -DDATADIR=\"$(srcdir)/\"

LDADD = libsummarizer.a                 \
        $(top_builddir)/libgtest.a      \
        @SUM_LIBS@

CXXFLAGS = @SUM_CFLAGS@                 \
           -I$(top_srcdir)/gtest/include

document_test_SOURCES = document_test.cc
klsum_test_SOURCES = klsum_test.cc
lexicon_test_SOURCES = lexicon_test.cc
news_postprocessor_test_SOURCES = news_postprocessor_test.cc
sampling_test_SOURCES = sampling_test.cc
summarizer_test_SOURCES = summarizer_test.cc
sum_test_SOURCES = sum_test.cc
test_postprocessor_test_SOURCES = test_postprocessor_test.cc
topicsumgibbs_test_SOURCES = topicsumgibbs_test.cc
topicsum_test_SOURCES = topicsum_test.cc
xml_parser_test_SOURCES = xml_parser_test.cc


example_simple_SOURCES = example_simple.cc
example_SOURCES = example.cc
run_KLSum_SOURCES = run_KLSum.cc
run_KLSum_sentence_SOURCES = run_KLSum_sentence.cc
run_KLSum_word_SOURCES = run_KLSum_word.cc 
run_TopicSum_sentence_SOURCES = run_TopicSum_sentence.cc
run_TopicSum_word_SOURCES = run_TopicSum_word.cc