#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>
#include "lsa.h"
#include "reader.h"
#include "writer.h"
#include "lsafeature.h"
#include "dmatrix.h"
#include "util.h"
#include "main.h"
#include "split_string.h"
#include "dirichlet.h"

using namespace std;

enum {TRAIN=0, INFER, COUNT, ADAPT, PRINT, PRIOR, BOOTSTRAP, ADDVOCAB, MERGE};

static Prior* dir_tree = 0;

extern bool prior_given;

static int nbestWord(LSA* lda, int k);
static map<string, bool> skip_vocab;

int main (int argc, char *argv[])
{
   document *data;
   char* new_vocab_file = 0;
   char* transform_file = 0;
   char* unigram_lm_file = 0;
   char* input_model_file = 0;
   char* skip_vocab_file = 0;
   char* prior_file = 0;
   char c;
   int max_vid = 0;
   double num_word = 0.0;
   int dlenmax    = DLENMAX_DEFAULT;	// default in lda.h
   int nclass     = CLASS_DEFAULT;	// default in lda.h
   int max_class  = 0;	                // default in lda.h
   int emmax      = EMMAX_DEFAULT;	// default in lda.h
   int demmax     = DEMMAX_DEFAULT;	// default in lda.h
   double epsilon = EPSILON_DEFAULT;	// default in lda.h
   double learn   = 0.5;	        // default in lda.h
   int mode = TRAIN;
   bool use_kl_wgt = false;
   bool save_mem = false;
   int from = 0;
   int len = -1;
   int window = 20;
   double forget = 0.4;
   int use_dirtree = 0;
   int branch = 2;
   int num_merge_topic = nclass;
   
   while ((c = getopt(argc, argv, "A:N:X:I:D:E:M:L:B:W:S:T:b:i:k:f:l:s:w:d:u:t:p:r:g:b:z:h")) != -1)
   {
      switch (c) {
      // add new vocab into existing vocab
      case 'A': new_vocab_file = optarg; break;
      case 'N': nclass  = atoi(optarg); break;
      case 'X': max_class = atoi(optarg); break;
      case 'I': emmax   = atoi(optarg); break;
      case 'D': demmax  = atoi(optarg); break;
      case 'E': epsilon = atof(optarg); break;
      case 'M': mode = atoi(optarg); break;
      case 'L': dlenmax = atoi(optarg); break;
      case 'B': branch = atoi(optarg); break;
      case 'S': skip_vocab_file = optarg; break;
      case 'T': transform_file = optarg; break;
      case 'i': input_model_file = optarg; break;
      case 'k': use_kl_wgt = (atoi(optarg) == 1); break;
      case 'f': from = atoi(optarg); break;
      case 'l': len  = atoi(optarg); break;
      case 's': save_mem = (atoi(optarg) == 1); break;
      case 'w': window = atoi(optarg); break;
      case 'd': forget = atof(optarg); break;
      case 'u': unigram_lm_file = optarg; break;
      case 't': use_dirtree = atoi(optarg); break;
      case 'r': learn = atof(optarg); break;
      case 'g': prior_file = optarg; break;
      case 'z': num_merge_topic = atoi(optarg); break;
      case 'h': usage (); break;
      default : usage (); break;
      }
   }
   if ((mode == TRAIN || mode == BOOTSTRAP) && !(argc - optind == 3) || 
       mode == INFER && !(argc - optind == 2))
      usage ();

   const char* vocab_file = argv[optind];
   const char* data_file  = argv[optind+1];

   if (use_dirtree == 1) {
      //dir_tree = new DirichletTree(nclass, 2, lda.get_alpha());
      if (nclass == branch) {
         // degenerate to standard LDA
         dir_tree = new DirichletTree();
         ((DirichletTree*) dir_tree)->build(nclass, branch);
      } else {
         // build full-binary tree
         dir_tree = new DirichletTree(nclass);
      }
   } else if (use_dirtree == 2) {
      dir_tree = new DirichletNet(nclass);
   }
   
   // create LDA obj
   LSA lda(input_model_file, vocab_file, nclass, max_class, dlenmax, demmax, dir_tree);

   // set kl weighting flag
   lda.set_kl_wgt(use_kl_wgt);

   // set save mem flag
   lda.set_save_mem(save_mem);

   if (mode == TRAIN) {
      const char* out_model_file = argv[optind+2];

      lda.learn(data_file, emmax, epsilon, from, len, learn);
      lda.write_model(out_model_file);
   } else if (mode == MERGE) {
      cout << "merge topics from " << nclass << " to " << num_merge_topic << endl;

      const char* out_model_file = argv[optind+2];

      // perform 1-EM iter to get the accumulators
      lda.learn(data_file, emmax=1, epsilon, from, len, learn);

      // perform node merging
      int diff = nclass - num_merge_topic;
      int final_num_class = nclass;
      for (int i=0; i < diff; i++) {
         final_num_class = lda.merge_topic();
      }

      assert(final_num_class == lda.get_num_class());

      lda.write_model(out_model_file);
   } else if (mode == BOOTSTRAP) {
      const char* out_model_file = argv[optind+2];

      lda.learn_bootstrap(data_file, emmax, epsilon, from, len, learn, prior_file);
      lda.write_model(out_model_file);
   } else if (mode == COUNT) {
      // deprecated
      cerr << "not implemented" << endl;
   } else if (mode == INFER || mode == ADAPT || mode == PRIOR) {
      double total_map_ll = 0.0;
      double total_var_ll = 0.0;
      int num_doc = 0;
      
      /* open data */
      if ((data = feature_matrix(data_file, &max_vid, &dlenmax, &num_doc, &num_word)) == NULL) {
         fprintf(stderr, "lda:: cannot open training data.\n");
         exit(1);
      }

      ofstream* ofs_prior = 0;

// change to binary mode to save disk space
      if (prior_file && (mode == PRIOR || mode == INFER))
#ifdef BINARY
         ofs_prior = new ofstream(prior_file, ios::out | ios::binary);
#else
         ofs_prior = new ofstream(prior_file);
#endif
      
      for (const document* dp = data; (dp->len) != -1; dp++) {

         if (mode == INFER) {
            // compute MAP log likelihood
            const double map_ll = lda.infer(dp);
            //const double var_ll = lda.var_ll(dp);

            //printf("map ll = %f, var ll = %f\n", map_ll, var_ll);

            total_map_ll += map_ll;
            //total_var_ll += var_ll;

            const double* cur_prior = lda.get_gamma();

// change to binary mode to save disk space
#ifdef BINARY
            ofs_prior->write((char*)cur_prior, sizeof(double)*nclass);
#else
            for (int k=0; k < nclass; k++) {
               if (k == nclass-1)
                  *ofs_prior << cur_prior[k];
               else
                  *ofs_prior << cur_prior[k] << " ";
            }
            *ofs_prior << endl;
#endif

         } else if (mode == PRIOR) {
            const double map_ll = lda.infer(dp);

            total_map_ll += map_ll;

            const double* cur_prior = lda.get_prior();

#ifdef BINARY
            ofs_prior->write((char*)cur_prior, sizeof(double)*nclass);
#else
            for (int k=0; k < nclass; k++) {
               if (k == nclass-1)
                  *ofs_prior << cur_prior[k];
               else
                  *ofs_prior << cur_prior[k] << " ";
            }

            *ofs_prior << endl;
#endif

         } else {
            cout << "adapt sent of length " << dp->len << endl;
            lda.adapt_sent(dp, forget);

            cout << "updating adapted weight" << endl;
            const double* src_gamma = lda.get_gamma();
            double* src_wgt = lda.get_MAP_wgt();
            copy_array(src_gamma, src_wgt, nclass);
            normalize(src_wgt, nclass);
         }
      }

      if (ofs_prior) {
         ofs_prior->close();
         delete ofs_prior;
      }

      const double map_ppl = exp(-total_map_ll / num_word);
      const double var_ppl = exp(-total_var_ll / num_word);

      printf("total map LL = %f, total var ll = %f\n", total_map_ll, total_var_ll);
      printf("total map PPL = %f, total var PPL = %f\n", map_ppl, var_ppl);
      
      free_feature_matrix(data);
   } else if (mode == PRINT) {
      if (transform_file)
         lda.transform_beta(transform_file);

      // load skip_vocab_file 
      if (skip_vocab_file) {
         ifstream is(skip_vocab_file);

         if (is.fail()) {
            cerr << "can't read " << skip_vocab_file << endl;
            exit(-1);
         }

         string word;
         while (is >> word) {
            skip_vocab[word] = true;
         }

         is.close();
      }

      // print topic if we need
      for (int k=0; k < nclass; k++)
         nbestWord(&lda, k);
   } else if (mode == ADDVOCAB) {
      const char* out_model_file = argv[optind+2];

      lda.add_vocab(new_vocab_file);
      lda.write_model(out_model_file);
   }

   // write out unigram
   if (unigram_lm_file)
      lda.write_arpa(unigram_lm_file);

   exit(0);
}

