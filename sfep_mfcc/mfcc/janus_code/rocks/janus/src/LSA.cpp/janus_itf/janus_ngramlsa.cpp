#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>

#include "NgramLSA.h"
#include "dirichlet.h"
#include "util.h"

//using namespace std;

// from janus
#include "itf.h"
#include "slimits.h"
#include "ngramLM.h"
//#include "lks.h"
// END

// compulsory
#include "janus_ngramlsa.h"


/* COMPRESSEDLM is defined in slimits.h and defines whether to use
   the compressed LM (scores are int) or the full float-type LM */
#ifdef COMPRESSEDLM
  #define   CompressScore(score) (signed short int)((score)*LMSCORE_SCALE_FACTOR-0.5)
  #define UnCompressScore(score) (float)(score)/LMSCORE_SCALE_FACTOR
#else
  #define   CompressScore(score) (score)
  #define UnCompressScore(score) (score)
#endif

//////////////////////////////////////////////////////////////////////
// Local variables
static int Initialized = 0;
static double lambda_bg = 0.8;
//////////////////////////////////////////////////////////////////////

static int createAccumItf( ClientData cd, int argc, char *argv[])
{
   NgramLSA* ngramlsa = (NgramLSA*) cd;

   ngramlsa->create_accum();

   return TCL_OK;
}

static int clearAccumItf( ClientData cd, int argc, char *argv[])
{
   NgramLSA* ngramlsa = (NgramLSA*) cd;

   ngramlsa->clear_accum();

   return TCL_OK;
}

static int accumItf( ClientData cd, int argc, char *argv[])
{
   char* doc_file = NULL;
   char* in_ap_file = NULL;
   char* out_ap_file = NULL;
   int demmax = 20;
   int unigram_lsa = 1;
   int accu = 1;
   bool fast_em = false;
   float threshold = -1;
   float document_weight = 1.0;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<doc file>",    ARGV_STRING, NULL, &doc_file,   NULL, "doc file (documents surrounded by <TEXT> word1 word 2 ... </TEXT>)",
      "-accu",         ARGV_INT,    NULL, &accu,       NULL, "Accu? (0/1)",
      "-iter",         ARGV_INT,    NULL, &demmax,     NULL, "# of VBEM iterations",
      "-unigram",      ARGV_INT,    NULL, &unigram_lsa, NULL, "use unigram LSA for VBEM? (0/1)",
      "-read_ap",      ARGV_STRING, NULL, &in_ap_file, NULL, "input AP file for VBEM",
      "-write_ap",     ARGV_STRING, NULL, &out_ap_file, NULL, "OUTPUT AP file for VBEM",
      "-threshold",    ARGV_FLOAT,  NULL, &threshold, NULL, "MI threshold to consider a bigram is sticky",
      "-doc_wgt",      ARGV_FLOAT,  NULL, &document_weight, NULL, "Document weight. Default is 1",
      NULL) != TCL_OK) return TCL_ERROR;

   int d = 0;
   NgramLSA* ngramlsa = (NgramLSA*) cd;
   File f = File(doc_file, "r");
   std::vector<VocabIndex*> ngram_array;

   cout << "processing file " << doc_file << endl;

   std::ifstream* ap_in = NULL;
   if (in_ap_file) {
      ap_in = new std::ifstream(in_ap_file, ios::in|ios::binary);
      fast_em = true;
   }

   std::ofstream* ap_out = NULL;
   if (out_ap_file) {
      ap_out = new std::ofstream(out_ap_file, ios::out|ios::binary);
   }

   while (1) {
      bool eof = false;

      if (threshold > 0) {
         ngramlsa->get_doc_ngrams(f, ngram_array, eof);

         if (eof) break;

         // for fast EM
         if (ap_in)
            ngramlsa->read_ap(*ap_in);

         // ACCU=yes, unigram LSA for E-step=yes
         //ngramlsa->phrase_vbem(ngram_array, ngram_array.size(), demmax, NULL, accu==1, unigram_lsa==1, sent_level==1);
         //cout << "perform phrase_vbem" << endl;
         ngramlsa->phrase_vbem(ngram_array, demmax, NULL, 0, ngram_array.size(), threshold, accu==1, unigram_lsa==1, document_weight);

         // for dumping ap
         if (ap_out)
            ngramlsa->write_ap(*ap_out);
      } else {
         NgramCounts<FloatCount>* doc_cnt = ngramlsa->gen_doc_cnt(f, eof);
   
         if (eof) break;

         // for fast EM
         if (ap_in)
            ngramlsa->read_ap(*ap_in);
   
         if (doc_cnt) {
            // ACCU=yes, unigram LSA for E-step=yes, fast-em=false
            ngramlsa->vbem(*doc_cnt, demmax, NULL, accu==1, unigram_lsa==1, fast_em, document_weight);
         } else {
            cout << "skipping empty doc " << d << " in " << doc_file << endl;

         }

         // write sth
         if (ap_out)
            ngramlsa->write_ap(*ap_out);
   
         delete doc_cnt;
      }

      d++;

/*
      double* tmp_g = new double[num_topic];
      copy_array(gamma, tmp_g, num_topic);
      gammas.push_back(tmp_g);

      d++;

      if ((d % 1000) == 0)
         dout() << "finished " << d << " docs" << endl;
*/
   }

   if (ap_in) {
      ap_in->close();
      delete ap_in;
   }

   if (ap_out) {
      ap_out->close();
      delete ap_out;
   }

   // delete ngram_array
   if (!ngram_array.empty()) {
      for (int i=0; i < (int) ngram_array.size(); i++)
         delete [] ngram_array[i];
      ngram_array.clear();
   }

   return TCL_OK;
}

