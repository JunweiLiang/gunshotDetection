/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Transcription
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  Transcription.h
    Date    :  $Id: Transcription.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef TRANSCRIPTION_YET_INCLUDED
    #define TRANSCRIPTION_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "PhonemeSet.h"
#include "SimpleNode.h"
#include "SimpleList.h"

/*-----------------------------------------------------------------------------
/  STRUCTURES
/----------------------------------------------------------------------------*/

struct Utterance
{
    int             number;
    int             frameStart;
    int             frameEnd;
    double          timeStart;
    double          timeEnd;
    SimpleList*     phonemeList;
} typedef Utterance;

struct AcousticalUnit
{
    int             number;
    double          timeStart;
    double          timeEnd;
    int             frameStart;
    int             frameEnd;
} typedef AcousticalUnit;

/*-----------------------------------------------------------------------------
/  FUNCTIONS
/----------------------------------------------------------------------------*/
int searchUtteranceNumber(void* item1, void* item2);
int searchAcousticalUnitTime(void* item1, void* item2);
int searchAcousticalUnitFrame(void* item1, void* item2);
void initTranscriptionJANUS(char** auNames, int nau);
Utterance* parseUtteranceJANUS(char* utteranceStr);

#endif
