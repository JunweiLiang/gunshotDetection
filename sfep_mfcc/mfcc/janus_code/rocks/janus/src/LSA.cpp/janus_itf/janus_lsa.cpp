#include <fstream>
#include <vector>
#include <string>
#include <numeric>

// from janus
#include "slimits.h"
//#include "lks.h"
#include "ngramLM.h"
#include "phraseLM.h"
#include "svmap.h"
// END

// From LDA
#include "lsa.h"
#include "janus_lsa.h"
#include "split_string.h"
#include "util.h"
#include "dmatrix.h"
#include "dirichlet.h"
// END From LDA

#define log10(x)   log(x)/LOG10

//////////////////////////////////////////////////////////////////////
// Local variables
static int Initialized = 0;
static double LOG10 = log(10.0);

static void sync_lm(LSA* lda);
//extern DirichletTree* dir_tree;
//extern Prior* dir_tree;
static Prior* dir_tree = 0;

// use dmarginal adaptation technique?
static float beta = 0.0;
static double* bg_unigram = NULL;
static double* lalpha = NULL;
static double* cache_table = NULL;
static bool* is_stopword = NULL;
static int* janusvocab2ldavocab = NULL;
//////////////////////////////////////////////////////////////////////

static int mergeVocabItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   int v1 = -1;
   int v2 = -1;
   char* new_vocab = 0;

   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<v1>", ARGV_INT, NULL, &v1, NULL, "v1",
      "<v2>", ARGV_INT, NULL, &v2, NULL, "v2",
      "-v",   ARGV_STRING,    NULL, &new_vocab, NULL, "new vocab file",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->merge_vocab(v1, v2);

   if (new_vocab)
      lda->write_vocab(new_vocab);

   return TCL_OK;
}

static int posteriorItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* word = 0;
   int rank = 1;
   int show = 0;
   float cumu = 0.95;

   const int K = lda->get_num_class();

   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<word>", ARGV_STRING, NULL, &word, NULL, "word",
      "-rank",  ARGV_INT,    NULL, &rank, NULL, "sort by rank? (0/1)",
      "-cumu",  ARGV_FLOAT,  NULL, &cumu, NULL, "cumulative prob stop threshold",
      "-show",  ARGV_INT,    NULL, &show, NULL, "show prob? 0/1",
      NULL) != TCL_OK) return TCL_ERROR;

   double* p = new double[K];
   double acc_prob = 0.0;

   lda->posterior(string(word), p);

   if (rank) {
      const vector<pair<int,double> >& result = sort_table(p, K);
   
      for (int n=0; n < K; n++) {
         const pair<int,double>& item = result[n];
         const int topic = item.first;
         const double prob = item.second;
   
         acc_prob += prob;

         // prepare result
         if (show)
            itfAppendResult("{%d %2.4e} ", topic, prob);
         else
            itfAppendResult("%d ", topic);

         if (acc_prob > (double)cumu) break;
      }
   } else {
      for (int k=0; k < K; k++)
         itfAppendResult("%e ", p[k]);
   }

   delete [] p;

   return TCL_OK;
}

static int updatePhraseLMItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   PhraseLM*  lmP = 0;

   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<PhraseLM>", ARGV_OBJECT, NULL, &lmP, "PhraseLM", "adapt Pr(w|c) in PhraseLM",
      NULL) != TCL_OK) return TCL_ERROR;

   const vector<string>& class_list = lda->get_classlist();

   for (int i=0; i < (int) class_list.size(); i++) {
      const string& c = class_list[i];

      // get Pr(w|c)
      const vector<pair<int, double> >& prob = lda->prob_word_class(c);
      for (int n=0; n < (int)prob.size(); n++) {
         const pair<int,double>& item = prob[n];
         const string& w = lda->get_vocab(item.first);
         const double  log10_p = log10(item.second);

         //cout << "find word idx " << w << endl;
         const int word_idx = listIndex ((List*)&(lmP->list), (char*)w.c_str(), 0);

         //cout << "word idx =" << word_idx << endl;
         // update log10 prob

         //cout << "update prob " << log10_p << endl;
         lmP->list.itemA[word_idx].score = CompressScore(log10_p);
      }
   }

   // set dirty flag
   lmP->lksP->dirty = TRUE;

   return TCL_OK;
}

