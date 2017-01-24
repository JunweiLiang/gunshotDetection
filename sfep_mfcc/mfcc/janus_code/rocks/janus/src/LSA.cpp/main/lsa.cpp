#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include <numeric>
#include "lsa.h"
#include "reader.h"
#include "writer.h"
#include "util.h"
#include "dmatrix.h"
#include "gamma.h"
#include "likelihood.h"
#include "lsafeature.h"
#include "newton.h"
#include "split_string.h"
#include "dirichlet.h"

using namespace std;

static double LOG10 = log(10.0);

//DirichletTree* dir_tree = 0;
//Prior* dir_tree = 0;

// Start Wilson 12/11/2006
// A flag to tell if the term exp(E_q[log theta_k]) is given
bool prior_given = false;
// End Wilson 12/11/2006

LSA::LSA(const char* _model_name, const char* vocab_file, int _nclass, int _max_class, int _dlenmax, int _demmax, Prior* _dir_tree) : 
name(0),
useN(0),
gammas(0),
betas(0),
data(0),
model_name(_model_name), 
nclass(_nclass),
max_class(_max_class),
dlenmax(_dlenmax),
demmax(_demmax),
num_doc(0),
num_word(0.0),
ngram_lm(0),
use_kl_wgt(false),
from_doc(0),
size_doc(-1),
psg(0.0),
M(0.0),
kl_acc(0.0),
save_memory(false),
dir_tree(_dir_tree),
topic_ratio(0)
{
   // set max. class allowed (useful when we add more topics later)
   if (max_class < nclass)
      max_class = nclass;

   // load LDA vocabulary list
   this->load_vocab(vocab_file);
   
   /*
   *  initialize buffers
   *
   */
   if ((q = dmatrix(dlenmax, max_class)) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate q.\n");
      exit(-1);
   }
   if ((gamma = new double[max_class]) == NULL)
   {
      fprintf(stderr, "lda_learn:: cannot allocate gamma.\n");
      exit(-1);
   }
   if ((ap = new double[max_class]) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate ap.\n");
      exit(-1);
   }
   if ((nt = new double[max_class]) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate nt.\n");
      exit(-1);
   }
   if ((pnt = new double[max_class]) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate pnt.\n");
      exit(-1);
   }
   if ((MAP_wgt = new double[max_class]) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate MAP_wgt.\n");
      exit(-1);
   }
   
   /* allocate parameters */
   if ((alpha = new double[max_class]) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate alpha.\n");
      exit(1);
   }
   if ((beta = dmatrix(nlex, max_class)) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate beta.\n");
      exit(1);
   }
   if ((adapt_alpha = new double[max_class]) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate adapt_alpha.\n");
      exit(1);
   }
   
   /* allocate kl_dist table */
   if ((kl_dist = new double[nlex]) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate kl_dist table.\n");
      exit(1);
   }
   set_array(kl_dist, nlex, 1.0);

   if ((spg = new double[max_class]) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate spg table.\n");
      exit(1);
   }
   set_array(spg, max_class, 0.0);
   
   /*
   *  randomize a seed
   *
   */
   srand(time(NULL));
   
   this->load_model(model_name);
      
   // create temp document
   _dp = new document(dlenmax);

   // create document
   qi2voc = new int[nlex];

   // get UNK idx
   unk_id = this->get_vocab_id(UNK);
}

void LSA::load_model(const char* model_name)
{
   /* open model file if any */
   FILE *f_ap, *f_bp;
   
#ifdef BINARY
   if (!model_name || ((f_ap = fopen(strconcat(model_name, ".alpha"), "rb")) == NULL)
      || ((f_bp = fopen(strconcat(model_name, ".beta"), "rb"))  == NULL))
#else
   if (!model_name || ((f_ap = fopen(strconcat(model_name, ".alpha"), "r")) == NULL)
      || ((f_bp = fopen(strconcat(model_name, ".beta"), "r"))  == NULL))
#endif
   {
      fprintf(stderr, "LSA:: cannot read model\n");
      model_name = 0;
      init_param();

      // uniform kl weight
      set_array(kl_dist, nlex, 1.0);
   } else {
      if (dir_tree) {
         dir_tree->read_alpha(f_ap);
         read_matrix(f_bp, beta, nlex, nclass);

         for (int k=0; k < nclass; k++)
            alpha[k] = 1.0 + RANDOM;
      } else {
         lda_read (f_ap, f_bp, alpha, beta, nclass, nlex);

         // check model integrity (alpha, beta > 0)
         for (int k=0; k < nclass; k++) {
            if (alpha[k] <= 0.0) {
               cerr << "Warning: alpha " << k << " <= 0!" << endl;
               alpha[k] = 1.0 + RANDOM;
            }
         }
      }

      bool error = false;
      for (int v=0; v < nlex; v++) {
         double sum_z = accumulate(beta[v], beta[v] + nclass, 0.0);
         if (sum_z <= 0.0) {
            for (int k=0; k < nclass; k++) {
               if (beta[v][k] <= 0.0) {
                  //cerr << "Warning: beta " << v << " " << k << " <= 0!" << endl;
                  beta[v][k] = SMALL_PROB;
               }
            }
            error = true;
         }
      }
      // renormalize beta due to zero prob
      if (error) {
         cerr << "Warning: have some zero beta, need to renormalize " << endl;
         normalize_matrix_col(beta, beta, nlex, nclass);
      }

      // compute kl using existing model
      //if (use_kl_wgt)
      this->compute_kl();

      fclose(f_ap);
      fclose(f_bp);
   }
   
   if (dir_tree) {
      dir_tree->reset();
      copy_array(dir_tree->MAP_prob(), MAP_wgt, nclass);
   } else {
      // update MAP-mix wgt
      copy_array(alpha, adapt_alpha, nclass);
      copy_array(alpha, MAP_wgt, nclass);
      normalize(MAP_wgt, nclass);
   }

   cout << "prior topic weight = " << endl;
   cout << "START" << endl;
   for (int k=0; k < nclass; k++) {
      cout << MAP_wgt[k] << endl;
   }
   cout << "END" << endl;
}

void LSA::init_param()
{
   fprintf(stderr, "Init model randomly.\n");

/*
*  initialize parameters
*
   */
   for (int i = 0; i < nclass; i++)
      alpha[i] = RANDOM;
   
   normalize(alpha, nclass);
   
   // alphas are greater than 1.0
   increment(alpha, nclass, 1.0);
   
   qsort(alpha, nclass, sizeof(double), // sort alpha initially
      (int (*)(const void *, const void *))doublecmp);
   
   for (int i = 0; i < nlex; i++)
      for (int j = 0; j < nclass; j++)
         beta[i][j] = (double) 1.0 / nlex;
         //beta[i][j] = RANDOM;

   normalize_matrix_col(beta, beta, nlex, nclass);
}

LSA::~LSA()
{
   free_dmatrix(q, dlenmax);
   free_dmatrix(beta, nlex);

   delete [] gamma;
   delete [] ap;
   delete [] nt;
   delete [] pnt;
   delete [] alpha;
   delete [] adapt_alpha;
   delete [] MAP_wgt;
   delete [] kl_dist;
   delete [] spg;

   if (data)
      free_feature_matrix(data);
   
   delete _dp;

   delete [] qi2voc;
}

double LSA::infer(const document *_dp)
{
   double ll = 0.0;
   
   if (dir_tree)
      this->vbem(_dp, dir_tree);
   else
      this->vbem(_dp, alpha);
   
   // compute MAP log likelihood
   ll = lda_MAP_LL(_dp, beta, gamma, nclass);
   
   return ll;
}

