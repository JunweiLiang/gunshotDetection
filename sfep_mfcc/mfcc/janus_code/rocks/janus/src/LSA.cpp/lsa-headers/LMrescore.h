#ifndef LMRESCORE_H
#define LMRESCORE_H

#include <string>
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

class LMrescore : public Debug
{
private:
// the 2 fields below are compulsory for janus module
   char* name;
   int   useN;
// end

   int order;
   int num_topic; 
   int debug_mode;

   Vocab* lmvocab;

   // topic-dependent backoff Ngram
   Ngram** topic_lm;

   // mixture weight
   double* gamma;

   // stopword list
   bool* is_stopword;

   // noise vocabulary
   std::map<std::string, bool> is_noise_vocab;

   // sentence pos in ngram_array
   std::vector<int> sent_pos;

   // word mapper
   std::map<std::string, std::string> word_mapper;

   // janus NGramLM obj
   void* lmP;

protected:
   void get_ngrams(char* sentence, std::vector<VocabIndex*>& ngram_idx);

   // only bigram history is supported at this moment
   void trace_history(const std::map<int, std::pair<int, std::string> >& prv_word, VocabIndex* context, int from_node, int len, const std::map<int, int>& start_time_map);

   // rescore LM prob in a link in lattice file (IBM format)
   int rescore_link(
      std::ofstream& lat_os,
      const int& prv_from,
      std::vector<std::string>& to_print,
      std::vector<std::string>& competitors,
      std::vector<double>& lm_prob,
      std::vector<bool>& stopwords,
      const std::map<int, std::pair<int, std::string> >& prv_word, 
      const std::map<int, int>& start_time_map,
      const std::map<int, std::vector<int> >& start2node_map);

   bool is_context_exists(VocabIndex* context);

public:
   LMrescore(Vocab* vocab, int _order, int _num_topic);
   ~LMrescore();

   int get_doc_ngrams(File& f, std::vector<VocabIndex*>& ngram_array, bool& eof);

   // generate adapted N-gram
   Ngram* gen_ngram_lm(const char* param_file);
   Ngram* mix_ngram_lm(const double* lambda);
   double* get_gamma() const { return gamma; }

   LogP word_prob(const VocabIndex* ngram, const double* wgt);

   void read_topic_model(const char* model_list, int limitVocab = 0);

   void load_stoplist(const char* filename);

   int get_order() const { return order;}
   Ngram* get_topic_lm(int k) { return topic_lm[k]; }

   // return 0 if everything is okay
   int rescore_lat(const char* inlat_file, const char* outlat_file);

   // words in this list will be skipped during context history tracing. e.g. <SIL>, $, <noise> etc
   void load_noise_vocab(const char* filename);

   // load word mapper
   void load_word_mapper(const char* filename);

   int get_num_topic() const {return num_topic;}

   Vocab* get_vocab() const {return lmvocab;}

   bool check_stopword(VocabIndex id) const { return is_stopword[id]; }

   void set_ngram_lm(void* obj) { lmP = obj; }
   void* get_ngram_lm() { return lmP; }
};

#endif
