#ifndef JANUS_LSA_H
#define JANUS_LSA_H

#ifdef __cplusplus
extern "C" {
#endif
  
extern int LSA_Init();
extern void Marginal_Adapt_Cache(void* lda_obj, lmScore *arr);

#ifdef __cplusplus
}
#endif

#endif