static int writeAccumItf( ClientData cd, int argc, char *argv[])
{
   char* filename = 0;
   int k = -1;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<filename>",  ARGV_STRING,    NULL, &filename, NULL, "output filename",
      "<topic idx>", ARGV_INT,       NULL, &k, NULL, "topic index >= 0",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   ngramlsa->write_cnt(filename, k);

   return TCL_OK;
}

static int updateItf( ClientData cd, int argc, char *argv[])
{
   char* count_file = 0;
   int k = -1;
   int discount_scheme = FKNESER;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<count file>", ARGV_STRING,    NULL, &count_file, NULL, "Input count filename for topic k",
      "<topic idx>",  ARGV_INT,       NULL, &k, NULL, "topic index >= 0",
      "-scheme",      ARGV_INT,       NULL, &discount_scheme, NULL, "0:kneser, 1:abs, 3:witten-bell",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   File f(count_file, "r");
   NgramCounts<FloatCount> cur_cnt_obj(*(ngramlsa->get_vocab()), ngramlsa->get_order());
   cur_cnt_obj.openVocab = false;

   cout << "# vocab before reading count " << ngramlsa->get_vocab()->numWords() << endl;
   cout << "Reading count " << count_file << " for topic " << k << endl;
   cur_cnt_obj.read(f);
   cout << "# vocab after reading count " << ngramlsa->get_vocab()->numWords() << endl;

   cout << "reestimate LM " << k << endl;
   ngramlsa->reestimate_lm(&cur_cnt_obj, k, discount_scheme);
   cout << "# vocab after reestimate_lm" << ngramlsa->get_vocab()->numWords() << endl;

   return TCL_OK;
}

static int saveItf( ClientData cd, int argc, char *argv[])
{
   char* lm_file = 0;
   int k = -1;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<LM filename>",   ARGV_STRING,    NULL, &lm_file, NULL, "output LM filename for topic k",
      "<topic idx>",  ARGV_INT,       NULL, &k, NULL, "topic index >= 0",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   cout << "write LM " << k << endl;
   Ngram* lm = ngramlsa->get_topic_lm(k);

   File fo(lm_file, "w");
   lm->write(fo);

   return TCL_OK;
}

