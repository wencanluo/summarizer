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

#include <iomanip>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "summarizer/logging.h"
#include "summarizer/sampling.h"
#include "summarizer/scoped_ptr.h"

namespace topicsum {

using std::fixed;
using std::ios;
using std::left;
using std::make_pair;
using std::setfill;
using std::setiosflags;
using std::setprecision;
using std::setw;

TopicSumGibbsSampler::TopicSumGibbsSampler(
    int iterations,
    int burnin,
    int lag,
    bool track_likelihood,
    double lambda[3],
    double gamma[3],
    const vector<const DocumentCollection*>& collections,
    const Distribution* background)
    : GibbsSampler(iterations, burnin, lag, track_likelihood),
      NS_(NULL),
      NWB_(NULL),
      NWC_(NULL),
      NWD_(NULL),
      NC_(NULL),
      ND_(NULL),
      NZS_(NULL),
      phi_B_(NULL),
      phi_C_(NULL),
      phi_D_(NULL),
      psi_(NULL),
      training_cols_(collections),
      lastZ_(NULL),
      background_(background) {
  // Copy the hyper-parameters and calculate their sums.
  lambda_sum_ = 0;
  gamma_sum_ = 0;

  for (int k = 0; k < 3; k++) {
    lambda_[k] = lambda[k];
    gamma_[k] = gamma[k];
    lambda_sum_ += lambda_[k];
    gamma_sum_ += gamma_[k];
  }

  lexicon_.reset(new Lexicon);
}

void TopicSumGibbsSampler::Reset() {
  if (NS_) {
    for (int c = 0; c < C_; c++) {
      int num_docs = training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++)
        delete[] NS_[c][d];
      delete[] NS_[c];
    }
    delete[] NS_;
  }

  if (NWB_)
    delete[] NWB_;

  if (NWC_) {
    for (int c = 0; c < C_; c++)
      delete[] NWC_[c];
    delete[] NWC_;
  }

  if (NWD_) {
    for (int c = 0; c < C_; c++) {
      int num_docs =  training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++)
        delete[] NWD_[c][d];
      delete[] NWD_[c];
    }
    delete[] NWD_;
  }

  if (NC_)
    delete[] NC_;

  if (ND_) {
    for (int c = 0; c < C_; c++)
      delete[] ND_[c];
    delete[] ND_;
  }

  if (NZS_) {
    for (int c = 0; c < C_; c++) {
      int num_docs =  training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++) {
        int num_sents = training_cols_[c]->document(d).sentence_size();
        for (int s = 0; s < num_sents; s++)
          delete[] NZS_[c][d][s];
        delete[] NZS_[c][d];
      }
      delete[] NZS_[c];
    }
    delete[] NZS_;
  }

  if (phi_B_)
    delete[] phi_B_;

  if (phi_C_) {
    for (int c = 0; c < C_; c++)
      delete[] phi_C_[c];
    delete[] phi_C_;
  }

  if (phi_D_) {
    for (int c = 0; c < C_; c++) {
      int num_docs =  training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++)
        delete[] phi_D_[c][d];
      delete[] phi_D_[c];
    }
    delete[] phi_D_;
  }

  if (psi_) {
    for (int c = 0; c < C_; c++) {
      int num_docs =  training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++) {
        int num_sents = training_cols_[c]->document(d).sentence_size();
        for (int s = 0; s < num_sents; s++)
          delete[] psi_[c][d][s];
        delete[] psi_[c][d];
      }
      delete[] psi_[c];
    }
    delete[] psi_;
  }

  if (lastZ_) {
    for (int c = 0; c < C_; c++) {
      int num_docs =  training_cols_[c]->document_size();
      for (int d = 0; d < num_docs; d++) {
        int num_sents = training_cols_[c]->document(d).sentence_size();
        for (int s = 0; s < num_sents; s++)
          delete[] lastZ_[c][d][s];
        delete[] lastZ_[c][d];
      }
      delete[] lastZ_[c];
    }
    delete[] lastZ_;
  }

  NS_ = NULL;
  NWB_ = NULL;
  NWC_ = NULL;
  NWD_ = NULL;
  NC_ = NULL;
  ND_ = NULL;
  NZS_ = NULL;
  phi_B_ = NULL;
  phi_C_ = NULL;
  phi_D_ = NULL;
  psi_ = NULL;
  lastZ_ = NULL;
}

