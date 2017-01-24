#ifndef NGRAMLSA_H
#define NGRAMLSA_H

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

// define topic index k to LogP p(w|h,k)
typedef std::map<unsigned short, FloatCount>   Topic_Cnt_Table;

// define compact count obj for N-gram LSA
class NgramLSAStats: public NgramCounts<NgramCount>
{
private:
   std::vector<Topic_Cnt_Table>   topic_cnt_table;

public:
   NgramLSAStats(Vocab &vocab, unsigned int order)
        : NgramCounts<NgramCount>(vocab, order) {};
   virtual ~NgramLSAStats();

   // insert count for topic k
   // this should basically first traverse the original trie-tree.
   // The node returned would be the idx to the topic_cnt_table.
   //FloatCount *insertCount(const VocabIndex *words, int k);
   void insertCount(const VocabIndex *words, int k, FloatCount cnt);

   // extract counts only for topic k
   void createCountTrie(NgramCounts<FloatCount>* cnt_obj, int k);
};


// Forward declaration
class DirichletTree;
class LSA;

enum SCHEME {FKNESER = 0, ABS = 1, FRAC_KNESER = 2, WITTEN = 3};

class NgramLSA : public Debug
{
private:
// the 2 fields below are compulsory for janus module
   char* name;
   int   useN;
// end

   int order;
   int num_topic; 
   int debug_mode;
   int demmax;
   int dlenmax;

   Vocab* lmvocab;

   // accumulator for NgramLSA
   //NgramCounts<FloatCount> **topic_float_acc;

   // new stuff (compact representation of topic_float_acc)
   NgramLSAStats*  topic_cnt_acc;

   // document-level ngram counts
   std::vector<NgramCounts<FloatCount>* > doc_ng_cnt;

   // topic-dependent backoff Ngram
   Ngram** topic_lm;

   // adapted N-gram LM
   Ngram* adapted_lm;

   // DirichletTree prior
   DirichletTree* dir_tree;

   // for VBEM only (temp storage)
   double* nt;
   double* pnt;
   double** q;
   double** topic_prob_cache;
   double* gamma;
   double* ap;
   std::vector<double*> gammas;
   double forget;

   // LSA (unigram)
   LSA* ref_lsa;

   // discount const D
   double discount1;
   double discount2;

   // a list of words whose mass on any context won't be adapted (useful for stopwords and the like)
   std::map<int,bool> non_adapt_word;

// START May 13 08
// total likelihood on the training data: before and after adaptation
   NgramCounts<FloatCount>* log_ll_bg;
   NgramCounts<FloatCount>* log_ll_adapt;
// END May 13 08

   // stopword list
   bool* is_stopword;

   // topic distribution per sentence
   double** doc_sent_prob;
   int num_sentence;

   // rescoring stuff
   double* prv_lsa_prob;
   double* cur_lsa_prob;
   double* lalpha;

   // noise vocabulary
   std::map<std::string, bool> is_noise_vocab;

   // sentence pos in ngram_array
   std::vector<int> sent_pos;

   // janus NGramLM obj
   void* lmP;

   // N-gram trie to store the sticky pair together with the pointwise mutual information score
   NgramCounts<FloatCount>* sticky_pair;

   // word mapper
   std::map<std::string, std::string> word_mapper;

protected:
   // set all n-gram entries in lm to 1/total_mass
   void uniformize_lm(Ngram* lm);

   // update topic LM (this should call reestimate_lm())
   void update_model(const char* model_cnt_flist, const char* tmp_dir, double learn, bool update_prior, int discount_scheme);

   void get_ngrams(char* sentence, std::vector<VocabIndex*>& ngram_idx);

   // given a list of ngrams (sorted by word position)
   // only ngrams from [0, win_size) will be considered for the E-step
   // useful for PPL evaluation

   void adapt_lm(Ngram* bg_lm, double* marginal, double scale);

   void accu_ll(NgramCounts<FloatCount>& doc_cnt, Ngram* bg_lm);

   // only bigram history is supported at this moment
   void trace_history(const std::map<int, std::pair<int, std::string> >& prv_word, VocabIndex* context, int from_node, int len, 
       const std::map<int, int>& start_time_map);

   // rescore LM prob in a link in lattice file (IBM format)
   int rescore_link(
      std::ofstream& lat_os,
      const int& prv_from,
      std::vector<std::string>& to_print,
      std::vector<std::string>& competitors,
      std::vector<double>& lm_prob,
      std::vector<bool>& stopwords,
      const std::map<int, std::pair<int, std::string> >& prv_word, 
      double lambda0,
      double lambda,
      const std::map<int, int>& start_time_map,
      const std::map<int, std::vector<int> >& start2node_map);

   // super-rescoring over the links across nodes
   int rescore_link1(
      std::ofstream& lat_os,
      std::vector<int>& parent_nodes,
      std::vector<std::string>& to_print,
      std::vector<std::string>& competitors,
      std::vector<double>& lm_prob,
      std::vector<bool>& stopwords,
      const std::map<int, std::pair<int, std::string> >& prv_word, 
      double lambda,
      const std::map<int, int>& start_time_map,
      const std::map<int, std::vector<int> >& start2node_map);

   void get_phrases(const std::vector<VocabIndex*>& ngram_array, FloatCount threshold, std::vector<int>& phrases);

   bool is_context_exists(VocabIndex* context);

public:
   NgramLSA(Vocab* vocab, int _order, int _num_topic);
   ~NgramLSA();

