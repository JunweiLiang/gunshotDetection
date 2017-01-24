#ifndef LDA_LDA_H
#define LDA_LDA_H

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
//#include "feature.h"

#define CLASS_DEFAULT		50
#define EMMAX_DEFAULT		100
#define DEMMAX_DEFAULT		20
#define DLENMAX_DEFAULT		20000
#define EPSILON_DEFAULT		1.0e-4
#define SMALL_PROB              1.0e-50
#define MIN_SPLIT_COUNT         2.0

#define UNK                     "<UNK>"

#define CONVERGENCE 1.0e-4
#define RANDOM ((double)rand()/(double)RAND_MAX)

#define MAX_GRAD_ITERATION 100

enum {COPY=0, ENTROPY, MODE};

//using namespace std;

/*
data[i]:    all training documents indexed by i
alpha[k]:   dirichlet prior
beta[v][k]: unigram prob of vocab v in each topic k
nclass:     # of latent topics
nlex:       size of vocabulary
dlenmax:    max vocabulary size of a training document
emmax:      max. iteration for outer-EM
demmax:     max. iteration for inner-EM
epsilon:    threshold for testing convergence

 Accumulators:
 betas:            accumulators for beta
 gammas[n][k]:     accumulators for gamma for each document n
*/

/* Notes for code reading by Wilson Tam */
/*
d:       multinomial document (d->id[.] is the vocabulary index and 
d->cnt[.] is # of occurrences in d)

gamma:   parameters of variational posterior of q(theta) for the current document

q[l][k]: variational posterior q_l(k) where l is the vocabulary position, 
d->id[l] denotes the corresponding vocabulary index, and
k is the latent topic

nt[k]:   sum_l q[l][k] (accumulated "votes" on topic k)

pnt[k]:  previous nt[k] (check for convergence)

ap[k]:   exp(digamma(alpha[k] + nt[k]) - digamma(sum_gamma)) ("prior" on topic k).
alpha[k] + nt[k] represents the total topic counts including the
prior from alpha[k].

alpha[k]:   dirichlet prior

beta[v][k]: unigram probabilities

L:          length of the current document

K:          total # of latent classes

emmax:      max. number of VB-EM iterations
*/

class document;
typedef std::vector<std::pair<int, double> > PROB_TABLE;

//class DirichletTree;
class Prior;

// From SRILM
class Ngram;
class Vocab;

class LSA
{
private:
// yct: fields particularly for janus purpose
   char* name;
   int useN;

   // buffer for variational inference
   double *gamma;
   double *nt;
   double *pnt;
   double *ap;
   double **q;
   
   // accumulators
   double **gammas;
   double **betas;
   
   // document array
   document *data;
   // current document pointer
   document *_dp;
   
   // model param
   double *alpha;
   double **beta;

   // for LM adaptation
   double *adapt_alpha;
   double *MAP_wgt;
   
   const char* model_name;
   int nclass;
   int max_class;
   int nlex;
   int dlenmax;
   int demmax;
   int num_doc;
   double num_word;
   
   // vocabulary list
   // map input std::string into LDA vocab idx (integer)
   std::map<std::string,int>   lda_vocab;
   
   // inverse mapping
   std::vector<std::string>     lda_idx2vocab;
   
   // synchronize with NGramLM* in janus
   void*   ngram_lm;
   
   // KL-divergence KL(uniform, p(k|v))
   double* kl_dist;
   bool    use_kl_wgt;

   // index for doc start and # of training doc
   int from_doc;
   int size_doc;

   double  psg;
   double* spg;
   double  M;
   double  kl_acc;

   // for class->vocab_id mapping
   std::map<std::string, std::vector<int> > class2word;

   // cache prob table for all classes
   std::map<std::string, PROB_TABLE>   prob_table;

   // flag to save memory
   bool save_memory;

   // mapping from i index in q[i] to lda voc
   int* qi2voc;

   // pointer to the prior
   Prior* dir_tree;

// START Wilson 25 May 2007
// add word map (e.g. surface->stem in arabic)
   std::map<std::string, std::string> wordmap;
// END Wilson 25 May 2007

// Wilson Sept 27 07
// for (bLSA stuff only during VB-EM)
   const double* topic_ratio;

   int unk_id;

protected:
   void load_vocab(const char* vocab_file);
   
   double lda_learn (document *data, int num_doc, int emmax, double epsilon, double learn);
   
   void init_param();
   
   void accum_gammas (int n, double wgt=1.0);
   //void accum_betas (const document *dp);
   void accum_betas (const document *dp, int doc_id, double wgt=1.0);

   void grad_ascent(int iter, double learn);

// Start Wilson 12/11/2006
   void fast_vbem(const document *d, Prior* dir_tree, double forget = 1.0);
   double lda_bootstrap(document *data, int n, int emmax, double epsilon, double learn, const char* prior_file);
// End Wilson 12/11/2006
   
public:
   LSA(const char* _model_name, const char* vocab_file, int _nclass, int _max_class, int _dlenmax, int _demmax, Prior* _dir_tree);
   ~LSA();
   
   // perform inference on current document (no adaptation)
   double infer(const document *_dp);
   
