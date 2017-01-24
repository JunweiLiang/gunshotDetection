#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include "split_string.h"
#include "lsa.h"
#include "reader.h"
#include "writer.h"
#include "lsafeature.h"
#include "dmatrix.h"
#include "util.h"
#include "main.h"

using namespace std;

static Prior* dir_tree = 0;

int main (int argc, char *argv[])
{
   char c;
   const char* input_model_file = 0;
   int dlenmax    = DLENMAX_DEFAULT;	// default in lda.h
   int nclass     = CLASS_DEFAULT;	// default in lda.h
   int demmax     = DEMMAX_DEFAULT;	// default in lda.h
   int N = 10;
   double forget = 0.4;
   
   while ((c = getopt(argc, argv, "N:D:L:i:m:f:h")) != -1) {
      switch (c) {
      case 'N': nclass  = atoi(optarg); break;
      case 'D': demmax  = atoi(optarg); break;
      case 'L': dlenmax = atoi(optarg); break;
      case 'i': input_model_file = optarg; break;
      case 'm': N = atoi(optarg); break;
      case 'f': forget = atof(optarg); break;
      case 'h': usage (); break;
      default : usage (); break;
      }
   }
   if ((argc - optind) != 1)
      usage ();
   
   const char* vocab_file = argv[optind];
   
   // create LDA obj
   LSA lda(input_model_file, vocab_file, nclass, nclass, dlenmax, demmax, dir_tree);

   string line;
   vector<string> tk;
   double* prob = new double[lda.get_num_vocab()];

   // read from stdin
   cerr << "ready for input>" << endl;
   while (1) {
      get_clean_line(cin, line);

      // split the line
      tk.clear();
      split(line, tk);

      //const document* dp = lda.multinomial(tk);

      // infer current sent
      lda.adapt_sent(tk, 0, 0, -1, forget);

      // print out the top-N words
      for (int v=0; v < lda.get_num_vocab(); v++)
         prob[v] = lda.MAP_prob(v);

      // sort table
      PROB_TABLE table_out = sort_table(prob, lda.get_num_vocab());

      for (int v=0; v < N; v++) {
         cout << "Rank " << v << " " << lda.get_vocab(table_out[v].first) << " " << table_out[v].second << endl;
      }
      cout << "DONE!" << endl;

      const double* gamma = lda.get_gamma();

/*
      // copy MAP prob on the current block of words
      copy_array(gamma, post, nclass);
      normalize(post, nclass);
*/

      // compute argmax on the topic and write the sent to the k-th output file stream
      const double* max_ptr = max_element(gamma, gamma + nclass);
      const int max_k = max_ptr - gamma;
   }

   exit(0);
}

void
usage ()
{
   printf(LDA_COPYRIGHT);
   printf("Build: %s %s\n", __DATE__, __TIME__);
   printf("usage: %s -m <top N> -N <classes> -i <input model> -f <forget factor> vocab\n",
      "demo");
   exit(0);
}
