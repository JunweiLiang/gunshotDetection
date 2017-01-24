/*----------------------------------------------------------------------*/
/*	featureDownsample.h						*/
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

extern short int ds_filter1(short int x, int mode);
extern short int ds_filter2(short int x, int mode);
extern short int ds_compand(short int x);

#ifdef __cplusplus
}
#endif
