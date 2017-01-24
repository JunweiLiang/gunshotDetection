#ifndef JANUS_NGRAMLSA_H
#define JANUS_NGRAMLSA_H

#include "slimits.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int NgramLSA_Init();
extern void NgramLSA_Linear_Interpol_Adapt_Cache(void* ngram_lda_obj, lmScore* arr, LVX* context, int context_size);
#ifdef __cplusplus
}
#endif

#endif
