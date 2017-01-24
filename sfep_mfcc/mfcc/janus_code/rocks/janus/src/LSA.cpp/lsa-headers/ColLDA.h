#ifndef ColLDA_H
#define ColLDA_H

#include <vector>
#include <map>
//#include <stdio.h>
//
//#include "LM.h"
#include "Ngram.h"
//#include "NgramStats.h"
//#include "Discount.h"
//
//#ifdef USE_SARRAY
//
//# define PROB_INDEX_T   SArray
//# define PROB_ITER_T    SArrayIter
//# include "SArray.h"
//
//#else /* ! USE_SARRAY */
//
//# define PROB_INDEX_T   LHash
//# define PROB_ITER_T    LHashIter
//# include "LHash.h"
//
//#endif /* USE_SARRAY */
//
//#include "Trie.h"

// Forward declaration
class DirichletTree;
class LSA;

enum SCHEME {FKNESER = 0, ABS = 1};

class ColLDA : public Debug
{
private:
   int order;
   int num_topic; 
   int debug_mode;
   int demmax;
   int dlenmax;

   Vocab* lmvocab;

   // accumulator for ColLDA
   NgramCounts<FloatCount>* ng_float_acc;

   // V*2
   double**      phrase_indicator_acc;
   double**      phrase_indicator;

   // document-level ngram counts
   std::vector<NgramCounts<FloatCount>* > doc_ng_cnt;

   // background backoff Ngram
   Ngram* bg_lm;

   // DirichletTree prior
   DirichletTree* dir_tree;

   // for VBEM only (temp storage)
   double* nt;
   double* pnt;
   double** q;
   double** qm;
   double* gamma;
   double* ap;
   std::vector<double*> gammas;
   double forget;

   // vocab file (LSA only)
   const char* lsa_vocab_file;

   // LSA (unigram)
   LSA* ref_lsa;

   // discount const D
   double discount1;
   unsigned* min_count;

   // max # of word per chunk (prior info)
   int max_chunk_len;

   // stopword list
   bool* is_stopword;

   // chunks prob per sentence in a current doc under VBEM: P(x|sentence)
   double* chunk_prob;

   double* ng_prob;
   double** lsa_lprob;
   double* num_chunk_ratio;
   double* num_chunk_ratio0;

protected:
   // set all n-gram entries in lm to 1/total_mass
   void uniformize_lm(Ngram* lm);

   // update topic LM (this should call reestimate_lm())
   void update_model(const char* tmp_dir, double learn, bool update_prior, int discount_scheme);

   // clear accumulators
   void clear_accum();

   // create accumulators
   void create_accum();

   // reestimate background LM
   void reestimate_lm(int discount_scheme);

   // reset dirichlet-tree prior
   void reset_prior();

   void get_ngrams(char* sentence, std::vector<VocabIndex*>& ngram_idx);
   int get_doc_ngrams(File& f, std::vector<VocabIndex*>& ngram_array, bool& eof);

   // given a list of ngrams (sorted by word position)
   // only ngrams from [0, win_size) will be considered for the E-step
   // useful for PPL evaluation
   void vbem(const std::vector<VocabIndex*>& ngram_array, int win_size, int iter, double* prior_nt, bool accu);
   void bootstrap_vbem(const std::vector<VocabIndex*>& ngram_array, int win_size, int iter, const double* src_ap, 
                            const double* foreign_num_chunk, bool accu);

   void init_chunk_ratio(const std::vector<VocabIndex*>& ngram_array, int win_size, const double* foreign_chunk_prob);

public:
   ColLDA(Vocab* vocab, int _order, int _num_topic);
   ~ColLDA();

   // init model
   void init(const char* bg_lm_file);

   // vbem on a doc count file
   void vbem(const char* doc_count_file, int iter, double* prior_nt, bool accu);
   //void vbem(NgramCounts<FloatCount>& doc_cnt, int iter, double* prior_nt, bool accu);
   void vbem(NgramCounts<FloatCount>& doc_cnt, int iter, double* prior_nt, bool accu);

   // write fractional topic counts
   void write_cnt(const char* output_cnt_file);

   // VBEM training
   void train(const char* train_cnt_flist, const char* test_cnt_flist, const char* tmp_dir, int iter, double learn, int discount_scheme);

   // set vocab file
   void set_lsa_vocab_file(const char* file) { lsa_vocab_file = file; }

   // load documents
   void read_doc(const char* doc_flist);
   NgramCounts<FloatCount>* gen_doc_cnt(File& f, bool& eof);

   // generate adapted N-gram
   Ngram* gen_ngram_lm(const char* param_file);

   // likelihood function (wgt: static wgt?)
   LogP log_likelihood(std::vector<VocabIndex*>& ngram_array, double& doc_cnt, bool bg_only);
   LogP word_prob(const VocabIndex* ngram, int pos, bool bg_only);

   // read & write model
   void write_model(const char* out_model) const;

   // read the reference unigram LSA model
   void read_ref_model(const char* ref_model);

   // set discount const D
   void set_discount1(double D) { discount1 = D; }

   // read a list of counts
   void read_counts(const char* count_list);

   // incr adaptation 
   void incr_test_PPL(const char* doc_file, double forget);

   // adaptation at the doc-level, then test PPL
   //void doc_test_PPL(const char* doc_file, double forget_factor);
   void doc_test_PPL(const char* doc_file, const char* hypo_file, double forget_factor);

   // adapt text and print out the best topic wgt
   void adapt_text(const char* doc_file);

   // set max chunk len
   void set_chunk_len(int len) { max_chunk_len = len; }

   // load stopword list
   void load_stoplist(const char* filename);

   // set min count
   void set_min_count(unsigned* cnt);

   // print context weights
   void print_context() const;

   // take nbest list
   void nbest_rescore(const char* doc_file, double forget_factor);

   friend class bColLDA;
};

class bColLDA : public Debug
{
private:
   ColLDA *src_lsa;
   ColLDA *tgt_lsa;

public:
   bColLDA(ColLDA* src, ColLDA* tgt) : src_lsa(src), tgt_lsa(tgt) {}
   ~bColLDA() {}

   void train(const char* src_flist, const char* tgt_flist, const char* tmp_dir, int iter, double learn, int discount_scheme);

   void write_model(const char* src_out_model, const char* tgt_out_model) const;
};

#endif
