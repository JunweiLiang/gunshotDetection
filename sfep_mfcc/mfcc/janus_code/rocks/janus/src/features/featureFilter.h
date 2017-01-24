/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Filter Type & Operators
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureFilter.h
    Date    :  $Id: featureFilter.h 2744 2007-02-08 09:50:30Z stueker $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

    This software is part of the JANUS Speech- to Speech Translation Project

    USAGE PERMITTED ONLY FOR MEMBERS OF THE JANUS PROJECT
    AT CARNEGIE MELLON UNIVERSITY OR AT UNIVERSITAET KARLSRUHE
    AND FOR THIRD PARTIES ONLY UNDER SEPARATE WRITTEN PERMISSION
    BY THE JANUS PROJECT

    It may be copied  only  to members of the JANUS project
    in accordance with the explicit permission to do so
    and  with the  inclusion  of  the  copyright  notices.

    This software  or  any  other duplicates thereof may
    not be copied or otherwise made available to any other person.

    No title to and ownership of the software is hereby transferred.

   ========================================================================

    $Log$
    Revision 3.5  2007/02/08 09:50:30  stueker
    Added additional code from Michael Wand

    Revision 3.4  2004/09/10 15:40:03  metze
    Added pointer to explanations of FIR filters

    Revision 3.3  2000/08/27 15:31:14  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _filter
#define _filter
	
#include "common.h"   /* includes error.h */
#include "itf.h"

/* ------------------------------------------------------------------------
    Filter
   --------
    Get coefficients from 

      http://www.dsptutor.freeuk.com/FIRFilterDesign/FIRFiltDes102.html
      http://www.isip.msstate.edu/projects/speech/software/demonstrations/applets/util/filter_design/current/index.html

    or somewhere else on the net...
   ------------------------------------------------------------------------ */
/* ===============================================================MM Jun.94=
   FIR-Filter:                                                           
             F(z)=A[0] + A[1]*z^-1 + A[2]*z^-2 ...                       
     with an offset of d                                                 
             F(z)=A[0]*z^-d + A[1]*z^(-d-1) + A[2]*z^(-d-2) ...          
                                                                        
   IIR-Filter:                                                           
                    A[0] + A[1]*z^-1 + A[2]*z^-2 ... + A[n]*z^-n         
             F(z)= ----------------------------------------------        
                      1  + B[1]*z^-1 + B[2]*z^-2 ... + B[m]*z^-m         
   filter definition:
   
     "offset {A[0] A[1] ... A[n]} [{B[1] B[2] ... B[m])}  ...]"
                                                                        
   Examples (in files):                                                             
   2                        0                   -2                                         
   1.0                      1.0                 {1.0 0 0 0 -1.0}
   two sample delay         -1.0                symmetric delta filter
                            integral

   -1
   {0.33 0.33 0.33}
   smoothing filter

  ========================================================================= */
typedef struct
{ 
   int           offset;        /* offset                       */
   int  	 n;		/* #of coefficients in a        */
   int    	 m;		/* #of coefficients in b        */
   double        *a;            /* coefficients of a            */
   double        *b;            /* coefficients of b            */
} Filter;

extern Filter*  filterCreate(   int m, int n);
extern void     filterFree(     Filter* f);
extern int      getFilter( ClientData cd, char* key, ClientData result, int *argcP, char *argv[]);

extern Filter*    filterGetItf(  char* value);
extern ClientData filterCreateItf ( ClientData cd, int argc, char *argv[]);
extern int        filterPutsItf( ClientData cd, int argc, char *argv[]);
extern int        Filter_Init ( void);

#endif /* _filter */

#ifdef __cplusplus
}
#endif
