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
#include "dirichlet.h"

using namespace std;

static Prior* dir_tree = 0;
static double* alpha = 0;
static double** beta = 0;
static int nclass     = CLASS_DEFAULT;
static int nlex       = 1;

void write_model(const char* out_model_file)
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

int main (int argc, char *argv[])
{
   char c;
   bool use_dirtree = true;
   bool use_random_beta = false;
   
   while ((c = getopt(argc, argv, "N:V:d:r:h")) != -1) {
      switch (c) {
      case 'N': nclass = atoi(optarg); break;
      case 'V': nlex   = atoi(optarg); break;
      case 'd': use_dirtree = (atoi(optarg) == 1); break;
      case 'r': use_random_beta = (atoi(optarg) == 1); break;
      case 'h': usage (); break;
      default : usage (); break;
      }
   }

   if ((argc - optind) != 1)
      usage ();
   
   const char* out_model_file = argv[optind];

   srand(time(NULL));

   beta = dmatrix(nlex, nclass);

   // init beta uniformly
   if (use_random_beta) {
      for (int v=0; v < nlex; v++)
      for (int k=0; k < nclass; k++)
         beta[v][k] = RANDOM;

      normalize_matrix_col(beta, beta, nlex, nclass);
   } else {
      for (int v=0; v < nlex; v++)
      for (int k=0; k < nclass; k++)
         beta[v][k] = 1.0 / nlex;
   }
   
   // build dirichlet-tree based given a dirichlet by topic clustering
   if (use_dirtree) {
      dir_tree = new DirichletTree(nclass, 2, 0);
      //dirtree.build(nclass, 2);
/*
   } else if (use_dirtree == 2) {
      dir_tree = new DirichletNet(nclass);
*/
   } else {
      // randomly init alpha
      alpha = new double[nclass];

      for (int k=0; k < nclass; k++)
         alpha[k] = 1.0 + RANDOM;
   }

   // write model
   write_model(out_model_file);

   exit(0);
}

void
usage ()
{
   printf(LDA_COPYRIGHT);
   printf("Build: %s %s\n", __DATE__, __TIME__);
   printf("usage: %s -N <classes> -V <vocab size> -d <0=lda/1=tree> -r <rand beta:0/1> <output model>\n",
      "initmodel");
   exit(0);
}
