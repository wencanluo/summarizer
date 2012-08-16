lib_LIBRARIES = libsummarizer.a
libsummarizer_a_CXXFLAGS = @SUM_CFLAGS@

libsummarizer_a_SOURCES = 	\
  file.cc			\
  summarizer.cc 		\
  summarizer/distribution.pb.cc \
  summarizer/document.pb.cc 	\
  summarizer/summarizer.pb.cc	\
  xml_parser.cc

pkginclude_HEADERS = 		\
  summarizer/distribution.pb.h	\
  summarizer/document.pb.h 	\
  summarizer/file.h 		\
  summarizer/summarizer.h 	\
  summarizer/summarizer.pb.h	\
  summarizer/xml_parser.h

TESTS =				\
  summarizer_test		\
  xml_parser_test

check_PROGRAMS = 		\
  summarizer_test 		\
  xml_parser_test

EXTRA_DIST = 				\
  testdata/article_14319162.xml

AM_CPPFLAGS = -DDATADIR=\"$(srcdir)/\"

LDADD = libsummarizer.a \
	$(top_builddir)/libgtest.a \
	@SUM_LIBS@

CXXFLAGS = @SUM_CFLAGS@ \
	   -I$(top_srcdir)/gtest-1.6.0/include

summarizer_test_SOURCES = summarizer_test.cc
xml_parser_test_SOURCES = xml_parser_test.cc