void TopicSumGibbsSampler::Init() {
  Reset();

  // Determine number of collections.
  C_ = training_cols_.size();

  // Initialize word dictionary with all the words from all documents in all
  // collections. At the same time, count the number of words.
  AW_ = 0;
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        const Sentence& sent = doc.sentence(s);
        for (int w = 0; w < sent.token_size(); w++) {
          const string& word = sent.token(w);
          lexicon_->add_token(word);
          ++AW_;
        }
      }
    }
  }

  // If a background was provided, add the background words to the lexicon.
  if (background_ != NULL) {
    for (Distribution::const_iterator cit = background_->begin();
         cit != background_->end(); ++cit) {
      lexicon_->add_token(cit->first);
    }
  }

  // Determine vocabulary size.
  W_ = lexicon_->size();

  // Determine number of documents.
  D_ = 0;
  for (int c = 0; c < C_; c++)
    D_ += training_cols_[c]->document_size();

  // Determine total number of sentences.
  S_ = 0;
  for (int c = 0; c < C_; c++)
    for (int d = 0 ; d < training_cols_[c]->document_size() ; d++)
      S_ += training_cols_[c]->document(d).sentence_size();

  // Allocate memory to store the size of all the sentences.
  // Note that the sentence sizes will be initialized in DoInitialAssignment.
  NS_ = new int**[C_]();
  for (int c = 0; c < C_; c++) {
    int num_docs =  training_cols_[c]->document_size();
    NS_[c] = new int*[num_docs]();
    for (int d = 0; d < num_docs; d++) {
      int num_sents = training_cols_[c]->document(d).sentence_size();
      NS_[c][d] = new int[num_sents]();
    }
  }

  // Allocate memory to store the topics distributions and initialize all
  // values to zero. The only exception is the background distribution, that
  // can come as given.
  phi_B_ = new double[W_]();

  phi_C_ = new double*[C_]();
  for (int c = 0; c < C_; c++)
    phi_C_[c] = new double[W_]();

  phi_D_ = new double**[C_]();
  for (int c = 0; c < C_; c++) {
    int num_docs =  training_cols_[c]->document_size();
    phi_D_[c] = new double*[num_docs]();
    for (int d = 0; d < num_docs; d++)
      phi_D_[c][d] = new double[W_]();
  }

  // Allocate memory to store the initialize sentence topic-assignment and
  // initialize all values to zero.
  psi_ = new double***[C_]();
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    psi_[c] = new double**[col.document_size()]();
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      psi_[c][d] = new double*[doc.sentence_size()]();
      for (int s = 0; s < doc.sentence_size(); s++) {
        psi_[c][d][s] = new double[3]();
      }
    }
  }

  // Allocate memory to store the counters and initialize all values to zero.
  NWB_ = new int[W_]();
  NWC_ = new int*[C_]();
  for (int c = 0; c < C_; c++)
    NWC_[c] = new int[W_]();

  NWD_ = new int**[C_]();
  for (int c = 0; c < C_; c++) {
    NWD_[c] = new int*[training_cols_[c]->document_size()]();
    for (int d = 0; d < training_cols_[c]->document_size(); d++)
      NWD_[c][d] = new int[W_]();
  }

  NB_ = 0;
  NC_ = new int[C_]();
  ND_ = new int*[C_]();
  for (int c = 0; c < C_; c++)
    ND_[c] = new int[training_cols_[c]->document_size()]();

  NZS_ = new int***[C_]();
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    NZS_[c] = new int**[col.document_size()]();
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      NZS_[c][d] = new int*[doc.sentence_size()]();
      for (int s = 0; s < doc.sentence_size(); s++) {
        NZS_[c][d][s] = new int[3]();
      }
    }
  }

  // Allocate memory to store the last assignments and initialize all values to
  // zero.
  lastZ_ = new int***[C_]();
  for (int c = 0 ; c < C_ ; c++) {
    const DocumentCollection& col = *training_cols_[c];
    lastZ_[c] = new int**[col.document_size()]();
    for (int d = 0 ; d < col.document_size() ; d++) {
      const Document& doc = col.document(d);
      lastZ_[c][d] = new int*[doc.sentence_size()]();
      for (int s = 0 ; s < doc.sentence_size() ; s++) {
        const Sentence& sent = doc.sentence(s);
        lastZ_[c][d][s] = new int[sent.token_size()]();
      }
    }
  }

  // If a background distribution was provided, initialize the variables.
  if (background_ != NULL) {
    double background_prior = gamma_[0] / gamma_sum_;

    // Initialize the probability distribution and normalize it if needed.
    double background_sum = 0.0;
    for (Distribution::const_iterator cit = background_->begin();
         cit != background_->end(); ++cit) {
      int word_id = lexicon_->token2id(cit->first);
      phi_B_[word_id] = cit->second;
      background_sum += cit->second;
    }
    if (background_sum != 1.0) {
      for (int i = 0; i < W_; ++i)
        phi_B_[i] /= background_sum;
    }

    // Initialize the frequency distributions.
    for (int i = 0; i < W_; ++i) {
      int frequency = static_cast<int>(phi_B_[i] * AW_ * background_prior);
      if (frequency > 0) {
        NWB_[i] = frequency;
        NB_ += frequency;
      }
    }
  }
}