void LSA::adapt_sent(const document *sent, double forget)
{
   if (dir_tree) {
      this->vbem(sent, dir_tree, forget);
      dir_tree->adapt_alpha();

      // update MAP topic mixture weight
// fix on july 13 06
      copy_array(gamma, MAP_wgt, nclass);
      //copy_array(dir_tree->MAP_prob(), MAP_wgt, nclass);
      //normalize(MAP_wgt, nclass);

/*
      cout << "topic wgt ";
      for (int k=0; k < nclass; k++)
         cout << MAP_wgt[k] << " ";
      cout << endl;
*/
   } else {
      // perform VB
      this->vbem(sent, adapt_alpha, forget);
      
      // update MAP topic mixture weight
      copy_array(gamma, adapt_alpha, nclass);
      copy_array(gamma, MAP_wgt, nclass);
      normalize(MAP_wgt, nclass);
   }
}

void LSA::adapt_sent(const vector<string>& tk, const double* conf, int start_pos, int len, double forget)
{
   // compute multinomial
   this->multinomial(tk, conf, start_pos, len);
   
   // perform adaptation
   this->adapt_sent(_dp, forget);
}

void LSA::reset_prior()
{
   if (dir_tree) {
      dir_tree->reset();
      copy_array(dir_tree->MAP_prob(), MAP_wgt, nclass);
   } else {
      copy_array(alpha, adapt_alpha, nclass);
   
      // update MAP topic mixture weight
      copy_array(adapt_alpha, MAP_wgt, nclass);
      normalize(MAP_wgt, nclass);
   }
}

void LSA::load_vocab(const char* vocab_file)
{
   // read vocab mapper
   ifstream is(vocab_file);
   
   if (is.fail()) {
      cerr << "can't open LDA vocab mapper: " << vocab_file << endl;
      exit(-1);
   }
   
   string line;
   nlex = 0;
   
   while (is >> line) {
      if (lda_vocab.find(line) != lda_vocab.end()) {
         cerr << "ERROR: duplicated vocab found: " << line << endl;
         cerr << "old vocab id = " << lda_vocab[line] << endl;
         cerr << "new vocab id = " << nlex << endl;
      }

      lda_vocab[line] = nlex;
      
      // reverse mapping
      lda_idx2vocab.push_back(line);
      
      nlex++;
   }

   is.close();
}

const document* LSA::multinomial(const vector<string>& tk, const double* conf, int start_pos, int len)
{
   map<int, double> count;
   //   vector<int>      wid;
   map<int, double>::const_iterator iter;
   
   if (len == -1) {
      len = tk.size();
      start_pos = 0;
   }
   
   // count the word occurrence weighed by confidence score if provided
   for (int n=start_pos; n < (start_pos + len); n++) {
      const string& v = tk[n];
      
      // first find the lda vocab idx
      const int lda_v = this->get_vocab_id(v);
      
      //      wid.push_back(lda_v);
      
      if (conf)
         count[lda_v] += conf[n];
      else
         count[lda_v]++;
   }
   
   if ((int)count.size() > dlenmax) {
      cerr << "input buffer exceeds limit! try resize the limit = " << dlenmax << endl;
      exit(-1);
   }
   
   int j=0;
   _dp->len = count.size();
   for (iter=count.begin(); iter!=count.end(); iter++, j++) {
      _dp->id[j] = iter->first;
      _dp->cnt[j] = iter->second;

      // create lda voc to qj map
      qi2voc[iter->first] = j;
   }
   
   return _dp;
}

int LSA::get_vocab_id(const string& w) const
{
// START Wilson May 25 2007
   // perform word->class map
   string word1 = this->get_wordmap(w);
// END Wilson May 25 2007

   // get lda vocab idx
   int lda_v = -1;
   
   map<string,int>::const_iterator iter = lda_vocab.find(word1);
   
   if (iter == lda_vocab.end()) {
      //lda_v = lda_vocab.find(UNK)->second;
      iter = lda_vocab.find(UNK);

      if (iter == lda_vocab.end())
         lda_v = -1;
      else
         lda_v = iter->second;
   } else {
      lda_v = iter->second;
   }

   //cout << w << " -> " << lda_v << endl;
   
   return lda_v;
}

void LSA::accu_doc()
{
   int i=0;
   document* dp;

   for (dp = data, i = 0; (dp->len) != -1; dp++, i++) {
      if ((dp->len) == -1) break;

      this->accum(dp, i);
   }
}

int LSA::read_doc(const char* data_file)
{
   int max_vid = -1;
   int max_dlen = -1;

   if (data)
      free_feature_matrix(data);
   
   /* open data */
   if ((data = feature_matrix(data_file, &max_vid, &max_dlen, &num_doc, &num_word)) == NULL) {
      fprintf(stderr, "LSA:: cannot open training data.\n");
      exit(1);
   }

   cout << "finished reading " << data_file << endl;

   if (max_dlen > dlenmax) {
      cout << "realloc buffer " << endl;

      // realloc buffer
      free_dmatrix(q, dlenmax);

      dlenmax = max_dlen;
      
      if ((q = dmatrix(dlenmax, nclass)) == NULL) {
         fprintf(stderr, "lda_learn:: cannot allocate q.\n");
         exit(-1);
      }
      
      // create temp document
      delete _dp;
      _dp = new document(dlenmax);
   }

   // assuming we take all documents
   from_doc = 0;
   size_doc = -1;

   cout << "# doc = " << num_doc << endl;
   cout << "max. vocab = " << max_dlen << endl;

   return num_doc;
}

double LSA::learn(const char* data_file, int emmax, double epsilon, int from, int len, double learn)
{
   this->read_doc(data_file);
   
   this->create_accum();
   
   // init model param using training data
   if (!model_name)
      init_param();

   // set range of training doc
   from_doc = from;
   size_doc = len;

   //cout << "start lda training" << endl;
   double ll = this->lda_learn(data, num_doc, emmax, epsilon, learn);
   //cout << "end lda training" << endl;
   
   free_feature_matrix(data);
   data = 0;
   
   return ll;
}

double LSA::learn_bootstrap(const char* data_file, int emmax, double epsilon, int from, int len, double learn, const char* prior_file)
{
   this->read_doc(data_file);
   
   this->create_accum();
   
   // init model param using training data
   if (!model_name)
      init_param();

   // set range of training doc
   from_doc = from;
   size_doc = len;

   double ll = this->lda_bootstrap(data, num_doc, emmax, epsilon, learn, prior_file);
   
   free_feature_matrix(data);
   data = 0;
   
   return ll;
}

void LSA::create_accum()
{
   if (!save_memory && gammas)
      free_dmatrix(gammas, num_doc);
   
   if (betas)
      free_dmatrix(betas, nlex);

      /*
      *  initialize accumulators
      *
   */
   if (!save_memory && num_doc > 0 && (gammas = dmatrix(num_doc, nclass)) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate gammas.\n");
      return;
   }
   if ((betas = dmatrix(nlex, nclass)) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate betas.\n");
      return;
   }

   if (dir_tree)
      dir_tree->create_accu();

   this->clear_accum();
}

void LSA::clear_accum()
{
   /* reset accumulators */
   set_dmatrix(betas, nlex, nclass, 0.0);

   if (!save_memory && gammas)
      set_dmatrix(gammas, num_doc, nclass, 0.0);

   psg = 0.0;

   for (int i = 0; i < nclass; i++)
      spg[i] = 0.0;

   // total # doc
   M = 0;

   // kl sum
   kl_acc = 0;

   if (dir_tree) {
      dir_tree->clear_gamma();
      dir_tree->clear_psi();
   }
}

void LSA::accum(const document* dp, int i)
{
   if (prior_given) {
      if (dir_tree)
         this->fast_vbem(dp, dir_tree);
      else {
         cerr << "bootstrap not yet supported with standard LDA implementation. Try use dirichlettree implementation" << endl;
         exit(-1);
      }
   } else {
      if (dir_tree)
         this->vbem(dp, dir_tree);
      else
         this->vbem(dp, alpha);
   }

   this->accum_gammas(i, 1.0);
   this->accum_betas(dp, i, 1.0);

   // incr # doc
   M++;

   // incr KL divergence
   for (int j=0; j < dp->len; j++)
      kl_acc += dp->cnt[j] * kl_dist[dp->id[j]];
}