   // perform inference on current word context and adapt the dirichlet prior.
   void adapt_sent(const document *_dp, double forget = 0.4);
   void adapt_sent(const std::vector<std::string>& tk, const double* conf=0, 
                   int start_pos=0, int len=-1, double forget = 0.4);
   
   void reset_prior();
   
   // compute multinomial given a buffer
   const document* multinomial(const std::vector<std::string>& tk, const double* conf=0, 
                               int start_pos=0, int len=-1);
   
   int get_vocab_id(const std::string& w) const;
   const std::string& get_vocab(int vid) const { return lda_idx2vocab[vid]; }
   
   // train the lda model given the multinomial training file
   double learn(const char* data_file, 
                int emmax=EMMAX_DEFAULT, 
                double epsilon=EPSILON_DEFAULT, int from=0, int len=-1, double learn=0.1);

   double learn_bootstrap(const char* data_file, int emmax, double epsilon, int from, int len, double learn, const char* prior_file);
   
   void load_model(const char* model_file);
   void write_model(const char* out_model_file);
   void write_arpa(const char* lm_file);

   // dump accumulators
   void write_accum(const char* out_accum_file);
   
   // compute prob for buffer tk
   double MAP_prob(const std::string& word) const;
   double MAP_prob(int v) const;

   int get_num_class() const { return nclass; }
   int get_num_vocab() const { return nlex; }
   int get_num_doc() const { return num_doc; }
   
   // training operators
   void create_accum();
   void clear_accum();
   void accum(const document* dp, int i);
   void update_model(double learn);
   void add_accum(const char* file);
   int read_doc(const char* data_file);
   void accu_doc();

   const document* get_doc(int idx);
   
   // get nbest words from class k
   PROB_TABLE get_nbest_word(int k) const;
   
   void  set_ngram_lm(void* lm) { ngram_lm = lm; }
   void* get_ngram_lm() const { return ngram_lm; }
   
   void compute_kl();
   PROB_TABLE sort_kl() const;
   
   // perform VB
   void vbem(const document *d, const double* myalpha=0, double forget=1.0);
   ///void vbem(const document *d, DirichletTree* dir_tree, double forget=1.0);
   void vbem(const document *d, Prior* dir_tree, double forget=1.0);

   // set flag for kl_wgt
   void set_kl_wgt(bool flag) { use_kl_wgt = flag; }

   void set_save_mem(bool flag) { save_memory = flag; }

   // add word class
   void add_wordclass(const std::string& class_name, const std::string& word);
   void add_wordclass(const char* filename, const std::string& class_name);
   void load_wordclass(const char* filelist);

   // write subsmodel to be embedded into class-based lm
   void write_subs(const char* filename, const std::string& class_name);

   // compute Pr(w | class) using LSA
   PROB_TABLE prob_word_class(const std::string& class_name, bool is_sort=true, bool diff = false);

   // return kl sum
   double get_kl() const { return kl_acc; }

   // get class_list
   std::vector<std::string> get_classlist() const;

   // compute Pr(k|w) based on current adapt_alpha
   void posterior(const std::string& word, double* p);

   int get_class_size(std::string c) const;

   // remove vocabulary from the unigram prob
   void remove_vocab(int vid);
   void merge_vocab(int v1, int v2);
   void write_vocab(const char* filename);

   // return posterior counts
   const double* get_gamma() const { return gamma; }
   const double* get_prior() const { return ap; }
   const double* const* get_q() const { return q; }

   // return adapted mixture weight
   double* get_MAP_wgt() const { return MAP_wgt; }

   double* get_alpha() const { return alpha; }
   double ** get_beta() const { return beta; }
   int* get_qi2voc() const { return qi2voc; }

   // compute variational log likelihood
   double var_ll(const document* _dp);

   double kl_divergence(const double* p, const double* q, int size) const;

// Start Wilson May 25 2007
   // wordmap
   bool load_wordmap(const char* wordmap_file);
   std::string get_wordmap(const std::string& str) const;
// End Wilson May 25 2007

   void set_topic_ratio(const double* _topic_ratio) {topic_ratio = _topic_ratio;}

   void transform_beta(const char* transform_matrix);

   void init_flat();

   // change vocab
   void add_vocab(const char* new_vocab_file);

   // write the log ratio (in base 10)
   //void write_log_ratio_arpa(const char* lm_file, double* bg_prob, const char* stopword_file, double beta=0.7);
   void write_log_ratio_arpa(const char* lm_file, double* bg_prob, const char* stopword_file, const char* vocab_file, double beta=0.7);

   bool is_unk(const std::string& w) const { return (this->get_vocab_id(w) == unk_id); }

   Prior* get_dirtree() const { return dir_tree; }

   void set_gammas(const double* gamma, int d);
   const double* get_gammas(int d) const { return gammas[d]; }

   // merge/shrink dirtree
   double check_info_loss(int k1, int k2);
   double entropy(double* p, int size);
   int merge_topic();

   friend class bLSA;
   friend class ColLDA;
};

// sorting operator for log prob table in descending order
class Sort_Table_GT
{
public:
   bool operator()(const std::pair<int,double>& a, const std::pair<int,double>& b) const { return (a.second > b.second); }
};

PROB_TABLE sort_table(double* table, int size);

#endif
