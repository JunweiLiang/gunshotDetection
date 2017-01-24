#ifndef _gs
#define _gs

#include "mach_ind_io.h"

typedef struct GSClusterSet_S GSClusterSet;
typedef struct GSCluster_S GSCluster;

#include "codebook.h"

struct GSCluster_S {
  int    active;	/* active clusters array (0= not active, -1 = active) */
  float *mean;		/* mean of cluster representant (float vector of dimN) */
  float *covar;		/* maybe one day we can compute covars as well (float vector of dimN) */
  int   *meanC;		/* compressed mean of cluster representant (float vector of dimN) */
  int   *covarC;	/* compressed maybe one day we can compute covars as well (float vector of dimN) */
  int    count;		/* how many gaussians belong to this cluster? */
  int    active_count;	/* how mamy times has this cluster been active? */
};

struct GSClusterSet_S {
  char        *name;		/* name of the GSClusterSet */

  GSCluster   *cluster;		/* pointer to cluster array */
  int          clusterN;	/* number of clusters */

  int          bestN;		/* activate the n-best clusters during decoding */
  int          dimN;		/* dimensions of the feature space */
  int          use;		/* use clusterset during scoring (-1) or not (0) */
  int          mode;		/* 0 = pure euclidean
				   1 = mahalanobis cluster covar
				   2 = mahalanobis codebook average covar
				   3 = mahalanobis gaussian-per-cluster average covar */
  int          gaussianN;	/* how many gaussians in total are used for ClusterSet */
  float        decrease;	/* [0...1] dimensionwise decrease of weight for distance messure (0=no decrease, 1=32th dimension has weight 0) */
  int          backoffMode;	/* 0 = auto, 1 = fix */
  float        backoffValue;	/* backoff value if gaussian is not active */
  unsigned int backoffValueI;	/* backoff value if gaussian is not active, integer */
  float        backoffDefault;	/* default value for backoff value */
  float        backoffFactor;	/* factor for auto backoff */
  int         *weightC;		/* weight vector in compressed space */
  CodebookSet *cbsP;
  float        diff;		/* squared average error gaussians<->cluster mean */
  int          gaussians_computed;	/* how many gaussians were evaluated? */
  int          gaussians_total;	/* how many have been processed in total? */
  int          eval_frames;	/* how many feature frames were evaluated? */
  short        **clusterX;	/* gaussian to cluster assign for GS ClusterSet   */
};

extern int GSClusterSet_Init (void);
extern int gsClusterSetActivate (GSClusterSet *gscsP, float *feat, int *featI, int bestN);


#endif