void LSA::update_model(double learn)
{
/*
*  VB-M step
*
   */
   /* Newton-Raphson for alpha */
   //newton_alpha(alpha, gammas, num_doc, nclass, 0);
//mar 30
   //write_accum("debug");

   if (!prior_given) {
      if (dir_tree)
         dir_tree->estimate_alpha(MAX_GRAD_ITERATION, learn);
      else
         this->grad_ascent(MAX_GRAD_ITERATION, learn);
   }
   
   /* smoothing with 1/nclass */
   add_dmatrix(betas, betas, nlex, nclass, 1.0 / nclass);

   normalize_matrix_col(beta, betas, nlex, nclass);

   // re-compute kl_weight
   this->compute_kl();
}

double LSA::lda_learn(document *data, int n, int emmax, double epsilon, double learn)
{
   double lik=0.0, plik = 0.0;
   double sum_kl=0.0, psum_kl = 0.0;
   document *dp;
   int i, t;
   int start, elapsed;
   int end = -1;
   const int check_iter = 1;
   
   printf("Number of documents      = %d\n", n);
   printf("Number of vocab          = %d\n", nlex);
   printf("Number of word counts    = %f\n", num_word);
   printf("Number of latent classes = %d\n", nclass);
   
   /*
   *  learn main
   *
   */
   start = myclock();
   for (t = 0; t < emmax; t++) {
      /* reset accumulators */
      this->clear_accum();
      //set_dmatrix(betas, nlex, nclass, 1.0/nclass);

      printf("iteration %d/%d..\t", t + 1, emmax);
      //fflush(stdout);
      /*
      *  VB-E step
      *
      */
      /* iterate for data. i: document index */
      end = (size_doc == -1)? n : (from_doc+size_doc);

      dp = data + from_doc;
      for (i=from_doc; i < end; i++, dp++) {
         if ((dp->len) == -1) break;

         this->accum(dp, i);
         //printf("doc %d\r", i);
         //fflush(stdout);
      }

      /*
      *  VB-M step
      *
      */
      // get current learning rate
      //const double cur_learn = learn * (1.0 - (double)t / (double)emmax);
      this->update_model(learn);

      // dump model every 10 iterations
      if (((t+1)%10) == 0) {
         char tmpfile[100];

         sprintf(tmpfile, "tmpmodel.i%d", t+1);

         //this->write_model(tmpfile);
      }

      // check convergence every 5 iterations (since it can be quite expensive when # of docs is huge!)
      if ((t%check_iter) != 0) {
         printf("\r");
         continue;
      }

      // compute sum of KL distances of all word tokens
      lik = 0.0;
      sum_kl = 0.0;
      dp = data + from_doc;

      for (i=from_doc; i < end; i++, dp++) {
         for (int j=0; j < dp->len; j++)
            sum_kl += dp->cnt[j] * kl_dist[dp->id[j]];

         if (!save_memory)
            lik += lda_MAP_LL (dp, beta, gammas[i], nclass);
      }

      if ((t > 1) && plik > lik) {
         cerr << "likelihood degrades! check your learning rate!" << endl;
      }
      
      /*
      *  converge?
      *
      */

      elapsed = myclock() - start;
      printf("KL = %g\t likelihood = %g\t", sum_kl, lik); fflush(stdout);

      // check convergence using KL divergence
      if ((t > 1) && psum_kl > sum_kl) {
         printf("\nKL divergence gets worse, possibly overfitting. stop here!\n");
         //break;
      }
      
/*
      // check convergence using likelihood
      if (!save_memory && (t > 1) && (fabs((plik - lik)/plik) < epsilon)) {
         if (t < 5) {
            printf("\nearly convergence. restarting..\n");
         
            init_param();
            return this->lda_learn (data, n, emmax, epsilon, learn);
         } else {
            printf("\nconverged. [%s]\n", rtime(elapsed));
            break;
         }
      }
*/

      plik = lik;
      psum_kl = sum_kl;
      /* 
      * ETA
      *
      */
      printf("ETA:%s (%d sec/step)\n",
		       rtime(elapsed * ((double) emmax / (t + 1) - 1)),
             (int)((double) elapsed / (t + 1) + 0.5));
   }
   
   return lik;
}

void LSA::write_model(const char* out_model_file)
{
   FILE *f_ap, *f_bp;
   
   /* open model outputs */
#ifdef BINARY
   if (((f_ap = fopen(strconcat(out_model_file, ".alpha"), "wb")) == NULL)
      || ((f_bp = fopen(strconcat(out_model_file, ".beta"), "wb"))  == NULL))
#else
      if (((f_ap = fopen(strconcat(out_model_file, ".alpha"), "w")) == NULL)
         || ((f_bp = fopen(strconcat(out_model_file, ".beta"), "w"))  == NULL))
#endif
   {
      fprintf(stderr, "LSA:: cannot open model outputs.\n");
      exit(1);
   }
   
   if (dir_tree) {
      dir_tree->write_alpha(f_ap);
      write_matrix(f_bp, beta, nlex, nclass);
   } else
      lda_write (f_ap, f_bp, alpha, beta, nclass, nlex);
   
   fclose(f_ap);
   fclose(f_bp);
}

// dump accumulators
void LSA::write_accum(const char* out_accum_file)
{
   FILE *f_betas, *f_gammas;
   
#ifdef BINARY
   if (((f_betas = fopen(strconcat(out_accum_file, ".betas"), "wb")) == NULL)
      || ((f_gammas= fopen(strconcat(out_accum_file, ".gammas"), "wb"))  == NULL))
#else
      if (((f_betas = fopen(strconcat(out_accum_file, ".betas"), "w")) == NULL)
         || ((f_gammas= fopen(strconcat(out_accum_file, ".gammas"), "w"))  == NULL))
#endif
   {
      fprintf(stderr, "LSA:: cannot open model outputs.\n");
      exit(1);
   }
   
   // write betas
   write_matrix (f_betas, betas,   nlex, nclass);

   // write accumulated gammas
   write_vector (f_gammas, &kl_acc, 1);

   if (dir_tree) {
      dir_tree->write_accum(f_gammas);
   } else {
      write_vector (f_gammas, &M, 1);
      write_vector (f_gammas, &psg, 1);
      write_vector (f_gammas, spg, nclass);
   }

/*
   cout << "writing M = " << M << endl;
   cout << "writing: psg = " << psg << " ";
   for (int k=0; k < nclass; k++)
      cout << "spg " << k << " = " << spg[k] << " ";
   cout << endl;
*/

/*
   if (!save_memory) {
      if (size_doc == -1)
         write_matrix (f_gammas, gammas, num_doc, nclass);
      else
         write_matrix (f_gammas, gammas, size_doc, nclass);
   }
*/

   fclose(f_betas);
   fclose(f_gammas);
}

double LSA::MAP_prob(const string& word) const
{
   // first find the lda vocab idx
   const int lda_v = this->get_vocab_id(word);

//   cout << word << "->" << lda_v << endl;
   
   return this->MAP_prob(lda_v);
}

double LSA::MAP_prob(int v) const
{
   double z = 0.0;
   const double* beta_v = beta[v];
   
   for (int k = 0; k < nclass; k++)
      z += beta_v[k] * MAP_wgt[k];
   
   return z;
}

// output arpa: vocab list derived from ngram wordlist
void LSA::write_arpa(const char* lm_file)
{
   ofstream os(lm_file);
   
   if (os.fail()) {
      cerr << "can't write to " << lm_file << endl;
      exit(-1);
   }
   
   // traverse the vocab set
   os << "\\data\\" << endl;
   os << "ngram 1=" << nlex << endl;
   os << endl;
   
   os << "\\1-grams:" << endl;
   for (int i=0; i < nlex; i++) {
      const string& word = lda_idx2vocab[i];
      double lda_l = this->MAP_prob(word);
      
      // print LM item
      os << log(lda_l) / LOG10 << " " << word << endl;
   }
   os << "\\end\\" << endl;
   
   os.close();
}