static int saveAlphaItf( ClientData cd, int argc, char *argv[])
{
   char* out_model_dir = NULL;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<out_dir>",   ARGV_STRING,    NULL, &out_model_dir, NULL, "output model directory",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   // write dirichlet-tree prior
   ngramlsa->write_alpha(out_model_dir);

   return TCL_OK;
}

static int clearItf( ClientData cd, int argc, char *argv[])
{
   int k = -1;

   // get arguments
   if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<topic idx>",  ARGV_INT,       NULL, &k, NULL, "topic index >= 0",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   cout << "delete LM " << k << endl;
   Ngram* lm = ngramlsa->get_topic_lm(k);

   lm->clear();

   return TCL_OK;
}

static int adaptItf( ClientData cd, int argc, char *argv[])
{
   char *text_file = NULL;
   char *prob_stream_file = NULL;
   float beta = 0.4;
   float forget = 0.4;
   int demmax = 20;
   int static_interpol = 0;
   int use_unigram = 1;
   int cur_sent_id = -1;
   int win_size = 20;
   float threshold = -1.0;  // no phrase-sticky stuff

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<text file>", ARGV_STRING, NULL, &text_file,    NULL, "text file for adaptation (surrounded by <TEXT> and </TEXT>)",
      "beta",        ARGV_FLOAT, NULL, &beta,    NULL,   "scaling factor for marginal adaptation (unigram LSA)",
      "-iter",         ARGV_INT,    NULL, &demmax,     NULL, "# of VBEM iterations",
      "-static",       ARGV_INT,    NULL, &static_interpol,     NULL, "compute static LM for rescoring? (0/1)",
      "-prob_file",    ARGV_STRING, NULL, &prob_stream_file,     NULL, "output prob stream file",
      "-unigram",      ARGV_INT,    NULL, &use_unigram,     NULL, "Use unigram LSA for inference? (0/1)",
      "-sent_id",      ARGV_INT,    NULL, &cur_sent_id,     NULL, "current sentence id (implies incremental adaptation)",
      "-win",          ARGV_INT,    NULL, &win_size,        NULL, "context window size",
      "-forget",       ARGV_FLOAT, NULL,  &forget,          NULL, "forget factor (for incremental adaptation only)",
      "-threshold",    ARGV_FLOAT, NULL,  &threshold,       NULL, "threshold for sticky-pair (phrase-level bigram LSA only)",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;
   std::vector<VocabIndex*> ngram_array;
   bool eof = false;
   File f(text_file, "r");

   ngramlsa->get_doc_ngrams(f, ngram_array, eof);

   if (eof) return TCL_OK;

   // run vbem: new mixing wgt should be in gamma array
   // doc-level adaptation
   if (threshold > 0) {
      ngramlsa->phrase_vbem(ngram_array, demmax, NULL, 0, ngram_array.size(), threshold, false, use_unigram==1);
   } else {
      ngramlsa->vbem(ngram_array, ngram_array.size(), demmax, NULL, false, use_unigram==1, false);
   }

   // check if incr adaptation
   if (cur_sent_id >= 0) {
      // update dirichlet prior
      DirichletTree* dir_tree = ngramlsa->get_dir_tree();

      // move posterior gammas to alpha
      dir_tree->adapt_alpha();

      // set forget factor
      ngramlsa->set_forget(forget);

      // start getting detailed posterior of the current sentence
      const int N = ngramlsa->get_num_sent();
      int start = ngramlsa->get_sent_pos(cur_sent_id);
      int end = ngramlsa->get_sent_pos(cur_sent_id+1);
      int size = end - start;

      // complement insufficient windows by moving the start pos to include the previous sentence
      int prv_sent_id = cur_sent_id;
      while (size < win_size && prv_sent_id >= 0) {
         // move to the start pos of the prev sentence 
         start = ngramlsa->get_sent_pos(prv_sent_id);
         size = end - start;

         prv_sent_id--;
      }

      // complement insufficient windows by moving the end pos to include the next sentence
      int next_sent_id = cur_sent_id+1;
      while (size < win_size && next_sent_id < N) {
         // move to the start pos of the prev sentence 
         end = ngramlsa->get_sent_pos(next_sent_id);
         size = end - start;

         next_sent_id++;
      }

      // perform inference of the current sentence
      if (threshold > 0) {
         ngramlsa->phrase_vbem(ngram_array, demmax, NULL, start, size, threshold, false, use_unigram==1);
      } else {
         ngramlsa->vbem(ngram_array, size, demmax, NULL, false, use_unigram==1, false, start);
      }
   }

   // update scale factor
   ngramlsa->update_scale_factor(beta);

   if (static_interpol) {
      ngramlsa->delete_adapted_lm();
      Ngram* adapt_lm = ngramlsa->mix_ngram_lm(ngramlsa->get_gamma());

      File f("tmplm.2.gz", "w");
      adapt_lm->write(f);
      ngramlsa->set_adapted_lm(adapt_lm);
   }

   // dump prob stream (for LM interpolation)
   if (prob_stream_file)
      ngramlsa->test_PPL(text_file, prob_stream_file);

   // delete ngram_array
   if (!ngram_array.empty()) {
      for (int i=0; i < (int) ngram_array.size(); i++)
         delete [] ngram_array[i];
      ngram_array.clear();
   }

   return TCL_OK;
}

