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

#include "summarizer/xml_parser.h"

#include <time.h>

namespace topicsum {

XmlParser::XmlParser() {
  xmlInitParser();
}

XmlParser::~XmlParser() {
  xmlCleanupParser();
}

bool XmlParser::ParseDocument(const string& xml_document, Document* document) {
  xmlDocPtr doc;

  doc = xmlReadMemory(xml_document.c_str(), xml_document.length(),
                      "noname.xml", NULL, 0);
  if (!doc) {
    return false;
  }

  // Parse article id
  if (!ParseElement(
      doc,
      reinterpret_cast<const xmlChar*>("/article/@id"),
      document->mutable_id())) {
    xmlFreeDoc(doc);
    return false;
  }

  // Parse article title
  if (!ParseElement(
      doc,
      reinterpret_cast<const xmlChar*>("/article/title"),
      document->mutable_title())) {
    xmlFreeDoc(doc);
    return false;
  }

  // Parse sentences
  if (!ParseSentences(doc, document)) {
    xmlFreeDoc(doc);
    return false;
  }

  // Cleanup
  xmlFreeDoc(doc);

  return true;
}

bool XmlParser::ParseSentences(const xmlDocPtr& doc, Document* document) {
  xmlXPathObjectPtr nodes;
  xmlNodeSetPtr nodeset;
  xmlXPathContextPtr context;

  // Find all sentences
  if (!ParseNodes(
      doc,
      reinterpret_cast<const xmlChar*>("/article/body/item/text/p/sentence"),
      &nodes)) {
    return false;
  }

  context = xmlXPathNewContext(doc);
  if (!context) {
    return false;
  }

  // Parse all sentences
  nodeset = nodes->nodesetval;
  for (int i = 0; i < nodeset->nodeNr; i++) {
    context->node = nodeset->nodeTab[i];
    if (!ParseSentence(doc, context, document->add_sentence())) {
      xmlXPathFreeContext(context);
      return false;
    }
  }

  // Count tokens
  int num_tokens = 0;
  for (int i = 0; i < document->sentence_size(); i++) {
    num_tokens += document->sentence(i).token_size();
  }
  document->set_num_tokens(num_tokens);

  // Cleanup
  xmlXPathFreeContext(context);

  return true;
}

bool XmlParser::ParseSentence(const xmlDocPtr& doc,
                              const xmlXPathContextPtr& context,
                              Sentence* sentence) {
  vector<string> tokens;
  vector<string> pos;

  // Parse cleartext
  if (!ParseElementFromContext(
      doc,
      context,
      reinterpret_cast<const xmlChar*>("./plainText"),
      sentence->mutable_raw_content())) {
    return false;
  }

  // Parse tokens
  if (!ParseElementsFromContext(
      doc,
      context,
      reinterpret_cast<const xmlChar*>("./tokens/token"),
      &tokens)) {
    return false;
  }

  for (vector<string>::iterator it = tokens.begin();
       it != tokens.end();
       it++) {
    sentence->add_token(*it);
  }

  // Parse part-of-speech
  if (!ParseElementsFromContext(
      doc,
      context,
      reinterpret_cast<const xmlChar*>("./tokens/token/@pos"),
      &pos)) {
    return false;
  }

  for (vector<string>::iterator it = pos.begin();
       it != pos.end();
       it++) {
    sentence->add_pos_tag(*it);
  }

  return true;
}

bool XmlParser::ParseElement(const xmlDocPtr& doc,
                             const xmlChar* xpath,
                             string* element) {
  vector<string> elements;

  if (!ParseElements(doc, xpath, &elements)) {
    return false;
  }

  if (elements.size() == 0) {
    return false;
  }

  *element = elements[0];

  return true;
}

bool XmlParser::ParseElementFromContext(const xmlDocPtr& doc,
                                        const xmlXPathContextPtr& context,
                                        const xmlChar* xpath,
                                        string* element) {
  vector<string> elements;

  if (!ParseElementsFromContext(doc, context, xpath, &elements)) {
    return false;
  }

  *element = elements[0];

  return true;
}

bool XmlParser::ParseElements(const xmlDocPtr& doc,
                              const xmlChar* xpath,
                              vector<string>* elements) {
  xmlXPathContextPtr context;

  context = xmlXPathNewContext(doc);
  if (!context) {
    return false;
  }

  bool result = ParseElementsFromContext(doc, context, xpath, elements);

  xmlXPathFreeContext(context);

  return result;
}

bool XmlParser::ParseElementsFromContext(const xmlDocPtr& doc,
                                         const xmlXPathContextPtr& context,
                                         const xmlChar* xpath,
                                         vector<string>* elements) {
  xmlXPathObjectPtr nodes;
  xmlNodeSetPtr nodeset;
  string content;

  if (!ParseNodesFromContext(context, xpath, &nodes)) {
    return false;
  }

  nodeset = nodes->nodesetval;

  int errors = 0;
  for (int i = 0; i < nodeset->nodeNr; i++) {
    if (ParseContentFromNode(doc, nodeset->nodeTab[i], &content)) {
      elements->push_back(content);
    } else {
      errors++;
    }
  }

  xmlXPathFreeObject(nodes);

  return (errors == 0);
}

bool XmlParser::ParseContentFromNode(const xmlDocPtr& doc,
                                     const xmlNodePtr& node,
                                     string* node_content) {
  xmlChar *element;

  element = xmlNodeListGetString(
      doc,
      node->xmlChildrenNode,
      1);
  node_content->assign(reinterpret_cast<const char *>(element));

  xmlFree(element);

  return true;
}

bool XmlParser::ParseNodes(const xmlDocPtr& doc,
                           const xmlChar* xpath,
                           xmlXPathObjectPtr* nodes) {
  xmlXPathContextPtr context;

  context = xmlXPathNewContext(doc);
  if (!context) {
    return false;
  }

  bool result = ParseNodesFromContext(context, xpath, nodes);

  xmlXPathFreeContext(context);

  return result;
}

bool XmlParser::ParseNodesFromContext(const xmlXPathContextPtr& context,
                                      const xmlChar* xpath,
                                      xmlXPathObjectPtr* nodes) {
  *nodes = xmlXPathEvalExpression(xpath, context);
  if (*nodes == NULL) {
    return false;
  }

  return true;
}

}  // namespace topicsum