PROB_TABLE LSA::get_nbest_word(int k) const
{
   PROB_TABLE a(nlex);
   
   // copy pr(v|k)
   for (int v=0; v < nlex; v++) {
      a[v].first = v;
      a[v].second = beta[v][k];
   }
   
   // sort the table by Pr(v|k)
   sort(a.begin(), a.end(), Sort_Table_GT());
   
   return a;
}

void LSA::posterior(const string& word, double* p)
{
   const int v = this->get_vocab_id(word);
   const double* beta_v = beta[v];
   double* prior = new double[nclass];

   // Variational prior exp(E_q[log \theta_k])
   const double psi_sum_gamma = psi(accumulate(adapt_alpha, adapt_alpha+nclass, 0.0));

   for (int k = 0; k < nclass; k++)
      prior[k] = exp(psi(adapt_alpha[k]) - psi_sum_gamma);

   for (int k=0; k < nclass; k++)
      p[k] = prior[k] * beta_v[k];
   
   normalize(p, nclass);

   delete [] prior;
}

// compute KL(unif, p(z|v)) for all word v
void LSA::compute_kl()
{
   const double C = log((double)nclass);
   double* p = new double[nclass];
   double* prior = new double[nclass];
   
   // MAP prior
   copy_array(alpha, prior, nclass);
   normalize(prior, nclass);

   for (int v=0; v < nlex; v++) {
      const double* beta_v = beta[v];
      double d = 0.0;
      
      for (int k=0; k < nclass; k++)
         p[k] = prior[k] * beta_v[k];

      normalize(p, nclass);
      
      for (int k=0; k < nclass; k++) {
         if (p[k] > 0.0)
            d += p[k] * log(p[k]);
         //d += log(p[k]);
      }
      
      //kl_dist[v] = -d / nclass - C;
      kl_dist[v] = d + C;
      
      //cout << this->get_vocab(v) << " " << kl_dist[v] << endl;
   }

   delete [] p;
   delete [] prior;
}

PROB_TABLE LSA::sort_kl() const
{  
   return sort_table(kl_dist, nlex);
}

/*
d:       multinomial document (d->id[.] is the vocabulary index and 
d->cnt[.] is # of occurrences in d)
*/
void LSA::vbem(const document *d, const double* myalpha, double forget)
{
   int j, k, l;
   const int L = d->len;
   const int K = nclass;
   double total_count = 0;

   if (!myalpha) myalpha = this->alpha;
   
   // try to re-weight counts using kl distance   
   for (l = 0; l < L; l++) {
      total_count += d->cnt[l];
   }

   //set_array(nt, K, (double)L / (double)K);
   set_array(nt, K, total_count / (double)K);
   set_array(pnt, K, 0.0);

// yct: bug fix
   // add prior alpha to nt
   for (k = 0; k < K; k++) {
      nt[k] += myalpha[k];
      //printf("gamma[%d] = %f\n", k, nt[k]);
   }
   
   for (j = 0; j < demmax; j++) {
      /* vb-estep */
      const double psi_sum_gamma = psi(accumulate(nt, nt+K, 0.0));
      
      // heuristics here: to avoid zero prior prob.
      for (k = 0; k < K; k++) {
// yct 7/28 06
         //ap[k] = exp(psi(nt[k]) - psi_sum_gamma);
         ap[k] = exp(psi(nt[k]) - psi_sum_gamma) + SMALL_PROB;

         //printf("I=%d, ap[%d] = %f\n", j, k, ap[k]);
      }

      for (k = 0; k < K; k++)
         nt[k] = forget * myalpha[k];
      
      // accumulate q (recompute vote for each word)
      for (l = 0; l < L; l++) {
         const int v = d->id[l];

         const double cnt = d->cnt[l];
         const double* beta_v = beta[v];
         double* q_l = q[l];

         if (topic_ratio)
            for (k = 0; k < K; k++)
               q_l[k] = beta_v[k] * ap[k] * topic_ratio[k];
         else
            for (k = 0; k < K; k++)
               q_l[k] = beta_v[k] * ap[k];

         normalize(q_l, K);

         for (k = 0; k < K; k++)
               nt[k] += cnt * q_l[k];
      }

/*
      for (k = 0; k < K; k++)
         printf("gamma[%d] = %f\n", k, nt[k]);
*/
         
      /* converge? */
      if ((j > 0) && converged(nt, pnt, K, CONVERGENCE)) {
         //cerr << "VBEM converged at " << j << " iterations" << endl;
         break;
      }
      
      copy_array(nt, pnt, K);
   }

/*
   if (j == demmax) 
      cerr << "VBEM not converged after " << j << " iterations" << endl;
*/
   
   copy_array(nt, gamma, K);
}

void LSA::vbem(const document *d, Prior* dir_tree, double forget)
{
   int j, k, l;
   const int L = (d->num_phrase == 0)? d->len : d->phrase_start;
   const int K = nclass;
   double total_count = 0;

   static double* q_phrase = new double[K]; // Wilson Oct 24 2008

   //if (!myalpha) myalpha = this->alpha;
   
   for (l = 0; l < d->len; l++)
      total_count += d->cnt[l];

   set_array(nt, K, total_count / (double)K);
   set_array(pnt, K, 0.0);

   dir_tree->clear_gamma();
   dir_tree->accu_gamma(nt);
   //dir_tree->accu_gamma(nt, K);

   // add alpha to gamma
   dir_tree->update_gamma(forget);
   
   for (j = 0; j < demmax; j++) {
      /* vb-estep */
/*
      const double psi_sum_gamma = psi(accumulate(nt, nt+K, 0.0));
      
      // heuristics here: to avoid zero prior prob.
      for (k = 0; k < K; k++)
         ap[k] = exp(psi(nt[k]) - psi_sum_gamma);
         //ap[k] = exp(psi(nt[k]) - psi_sum_gamma) + SMALL_PROB;
*/
      // heuristics here: to avoid zero prior prob.
// yct 8/1
// remove normalization
      dir_tree->get_prior(ap, K);
      //dir_tree->get_prior(ap, K, false);

      for (k = 0; k < K; k++) {
         ap[k] += SMALL_PROB;
         //printf("I=%d, ap[%d] = %f\n", j, k, ap[k]);
      }

      dir_tree->clear_gamma();

      for (k = 0; k < K; k++)
         nt[k] = 0.0;
      
      // accumulate q (recompute vote for each word)
      for (l = 0; l < L; l++) {
         const int v = d->id[l];
         const double cnt = d->cnt[l];
         const double* beta_v = beta[v];
         double* q_l = q[l];

         if (topic_ratio)
            for (k = 0; k < K; k++)
               q_l[k] = beta_v[k] * ap[k] * topic_ratio[k];
         else
            for (k = 0; k < K; k++)
               q_l[k] = beta_v[k] * ap[k];

         normalize(q_l, K);

         for (k = 0; k < K; k++)
            nt[k] += cnt * q_l[k];
      }

// Start wilson Oct 24 2008
// perform EM on phrases if defined in the input document
      l = d->phrase_start;
      for (int i=0; i < d->num_phrase; i++) {
         set_array(q_phrase, K, 0.0);

         // get phrase cnt
         double phrase_cnt = d->cnt[l];

         // get acc log likelihood of that phrase for each topic k
         for (int jj=0; jj < d->phrase_size[i]; jj++, l++) {
            const int v = d->id[l];
            const double* beta_v = beta[v];

            for (k=0; k < K; k++) {
               q_phrase[k] += log(beta_v[k]);
               //printf("iter=%d pos=%d v=%d, p=%f, k=%d\n", j, l, v, beta_v[k], k);
            }
         }

         // find the max of q_phrase, then subtract it to all topic k
         const double ll_max = *max_element(q_phrase, q_phrase + K);

         if (topic_ratio)
            for (k = 0; k < K; k++)
               q_phrase[k] = exp(q_phrase[k]-ll_max) * ap[k] * topic_ratio[k];
         else
            for (k = 0; k < K; k++) {
               q_phrase[k] = exp(q_phrase[k]-ll_max) * ap[k];
               //printf("iter=%d q_phrase=%f phrase=%d k=%d\n", j, q_phrase[k], i, k);
            }

         normalize(q_phrase, K);

         for (k = 0; k < K; k++) {
            nt[k] += phrase_cnt * q_phrase[k];
            //printf("iter=%d phrase count =%f phrase=%d k=%d\n", j, phrase_cnt, i, k);
         }
      }
// End wilson Oct 24 2008

      //dir_tree->accu_gamma(nt, K);
      dir_tree->accu_gamma(nt);

      // add alpha to gamma
      dir_tree->update_gamma(forget);
         
      /* converge? */
      if ((j > 0) && converged(nt, pnt, K, CONVERGENCE)) {
         //cerr << "VBEM converged at " << j << " iterations" << endl;
         break;
      }
      
      copy_array(nt, pnt, K);
   }

/*
   if (j == demmax) 
      cerr << "VBEM not converged after " << j << " iterations" << endl;
*/
   // update branch prob
   dir_tree->update_branch_prob();
   
   copy_array(dir_tree->MAP_prob(), gamma, K);

/*
   cout << "VBEM ";
   for (int k=0; k < K; k++) {
      cout << gamma[k] << " ";
   }
   cout << endl;
*/
}