static int xadaptItf( ClientData cd, int argc, char *argv[])
{
   char *src_text_file = NULL;
   char *prob_stream_file = NULL;
   char *out_src_lm_file = NULL;
   char *out_tgt_lm_file = NULL;
   float beta = 0.4;
   int demmax = 20;
   int static_interpol = 0;
   NgramLSA* src_ngramlsa = NULL;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<source ngram LSA>", ARGV_OBJECT, NULL, &src_ngramlsa,   "NgramLSA", "Source NgramLSA obj",
      "<source text file>", ARGV_STRING, NULL, &src_text_file,  NULL, "source text file for adaptation (surrounded by <TEXT> and </TEXT>)",
      "-beta",         ARGV_FLOAT, NULL, &beta,    NULL,   "scaling factor for marginal adaptation (unigram LSA)",
      "-iter",         ARGV_INT,    NULL, &demmax,     NULL, "# of VBEM iterations",
      "-static",       ARGV_INT,    NULL, &static_interpol,     NULL, "compute static LM for rescoring? (0/1)",
      "-prob_file",    ARGV_STRING,    NULL, &prob_stream_file,     NULL, "output prob stream file (source side)",
      "-out_src_lm",       ARGV_STRING,    NULL, &out_src_lm_file,     NULL, "output static interpolated LM on the source side",
      "-out_tgt_lm",       ARGV_STRING,    NULL, &out_tgt_lm_file,     NULL, "output static interpolated LM on the target side",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* tgt_ngramlsa = (NgramLSA*) cd;
   std::vector<VocabIndex*> ngram_array;
   bool eof = false;
   File f(src_text_file, "r");

   src_ngramlsa->get_doc_ngrams(f, ngram_array, eof);

   if (eof) return TCL_OK;

   // run vbem: new mixing wgt should be in gamma array
   src_ngramlsa->vbem(ngram_array, ngram_array.size(), demmax, NULL, false, false, false);

   // copy wgt from src to tgt
   copy_array(src_ngramlsa->get_gamma(), tgt_ngramlsa->get_gamma(), src_ngramlsa->get_num_topic());

   // update scale factor
   tgt_ngramlsa->update_scale_factor(beta);

   if (static_interpol) {
      src_ngramlsa->delete_adapted_lm();

      // use the inferred weight from source side for LM adaptation
      Ngram* src_adapt_lm = src_ngramlsa->mix_ngram_lm(src_ngramlsa->get_gamma());
      src_ngramlsa->set_adapted_lm(src_adapt_lm);

      // write src adapted LM
      if (out_src_lm_file) {
         File f(out_src_lm_file, "w");
         src_adapt_lm->write(f);
      }

      tgt_ngramlsa->delete_adapted_lm();

      // use the inferred weight from source side for LM adaptation
      Ngram* tgt_adapt_lm = tgt_ngramlsa->mix_ngram_lm(tgt_ngramlsa->get_gamma());
      tgt_ngramlsa->set_adapted_lm(tgt_adapt_lm);

      // write tgt adapted LM
      if (out_tgt_lm_file) {
         File f(out_tgt_lm_file, "w");
         tgt_adapt_lm->write(f);
      }
   }

   // dump prob stream (for LM interpolation)
   if (prob_stream_file)
      src_ngramlsa->test_PPL(src_text_file, prob_stream_file);

   // delete ngram_array
   if (!ngram_array.empty()) {
      for (int i=0; i < (int) ngram_array.size(); i++)
         delete [] ngram_array[i];
      ngram_array.clear();
   }

   return TCL_OK;
}

