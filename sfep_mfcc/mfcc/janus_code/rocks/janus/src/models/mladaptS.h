/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Maximum Likelihood Signal Adaptation
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  mladapt.h
    Date    :  $Id: mladaptS.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

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
    Revision 1.8  2003/08/14 11:20:16  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.7.50.1.2.4  2002/04/29 09:40:46  soltau
    beautified

    Revision 1.7.50.1.2.3  2001/11/20 11:03:30  metze
    Made number of aX and wX an option at creation time

    Revision 1.7.50.1.2.2  2001/11/06 15:45:28  metze
    Changes for streams

    Revision 1.7.50.1.2.1  2001/10/22 09:39:32  soltau
    reduced nr. of accus and transformations

    Revision 1.7.50.1  2001/01/19 12:34:47  soltau
    Replaced by my version 1.6

    Revision 1.5  2000/03/22 12:48:27  soltau
    Changed interface

    Revision 1.4  2000/03/15 15:21:59  soltau
    Added stuff to store/restore accu's

    Revision 1.3  1998/09/30 16:58:39  soltau
    *** empty log message ***

    Revision 1.2  1998/09/28 11:26:13  soltau
    made it working

    Revision 1.1  1998/09/25 10:25:41  soltau
    Initial revision

 
   ======================================================================== */

#ifndef _mladaptS
#define _mladaptS

#include "codebook.h"
#include "distrib.h"
#include "senones.h"


/* ========================================================================
    SignalAdapt
   ======================================================================== */

typedef struct {
  int   refX;
  float gamma;
} SortElement;


typedef struct {
  char* name;
  int   useN;

  FeatureSet*  fsP;
  CodebookSet* cbsP;
  DistribSet*  dssP;
  SenoneSet*   snsP;

  int          topN; 
  float        shift;
  
  DMatrix**    w;

  float*       beta;
  int*         count;
  DMatrix**    z;
  DMatrix***   g;

  int*         dsXA;
  int          dimN;
  int          featX;
  
  float*       addCount;
  SortElement* gammaX;   
  DMatrix**    ginv;  

  int          strX; /* stream index */

  int          max_accu; /* No of accus/trfs allocated */
  int          max_tran;

} SignalAdapt;


#endif