void LSA::accum_gammas (int n, double wgt)
{
   /* gammas(n,:) = gamma for Newton-Raphson of alpha */

   if (!save_memory) {
      double* gammas_n = gammas[n];
      copy_array(gamma, gammas_n, nclass);
   }

   if (dir_tree) {
      dir_tree->accu_psi(wgt);
   } else {
      // accumulate sufficient statistics for alpha update
      psg += psi(accumulate(gamma, gamma+nclass, 0.0));
      
      for (int i = 0; i < nclass; i++)
         spg[i] += psi(gamma[i]);
   }
}

void LSA::accum_betas (const document *dp, int doc_id, double wgt)
{
   for (int i = 0; i < dp->len; i++) {
      const int v = dp->id[i];
      double* betas_v = betas[v];
      const double* q_v = q[i];
   
      const double cnt = dp->cnt[i];
      
      for (int k = 0; k < nclass; k++)
         betas_v[k] += q_v[k] * cnt * wgt;
   }
}

void LSA::add_accum(const char* file)
{
   /* open model file if any */
   FILE *f_betas, *f_gammas;
   double** tmp_betas;

   if ((tmp_betas = dmatrix(nlex, nclass)) == NULL) {
      fprintf(stderr, "lda_learn:: cannot allocate tmp_betas.\n");
      return;
   }
   
#ifdef BINARY
   if (((f_betas = fopen(strconcat(file, ".betas"), "rb")) == NULL)
      || ((f_gammas = fopen(strconcat(file, ".gammas"), "rb"))  == NULL))
#else
      if (((f_betas = fopen(strconcat(file, ".betas"), "r")) == NULL)
         || ((f_gammas = fopen(strconcat(file, ".gammas"), "r"))  == NULL))
#endif
   {
      fprintf(stderr, "LSA:: cannot read accu.\n");
       return;
   } else {
      read_matrix(f_betas, tmp_betas, nlex, nclass);

      // add beta accumulators
      for (int i = 0; i < nlex; i++) {
         const double* tmp_betas_i = tmp_betas[i];
         double* betas_i = betas[i];
   
         for (int j = 0; j < nclass; j++) {
            betas_i[j] += tmp_betas_i[j];
         }
      }

      double tmp_psg = 0.0;
      double* tmp_spg = new double[nclass];
      double tmp_num_doc = 0.0;
      double tmp_kl_acc = 0.0;

      // load gamma statistics
      read_vector (f_gammas, &tmp_kl_acc, 1);
      kl_acc += tmp_kl_acc;

      if (dir_tree) {
         dir_tree->add_accum(f_gammas);
      } else {
         read_vector (f_gammas, &tmp_num_doc, 1);
         read_vector (f_gammas, &tmp_psg, 1);
         read_vector (f_gammas, tmp_spg, nclass);

         M += tmp_num_doc;

         psg += tmp_psg;
         for (int k=0; k < nclass; k++)
            spg[k] += tmp_spg[k];
      }

/*
      cout << "reading M = " << tmp_num_doc << endl;
      cout << "reading: psg = " << psg << " ";
      for (int k=0; k < nclass; k++)
         cout << "spg " << k << " = " << spg[k] << " ";
      cout << endl;
*/

      //cout << "M = " << M << endl;
      //cout << "kl = " << kl_acc << endl;

/*
      if (!save_mem) {
         read_matrix(f_gammas, gammas, num_doc, nclass);
         cur_doc += num_doc;
      }
*/

      delete [] tmp_spg;
   }

   free_dmatrix(tmp_betas, nlex);

   fclose(f_betas);
   fclose(f_gammas);
}