   // reset dirichlet-tree prior
   void reset_prior();

   int get_doc_ngrams(File& f, std::vector<VocabIndex*>& ngram_array, bool& eof);

   // reestimate topic LM k
   //void reestimate_lm(int k, int discount_scheme);
   void reestimate_lm(NgramCounts<FloatCount>* cur_count_obj, int k, int discount_scheme);

   // init model
   void init(const char* bg_lm_file);

   // vbem on a doc count file
   void vbem(const char* doc_count_file, int iter, double* prior_nt, bool accu, double wgt=1.0);
   //void vbem(NgramCounts<FloatCount>& doc_cnt, int iter, double* prior_nt, bool accu);
   void vbem(NgramCounts<FloatCount>& doc_cnt, int iter, double* prior_nt, bool accu, bool use_unigram, bool fast_em, double wgt=1.0);

   void vbem(std::vector<VocabIndex*> ngram_array, int win_size, int iter, double* prior_nt, bool accu, bool use_unigram, bool sentence_level, int start=0, double wgt=1.0);

   // phrase-level VBEM
   //void phrase_vbem(std::vector<VocabIndex*> ngram_array, int iter, double* prior_nt, FloatCount threshold, bool accu, bool use_unigram);
   void phrase_vbem(std::vector<VocabIndex*> ngram_array, int iter, double* prior_nt, int start, int win_size, FloatCount threshold, bool accu, bool use_unigram, double wgt=1.0);

   // write fractional topic counts
   void write_cnt(const char* output_cnt_file, int k);

   // VBEM training
   void train(const char* train_cnt_flist, const char* test_cnt_flist, const char* tmp_dir, int iter, double learn, int discount_scheme);

   // bootstrap
   void bootstrap(const char* doc_cnt_flist, const char* model_cnt_flist, const char* tmp_dir, int discount_scheme, Ngram* bg_lm);

   // load documents
   void read_doc_cnts(const char* doc_cnt_flist);
   void read_doc(const char* doc_flist);
   NgramCounts<FloatCount>* gen_doc_cnt(File& f, bool& eof);

   // generate adapted N-gram
   Ngram* gen_ngram_lm(const char* param_file);
   Ngram* mix_ngram_lm(const double* lambda);
   void set_adapted_lm(Ngram* lm) { adapted_lm = lm; }
   Ngram* get_adapted_lm() { return adapted_lm; }
   void delete_adapted_lm() { if (adapted_lm) delete adapted_lm; adapted_lm = NULL; }
   double* get_gamma() const { return gamma; }

   // likelihood function (wgt: static wgt?)
   LogP log_likelihood(NgramCounts<FloatCount>* doc_cnt_obj, double& doc_cnt, double* wgt);
   LogP word_prob(const VocabIndex* ngram, const double* wgt);

   // read & write model
   void write_model(const char* out_model) const;
   void write_alpha(const char* out_model) const;
   //void read_model(const char* in_model);

   // read the reference unigram LSA model
   void read_lsa_model(const char* lsa_model, const char* vocab_file);
   void read_topic_model(const char* model_list, int limitVocab = 0);

   // set discount const D
   void set_discount1(double D) { discount1 = D; }

   void set_discount2(double D) { discount2 = D; }

   // for PPL evaluation
   void testPPL(const char* doc_cnt_flist);

   // incr adaptation 
   void incr_test_PPL(const char* doc_file, double forget);

   // adapt text and print out the best topic wgt
   //void adapt_text(const char* doc_file);
   void adapt_text(const char* doc_file, const char* output_lm = NULL);

   // perform marginal adaptation on the background LM to each LSA topic
   void marginal_adapt(const char* bg_lm_file, const char* non_adapt_word_file, const char* output_lm, double scale);

   void load_non_adapt_list(const char* non_adapt_word_file);

   void load_stoplist(const char* filename);

   // clear accumulators
   void clear_accum();

   // create accumulators
   void create_accum();

   int get_order() const { return order;}
   Ngram* get_topic_lm(int k) { return topic_lm[k]; }

   // return 0 if everything is okay
   int rescore_lat(const char* inlat_file, const char* outlat_file, double lambda0, double lambda);

   // new lat rescoring with node merging (=> bad results, don't use it)
   int rescore_lat1(const char* inlat_file, const char* outlat_file, double lambda);

   void update_scale_factor(double beta);
   // words in this list will be skipped during context history tracing. e.g. <SIL>, $, <noise> etc
   void load_noise_vocab(const char* filename);

   void test_PPL(const char* input_doc_file, const char* prob_stream_file);

   void read_ap(std::ifstream& is);
   void write_ap(std::ofstream& os);

   int get_num_topic() const {return num_topic;}

   Vocab* get_vocab() const {return lmvocab;}

   void set_forget(double f) { forget = f; }

   int get_sent_pos(int id) const { return sent_pos[id]; }

   DirichletTree* get_dir_tree() const {return dir_tree;}

   int get_num_sent() const { return sent_pos.size()-1; }

   bool check_stopword(VocabIndex id) const { return is_stopword[id]; }

   void set_ngram_lm(void* obj) { lmP = obj; }
   void* get_ngram_lm() { return lmP; }

   // load sticky pair
   void load_sticky_pair(const char* filename);

   // test program to chunk the input document based on the sticky pair info
   void test_sticky_pair(const char* input_doc_file, FloatCount threshold);

   // load word mapper
   void load_word_mapper(const char* filename);
};

#endif