string TopicSumGibbsSampler::PrintSamplerInfo() {
  std::ostringstream oss;

  oss << GibbsSampler::PrintSamplerInfo();

  oss << "Model hyper-parameters: \n";
  oss << "\tlambda (BG / COL / DOC): \n";
  for (int k = 0; k < K_; k++)
    oss << "\t\t" << lambda_[k] << "\n";

  oss << "\tgamma (BG / COL / DOC): \n";
  for (int k = 0; k < 3; k++)
    oss << "\t\t" << gamma_[k] << "\n";

  return oss.str();
}

string TopicSumGibbsSampler::PrintSamplingIterationInfo() {
  std::ostringstream oss;

  oss << GibbsSampler::PrintSamplingIterationInfo();

  oss << "Counts of words in BKG Topic:\n";
  oss << setfill(' ') << setw(20) << "Word";
  oss << setfill(' ') << setw(20) << "Count\n";

  for (int w = 0; w < W_; w++) {
    if (NWB_[w] > 0) {
      oss << setfill(' ') << setw(20) << lexicon_->id2token(w);
      oss << setfill(' ') << setw(20) << NWB_[w];
      oss << "\n";
    }
  }

  for (int c = 0; c < C_; c++) {
    oss << "Count of words in the COL Topic: " << c << "\n";
    oss << setfill(' ') << setw(20) << "Word";
    oss << setfill(' ') << setw(20) << "Count\n";

    for (int w = 0; w < W_; w++) {
      if (NWC_[c][w] > 0) {
        oss << setfill(' ') << setw(20) << lexicon_->id2token(w);
        oss << setfill(' ') << setw(20) << NWC_[c][w];
        oss << "\n";
      }
    }
  }

  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      oss << "Count of words in the DOC Topic: " << d << " COL: " << c << "\n";
      oss << setfill(' ') << setw(20) << "Word";
      oss << setfill(' ') << setw(20) << "Count\n";

      for (int w = 0; w < W_; w++) {
        if (NWD_[c][d][w] > 0) {
          oss << setfill(' ') << setw(20) << lexicon_->id2token(w);
          oss << setfill(' ') << setw(20) << NWD_[c][d][w];
          oss << "\n";
        }
      }
    }
  }

  oss << "Total words in BKG Topic: ";
  oss << setfill(' ') << setw(20) << NB_;
  oss << "\n";

  for (int c = 0; c < C_; c++) {
    oss << "Total words in the COL Topic " << c << ": ";
    oss << NC_[c];
    oss << "\n";
  }

  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      oss << "Total words in the DOC Topic " << d << " of COL " << c << ": ";
      oss << ND_[c][d];
      oss << "\n";
    }
  }

  // Calculate model parameters
  CalculateParams();

  oss << "Background probability distribution:\n";

  oss << setfill(' ') << setw(20) << "Word";
  oss << setfill(' ') << setw(20) << "Proba\n";

  for (int w = 0; w < W_; w++) {
    oss << setfill(' ') << setw(20) << lexicon_->id2token(w);
    oss << setfill(' ') << setw(20);
    oss << fixed << setprecision(2) << phi_B_[w];
    oss << "\n";
  }

  oss << "Collection probability distributions:\n";

  for (int c = 0; c < C_; c++) {
    oss << "Probability distribution of COL Topic: " << c << "\n";
    oss << setfill(' ') << setw(20) << "Word";
    oss << setfill(' ') << setw(20) << "Proba\n";

    for (int w = 0; w < W_; w++) {
      oss << setfill(' ') << setw(20) << lexicon_->id2token(w);
      oss << setfill(' ') << setw(20);
      oss << fixed << setprecision(2) << phi_C_[c][w];
      oss << "\n";
    }
    oss << "\n";
  }

  oss << "Sentence topic distribution:\n";
  oss << setiosflags(ios::left);
  oss << setfill(' ') << setw(30) << "Sentence ID(COL-DOC-SENT)";
  oss << setfill(' ') << setw(10) << "BKG Topic";
  oss << setfill(' ') << setw(10) << "COL Topic";
  oss << setfill(' ') << setw(10) << "DOC Topic";
  oss << "\n";

  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        oss << c << "-" << d << "-" << setw(30) << left << s;
        for (int k = 0; k < K_; k++)
          oss << setfill(' ')
              << setw(10)
              << fixed
              << setprecision(2)
              << psi_[c][d][s][k];
        oss << "\n";
      }
    }
  }

  oss << "Last assignments:\n";
  oss << setfill(' ') << setw(30) << "Sentence ID(COL-DOC-SENT)";
  oss << setfill(' ') << setw(60) << "W1:T W2:T ...";
  oss << "\n";

  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        const Sentence& sent = doc.sentence(s);
        oss << c << "-" << d << "-" << setw(30) << left << s;
        string topic_list;
        for (int w = 0; w < sent.token_size(); w++) {
          const string& word = sent.token(w);
          int last_topic = lastZ_[c][d][s][w];

          oss << word.c_str() << ":" << last_topic << " ";
        }
        oss << "\n";
      }
    }
  }

  return oss.str();
}


