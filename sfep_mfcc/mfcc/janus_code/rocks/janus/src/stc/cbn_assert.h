/* -------------------------------------------------------------------
   CBNew (new Codebook class)
   Matthias Seeger
   -------------------------------------------------------------------
   assertions module
   -------------------------------------------------------------------
   C header file
   Last modified: Jan. 26, 1998
   ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cbn_assert__
#define __cbn_assert__

#include "cbn_global.h"

#ifdef DEBUG

#include <assert.h>

struct CBNew;
struct CBNewSet;
struct CBNewParMatrix;
struct CBNewParMatrixSet;

/* --- Exported macros --- */

#define MYASS(COND) { if (!(COND)) { printf("MYASS %s (line %d):\n",file,line); assert (COND); } }

#define ASSERT_PARMAT(PMAT,LF) assert_parmat(PMAT,LF,__FILE__,__LINE__)
#define ASSERT_PMSET(PMS,LF) assert_pmset(PMS,LF,__FILE__,__LINE__)
#define ASSERT_PROT(CB) assert_prot(CB,__FILE__,__LINE__)
#define ASSERT_LINK(PMAT,CB,K) assert_link(PMAT,CB,K,__FILE__,__LINE__)
#define ASSERT_ACCU_WORK(CB,I) assert_accu_work(CB,I,__FILE__,__LINE__)
#define ASSERT_MAIN(CB,LF) assert_main(CB,LF,__FILE__,__LINE__)
#define ASSERT_CB(CB,LF) assert_cb(CB,LF,__FILE__,__LINE__)
#define ASSERT_CBSET(CBS,LF) assert_cbset(CBS,LF,__FILE__,__LINE__)

/* Checks if everything is OK with matrix MAT */
#define ASSERT_MATRIX(MAT,M,N)\
{\
  assert (MAT);\
  assert ((MAT)->m == (M) && (MAT)->n == (N));\
}


/* --- Interface --- */

void assert_parmat(struct CBNewParMatrix* pmat,int linkflag,char* file,
		   int line);
void assert_pmset(struct CBNewParMatrixSet* pms,int linkflag,char* file,
		  int line);
void assert_prot(struct CBNew* cb,char* file,int line);
void assert_link(struct CBNewParMatrix* pmat,struct CBNew* cb,int k,char* file,
		 int line);
void assert_accu_work(struct CBNew* cb,int index,char* file,int line);
void assert_main(struct CBNew* cb,int linkflag,char* file,int line);
void assert_cb(struct CBNew* cb,int linkflag,char* file,int line);
void assert_cbset(struct CBNewSet* cbs,int linkflag,char* file,int line);

#endif

#endif











#ifdef __cplusplus
}
#endif
