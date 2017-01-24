/******************************************************************************/
/* KEYSPOTTER DECLARATION                                                     */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _keySpot
#define _keySpot




/***************/
/* DEFINITIONS */
/***************/
#define MAX_FLOAT 1E20
#define MAX_PH 1000
#define DYN_MEM_BLOCK_SIZE 256

  
/************/
/* INCLUDES */
/************/
#include <float.h>
#include <memory.h>
#include "senones.h"




  
/******************************************************************************/
/* STRUCTURE KEYWORD                                                          */
/******************************************************************************/
  typedef struct
{
    int*      senoneIndexA;
    int       senonesN;

    float     scoreThresh;
    float     entropyThresh;

    float*    dC[2];
  	float*    tC[2];
  	float*    eC[2];
    float*    N;
} Keyword;



/******************************************************************************/
/* STRUCTURE KEYSPOTTER                                                       */
/******************************************************************************/
typedef struct
{
    SenoneSet   *senoneSet;                 /* senone set used by the keyspotter */

    Keyword**   keywordA;                   /* array of keywords */
    int         keywordsN;                  /* number of keywords */

    int*        senoneIndecesA;             /* array of senone indices (occuring in keywords) */
    int         senoneIndecesN;             /* number of senone indices */
    int         senoneIndecesASize;         /* (dynamic programming) size of the senone indice array (in bytes) */

    float       *scoreA;                    /* score array (scores of the senones) */

    int         counter;
    int         prevBeginPoint;
    int         beginPoint;

    int         CURR;
    int         LAST;

} Keyspotter;





/******************************************************************************/
/* FUNCTION DECLRARATIONS                                                     */
/******************************************************************************/
int Keyspot_Init (void);









#endif  /* _keySpot */



#ifdef __cplusplus
}
#endif
