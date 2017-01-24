#ifndef UTIL_NEW_H
#define UTIL_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"

#define PI           3.14159265358979
#define TWO_PI       6.28318530717959

#define LZERO  (-1.0E10)                          /* ~log(0) */
#define LSMALL (-0.5E10)                          /* log values < LSMALL are set to LZERO */


#ifndef ASSERT
   #define ASSERT(X) assert(X)
#endif

// Typedefs
typedef unsigned long   ULong;
typedef unsigned short  UShort;

float LogAdd(float lx, float ly);
double addLogProbs2 (double p1,double p2);
void swapShort(char *val);
void swapLong(char *val);

//memory allocation/deallocation routines
template <class T>
inline T*& New1DArray(T*& a, const ULong& d, T val=T()) {
   long i;

   if (d > 0) {
      //a = new T[d];
      a = (T*) malloc(d*sizeof(T));
      for (i=0;i<(long)d;i++) a[i] = val;
   }

   return a;
}


template <class T>
inline T*& Delete1DArray(T*& a,const ULong& d) {
   if (!a) return a;

   //if (d > 0) delete[] a;
   if (a && d > 0) free(a);

   a = 0;

   return a;
}


template <class T>
inline void copy1DArray(T*& a, const T* b, const ULong& d) {

   for (long i=0; i<d; i++) {
      a[i] = b[i];
   }
}


// memory problem
template <class T>
T**& New2DArray(T**& a, const long& d1, const long& d2, T val = T()) {
   long i,j;

   if (d1 > 0) {
      //a = new T*[d1];
      a = (T**) malloc(d1*sizeof(T*));

      if (d2 > 0) {
         for (i=0;i<d1;i++) {
            //a[i] = new T[d2];
            a[i] = (T*) malloc(d2*sizeof(T));
            for (j=0;j<d2;j++) a[i][j] = val;
         }
      }
   }

   return a;
}

template <class T>
void Delete_1D_Array(T*& a, const long* d, const long* offset)
{
   long offset0 = offset[0];

   delete [] (a-offset0);
}

template <class T>
void Delete_2D_Array(T**& a, const long* d, const long* offset)
{
   long d0 = d[0];
   long offset0 = offset[0];

   for (long i=-offset0;i < d0-offset0;i++) 
      Delete_1D_Array(a[i], d+1, offset+1);

   delete [] (a-offset0);
}

template <class T>
void Delete_3D_Array(T***& a, const long* d, const long* offset)
{
   long d0 = d[0];
   long offset0 = offset[0];

   for (long i=-offset0;i < d0-offset0;i++) 
      Delete_2D_Array(a[i], d+1, offset+1);

   delete [] (a-offset0);
}

template <class T>
void Delete_4D_Array(T****& a, const long* d, const long* offset)
{
   long d0 = d[0];
   long offset0 = offset[0];

   for (long i=-offset0;i < d0-offset0;i++) 
      Delete_3D_Array(a[i], d+1, offset+1);

   delete [] (a-offset0);
}

template <class T>
void Delete_5D_Array(T*****& a, const long* d, const long* offset)
{
   long d0 = d[0];
   long offset0 = offset[0];

   for (long i=-offset0;i < d0-offset0;i++) 
      Delete_4D_Array(a[i], d+1, offset+1);

   delete [] (a-offset0);
}

template <class T>
T** New_2D_Array(long* d, long* offset, T val = T()) 
{
   long d1 = d[0];
   long d2 = d[1];
   long offset1 = offset[0];
   long offset2 = offset[1];

   T** a = new T*[d1];

   for (long i=0;i<d1;i++) {
      a[i] = new T[d2];
      for (long j=0;j<d2;j++) {
         a[i][j] = val;
      }
      a[i] += offset2;
   }

   a += offset1;

   return a;
}

template <class T>
T*** New_3D_Array(long* d, long* offset, T val = T()) 
{
   long d1 = d[0];
   long offset1 = offset[0];

   T*** a = new T**[d1];

   for (long i=0;i<d1;i++) {
      a[i] = New_2D_Array(d+1, offset+1, val);
   }

   a += offset1;

   return a;
}

template <class T>
T**** New_4D_Array(long* d, long* offset, T val = T()) 
{
   long d1 = d[0];
   long offset1 = offset[0];

   T**** a = new T***[d1];

   for (long i=0;i<d1;i++) {
      a[i] = New_3D_Array(d+1, offset+1, val);
   }

   a += offset1;

   return a;
}

template <class T>
T***** New_5D_Array(long* d, long* offset, T val = T()) 
{
   long d1 = d[0];
   long offset1 = offset[0];

   T***** a = new T****[d1];

   for (long i=0;i<d1;i++) {
      a[i] = New_4D_Array(d+1, offset+1, val);
   }

   a += offset1;

   return a;
}


#endif