static int probWordClassItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* class_name = 0;
   int show_prob = 0;
   int top_n = 10;
   int show_diff = 0;
   bool is_sort = true;

   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<class name>", ARGV_STRING, NULL, &class_name, NULL, "name of class",
      "-top",          ARGV_INT,   NULL, &top_n,      NULL, "show only top N word",
      "-show",         ARGV_INT,   NULL, &show_prob,  NULL, "show prob?(0/1)",
      "-diff",         ARGV_INT,   NULL, &show_diff,  NULL, "show diff?(0/1)",
      NULL) != TCL_OK) return TCL_ERROR;

   vector<pair<int,double> > prob = lda->prob_word_class(string(class_name), is_sort, show_diff==1);

   itfAppendResult("Class:%s ", class_name);
   itfAppendResult("Total:%d ", prob.size());
   for (int n=0; n < (int)prob.size(); n++) {
      if (n >= top_n) break;

      const pair<int,double>& item = prob[n];
      const string& w = lda->get_vocab(item.first);
      const double  p = item.second;

      // prepare result
      if (show_prob)
         itfAppendResult("{{%s} {%2.4e}} ", w.c_str(), p);
      else
         itfAppendResult("%s ", w.c_str());
   }

   return TCL_OK;
}

static int loadClassItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* filelist = 0;

   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filelist>",   ARGV_STRING, NULL, &filelist,   NULL, "path and class name",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->load_wordclass(filelist);

   return TCL_OK;
}

static int addWordClassItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* class_name = 0;
   char* wordlist = 0;

   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<class name>", ARGV_STRING, NULL, &class_name, NULL, "name of class",
      "<wordlist>",   ARGV_STRING, NULL, &wordlist,   NULL, "file containing list of word members",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->add_wordclass(wordlist, string(class_name));

   return TCL_OK;
}

static int writeSubsItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* class_name = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<class name>", ARGV_STRING, NULL, &class_name, NULL, "name of class",
       NULL) != TCL_OK) return TCL_ERROR;

   string output = string(class_name) + ".subs";

   lda->write_subs(output.c_str(), string(class_name));

   return TCL_OK;
}

static int readSubsItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   NGramLM* lmP = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<ngramLM>", ARGV_OBJECT, NULL, &lmP,   "NGramLM", "NGramLM obj",
       NULL) != TCL_OK) return TCL_ERROR;

   map<LVX, vector<string> > class2word;
   //map<LVX, vector<string> >::const_iterator iter;

  // class section in the arpa LM file
  for (int i = 0; i < lmP->subslist.itemN; i++) {
    vector<string> tk;

    const int num_field = split(lmP->subslist.itemA[i].line, tk);

    if (num_field == 3) {
        const string& class_label = tk[2];
        const string& search_vocab = tk[1];

        // create class->word map
        lda->add_wordclass(class_label, search_vocab);
    }
  }

  return TCL_OK;
}

static int updateSVMapItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   SVMap* svmapP = 0;
   NGramLM* lmP = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<svmap>",   ARGV_OBJECT, NULL, &svmapP, "SVMap",   "SVMap obj",
      "<ngramLM>", ARGV_OBJECT, NULL, &lmP,   "NGramLM", "NGramLM obj",
       NULL) != TCL_OK) return TCL_ERROR;

   map<LVX, vector<string> > class2word;
   map<LVX, vector<string> >::const_iterator iter;

  // class section in the arpa LM file
  for (int i = 0; i < lmP->subslist.itemN; i++) {
    vector<string> tk;

    const int num_field = split(lmP->subslist.itemA[i].line, tk);

    if (num_field == 3) {
        const string& class_label = tk[2];
        const string& search_vocab = tk[1];
        //const string& score = tk[0];

        LVX lvX = svmapP->lksP->itemX (svmapP->lksP, (char*) class_label.c_str());
        SVX svX = listIndex((List*)&(svmapP->svocabP->list), (char*)search_vocab.c_str(), 0);

        if (lvX != LVX_NIL && svX != SVX_NIL) {
          // create class->word map
          class2word[lvX].push_back(search_vocab);

          //svmapExtMap (svmapP, lvX, svX, CompressScore (atof(av[0])));
        }
    }
  }

  // re-compute the Pr(w|c) using LDA probabilities
  for (iter=class2word.begin(); iter != class2word.end(); iter++) {
     //const LVX& c = iter->first;
     const vector<string>& words = iter->second;
     vector<double> prob(words.size(), 0.0);

     for (int v=0; v < (int) words.size(); v++) {
        const char* w = words[v].c_str();

        prob[v] = lda->MAP_prob(w);
     }

     // compute normalization factor
     const double z = accumulate(prob.begin(), prob.end(), 0.0);

     for (int v=0; v < (int) words.size(); v++) {
        const char* w = words[v].c_str();
        SVX svX = listIndex((List*)&(svmapP->svocabP->list), (char*)w, 0);

        // this is the adapted Pr(w|c,h)
        prob[v] /= z;

        // update svmap
        svmapP->lvxSA[svX] = CompressScore(log10(prob[v]));
     }

     // set dirty flag
     svmapP->dirty = TRUE;
  }

   return TCL_OK;
}