void TopicSumGibbsSampler::GenerateConditionalDistribution(
    int colid,
    int docid,
    int sentid,
    int wordid,
    double* distri) {
  // Frequency distributions of words in topic k.
  int* NWZ = NULL;
  // Total number of words in topic k.
  int NZ = 0;
  // Probability of choosing topic k.
  double pk;
  double sum = 0;

  // Generates the distribution (P(z_i=j|z_{-i}, w))_j and stores it in distri.
  // We use the following formula to estimate that probability:
  // p(z_i=j|z_{-i},w) & \propto &
  //        \frac{n_{-i,j}^{w} + \lambda_j}
  //             {\sum_{w'} (n_{-i,j}^{w'} + \lambda_j)}
  //        .
  //        \frac{n_{-i,j}^{(cds)} + \gamma_j}
  //             {\sum_{k} ( n_{-i,k}^{(cds)} + \gamma_k)}
  //
  // n_{-i,j} in the formula means the number of words assigned to topic j,
  // except the current word.
  // In the main Train() function DecrementCounters has been called before
  // invoking this function, so the words taking into account already exclude
  // the current one.

  for (int k = 0; k < K_; k++) {
    if (k == 0) {
      NWZ = NWB_;
      NZ = NB_;
    } else if (k == 1) {
      NWZ = NWC_[colid];
      NZ = NC_[colid];
    } else if (k == 2) {
      NWZ = NWD_[colid][docid];
      NZ = ND_[colid][docid];
    } else {
      LOG(FATAL) << "In TopicSum, a word can only be generated from one of "
                 << "three topics: background, collection or document.";
    }
    CHECK(NWZ != NULL);
    pk = (NWZ[wordid] + lambda_[k])
      * (NZS_[colid][docid][sentid][k] + gamma_[k])
      / ((NZ + W_ * lambda_[k]) * (NS_[colid][docid][sentid] + gamma_sum_));
    distri[k] = pk;
    sum += pk;
  }

  // One must normalize, because previous equations are only proportional
  // to the true probability.
  if (sum) {
    for (int k = 0; k < K_; k++)
      distri[k] /= sum;
  }
}

