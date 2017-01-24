/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  classifier.h
    Date    :  $Id: classifier.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:10  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  08:59:38  fritsch
 * Initial revision
 * 
     Revision 1.1  1996/01/31  05:11:37  fritsch
     Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _classifier
#define _classifier



/* ================================================================ */
/*  Classifier                                                      */
/* ================================================================ */

typedef struct {

  char          *name;             /* name of the Classifier type */

  ClientData    (*createFct)       (int classN, int dimN,
				    int argc, char *argv[]);
  ClientData    (*copyFct)         (ClientData cd, double perturbe);
  int           (*initFct)         (ClientData cd);
  int           (*freeFct)         (ClientData cd);

  int           (*saveFct)         (ClientData cd, FILE *fp);
  ClientData    (*loadFct)         (FILE *fp);

  int           (*createAccusFct)  (ClientData cd);
  int           (*freeAccusFct)    (ClientData cd);
  int           (*clearAccusFct)   (ClientData cd);
  int           (*saveAccusFct)    (ClientData cd, FILE *fp);
  int           (*loadAccusFct)    (ClientData cd, FILE *fp);

  int           (*configureFct)    (ClientData cd, int argc, char *argv[]);
  
  int           (*scoreFct)        (ClientData cd, float *pattern, int dimN,
				                   double *score, int classN);
  int           (*accuSoftFct)     (ClientData cd, float *pattern, int dimN,
				                   double *target,
				                   double *act,
				                   int classN,
				                   double g, double h);
  int           (*accuHardFct)     (ClientData cd, float *pattern, int dimN,
				                   int targetX,
				                   double *act,
				                   int classN,
				                   double g, double h);
  int           (*updateFct)       (ClientData cd);
  
} Classifier;



/* ================================================================ */
/*  Classifier Array                                                */
/* ================================================================ */

typedef struct {

  Classifier      **classPA;
  int             classMax;
  int             classN;

} ClassifierArray;



extern int          classArrayInit    (int classMax);
extern int          classNewType      (Classifier *classPtr);
extern int          classIndex        (char *name);
extern Classifier   *classGetByIdx    (int index);
extern Classifier   *classGetByName   (char *name);



#endif








#ifdef __cplusplus
}
#endif
