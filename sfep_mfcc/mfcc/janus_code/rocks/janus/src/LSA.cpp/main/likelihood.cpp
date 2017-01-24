/*
likelihood.c
$Id: likelihood.c,v 1.1 2004/11/05 08:28:15 dmochiha Exp $

*/
#include <cstdlib>
#include <cmath>
#include "likelihood.h"
#include "lsafeature.h"
#include "dmatrix.h"
#include "util.h"
#include "dirichlet.h"

double lda_MAP_LL (document *data, double **beta, double **gammas, int m, int nclass)
{
   double **egammas; // MAP estimates of the topic mixture weights for each document m
   double z, lik = 0.0;
   document *dp;
   int i, j, k;
   int n;
   
   if ((egammas = dmatrix(m, nclass)) == NULL) {
      fprintf(stderr, "lda_likelihood:: cannot allocate egammas.\n");
      exit(1);
   }
   normalize_matrix_row(egammas, gammas, m, nclass);
   
   for (dp = data, i = 0; (dp->len) != -1; dp++, i++)
   {
      n = dp->len;
      for (j = 0; j < n; j++) {
         for (k = 0, z = 0; k < nclass; k++)
            z += beta[dp->id[j]][k] * egammas[i][k];
         
         lik += dp->cnt[j] * log(z);
      }
   }
   
   free_dmatrix(egammas, m);
   return lik;
}

double lda_MAP_LL (const document *dp, const double* const* beta, const double* gamma, int nclass)
{
   double *egamma = new double[nclass];
   double z, lik = 0.0;
   int j, k;
   int n;
   
   copy_array(gamma, egamma, nclass);
   normalize(egamma, nclass);

   n = dp->len;
   for (j = 0; j < n; j++) {
      for (k = 0, z = 0; k < nclass; k++)
         z += beta[dp->id[j]][k] * egamma[k];
      
      lik += dp->cnt[j] * log(z);
   }
   
   delete [] egamma;
   
   return lik;
}

// implement Blei's formula
// ap[k] = E_q[log theta_k]
double lda_VAR_LL(const document* dp, const double* const* beta, const double* alpha, const double* gamma, const double* ap, const double* const* q, int nclass)
{
   Dirichlet f_theta(alpha, nclass);
   Dirichlet q_theta(gamma, nclass);
   const double term_theta = f_theta.log_prob(ap, true) - q_theta.log_prob(ap, true);
   
   double term_z = 0.0;
   double term_beta = 0.0;
   
   for (int w = 0; w < dp->len; w++) {
      const int v = dp->id[w];
      const double v_count = dp->cnt[w];
      const double* beta_w = beta[v];
      
      // var posterior of each word w, topic j=0
      const double* q_w = q[w];
      
      for (int k = 0; k < nclass; k++) {
         const double q_wk = q_w[k];
         
         // E_q[log f(z|theta) / q(z)]
// yct july 31: bug fix
         //term_z += v_count * q_wk * (log(ap[k]) - log(q_wk));
         term_z += v_count * q_wk * (ap[k] - log(q_wk));
         
         // term 5: E_q[log f(v|z)]
         term_beta += v_count * q_wk * log(beta_w[k]);
      } // end of each class k 
   }

   //printf("var: %f %f %f\n", term_theta, term_z, term_beta);

/*
   printf("term beta = %f\n", term_beta);
   printf("term z = %f\n", term_z);
   printf("term theta = %f\n", term_theta);
*/
   
   // return var. log likelihood
   return (term_theta + term_z + term_beta);
}