static int updateRatioItf( ClientData cd, int argc, char *argv[])
{
   float beta = 0.4;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "beta",   ARGV_FLOAT, NULL, &beta,    NULL,   "scaling factor for marginal adaptation (unigram LSA)",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   ngramlsa->update_scale_factor(beta);

   return TCL_OK;
}

static int rescoreLatItf( ClientData cd, int argc, char *argv[])
{
   char *in_lat_file = NULL;
   char *out_lat_file = NULL;
   float lambda0 = 0.0; // default: disable linear interpolation mode
   float lambda = 0.8;
   int join_node = 0;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<in Lattice file>", ARGV_STRING, NULL, &in_lat_file,    NULL, "input lattice file (in IBM format)",
      "<out Lattice file>", ARGV_STRING, NULL, &out_lat_file,    NULL, "output lattice file (in IBM format)",
      "-lambda0",   ARGV_FLOAT, NULL, &lambda0,    NULL,   "interpolation factor (for background LM + unigram LSA)",
      "-lambda",   ARGV_FLOAT, NULL, &lambda,    NULL,   "interpolation factor (for background LM + bigram LSA)",
      "-join",   ARGV_INT, NULL, &join_node,    NULL,   "join nodes? (0/1)",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   if (join_node) {
      cout << "join_node disabled!" << endl;
      //cout << in_lat_file << " --> " << out_lat_file << " (join nodes)" << endl;
      //ngramlsa->rescore_lat1(in_lat_file, out_lat_file, lambda);
   } else {
      cout << in_lat_file << " --> " << out_lat_file << endl;
      ngramlsa->rescore_lat(in_lat_file, out_lat_file, lambda0, lambda);
   }

   return TCL_OK;
}

static int loadItf( ClientData cd, int argc, char *argv[])
{
   char *model_file = NULL;
   int limit_vocab = 1;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<model file>", ARGV_STRING, NULL, &model_file,    NULL, "a list of arpa ngram model (1st one being the Dirichlet-tree prior)",
      "-limit", ARGV_INT, NULL, &limit_vocab,    NULL, "limit vocab to a small subset? (0/1)",
      NULL) != TCL_OK) return TCL_ERROR;

   NgramLSA* ngramlsa = (NgramLSA*) cd;

   cout << "loading topic LM " << model_file << endl;
   ngramlsa->read_topic_model(model_file, limit_vocab);

   return TCL_OK;
}

// print the object
static int PutsItf( ClientData cd, int argc, char *argv[])
{
   //NgramLSA* obj = (NgramLSA*) cd;

   //itfAppendResult("value=%d", obj->get_value());

   return TCL_OK;
}

// destroy the object
static int FreeItf(ClientData cd)
{
   NgramLSA* obj = (NgramLSA*) cd;

   // delete vocab first
   delete obj->get_vocab();

   // delete obj
   delete obj;

   return TCL_OK;
}

static int resetItf( ClientData cd, int argc, char *argv[])
{
   NgramLSA* obj = (NgramLSA*) cd;

   obj->reset_prior();

   // disable linear interpolation during decoding by setting ngram_lda_obj to NULL
   NGramLM* lmP = (NGramLM*) obj->get_ngram_lm();

   if (lmP != NULL)
      lmP->ngram_lda_obj = NULL;

   return TCL_OK;
}

