/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  PhonemeSet.c
    Date    :  $Id: PhonemeSet.c 2844 2008-11-25 12:00:00Z wolfel $
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

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#include "PhonemeSet.h"

/*-----------------------------------------------------------------------------
/  PHONEME-SET DEFINITION
/----------------------------------------------------------------------------*/

static const int   phonemeSetSize = 50; 
static const char* phonemeSet[50] = 
{
    "PAD",          "IY",           "IH",           "EH",           "AE",
    "IX",           "AX",           "AH",           "UW",           "UH",
    "AO",           "AA",           "EY",           "AY",           "OY",
    "AW",           "OW",           "L",            "R",            "Y",
    "W",            "ER",           "AXR",          "M",            "N",
    "NG",           "CH",           "JH",           "DH",           "B",
    "D",            "G",            "P",            "T",            "K",
    "Z",            "ZH",           "V",            "F",            "TH",
    "S",            "SH",           "HH",           "XL",           "XM",
    "XN",           "SIL",          "GARBAGE",      "+FILLER+",     "+BREATH+"
};

/*===================================================================================
/  getPhonemeNumberByName
===================================================================================*/
int getPhonemeNumberByName(char* name)
{
    int i;

    if (name[0]==0) return(-1);
    for (i=0; i<phonemeSetSize; i++) if (strcmp(phonemeSet[i],name)==0) return(i);

    return(-1);
}

/*===================================================================================
/  getPhonemeNameByNumber
===================================================================================*/
char* getPhonemeNameByNumber(int number)
{
    if ((number>=0) && (number<phonemeSetSize)) return((char*)phonemeSet[number]);

    return((char*)0);
}
