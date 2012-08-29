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

#ifndef SUMMARIZER_XML_PARSER_H_
#define SUMMARIZER_XML_PARSER_H_

#include <string>
#include <vector>

#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "summarizer/document.pb.h"

namespace topicsum {

using std::string;
using std::vector;

class XmlParser {
 public:
  XmlParser();

  virtual ~XmlParser();

  // Parse an entire document from xml string. Return true if successful.
  bool ParseDocument(const string& xml_string, Document *document);

 protected:
  // Parse all sentences in document.
  bool ParseSentences(const xmlDocPtr& doc, Document* document);

  // Parse one sentence from given context.
  bool ParseSentence(const xmlDocPtr& doc,
                      const xmlXPathContextPtr& context,
                      Sentence* sentence);

  // Parse content of one element matching xpath.
  bool ParseElement(const xmlDocPtr& doc, const xmlChar* xpath,
                    string* element_content);

  // Parse content of one element from given context matching xpath
  bool ParseElementFromContext(const xmlDocPtr& doc,
                               const xmlXPathContextPtr& context,
                               const xmlChar* xpath,
                               string* element);

  // Parse contents of all elements matching xpath.
  bool ParseElements(const xmlDocPtr& doc, const xmlChar* xpath,
                     vector<string>* elements);

  // Parse all elements from given context matching xpath.
  bool ParseElementsFromContext(const xmlDocPtr& doc,
                                const xmlXPathContextPtr& context,
                                const xmlChar* xpath,
                                vector<string>* elements);

  // Parse content from specified node.
  bool ParseContentFromNode(const xmlDocPtr& doc, const xmlNodePtr& node,
                            string* node_content);

  // Parse all nodes matching xpath.
  bool ParseNodes(const xmlDocPtr& doc, const xmlChar* xpath,
                  xmlXPathObjectPtr* nodes);

  // Parse all nodes from given context matching xpath.
  bool ParseNodesFromContext(const xmlXPathContextPtr& context,
                             const xmlChar* xpath,
                             xmlXPathObjectPtr* nodes);

  XmlParser(const XmlParser&);
  void operator=(const XmlParser&);
};

}  // namespace topicsum

#endif  // SUMMARIZER_XML_PARSER_H_
