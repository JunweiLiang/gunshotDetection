/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: hmeStream
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hmeStream.h
    Date    :  $Id: hmeStream.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:17  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.2  96/09/27  08:59:11  fritsch
 * *** empty log message ***
 * 

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hmeStream
#define _hmeStream


#include "hme.h"
#include "tree.h"
#include "hmeSet.h"



/* ======================================================================= */
/*  HmeStream                                                              */
/* ======================================================================= */

typedef struct {

  char            *name;      /* name of object */

  Tree            *tree;      /* polyphone context clustering tree */
  HmeSet          *hmeSet;    /* set of CI/CD Hme's */
  
} HmeStream;




int         hmeStreamTag           (ClientData cd, char *name);
int         hmeStreamGet           (ClientData cd, int tag, Word *wP,
			            int left, int right);
int         hmeStreamIndex         (ClientData cd, char *name);
char        *hmeStreamName         (ClientData cd, int index);
float       hmeStreamScore         (ClientData cd, int idx, int frameX);
int         hmeStreamAccu          (ClientData cd, int idx, int frameX,
			            float factor);
int         hmeStreamUpdate        (ClientData cd);
int         hmeStreamFrameN        (ClientData cd, int *from, int *shift,
				    int *ready);
FeatureSet  *hmeStreamFesUsed      (ClientData cd);



ClientData  hmeStreamCreateItf     (ClientData cd, int argc, char *argv[]);
int         hmeStreamFreeItf       (ClientData cd);
int         hmeStreamPutsItf       (ClientData cd, int argc, char *argv[]);
int         hmeStreamConfigureItf  (ClientData cd,  char *var, char *val);
ClientData  hmeStreamAccessItf     (ClientData cd, char *name, TypeInfo **ti);
int         hmeStreamIndexItf      (ClientData cd, int argc, char *argv[]);
int         hmeStreamNameItf       (ClientData cd, int argc, char *argv[]);
int         hmeStreamGetItf        (ClientData cd, int argc, char *argv[]);
int         hmeStreamScoreItf      (ClientData cd, int argc, char *argv[]);
int         hmeStreamAccuItf       (ClientData cd, int argc, char *argv[]);
int         hmeStreamUpdateItf     (ClientData cd, int argc, char *argv[]);  



#endif









#ifdef __cplusplus
}
#endif