static int nbestKLWordItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   int nbest = lda->get_num_vocab();
   int show_kl = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-N",          ARGV_INT, NULL, &nbest,   NULL,      "N-best",
      "-kl",         ARGV_INT, NULL, &show_kl, NULL,      "output prob?(0/1)",
       NULL) != TCL_OK) return TCL_ERROR;

   lda->compute_kl();

   const vector<pair<int,double> >& result = lda->sort_kl();

   for (int n=0; n < nbest; n++) {
      const pair<int,double>& item = result[n];
      const string& w = lda->get_vocab(item.first);
      const double  kl = item.second;

      // prepare result
      if (show_kl)
         itfAppendResult("{{%s} {%2.4e}} ", w.c_str(), kl);
      else
         itfAppendResult("%s ", w.c_str());
   }

   return TCL_OK;
}

static int nbestWordItf( ClientData cd, int argc, char *argv[])
{
   int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   int nbest = lda->get_num_vocab();
   int k = 0;
   int prob = 0;
   float cumu = 0.95;
   int n;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<class idx>", ARGV_INT, NULL, &k, NULL,         "class idx",
      "-N",          ARGV_INT, NULL, &nbest, NULL,     "top N words",
      "-prob",       ARGV_INT, NULL, &prob, NULL,      "output prob?(0/1)",
      "-cumu",       ARGV_FLOAT, NULL, &cumu, NULL,    "cumulative prob stop threshold",
       NULL) != TCL_OK) return TCL_ERROR;

   const vector<pair<int,double> >& result = lda->get_nbest_word(k);
   double acc_prob = 0.0;

   for (n=0; n < nbest; n++) {
      const pair<int,double>& item = result[n];
      const string& w = lda->get_vocab(item.first);
      const double  p = item.second;

      acc_prob += p;

      // prepare result
      if (prob)
         itfAppendResult("{{%s} {%2.4e} {%2.4e}} ", w.c_str(), p, acc_prob);
      else
         itfAppendResult("%s ", w.c_str());

      if (acc_prob > (double)cumu) break;
   }
   itfAppendResult("%d", n);

   return TCL_OK;
}


static int createAccumItf( ClientData cd, int argc, char *argv[])
{
//   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;

   LSA* lda = (LSA*) cd;

/*
   int num_doc = 1000;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<num doc>",          ARGV_INT,    NULL, &num_doc,     NULL, "# of documents",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->create_accum(num_doc);
*/
   lda->create_accum();

   return TCL_OK;
}

static int writeAccumItf( ClientData cd, int argc, char *argv[])
{
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* filename = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING,    NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->write_accum(filename);

   return TCL_OK;
}

static int clearAccumItf( ClientData cd, int argc, char *argv[])
{
   LSA* lda = (LSA*) cd;

   lda->clear_accum();

   return TCL_OK;
}

static int updateItf( ClientData cd, int argc, char *argv[])
{
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   float learn = 0.1;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-learn",  ARGV_FLOAT,  NULL, &learn, NULL, "learning rate for alpha",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->update_model((double)learn);

   // get kl sum from previous iteration
   const double kl = lda->get_kl();

   itfAppendResult("%e", kl);

   return TCL_OK;
}

static int accumItf( ClientData cd, int argc, char *argv[])
{
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;
   char* mult_doc = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<doc>",    ARGV_STRING, NULL, &mult_doc,   NULL, "multinomial doc file",
      NULL) != TCL_OK) return TCL_ERROR;

   // read doc
   lda->read_doc(mult_doc);
   lda->accu_doc();

   return TCL_OK;
}