double TopicSumGibbsSampler::CalculateModelLikelihood() const {
  double modlike = 0;
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        modlike += CalculateSentenceLikelihood(c, d, s);
      }
    }
  }
  return modlike;
}

double TopicSumGibbsSampler::CalculateSentenceLikelihood(int colid,
                                                         int docid,
                                                         int sentid) const {
  double sentlike = 0;

  const Sentence& sent =
    (*training_cols_[colid]).document(docid).sentence(sentid);

  for (int w = 0; w < sent.token_size(); w++) {
    int wordid = lexicon_->token2id(sent.token(w));

    double wordlike = 0;
    // Add likelihood of the word to be generated by the BKG.
    wordlike += phi_B_[wordid] * psi_[colid][docid][sentid][0];

    // Add likelihood of the word to be generated by the COL.
    wordlike += phi_C_[colid][wordid] * psi_[colid][docid][sentid][1];

    // Add likelihood of the word to be generated by the DOC.
    wordlike += phi_D_[colid][docid][wordid] * psi_[colid][docid][sentid][2];

    if (wordlike > 0)
      sentlike += log(wordlike);
  }
  return sentlike;
}

void TopicSumGibbsSampler::GetCollectionDistribution(int colid,
                                                     Distribution* wd) const {
  CHECK_GE(colid, 0);
  CHECK_LT(colid, C_);
  CHECK(wd);

  for (int w = 0; w < W_; w++) {
    double v = phi_C_[colid][w];
    if (v > 0) {
      const string& word = lexicon_->id2token(w);
      wd->insert(pair<string, double>(word, v));
    }
  }

  NormalizeDistribution(wd);
}

bool TopicSumGibbsSampler::GetPosteriorDistribution(string term,
                                                    Distribution* cd) const {
  if (!lexicon_->has_token(term)) return false;
  int termid = lexicon_->token2id(term);

  // Number of times that this term is assigned to the background distribution.
  cd->insert(make_pair("background", NWB_[termid]));

  // Number of times that this term is assigned to each of the collection
  // distributions.
  for (int c = 0; c < C_; c++) {
    cd->insert(make_pair(training_cols_[c]->id(), NWC_[c][termid]));
  }

  // Number of times that this term is assigned to any of the document-specific
  // distributions.
  int total_doc_frequency = 0;
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      total_doc_frequency += NWD_[c][d][termid];
    }
  }
  cd->insert(make_pair("document", total_doc_frequency));

  NormalizeDistribution(cd);
  return true;
}

void TopicSumGibbsSampler::GetBackgroundDistribution(Distribution* wd) const {
  CHECK(wd);
  wd->clear();

  for (int w = 0; w < W_; w++) {
    double v = phi_B_[w];
    if (v > 0) {
      const string& word = lexicon_->id2token(w);
      wd->insert(pair<string, double>(word, v));
    }
  }

  NormalizeDistribution(wd);
}

void TopicSumGibbsSampler::MultiplyParams(double multiplier) {
  // Update psi.
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        for (int k = 0; k < K_; k++)
          psi_[c][d][s][k] *= multiplier;
      }
    }
  }

  // Update phi for BKG.
  if (background_ == NULL) {
    for (int w = 0; w < W_; w++)
      phi_B_[w] *= multiplier;
  }

  // Update phi for COL.
  for (int c = 0; c < C_; c++)
    for (int w = 0; w < W_; w++)
      phi_C_[c][w] *= multiplier;

  // Update phi for DOC.
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++)
      for (int w = 0; w < W_; w++)
        phi_D_[c][d][w] *= multiplier;
  }
}

