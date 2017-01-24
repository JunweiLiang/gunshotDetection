/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set Maintainance Functions
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  phones.h
    Date    :  $Id: phones.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:26  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.3  1997/07/25 17:32:31  tschaaf
    gcc / DFKI - Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.5  95/08/18  08:22:25  finkem
    *** empty log message ***
    
    Revision 1.4  1995/08/16  14:41:43  rogina
    *** empty log message ***

    Revision 1.3  1995/08/10  13:37:56  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _phones
#define _phones

#include "list.h"

/* ========================================================================
    Phone
   ======================================================================== */

typedef struct
{
  char*  name;

} Phone;

extern int     phoneInit(   Phone* phone, ClientData CD);
extern int     phoneDeinit( Phone* phone);

/* ========================================================================
    Phones
   ======================================================================== */

typedef struct LIST(Phone) PhoneList;

typedef struct
{
  char*     name;            /* name of the phones object */
  int       useN;            /* number of references      */
  PhoneList list;
  char      commentChar;
} Phones;

extern int     Phones_Init ( void );
extern int     phonesInit(   Phones* phones, ClientData CD);
extern int     phonesDeinit( Phones* phones);
extern int     phonesLinkN(  Phones* phones);

extern Phones* phonesCreate( char*   name);
extern int     phonesFree(   Phones* phones);

extern int     phonesIndex(  Phones *phones, char *phone);
extern char*   phonesName(   Phones* phones, int i);
extern int     phonesNumber( Phones* phones);

extern int     phonesAdd(    Phones *phones, int argc, char* argv[]);
extern int     phonesDelete( Phones *phones, int argc, char* argv[]);
extern int     phonesWrite(  Phones *phones, char *filename);
extern int     phonesRead(   Phones *phones, char *filename);

extern int     phonesCharFunc (Phones *superset, Phones *subset, char *charFunc);
/* ========================================================================
    PhonesSet
   ======================================================================== */

typedef struct LIST(Phones) PhonesList;

typedef struct
{
  char*      name;
  int        useN;

  PhonesList list;

  char       commentChar;
}
PhonesSet;

extern int        phonesSetInit(   PhonesSet* phonesSet, ClientData CD);
extern int        phonesSetDeinit( PhonesSet* phonesSet);
extern int        phonesSetLinkN(  PhonesSet* phonesSet);

extern PhonesSet* phonesSetCreate( char* name);
extern int        phonesSetFree(   PhonesSet* phonesSet);

extern int        phonesSetIndex(  PhonesSet* phones, char *phone);
extern char*      phonesSetName(   PhonesSet* phones, int i);
extern int        phonesSetNumber( PhonesSet* phones);

extern int        phonesSetAdd(    PhonesSet* phones, char* name, 
                                                      int argc, char* argv[]);
extern int        phonesSetDelete( PhonesSet* phones, int argc, char* argv[]);
extern int        phonesSetWrite(  PhonesSet* phones, char *filename);
extern int        phonesSetRead(   PhonesSet* phones, char *filename);

extern int        phonesConfigureItf(ClientData cd, char *var, char *val);
extern ClientData phonesAccessItf (  ClientData cd, char *name, TypeInfo **ti);
extern int        phonesIndexItf(    ClientData cd, int argc, char *argv[]);
extern int        phonesNameItf(     ClientData cd, int argc, char *argv[]);

#endif


#ifdef __cplusplus
}
#endif