static void sync_lm(LSA* lda)
{
   // sync with LSA NgramLM in janus
   NGramLM* lmP = (NGramLM*) lda->get_ngram_lm();

   if (lmP) {
      cerr << "sync with NGramLM, beta = " << beta << endl;

      // update log10 prob table
      double lprob;

      // # of vocab defined in lks
      const int N = lmP->list.itemN;

      if (N != lda->get_num_vocab()) {
         cerr << "Warning: # of vocabulary defined in LSA and NGramLM does not match!: " << N << " " << lda->get_num_vocab() << endl;
      }

      for (int i=0; i < N; i++) {
         const char* word = lmP->list.itemA[i].name;
         const int v = lda->get_vocab_id(word);

         lprob = log10(lda->MAP_prob(v));

         if (beta > 0.0) {
            // use marginal adaptation
            lprob = beta * (lprob - bg_unigram[v]);
         }

         lmP->mgram[0].probA[i] = CompressScore(lprob);
      }

      // set the dirty flag (for safety)
      lmP->lksP->dirty = TRUE;
   }
}

// Start Wilson Oct 9 2008
// arr contains the p(w|h) where w is the vocabulary index from 0 to V-1
// We can manipulate the cache table via marginal adaptation before being used by the decoder

static void updateScale(LSA* lda)
{
   for (int v=0; v < lda->get_num_vocab(); v++)
      lalpha[v] = beta * (log10(lda->MAP_prob(v)) - bg_unigram[v]);
}

void Marginal_Adapt_Cache(void* lda_obj, lmScore *arr)
{
   LSA* lda = (LSA*) lda_obj;

   // obtain the background LM
   NGramLM* lmP = (NGramLM*) lda->get_ngram_lm();

   if (lmP) {
      //cerr << "perform marginal adapt on the cache table, beta = " << beta << endl;

      // # of vocab defined in lks
      const int N = lmP->list.itemN;

      if (N != lda->get_num_vocab())
         cerr << "Warning: # of vocabulary defined in LSA and NGramLM does not match!: " << N << " " << lda->get_num_vocab() << endl;

      double stopword_mass = 0.0;
      for (int i=0; i < N; i++) {
         //const char* word = lmP->list.itemA[i].name;
         const int v = janusvocab2ldavocab[i];
         double lprob = UnCompressScore(arr[i]);

         // check if v is a stopword, if not, do not update the prob
         if (is_stopword[v]) {
            // disable this item with tiny prob (do not update)
            cache_table[i] = -20.0;
            stopword_mass += exp(lprob*LOG10);
         } else
            cache_table[i] = lprob + lalpha[v];
      }

      if (stopword_mass >= 1.0)
         cerr << "Warning: sum stopword mass bigger than 1!" << stopword_mass << endl;

      const double log_remain_mass = (stopword_mass == 0.0)? 0.0 : log10(1.0 - stopword_mass);

      // perform log add to compute the norm factor
      double lnorm = log10_add(cache_table, N);

      // normalize prob
      for (int i=0; i < N; i++) {
         const int v = janusvocab2ldavocab[i];
         if (!is_stopword[v])
            arr[i] = CompressScore(log_remain_mass + cache_table[i] - lnorm);
      }
   }
}
// End Wilson Oct 9 2008

static int numDocItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   LSA*  lda = (LSA*) cd;

   const int num_doc = lda->get_num_doc();

   itfAppendResult("%d", num_doc);

   return TCL_OK;
}

static int loadDocItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA*  lda = (LSA*) cd;
   char* doc_file = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<doc file>",   ARGV_STRING, NULL, &doc_file, NULL, "doc file (multinomial file)",
      NULL) != TCL_OK) return TCL_ERROR;

   lda->read_doc(doc_file);

   return TCL_OK;
}

static int printWeightItf( ClientData cd, int argc, char *argv[])
{
   LSA* lda = (LSA*) cd;

   const double* wgt = lda->get_MAP_wgt();

   for (int k=0; k < lda->get_num_class(); k++)
      itfAppendResult("%e ", wgt[k]);

   return TCL_OK;
}

