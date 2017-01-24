/*
newton.c
Newton-Raphson iteration for alpha of LDA
$Id: newton.c,v 1.3 2004/11/04 13:43:43 dmochiha Exp $

*/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "newton.h"
#include "gamma.h"
#include "util.h"

using namespace std;

void
newton_alpha (double *alpha, double **gammas, int M, int K, int level)
{
   int i, j, t;
   double *g, *h, *pg, *palpha;
   double z, sh, hgz;
   double psg, spg, gs;
   double alpha0, palpha0;
   double learn = 1.0;
   
   /* allocate arrays */
   if ((g = new double[K]) == NULL) {
      fprintf(stderr, "newton:: cannot allocate g.\n");
      return;
   }
   if ((h = new double[K]) == NULL) {
      fprintf(stderr, "newton:: cannot allocate h.\n");
      return;
   }
   if ((pg = new double[K]) == NULL) {
      fprintf(stderr, "newton:: cannot allocate pg.\n");
      return;
   }
   if ((palpha = new double[K]) == NULL) {
      fprintf(stderr, "newton:: cannot allocate palpha.\n");
      return;
   }

   /* initialize */
/*
   if (level == 0)
   {
      for (i = 0; i < K; i++) {
         for (j = 0, z = 0; j < M; j++)
            z += gammas[j][i];
         alpha[i] = z / (M * K);
      }
   } else {
      for (i = 0; i < K; i++) {
         for (j = 0, z = 0; j < M; j++)
            z += gammas[j][i];
         alpha[i] = z / (M * K * pow(10, (double)level));
      }
   }
*/
   
   // "i" is the document index, M is the total document
   // "gs" means sum_k gamma_k for each document i
   psg = 0;
   for (i = 0; i < M; i++) {
      for (j = 0, gs = 0; j < K; j++)
         gs += gammas[i][j];
      psg += psi(gs);
	//printf("%d: psi(%f)=%f\n", i, gs, psi(gs));
   }
   for (i = 0; i < K; i++) {
      for (j = 0, spg = 0; j < M; j++)
         spg += psi(gammas[j][i]);
      pg[i] = spg - psg;

      //cout << "pg " << i << " " << pg[i] << endl;
   }
   
   /* main iteration */
   for (t = 0; t < MAX_NEWTON_ITERATION; t++)
   {
      for (i = 0, alpha0 = 0; i < K; i++)
         alpha0 += alpha[i];
      palpha0 = psi(alpha0);
      
      // compute gradient[k] over alpha_k
      for (i = 0; i < K; i++)
         g[i] = M * (palpha0 - psi(alpha[i])) + pg[i];

      for (i = 0; i < K; i++)
         h[i] = - 1 / ppsi(alpha[i]);

      for (i = 0, sh = 0; i < K; i++)
         sh += h[i];
      
      for (i = 0, hgz = 0; i < K; i++)
         hgz += g[i] * h[i];
      hgz /= (1 / ppsi(alpha0) + sh);

      // determine the learn rate by enforcing constraint that alpha > 0.0
      bool reduce = false;
      for (i = 0; i < K; i++) {
         double learn_i = alpha[i] / (h[i] * (g[i] - hgz) / M);

         printf("%d %f\n", i, learn_i);

         if (learn_i > 0 && learn > learn_i) {
            learn = learn_i;
            reduce = true;
         }

      }
      //printf("\n");

      if (reduce)
         learn *= 0.5;

      printf("\n%f\n", learn);

      for (i = 0; i < K; i++) {
         printf("t%d: %f->", t, alpha[i]);
         alpha[i] = alpha[i] - learn * (h[i] * (g[i] - hgz) / M);
         printf("%f ", alpha[i]);
      }
      
/*
      for (i = 0; i < K; i++)
         if (alpha[i] < 0) {
            if (level >= MAX_RECURSION_LIMIT) {
               fprintf(stderr, "newton:: maximum recursion limit reached.\n");
               exit(1);
            } else {
               delete [] g;
               delete [] h;
               delete [] pg;
               delete [] palpha;
               return newton_alpha(alpha, gammas, M, K, 1 + level);
            }
         }
*/
         
      if ((t > 0) && converged(alpha, palpha, K, 1.0e-4)) {
         delete [] g;
         delete [] h;
         delete [] pg;
         delete [] palpha;
         return;
      } else
         for (i = 0; i < K; i++)
            palpha[i] = alpha[i];
   }
   fprintf(stderr, "newton:: maximum iteration reached. t = %d\n", t);
   
   return;
   
}
