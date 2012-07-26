.SUFFIXES: .proto .pb.h .pb.cc

CXXFLAGS   = -Wall
LIBS 	   =
CXX 	   = g++

CXXFLAGS  += `pkg-config --cflags protobuf libxml-2.0` -I . -I include/
LIBS	  += `pkg-config --libs protobuf libxml-2.0` -l protobuf

PROTOC     = protoc
PFLAGS	   = --cpp_out=.

PROTOS	   = proto/distribution.proto proto/document.proto proto/summarizer.proto proto/query.proto
PROTOH	   = $(PROTOS:.proto=.pb.h)
PROTOCC    = $(PROTOS:.proto=.pb.cc)
PROTO_OBJ  = $(PROTOCC:.cc=.o)

SOURCES	   = src/summarizer.cc src/xml_parser.cc src/file.cc
OBJECTS	   = $(SOURCES:.cc=.o)

GTEST_DIR  = gtest-1.6.0
TESTS 	   = summarizer_test xml_parser_test
TESTS_SRC  = tests/summarizer_test.cc tests/xml_parser_test.cc
TESTS_OBJ  = $(TESTS_SRC:.cc=.o)
CXXFLAGS  += -I${GTEST_DIR}/include

all: $(TESTS)

test: $(TESTS)
	./summarizer_test
	./xml_parser_test

clean:
	rm -f $(OBJECTS) $(PROTOH) $(PROTOCC) $(TESTS) $(PROTO_OBJ) libgtest.a gtest-all.o

$(TESTS): $(TESTS_SRC) $(OBJECTS) libgtest.a
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(PROTO_OBJ) tests/$@.cc -o $@ $(LIBS) libgtest.a

$(OBJECTS): $(PROTO_OBJ) $(SOURCES)

$(PROTO_OBJ): $(PROTOCC)

libgtest.a:
	$(CXX) -I${GTEST_DIR}/include -I${GTEST_DIR} -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o

.cc.o:
	$(CXX) $(CXXFLAGS) $< -c -o $@

.proto.pb.cc:
	$(PROTOC) $< $(PFLAGS)