static int adaptSentItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   FArray conf = {NULL, 0};
   char* hypo = 0;
   float forget = 0.4;
   int use_kl_wgt = 0;
   int print_wgt = 0;
   int use_marginal_adapt = 0; // Wilson Oct 9 2008
   int print_mult = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<sentence>",    ARGV_STRING, NULL, &hypo,    NULL, "input adaptation sentence",
      "-conf",         ARGV_FARRAY, NULL, &conf,    NULL, "word confidence scores",
      "-forget",       ARGV_FLOAT,  NULL, &forget,  NULL, "forget factor <= 1.0",
      "-kl",           ARGV_INT,    NULL, &use_kl_wgt, NULL, "use KL wght? (0/1)",
      "-print",        ARGV_INT,    NULL, &print_wgt,  NULL, "print adapted wght? (0/1)",
      "-marginal",     ARGV_INT,    NULL, &use_marginal_adapt,  NULL, "use marginal adaptation? (0/1)",
      "-print_mult",   ARGV_INT,    NULL, &print_mult,  NULL, "print word-level p(z_i)? (0/1)",
      NULL) != TCL_OK) return TCL_ERROR;

   // tokenize hypo into av, size(av) = ac
   static vector<string> tk;

   tk.clear();
   const int num_word = split(hypo, tk);

   if (conf.itemN > 0 && num_word != conf.itemN) {
      SERROR( "# item in hypo and conf lists not the same: '%d' != '%d'\n", num_word, conf.itemN);

      return TCL_ERROR;
   }

   double *c = 0;

   if (conf.itemN > 0) {
      c = new double[conf.itemN];

      for (int pos=0; pos < num_word; pos++)
         c[pos] = (double) conf.itemA[pos];
   }

   lda->set_kl_wgt(use_kl_wgt==1);

   lda->adapt_sent(tk, c, 0, -1, (double) forget);

   if (print_wgt) {
      const double* wgt = lda->get_MAP_wgt();

      for (int k=0; k < lda->get_num_class(); k++)
         itfAppendResult("%e ", wgt[k]);
   }

   if (use_marginal_adapt) {
      // update scaling factor
      updateScale(lda);

      // tell janus to use marginal adaptation on cache table
      NGramLM* lmP = (NGramLM*) lda->get_ngram_lm();
      if (lmP)
         lmP->lda_obj = lda;
   } else if (beta == 0.0) {
      // not using marginal adaptation
      sync_lm(lda);
   }

   // only useful for analysis purpose
   if (print_mult) {
      const double* const* q = lda->get_q();

      if (q == NULL) {
         cerr << "q is NULL!" << endl;
      }

      for (int pos=0; pos < num_word; pos++) {
         for (int k=0; k < lda->get_num_class(); k++) {
            itfAppendResult("%e ", q[pos][k]);
         }
         itfAppendResult("\n");
      }
   }

   // clean up
   if (conf.itemA)
      farrayDeinit(&conf);

   if (c) delete [] c;

   return TCL_OK;
}

static int wordProbItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   double prob = 0.0;
   char* hypo = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<word>",   ARGV_STRING, NULL, &hypo,    NULL,          "string",
       NULL) != TCL_OK) return TCL_ERROR;

   prob = lda->MAP_prob(string(hypo));

   itfAppendResult("%e", prob);

   return TCL_OK;
}


// print the object
static int PutsItf( ClientData cd, int argc, char *argv[])
{
   LSA* lda = (LSA*) cd;

   itfAppendResult("{nclass %d} {nlex %d} ", lda->get_num_class(), lda->get_num_vocab());

   // print class list if available
   const vector<string>& clist = lda->get_classlist();

   itfAppendResult("{");
   for (int i=0; i < (int) clist.size(); i++) {
      itfAppendResult("%s ", clist[i].c_str());
   }
   itfAppendResult("}");

   return TCL_OK;
}

static int resetPriorItf( ClientData cd, int argc, char *argv[])
{
   //int  ac  = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   // reset gamma to it's original alphas
   lda->reset_prior();

   NGramLM* lmP = (NGramLM*) lda->get_ngram_lm();

   // tell janus to not to use marginal adaptation on cache table after reset
   if (!lmP) return TCL_OK;

   if (lmP->lda_obj)
      lmP->lda_obj = NULL;
   else if (beta == 0.0)
      sync_lm(lda);

   return TCL_OK;
}

// destroy the object
static int FreeItf(ClientData cd)
{
   LSA* lda = (LSA*) cd;

   delete lda;

   return TCL_OK;
}

static int saveLMItf (ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   char* model_file = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<arpa lm file>",   ARGV_STRING, NULL, &model_file, NULL, "arpa lm file",
       NULL) != TCL_OK) return TCL_ERROR;

   lda->write_arpa(model_file);

   return TCL_OK;
}