void LSA::grad_ascent(int iter, double learn)
{
   double* lalpha = new double[nclass];

   // perform parameter transformation
   copy_array(alpha, lalpha, nclass);
   natural_log(lalpha, nclass);

   int i, t;
   double *g, *pg, *palpha;
   //double psg, spg, gs;
   double alpha0, palpha0;
   const int K = nclass;
   //const int M = num_doc;
   
   /* allocate arrays */
   if ((g = new double[K]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate g.\n");
      return;
   }
   if ((pg = new double[K]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate pg.\n");
      return;
   }
   if ((palpha = new double[K]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate palpha.\n");
      return;
   }

   // "i" is the document index, M is the total document
   // "gs" means sum_k gamma_k for each document i
/*
   psg = 0;
   for (i = 0; i < M; i++) {
      for (j = 0, gs = 0; j < K; j++)
         gs += gammas[i][j];
      psg += psi(gs);
   }
   for (i = 0; i < K; i++) {
      for (j = 0, spg = 0; j < M; j++)
         spg += psi(gammas[j][i]);
      pg[i] = spg - psg;
   }
*/

   for (i = 0; i < K; i++)
      pg[i] = spg[i] - psg;
   
   /* main iteration */
   for (t = 0; t < iter; t++) {
      for (i = 0, alpha0 = 0; i < K; i++)
         alpha0 += alpha[i];
      palpha0 = psi(alpha0);
      
      // compute gradient[k] over alpha_k
      for (i = 0; i < K; i++)
         g[i] = (M * (palpha0 - psi(alpha[i])) + pg[i]) * alpha[i];

      // determine learning rate
      const double cur_learn = (1.0 - double(t)/double(iter)) * learn / M;

      // gradient ascent
      for (i = 0; i < K; i++)
         lalpha[i] += cur_learn * g[i];

      // inverse transform alpha
      for (i = 0; i < K; i++) {
         //printf("%f->", alpha[i]);
         alpha[i] = exp(lalpha[i]);
         //printf("%f\n", alpha[i]);
      }
      
      if ((t > 0) && converged(alpha, palpha, K, 1.0e-4))
         break;
      else
         copy_array(alpha, palpha, K);
   }

   if (t == iter)
      printf("alpha is not converged after %d iterations\n", iter);

   delete [] g;
   delete [] pg;
   delete [] palpha;
   delete [] lalpha;
}

void LSA::add_wordclass(const string& class_name, const string& word)
{
   map<string, vector<int> >::iterator iter = class2word.find(class_name);

   const int lda_v = this->get_vocab_id(word);
   const int unk_id = this->get_vocab_id(UNK);

   // skip <UNK> entry
   if (lda_v == unk_id) return;

   if (iter == class2word.end()) {
      // create new class
      vector<int> vid;

      vid.push_back(lda_v);

      class2word[class_name] = vid;
   } else {
      // append element
      iter->second.push_back(lda_v);
   }
}

// load all classes described in a list
void LSA::load_wordclass(const char* filelist)
{
   ifstream is(filelist);

   if (is.fail()) {
      cerr << "add_wordclass: can't open " << filelist << endl;
      return;
   }

   string path, cname;

   while (is >> path >> cname) {
      this->add_wordclass(path.c_str(), cname);
   }

   is.close();
}

void LSA::add_wordclass(const char* filename, const string& class_name)
{
   ifstream is(filename);

   if (is.fail()) {
      cerr << "add_wordclass: can't open " << filename << endl;
      return;
   }

   string word;
   vector<int> vid;
   const int unk_id = this->get_vocab_id(UNK);

   while (is >> word) {
      const int lda_v = this->get_vocab_id(word);

      // skip <UNK> entry
      if (lda_v == unk_id) continue;

      vid.push_back(lda_v);
   }

   // insert mapping
   if (vid.size() > 1) {
      class2word[class_name] = vid;

      // update LSA prob for that class
      prob_table[class_name] = this->prob_word_class(class_name, false, false);
   }

   is.close();
}

vector<string> LSA::get_classlist() const
{
   vector<string> clist;

   map<string, vector<int> >:: const_iterator iter;

   for (iter=class2word.begin(); iter != class2word.end(); iter++) {
      clist.push_back(iter->first);
   }

   return clist;
}

// allows output diff prob from the previous (prior) cached value
PROB_TABLE LSA::prob_word_class(const string& class_name, bool is_sort, bool diff)
{
   map<string, vector<int> >:: const_iterator iter;
   PROB_TABLE prob;

   iter = class2word.find(class_name);

   if (iter == class2word.end()) {
      cerr << "can't find " << class_name << endl;
      return prob;
   }

   const vector<int>& vid = iter->second;
   double z = 0.0;
   double p = 0.0;

   for (int i=0; i < (int) vid.size(); i++) {
      p = this->MAP_prob(vid[i]);

      prob.push_back(pair<int,double>(vid[i],p));
      z += p;
   }

   // normalize (and diff if necessary)
   if (diff) {
      const PROB_TABLE& prv_tab = this->prob_table[class_name];

      for (int i=0; i < (int) vid.size(); i++) {
         prob[i].second /= z;
         prob[i].second -= prv_tab[i].second;
      }
   } else {
      for (int i=0; i < (int) vid.size(); i++)
         prob[i].second /= z;
   }

   // sort the table by Pr(v|c)
   if (is_sort)
      sort(prob.begin(), prob.end(), Sort_Table_GT());

   return prob;
}

void LSA::write_subs(const char* filename, const string& class_name)
{
   // format: log10p \t word \t class
   ofstream os(filename);

   if (os.fail()) {
      cerr << "can't write subs: " << filename << endl;
      return;
   }

   PROB_TABLE prob = this->prob_word_class(class_name);

   for (int n=0; n < (int)prob.size(); n++) {
      const pair<int,double>& item = prob[n];
      const string& w = this->get_vocab(item.first);
      const double  p = item.second;

      os << log(p) / LOG10 << " " << w.c_str() << " " << class_name << endl;
   }

   os.close();
}

int LSA::get_class_size(string c) const
{
   map<string, vector<int> >::const_iterator iter;

   iter = class2word.find(c);

   if (iter == class2word.end()) {
      cerr << "class not found: " << c << endl;
      return 0;
   }
   
   return iter->second.size();
}

// merge vocabulary
void LSA::merge_vocab(int v1, int v2)
{
   if (v1 > v2) {
      // swap idx
      int tmp = v2;
      v2 = v1;
      v1 = tmp;
   }

   double* b1 = beta[v1];
   double* b2 = beta[v2];

   for (int k=0; k < nclass; k++)
      b1[k] += b2[k];

   // remove v2
   this->remove_vocab(v2);
}

// remove vocabulary from the unigram prob
void LSA::write_vocab(const char* filename)
{
   ofstream os(filename);

   if (os.fail()) {
      cerr << "can't write to " << filename << endl;
      return;
   }

   for (int v=0; v < nlex; v++)
      os << lda_idx2vocab[v] << endl;

   os.close();
}

// remove vocabulary from the unigram prob
void LSA::remove_vocab(int vid)
{
   // shift upward
   for (int v=vid+1; v < nlex; v++)
      for (int k=0; k < nclass; k++)
         beta[v-1][k] = beta[v][k];

   nlex--;

   // renormalize (hack)
   normalize_matrix_col(beta, beta, nlex, nclass);

   // remove word->idx map
   lda_vocab.erase(lda_idx2vocab[vid]);
      
   // remove idx->word map
   lda_idx2vocab.erase(lda_idx2vocab.begin() + vid);
}

// helper function
PROB_TABLE sort_table(double* table, int size)
{
   PROB_TABLE a(size);
   
   // copy pr(v|k)
   for (int v=0; v < size; v++) {
      a[v].first = v;
      a[v].second = table[v];
   }
   
   // sort the table by Pr(v|k) (in descending order)
   sort(a.begin(), a.end(), Sort_Table_GT());
   
   return a;
}

// compute variational log likelihood
double LSA::var_ll(const document* _dp)
{
   double result = 0.0;

   if (dir_tree) {
      this->vbem(_dp, dir_tree);

      double term_z = 0.0;
      double term_beta = 0.0;

      // ap[k] would be equal to E_q[log \theta_k] without exp(.)
      dir_tree->get_prior(ap, nclass, false);

/*
      for (int k=0; k < nclass; k++)
         printf("LDTA: ap[%d] = %f\n", k, ap[k]);
*/
      
      for (int w = 0; w < _dp->len; w++) {
         const int v = _dp->id[w];
         const double v_count = _dp->cnt[w];
         const double* beta_w = beta[v];
         
         // var posterior of each word w, topic j=0
         const double* q_w = q[w];
         
         for (int k = 0; k < nclass; k++) {
            const double q_wk = q_w[k];
            
            if (q_wk > 0) {
               // E_q[log f(z|theta) / q(z)]
               term_z += v_count * q_wk * (ap[k] - log(q_wk));
            
               // term 5: E_q[log f(v|z)]
               term_beta += v_count * q_wk * log(beta_w[k]);
            }
         } // end of each class k 
      }
 
      double term_theta = dir_tree->var_ll();

/*
      printf("term beta = %f\n", term_beta);
      printf("term z = %f\n", term_z);
      printf("term theta = %f\n", term_theta);
*/

      result = term_beta + term_z + term_theta;
   } else {
      this->vbem(_dp, alpha);

      // compute the dirichlet term
      const double psi_sum_gamma = psi(accumulate(gamma, gamma+nclass, 0.0));
      
      for (int k = 0; k < nclass; k++)
         ap[k] = psi(gamma[k]) - psi_sum_gamma;

/*
      for (int k=0; k < nclass; k++)
         printf("LDA: ap[%d] = %f\n", k, ap[k]);
*/

      result = lda_VAR_LL(_dp, beta, alpha, gamma, ap, q, nclass);
   }

   return result;
}

// Start Wilson 12/11/2006
// E-step given that the prior value is fixed
void LSA::fast_vbem(const document *d, Prior* dir_tree, double forget)
{
   int k, l;
   const int L = d->len;
   const int K = nclass;

   /* vb-estep */
   dir_tree->clear_gamma();

   for (k = 0; k < K; k++)
      nt[k] = 0.0;
   
   // accumulate q (recompute vote for each word)
   for (l = 0; l < L; l++) {
      const int v = d->id[l];
      const double cnt = d->cnt[l];
      const double* beta_v = beta[v];
      double* q_l = q[l];

      if (topic_ratio)
         for (k = 0; k < K; k++)
            q_l[k] = beta_v[k] * ap[k] * topic_ratio[k];
      else
         for (k = 0; k < K; k++)
            q_l[k] = beta_v[k] * ap[k];

      normalize(q_l, K);

      for (k = 0; k < K; k++)
         nt[k] += cnt * q_l[k];
   }

   //dir_tree->accu_gamma(nt, K);
   dir_tree->accu_gamma(nt);

   // add alpha to gamma
   dir_tree->update_gamma(forget);
      
   // update branch prob
   dir_tree->update_branch_prob();
   
   copy_array(dir_tree->MAP_prob(), gamma, K);
}

double LSA::lda_bootstrap(document *data, int n, int emmax, double epsilon, double learn, const char* prior_file)
{
   double lik=0.0, plik = 0.0;
   double sum_kl=0.0, psum_kl = 0.0;
   document *dp;
   int i, t;
   int start, elapsed;
   int end = -1;
   const int check_iter = 1;
   
   printf("Number of documents      = %d\n", n);
   printf("Number of vocab          = %d\n", nlex);
   printf("Number of word counts    = %f\n", num_word);
   printf("Number of latent classes = %d\n", nclass);

   // set flag
   prior_given = true;
   
   /*
   *  learn main
   *
   */
   start = myclock();
   for (t = 0; t < emmax; t++) {
      // open the prior file
      // change to binary mode
#ifdef BINARY
      ifstream prior_is(prior_file, ios::in | ios::binary);
#else
      ifstream prior_is(prior_file);
#endif

      if (prior_is.fail()) {
         cerr << "can't open " << prior_file << endl;
         exit(-1);
      }

      /* reset accumulators */
      this->clear_accum();
      //set_dmatrix(betas, nlex, nclass, 1.0/nclass);

      printf("iteration %d/%d..\t", t + 1, emmax);
      //fflush(stdout);
      /*
      *  VB-E step
      *
      */
      /* iterate for data. i: document index */
      end = (size_doc == -1)? n : (from_doc+size_doc);

      dp = data + from_doc;
      for (i=from_doc; i < end; i++, dp++) {
         if ((dp->len) == -1) {
            cerr << "Warning: empty document!" << endl;
            break;
         }

#ifdef BINARY
// change to binary mode
         prior_is.read((char*) ap, sizeof(double)*nclass);
#else
         for (int k=0; k < nclass; k++)
            prior_is >> ap[k];
#endif

         this->accum(dp, i);
         //printf("doc %d\r", i);
         //fflush(stdout);
      }

      // close file stream
      prior_is.close();

      /*
      *  VB-M step
      *
      */
      // get current learning rate
      //const double cur_learn = learn * (1.0 - (double)t / (double)emmax);
      this->update_model(learn);

      // dump model every 10 iterations
      if (((t+1)%10) == 0) {
         char tmpfile[100];

         sprintf(tmpfile, "tmpmodel.i%d", t+1);

         //this->write_model(tmpfile);
      }

      // check convergence every 5 iterations (since it can be quite expensive when # of docs is huge!)
      if ((t%check_iter) != 0) {
         printf("\r");
         continue;
      }

      // compute sum of KL distances of all word tokens
      lik = 0.0;
      sum_kl = 0.0;
      dp = data + from_doc;
      for (i=from_doc; i < end; i++, dp++) {
         for (int j=0; j < dp->len; j++)
            sum_kl += dp->cnt[j] * kl_dist[dp->id[j]];

         if (!save_memory)
            lik += lda_MAP_LL (dp, beta, gammas[i], nclass);
      }

      if ((t > 1) && plik > lik) {
         cerr << "likelihood degrades! check your learning rate!" << endl;
      }
      
      /*
      *  converge?
      *
      */

      elapsed = myclock() - start;
      printf("KL = %g\t likelihood = %g\t", sum_kl, lik); fflush(stdout);

      // check convergence using KL divergence
      if ((t > 1) && psum_kl > sum_kl) {
         printf("\nKL divergence gets worse, possibly overfitting. stop here!\n");
         //break;
      }
      
/*
      // check convergence using likelihood
      if (!save_memory && (t > 1) && (fabs((plik - lik)/plik) < epsilon)) {
         if (t < 5) {
            printf("\nearly convergence. restarting..\n");
         
            init_param();
            return this->lda_learn (data, n, emmax, epsilon, learn);
         } else {
            printf("\nconverged. [%s]\n", rtime(elapsed));
            break;
         }
      }
*/

      plik = lik;
      psum_kl = sum_kl;
      /* 
      * ETA
      *
      */
      printf("ETA:%s (%d sec/step)\n",
		       rtime(elapsed * ((double) emmax / (t + 1) - 1)),
             (int)((double) elapsed / (t + 1) + 0.5));
   }
   
   return lik;
}
// End Wilson 12/11/2006

double LSA::kl_divergence(const double* p, const double* q, int size) const
{
   double dist = 0.0;

   for (int k=0; k < size; k++) {
      if (p[k] > 0.0) {
         dist += p[k] * log(p[k] / q[k]);
      }
   }

   return dist;
}

bool LSA::load_wordmap(const char* wordmap_file)
{
   ifstream is(wordmap_file);

   if (is.fail()) {
      cerr << "can't load wordmap " << wordmap_file << endl;
      exit(-1);
   }

   string word, word_class;
 
   while (is >> word >> word_class)
      wordmap[word] = word_class;

   is.close();

   cerr << "load_wordmap file finished: " << wordmap_file << endl;

   return true;
}

// START Wilson May 25 2007
string LSA::get_wordmap(const string& str) const
{
   // perform word->class map
   if (!wordmap.empty()) {
      map<string,string>::const_iterator iter = wordmap.find(str);

      // change word form
      if (iter != wordmap.end())
         return iter->second;
      else
         return str;
   }

   return str;
}
// END Wilson May 25 2007

void LSA::transform_beta(const char* transform_matrix)
{
   /* open model file if any */
   FILE *f_ap = 0;
   
#ifdef BINARY
   if ((f_ap = fopen(transform_matrix, "rb")) != NULL)
#else
   if ((f_ap = fopen(transform_matrix, "r")) != NULL)
#endif
   {
      double** trans = 0;
      double** new_beta = 0;
      if ((trans = dmatrix(nclass, nclass)) == NULL) {
         fprintf(stderr, "lda_learn:: cannot allocate trans.\n");
         exit(-1);
      }

      if ((new_beta = dmatrix(nlex, nclass)) == NULL) {
         fprintf(stderr, "lda_learn:: cannot allocate new_beta.\n");
         exit(-1);
      }
      
      // load matrix
      read_matrix(f_ap, trans, nclass, nclass);

      // this will get P(i|j)
      normalize_matrix_col(trans, trans, nclass, nclass);
      
      // transform beta
      for (int v=0; v < nlex; v++) {
         double* new_beta_v = new_beta[v];
         double* beta_v = beta[v];

         for (int j=0; j < nclass; j++) {
            double& new_beta_vj = new_beta_v[j] = 0.0;

            for (int i=0; i < nclass; i++) {
               new_beta_vj += beta_v[i] * trans[i][j];
            }
         }
      }

      // update beta
      copy_dmatrix(new_beta, beta, nlex, nclass);

      // cleanup
      fclose(f_ap);
      free_dmatrix(trans, nclass);
      free_dmatrix(new_beta, nlex);
   }
}

void LSA::init_flat()
{
   // uniform multinomial
   for (int v=0; v < nlex; v++)
   for (int k=0; k < nclass; k++)
      beta[v][k] = 1.0 / nlex;

   // random init prior
   if (dir_tree) {
      dir_tree->init_flat();
   } else {
      for (int k=0; k < nclass; k++)
         alpha[k] = 1.0 + RANDOM;
   }
}

const document* LSA::get_doc(int idx)
{
   return data + idx;
}

void LSA::add_vocab(const char* new_vocab_file)
{
   // read vocab mapper
   ifstream is(new_vocab_file);
   
   if (is.fail()) {
      cerr << "can't open LDA new vocab mapper: " << new_vocab_file << endl;
      exit(-1);
   }
   
   string line;
   int new_nlex = nlex;
   double** new_beta = NULL;
   
   // add new vocab
   while (is >> line) {
      if (lda_vocab.find(line) != lda_vocab.end()) {
         cerr << "ERROR: duplicated vocab found: " << line << endl;
         cerr << "old vocab id = " << lda_vocab[line] << endl;
         cerr << "new vocab id = " << new_nlex << endl;
      }

      lda_vocab[line] = new_nlex;
      cout << line << " --> " << new_nlex << endl;
      
      // reverse mapping
      lda_idx2vocab.push_back(line);
      
      new_nlex++;
   }
   // close fstream
   is.close();

   cout << "# vocab added = " << new_nlex - nlex << endl;

   // allocate new beta
   if ((new_beta = dmatrix(new_nlex, max_class)) == NULL) {
      fprintf(stderr, "LSA:: cannot allocate new beta.\n");
      exit(1);
   }

   // copy old beta into new beta
   for (int v=0; v < nlex; v++) {
      for (int k=0; k < max_class; k++)
         new_beta[v][k] = beta[v][k];
   }

   const int unk_id = this->get_vocab_id(UNK);

   // assign prob for new words based on p(<UNK>)
   for (int k=0; k < max_class; k++) {
      // share prob with new old
      const double old_unk_prob = beta[unk_id][k];
      const double new_unk_prob = old_unk_prob / (1.0 + new_nlex - nlex);

      new_beta[unk_id][k] = new_unk_prob;

      for (int v=nlex; v < new_nlex; v++)
         new_beta[v][k] = new_unk_prob;
   }

   // reallocate memory
   if (kl_dist)
      kl_dist = (double*) realloc(kl_dist, sizeof(double) * new_nlex);

   // reallocate memory
   if (qi2voc)
      qi2voc= (int*) realloc(qi2voc, sizeof(int) * new_nlex);

   // free beta
   free_dmatrix(beta, nlex);

   // assign new_beta to beta
   beta = new_beta;

   // renormalize (just in case)
   normalize_matrix_col(beta, beta, new_nlex, nclass);

   // realloc betas
   if (betas) {
      free_dmatrix(betas, nlex);

      if ((betas = dmatrix(new_nlex, nclass)) == NULL) {
         fprintf(stderr, "lda_learn:: cannot allocate betas.\n");
         return;
      }
   }

   // change nlex
   nlex = new_nlex;
}

void LSA::write_log_ratio_arpa(const char* lm_file, double* bg_prob, const char* stopword_file, const char* vocab_file, double beta)
{
   ofstream os(lm_file);
   
   if (os.fail()) {
      cerr << "can't write to " << lm_file << endl;
      exit(-1);
   }

// START Jan 18 08
// skip stopwords if defined: write their log ratios to 0.0
   // load stopword if defined
   map<string, bool> stoplist;
   if (stopword_file) {
      ifstream is(stopword_file);

      if (is.fail()) {
         cerr << "can't open stopword file" << endl;
         exit(-1);
      }

      string word;
      while (is >> word) {
         stoplist[word] = true;
      }

      is.close();
   }

   // load vocab list if defined
   map<string, bool> vocablist;
   if (vocab_file) {
      ifstream is(vocab_file);

      if (is.fail()) {
         cerr << "can't open vocab file" << endl;
         exit(-1);
      }

      string word;
      while (is >> word) {
         vocablist[word] = true;
      }

      is.close();
   }
// END Jan 18 08

   if (vocablist.empty()) {
      // traverse the vocab set
      os << "\\data\\" << endl;
      os << "ngram 1=" << nlex << endl;
      os << endl;
      
      os << "\\1-grams:" << endl;
      for (int i=0; i < nlex; i++) {
         const string& word = lda_idx2vocab[i];
         // compute the ratio
         double lda_l = this->MAP_prob(word) / bg_prob[i];
   
        if (!stoplist.empty() && stoplist.find(word) != stoplist.end()) {
           os << 0.0 << " " << word << endl;
        } else {
           // print LM item
           os << beta * log(lda_l) / LOG10 << " " << word << endl;
        }
      }
      os << "\\end\\" << endl;
   } else {
      // traverse the vocab set
      os << "\\data\\" << endl;
      os << "ngram 1=" << vocablist.size() << endl;
      os << endl;

      map<string, bool>::const_iterator iter;

      os << "\\1-grams:" << endl;
      for (iter=vocablist.begin(); iter!=vocablist.end(); iter++) {
         const string& word = iter->first;

         if (this->is_unk(word) || (!stoplist.empty() && stoplist.find(word) != stoplist.end())) {
            os << 0.0 << " " << word << endl;
         } else {
            const int i = this->get_vocab_id(word);

            // compute the ratio
            const double lda_l = this->MAP_prob(word) / bg_prob[i];

            // print LM item
            os << beta * log(lda_l) / LOG10 << " " << word << endl;
         }
      }
      os << "\\end\\" << endl;
   }
   
   os.close();
}

void LSA::set_gammas(const double* cur_gamma, int d)
{
   copy_array(cur_gamma, gammas[d], nclass);
}

// shrink the node which has the least entropy gain by manipulating on the accumulator
// this procedure can be called before right after accumulation and before next training iteration
double LSA::entropy(double* p, int size)
{
   double value = 0.0;

   for (int i=0; i < size; i++) {
      if (p[i] == 0) continue;

      value += p[i] * log(p[i]);
   }

   return -value;
}

double LSA::check_info_loss(int k1, int k2)
{
   double* combine_p = new double[nlex];
   double* p1 = new double[nlex];
   double* p2 = new double[nlex];

   for (int v=0; v < nlex; v++) {
      const double* betas_v = betas[v];

      // obtain counts for v|k
      p1[v] = betas_v[k1];
      p2[v] = betas_v[k2];

      // merge counts
      combine_p[v] = p1[v] + p2[v];
   }

   // get total counts from p1 and p2
   const double cnt1 = accumulate(p1, p1+nlex, 0.0);
   const double cnt2 = accumulate(p2, p2+nlex, 0.0);

   // normalize to get prob dist
   normalize(p1, nlex);
   normalize(p2, nlex);
   normalize(combine_p, nlex);
   
   double h_combine = entropy(combine_p, nlex);
   double h1 = entropy(p1, nlex);
   double h2 = entropy(p2, nlex);

   delete [] combine_p;
   delete [] p1;
   delete [] p2;

   // compute weighted info loss after merging node
   return (cnt1+cnt2) * h_combine - cnt1 * h1 - cnt2 * h2;
}

// merge one topic
int LSA::merge_topic()
{
   DirichletTree* tree = (DirichletTree*) dir_tree;

   // sanity check
   assert(tree->get_num_topic() == nclass);

   // make a copy of the path structure
   DirichletTree::path* topic_path = (tree->get_topic_path());
   vector<int>* cluster = new vector<int>[nclass*2];

   // build the map for node merging at the bottom level
   for (int k=0; k < nclass; k++) {
      int parent_node = topic_path[k][0].node;
      cluster[parent_node].push_back(k);
   }

   // for each possible merge according to merge_node, check info_loss
   double min_info_loss = 99999.0;
   int best_c = -1;
   for (int c=0; c < nclass*2; c++) {
      const vector<int>& cluster_c = cluster[c];
      // skip empty cluster
      if (cluster_c.size() <= 1) continue;

      if (cluster_c.size() != 2) {
         cerr << "can't proceed merge_topic() since only binary dirtree is supported!" << endl;
         break;
      }

      int k1 = cluster_c[0];
      int k2 = cluster_c[1];

      assert(k1 < k2);

      const double cur_info_loss = this->check_info_loss(k1, k2);

      if (cur_info_loss < min_info_loss) {
         min_info_loss = cur_info_loss;
         best_c = c;
      }
   }

   // merge accumulators k1 <- k1+k2
   int k1 = cluster[best_c][0];
   int k2 = cluster[best_c][1];

   // pick the best cluster with least info loss
   cout << "best cluster = " << best_c << " merge topics " << k1 << " " << k2 << " info loss = " << min_info_loss << endl;

   // shift accumulators to the left
   for (int v=0; v < nlex; v++) {
      double* betas_v = betas[v];
      for (int k=k2+1; k < nclass; k++) {
         betas_v[k-1] = betas_v[k];
      }
   }

   // prune dirichlet-tree => update path, remove dirichlet nodes, update leaf2node idx
   tree->merge_node(k1, k2);

   // reduce number of class by 1
   nclass--;

   delete [] cluster;

   return nclass;
}