static int nbestWord(LSA* lda, int k)
{
// only output top 200
   //int nbest = lda->get_num_vocab();
   int nbest = 500;
   //int prob = 0;
   float cumu = 0.8;
   //float cumu = 1.0;
   int n;

/*
   // get arguments
   if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<class idx>", ARGV_INT, NULL, &k, NULL,         "class idx",
      "-N",          ARGV_INT, NULL, &nbest, NULL,     "top N words",
      "-prob",       ARGV_INT, NULL, &prob, NULL,      "output prob?(0/1)",
      "-cumu",       ARGV_FLOAT, NULL, &cumu, NULL,    "cumulative prob stop threshold",
       NULL) != TCL_OK) return TCL_ERROR;
*/

   const vector<pair<int,double> >& result = lda->get_nbest_word(k);
   double acc_prob = 0.0;
   int found = 0;

   printf("%d: ", k);
   for (n=0; n < lda->get_num_vocab();n++) {
      const pair<int,double>& item = result[n];
      const string& w = lda->get_vocab(item.first);

      if (skip_vocab.find(w) != skip_vocab.end()) {
         continue;
      }

      const double  p = item.second;

      acc_prob += p;

      //printf("%d: %s(%d)\n", k, w.c_str(), found);
      //printf("%s(%f) ", w.c_str(), log(p));
      printf("%s ", w.c_str());
/*
      // prepare result
      if (prob)
         itfAppendResult("{{%s} {%2.4e} {%2.4e}} ", w.c_str(), p, acc_prob);
      else
         itfAppendResult("%s ", w.c_str());
*/
      found++;

      if (acc_prob > (double)cumu) break;
      if (found > nbest) break;
   }
   printf("%d\n", found);

   return found;
}

void
usage ()
{
   printf(LDA_COPYRIGHT);
   printf("Build: %s %s\n", __DATE__, __TIME__);
   printf("usage: %s -T <transform matrix> -N classes [-X max_classes -I emmax -D demmax -E epsilon -M <0=train/1=infer/2=none/3=adapt/4=print/5=dump VB prior/6=Bootstrap with per-document prior/7=add_new_vocab/8=merge topic> -k <0=use_kl_wgt/1> -i <input model> -s <0/1=save_memory>] [-w <window> -d <forget factor> only for option=2] [-u <arpa lm>] [-t <0/1=dirtree/2=dirnet>] [-B <branch: for dirtree only>] [-p <print topic:0/1>] [-g <input/output prior file>] [-A new_vocab_file] [-S skip_vocab_file] [-p <stopword list> -b <bg ARPA unigram LM>] -z <num merged topics> vocab train [out_model]\n",
      "lda");
   exit(0);
}