static int loadItf (ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   char* model_file = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<input file>",   ARGV_STRING, NULL, &model_file, NULL, "model file",
       NULL) != TCL_OK) return TCL_ERROR;

   lda->load_model(model_file);

   return TCL_OK;
}


static int saveItf (ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   char* model_file = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<output file>",   ARGV_STRING, NULL, &model_file, NULL, "model file",
       NULL) != TCL_OK) return TCL_ERROR;

   lda->write_model(model_file);

   return TCL_OK;
}

static int addAccumItf(ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   char* acc_file = 0;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<accum file>",   ARGV_STRING, NULL, &acc_file, NULL, "accumulator file",
       NULL) != TCL_OK) return TCL_ERROR;

   lda->add_accum(acc_file);

   return TCL_OK;
}

static int trainItf (ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   LSA* lda = (LSA*) cd;

   char* train_file = 0;
   int emmax = EMMAX_DEFAULT;
   float epsilon = EPSILON_DEFAULT;
   int from = 0;
   int len = -1;

   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<train file>",   ARGV_STRING, NULL, &train_file, NULL, "multinomial train file",
      "-i",             ARGV_INT, NULL, &emmax, NULL, "max. EM iterations",
      "-e",             ARGV_FLOAT, NULL, &epsilon, NULL, "threshold for convergence checking",
      "-f",             ARGV_INT, NULL, &from, NULL, "starting doc index",
      "-l",             ARGV_INT, NULL, &len, NULL, "# of doc to be processed",
       NULL) != TCL_OK) return TCL_ERROR;

   double ll = lda->learn(train_file, emmax, (double) epsilon, from, len);

   itfAppendResult("%e", ll);

   return TCL_OK;
}

// construct the object
static ClientData CreateItf (ClientData cd, int argc, char *argv[])
{
  int       ac    = argc /* (argc > 1) ? 1 : 0 */ ;
  char      *name = NULL;
  char      *input_model = NULL;
  char      *vocab_file = NULL;
  char      *stopword_file = NULL;
  int       nclass = 0;
  int       use_kl = 0;
  int       max_class = 0;
  int       dlenmax = 10000;
  int       demmax = DEMMAX_DEFAULT;
  int       use_dirtree = 0;
  NGramLM*  lmP = 0;

  if ( itfParseArgv( argv[0], &ac, argv, 1,
      "<name>",              ARGV_STRING, NULL, &name,        NULL,      "object name",
      "<model file>",        ARGV_STRING, NULL, &input_model, NULL,      "model file",
      "<vocab file>",        ARGV_STRING, NULL, &vocab_file,  NULL,      "vocab file",
      "<# classes>",         ARGV_INT,    NULL, &nclass,      NULL,      "# of latent topic",
      "-x",                  ARGV_INT,    NULL, &max_class,   NULL,      "max. # of latent topic allowed",
      "-s",                  ARGV_INT,    NULL, &dlenmax,     NULL,      "max. buffer size",
      "-i",                  ARGV_INT,    NULL, &demmax,      NULL,      "max. inner-EM iterations",
      "-kl",                 ARGV_INT,    NULL, &use_kl,      NULL,      "use kl wgt? 0/1",
      "-t",                  ARGV_INT,    NULL, &use_dirtree, NULL,      "use dirTree? 0/1",
      "-beta",               ARGV_FLOAT,  NULL, &beta,        NULL,      "dmarginal beta: 0 = disable",
      "-ng",                 ARGV_OBJECT, NULL, &lmP,         "NGramLM", "LingKS (unigram NgramLM (interpol) | bg NgramLM (dmarginal))",
      "-stoplist",           ARGV_STRING, NULL, &stopword_file,  NULL,   "stopword file (only for marginal adaptation)",
      NULL) != TCL_OK) return NULL;

// START June 1 06
   if (use_dirtree) {
      //dir_tree = new DirichletTree(nclass, 2, lda.get_alpha());
      dir_tree = new DirichletTree(nclass, 2, 0);
   }
// END June 1 06

   // create LDA-based LSA object
   LSA* lda = new LSA(input_model, vocab_file, nclass, max_class, dlenmax, demmax, dir_tree);

   lda->set_save_mem(true);
   lda->set_kl_wgt(use_kl);

   is_stopword = new bool[lda->get_num_vocab()];
   for (int v=0; v < lda->get_num_vocab(); v++)
      is_stopword[v] = false;

   if (stopword_file) {
      ifstream is(stopword_file);

      if (is.fail()) {
         cerr << "can't open stopword file" << endl;
         exit(-1);
      }

      string word;
      while (is >> word) {
         const int v = lda->get_vocab_id(word.c_str());
         is_stopword[v] = true;
      }

      is.close();
   }

   // synchronize with the corresponding NgramLM representation of LSA if provided
   if (lmP) {
      lda->set_ngram_lm(lmP);

      // get bg unigram from lmP
      // # of vocab defined in lks
      const int N = lmP->list.itemN;
   
      if (N != lda->get_num_vocab()) {
         cerr << "Warning: # of vocabulary defined in LSA and NGramLM does not match!: " << N << " " << lda->get_num_vocab() << endl;
      }
   
      bg_unigram = new double[N];
      lalpha = new double[N];
      cache_table = new double[N];
      janusvocab2ldavocab = new int[N];
   
      for (int i=0; i < N; i++) {
         // ng i -> lsa v index mapping
         const char* word = lmP->list.itemA[i].name;
         const int v = lda->get_vocab_id(word);
   
         bg_unigram[v] = UnCompressScore(lmP->mgram[0].probA[i]);
         lalpha[v] = 0.0;
         janusvocab2ldavocab[i] = v;

         //lmP->mgram[0].probA[i] = CompressScore(lprob);
      }
   }

   return (ClientData) lda;
}