// construct the object
static ClientData CreateItf (ClientData cd, int argc, char *argv[])
{
   //int       ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   char      *name = NULL;
   char      *lsa_vocab_file = NULL; // full vocab 
   char      *vocab_file = NULL; // reduced vocab for topicLM (usually generated from lattices)
   char      *stopword_file = NULL;
   char      *noise_file = NULL;
   char      *word_mapper_file = NULL;
   char      *lsa_file = NULL;
   char      *rescore_lm_file = NULL;
   char      *sticky_pair_file = NULL;
   int order = 2;
   int num_topic = 30;
   float discount1 = 0.4;
   float discount2 = 0.8;
   NGramLM*  lmP = NULL; // NgramLM obj in janus

   if (itfParseArgv("NgramLSA", &argc, argv, 0,
      "<name>", ARGV_STRING, NULL, &name, cd, "object name",
      "<vocab file>", ARGV_STRING, NULL, &vocab_file,    NULL, "active vocab file (rescoring/decoding)",
      "<lsa vocab file>", ARGV_STRING, NULL, &lsa_vocab_file,    NULL, "LSA vocab file (training)",
      "<LM order>",   ARGV_INT, NULL, &order,    NULL,   "LM order",
      "<num topic>",  ARGV_INT, NULL, &num_topic,    NULL,   "Number of topics",
      "-discount",   ARGV_FLOAT, NULL, &discount1,    NULL,   "Discount constant D",
      "-discount2",  ARGV_FLOAT, NULL, &discount2,    NULL,   "Discount constant D2 for trigram",
      "-stoplist",   ARGV_STRING, NULL, &stopword_file,    NULL,   "list of stopwords",
      "-noise",      ARGV_STRING, NULL, &noise_file,    NULL,   "list of noise words (e.g. [UH} +noise+)",
      "-mapper",     ARGV_STRING, NULL, &word_mapper_file,    NULL,  "word mapper file(e.g. word -> stem)",
      "-lsa",        ARGV_STRING, NULL, &lsa_file,    NULL,   "LSA model file (used for bootstrapping)",
      "-lm",         ARGV_STRING, NULL, &rescore_lm_file,    NULL,   "LM file for rescoring",
      "-sticky",     ARGV_STRING, NULL, &sticky_pair_file, NULL, "file containing sticky-pair Mutual Info => phrase-level VBEM",
      "-ng",         ARGV_OBJECT, NULL, &lmP,         "NGramLM", "LingKS (background NgramLM)",
		   NULL) != TCL_OK) return NULL;

   File vocab_is(vocab_file, "r");
   Vocab* vocab = new Vocab();

   // read vocab file
   vocab->read(vocab_is);

   // use <UNK> (open vocab)
   vocab->unkIsWord() = true;

   // don't apply lowercase
   vocab->toLower() = false;

   // use uppercase <UNK>
   vocab->remove(vocab->unkIndex());
   vocab->unkIndex() = vocab->addWord("<UNK>");

   NgramLSA* ngramlsa = new NgramLSA(vocab, order, num_topic);

   ngramlsa->set_discount1(discount1);
   ngramlsa->set_discount2(discount2);

   if (stopword_file) {
      cout << "loading stoplist " << stopword_file << endl;
      ngramlsa->load_stoplist(stopword_file);
   }

   if (noise_file) {
      cout << "loading noiselist " << noise_file << endl;
      ngramlsa->load_noise_vocab(noise_file);
   }

   if (lsa_file) {
      cout << "loading LSA " << lsa_file << endl;
      ngramlsa->read_lsa_model(lsa_file, lsa_vocab_file);
   }

   if (rescore_lm_file) {
      ngramlsa->delete_adapted_lm();

      Ngram* adapt_lm = new Ngram(*vocab, order);
      File f(rescore_lm_file, "r");

      adapt_lm->read(f, 1);

      cout << "setting rescored LM " << rescore_lm_file << endl;
      ngramlsa->set_adapted_lm(adapt_lm);
   }

   if (lmP) {
      cout << "linking ngramlsa with janus NGramLM => linear interpol will be applied during decoding" << endl;
      ngramlsa->set_ngram_lm(lmP);
   }

   if (sticky_pair_file) {
      // load sticky pair
      ngramlsa->load_sticky_pair(sticky_pair_file);
      cout << "load sticky pair okay" << endl;
   }

   if (word_mapper_file) {
      cout << "loading word mapper " << word_mapper_file << endl;
      ngramlsa->load_word_mapper(word_mapper_file);
   }

   return (ClientData) ngramlsa;
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

// Start Wilson Oct 10 2008
// perform linear interpolation with each item in the cache table
void NgramLSA_Linear_Interpol_Adapt_Cache(void* ngram_lda_obj, lmScore* arr, LVX* janus_context, int context_size)
{
   // don't perform interpolation for NULL context
   if (context_size == 0) return;

   NgramLSA* obj = (NgramLSA*) ngram_lda_obj;
   Ngram* adapted_lm = obj->get_adapted_lm();
   NGramLM* lmP = (NGramLM*) obj->get_ngram_lm();
   const int lsa_order = obj->get_order();

   if (adapted_lm == NULL) {
      cerr << "adapted lm not created for bigram LSA! skip linear interpolation!" << endl;
      return;
   }

   // prepare ngram context from context
   Vocab* vocab_obj = obj->get_vocab();

   // convert context into char*
   int i=0;
   int j = context_size-1;
   static VocabIndex context[10];

   for (i=0; i < lsa_order-1 && j >= 0; i++,j--) {
      const char* word = lmP->list.itemA[janus_context[j]].name;

      VocabString str = word;
      context[i] = vocab_obj->getIndex(str);
   }
   context[i] = Vocab_None;

   // enquire all possible words P(w|context) and save them into arr
   const int N = lmP->list.itemN;
   double lambda_lsa = 1.0 - lambda_bg;
   for (i=0; i < N; i++) {
      const char* word = lmP->list.itemA[i].name;
      VocabString str = word;
      VocabIndex wid = vocab_obj->getIndex(str);

      double prob_lsa = LogPtoProb(adapted_lm->wordProb(wid, context));
      double prob_bg = LogPtoProb(UnCompressScore(arr[i]));
      double combined = lambda_bg * prob_bg + lambda_lsa * prob_lsa;

      // perform 
      arr[i] = CompressScore(ProbToLogP(combined));
   }
}
// End Wilson Oct 10 2008

// Add your methods here
static Method method[] =
{
  { "puts",        PutsItf,        "display object info" },
  { "createAccus", createAccumItf, "create accumulators" },
  { "clearAccus",  clearAccumItf,  "clear accumulators" },
  { "writeAcc",    writeAccumItf,  "write accumulators" },
  { "accu",        accumItf,       "accumulate training text" },
  { "update",      updateItf,      "update model" },
  { "save",        saveItf,        "save a single topic LM" },
  { "save_alpha",  saveAlphaItf,   "save dirichlet-tree prior" },
  { "clear",       clearItf,       "clear topic model" },
  { "load",        loadItf,        "load N-gram topic model" },
  { "rescore",     rescoreLatItf,  "rescore lattices (IBM format)" },
//  { "updateRatio", updateRatioItf, "update ratio factors for marginal adaptation" },
  { "adapt",       adaptItf,       "perform LSA adaptation" },
  { "xadapt",      xadaptItf,      "perform crosslingual LSA adaptation" },
  { "reset",       resetItf,       "reset LSA (Dirichlet-Tree prior)" },
  {  NULL,         NULL,           NULL }
};

static TypeInfo objInfo = { "NgramLSA", 0, -1, method,
                      CreateItf,    FreeItf,
                      ConfigureItf, AccessItf, NULL,
                     "NgramLSA" } ;

// init()
int NgramLSA_Init()
{
   if (Initialized) return TCL_OK;

   // register object
   itfNewType( &objInfo);

   Initialized = 1;
   return TCL_OK;
}
