/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  PhonemeSet.h
    Date    :  $Id: PhonemeSet.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef PHONEME_SET_YET_INCLUDED
#define PHONEME_SET_YET_INCLUDED

#include <string.h>

/*-----------------------------------------------------------------------------
/  FUNCTIONS
/----------------------------------------------------------------------------*/
int     getPhonemeNumberByName(char* name);
char*   getPhonemeNameByNumber(int number);

#endif
