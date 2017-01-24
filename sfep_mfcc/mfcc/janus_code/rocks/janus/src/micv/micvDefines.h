/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvDefines.h 2633 2005-03-08 17:30:28Z metze $
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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#ifndef _micvDefines
#define _micvDefines

//-----------------------------------------------------------------------------
// system settings
//-----------------------------------------------------------------------------

// enables multithreading (optimized code for multi processor systems)
#ifdef MTHREAD
#define MICV_MULTITHREADED
#endif

#ifdef MICV_MULTITHREADED
	#define MICV_THREADS		4
#else
	#define MICV_THREADS		1
#endif


//-----------------------------------------------------------------------------
// EM settings
//-----------------------------------------------------------------------------
#define cMinFramesPerGaussian			100
#define cBadScoreValue					1.0e+10

//-----------------------------------------------------------------------------
// constants, macros, debug macros
//-----------------------------------------------------------------------------

#define PI		3.14159265358979323846264338327950288419716939937510582097494459230

#define sqr(x)	((x)*(x))

#ifndef BOOL
	typedef int BOOL;
#endif


#define SAFE_FREE(ptr) if(ptr != NULL) { free(ptr); ptr = 0; }
#define VERIFY_MALLOC(ptr, action) if((ptr) == NULL) { SERROR("out of memory!\n"); action; }


#ifdef _DEBUG
	#define MICV_ASSERTMATRIX(pMatrix) micv_AssertMatrix(pMatrix)
	#define MICV_ASSERTVECTOR(pVector) micv_AssertVector(pVector)
	#define MICV_ASSERT(pMicv) micv_Assert(pMicv)
	#define MICVSET_ASSERT(pMicvSet) micvset_Assert(pMicvSet)
	#define ASSERT(cond) assert(cond)
#else
	#define MICV_ASSERTMATRIX(pMatrix)
	#define MICV_ASSERTVECTOR(pVector)
	#define MICV_ASSERT(pMicv)
	#define MICVSET_ASSERT(pMicvSet)
	#define ASSERT(cond)
#endif


#endif	// _micvDefines