void TopicSumGibbsSampler::CalculateParams() {
  // Update psi.
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        for (int k = 0; k < K_; k++)
          psi_[c][d][s][k] += (NZS_[c][d][s][k] + gamma_[k])
                              / (NS_[c][d][s] + gamma_sum_);
      }
    }
  }

  // Update phi for BKG.
  if (background_ == NULL) {
    for (int w = 0; w < W_; w++)
      phi_B_[w] += (NWB_[w] + lambda_[0]) / (NB_ + W_ * lambda_[0]);
  }

  // Update phi for COL.
  for (int c = 0; c < C_; c++)
    for (int w = 0; w < W_; w++)
      phi_C_[c][w] += (NWC_[c][w] + lambda_[1]) / (NC_[c] + W_ * lambda_[1]);

  // Update phi for DOC.
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++)
      for (int w = 0; w < W_; w++)
        phi_D_[c][d][w] += (NWD_[c][d][w] + lambda_[2])
                           / (ND_[c][d] + W_ * lambda_[2]);
  }
}

void TopicSumGibbsSampler::IncrementCounters(int colid,
                                             int docid,
                                             int sentid,
                                             int wordid,
                                             int topic) {
  if (topic == 0) {
    if (background_ == NULL) {
      NWB_[wordid]++;
      NB_++;
    }

    NZS_[colid][docid][sentid][0]++;
    NS_[colid][docid][sentid]++;
  } else if (topic == 1) {
    NWC_[colid][wordid]++;
    NC_[colid]++;

    NZS_[colid][docid][sentid][1]++;
    NS_[colid][docid][sentid]++;

  } else if (topic == 2) {
    NWD_[colid][docid][wordid]++;
    ND_[colid][docid]++;

    NZS_[colid][docid][sentid][2]++;
    NS_[colid][docid][sentid]++;
  }
}

void TopicSumGibbsSampler::DecrementCounters(int colid,
                                             int docid,
                                             int sentid,
                                             int wordid,
                                             int topic) {
  if (topic == 0) {
    if (background_ == NULL) {
      NWB_[wordid]--;
      NB_--;
    }

    NZS_[colid][docid][sentid][0]--;
    NS_[colid][docid][sentid]--;
  } else if (topic == 1) {
    NWC_[colid][wordid]--;
    NC_[colid]--;

    NZS_[colid][docid][sentid][1]--;
    NS_[colid][docid][sentid]--;

  } else if (topic == 2) {
    NWD_[colid][docid][wordid]--;
    ND_[colid][docid]--;

    NZS_[colid][docid][sentid][2]--;
    NS_[colid][docid][sentid]--;
  }
}

void TopicSumGibbsSampler::DoInitialAssignment() {
  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        const Sentence& sent = doc.sentence(s);
        for (int w = 0; w < sent.token_size(); w++) {
          const string& word = sent.token(w);
          int wordid = lexicon_->token2id(word);

          int new_topic = SampleUniform(K_);

          IncrementCounters(c, d, s, wordid, new_topic);

          lastZ_[c][d][s][w] = new_topic;
        }
      }
    }
  }
}

void TopicSumGibbsSampler::DoIteration() {
  scoped_array<double> distri(new double[K_]());

  for (int c = 0; c < C_; c++) {
    const DocumentCollection& col = *training_cols_[c];
    for (int d = 0; d < col.document_size(); d++) {
      const Document& doc = col.document(d);
      for (int s = 0; s < doc.sentence_size(); s++) {
        const Sentence& sent = doc.sentence(s);
        for (int w = 0; w < sent.token_size(); w++) {
          const string& word = sent.token(w);
          int wordid = lexicon_->token2id(word);
          int last_topic = lastZ_[c][d][s][w];

          DecrementCounters(c, d, s, wordid, last_topic);

          GenerateConditionalDistribution(c, d, s, wordid, distri.get());

          int new_topic = SampleMulti(distri.get(), K_);

          IncrementCounters(c, d, s, wordid, new_topic);

          lastZ_[c][d][s][w] = new_topic;
        }
      }
    }
  }
}

}  // namespace topicsum