// configure the object
static int ConfigureItf (ClientData cd, char *var, char *val)
{
   return TCL_OK;
}

// access sub-objects
static ClientData AccessItf( ClientData cd, char* name, TypeInfo** ti)
{
  return 0;
}

// Add your methods here
static Method method[] =
{
  { "puts",        PutsItf,        "display object info" },
  { "reset",       resetPriorItf,  "reset prior" },
  { "adapt_sent",  adaptSentItf,   "adapt LSA given a sentence" },
  { "wprob",       wordProbItf,    "compute prob of a word" },
  { "train",       trainItf,       "train LSA model" },
  { "save",        saveItf,        "save model" },
  { "load",        loadItf,        "load model" },
  { "createAccus", createAccumItf, "create accumulators" },
  { "clearAccus",  clearAccumItf,  "clear accumulators" },
  { "writeAcc",    writeAccumItf,  "write accumulators" },
  { "addAccus",    addAccumItf,    "add accumulators" },
  { "accu",        accumItf,       "accumulate training text" },
  { "update",      updateItf,      "update model" },
  { "save_lm",     saveLMItf,      "save LM (unigram) in arpa format" },
  { "nbest_w",     nbestWordItf,   "get the top-N word from class k" },
  { "nbest_kl",    nbestKLWordItf, "get the top-N word in terms of KL divergence from uniform topic distribution" },
  { "addWordClass", addWordClassItf, "Add word-class" },
  { "lclass",      loadClassItf,     "load word-class from filelist" },
  { "wclass",      probWordClassItf, "compute Pr(w|class, context)" },
  { "readSubs",    readSubsItf,      "read subsmodels from LingKS obj" },
  { "writeSubs",   writeSubsItf,     "write subsmodels" },
  { "updateSVMap", updateSVMapItf,   "update Pr(w|c) in SVMap" },
  { "updatePhraseLM", updatePhraseLMItf, "update Pr(w|c) in PhraseLM" },
  { "post",        posteriorItf,     "compute Pr(k|w) based on currently adapted prior" },
  { "merge_w",     mergeVocabItf,    "merge vocab" },

  { "load_doc",  loadDocItf,      "load document set (multinomial file)" },
  { "num_doc",  numDocItf,      "get # of doc in the doc file" },
  { "print_wgt",  printWeightItf,      "print the adapted mixture topic weight" },
  {  NULL,    NULL,          NULL }
};

static TypeInfo objInfo = { "LSA", 0, -1, method,
                      CreateItf,    FreeItf,
                      ConfigureItf, AccessItf, NULL,
                     "LSA" } ;

// init()
int LSA_Init()
{
   if (Initialized) return TCL_OK;

   //  if (Lh_Init() == TCL_ERROR) return TCL_ERROR;

   // register object
   itfNewType( &objInfo);

   Initialized = 1;
   return TCL_OK;
}
