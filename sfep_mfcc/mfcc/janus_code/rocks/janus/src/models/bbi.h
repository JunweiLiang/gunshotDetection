/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Bucket Box Intersection search trees
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  bbi.c
    Date    :  $Id: bbi.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.6  2003/08/14 11:20:13  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.6  2002/11/20 13:23:28  soltau
    moved cbx declaration to codebook

    Revision 1.1  1995/07/25  13:49:57  fritsch
    Initial revision

 
   ======================================================================== */

#ifndef _bbitree
#define _bbitree

#include "codebook.h"
#include "bmem.h"

typedef struct {
  int         dimN;      /* number of dimensions */
  float       *min;      /* array of min values */
  float       *max;      /* array of max values */
} Bucket;

typedef struct {
  char        label;     /* label of projection (0=lower, 1=upper) */
  float       value;     /* value of projection */
} SortItem;

typedef struct {
  int         dimN;       /* number of dimensions */
  int         projN;      /* number of projection items */
  SortItem    **proj;     /* arrays of projection items */
} ProjData;

typedef struct {
  int                    cbvN;          /* number of Gaussians */
  int                    dimN;          /* dimension */
  char                   *active;       /* array of activation flags */
  float                  **min;         /* array of lower bounds */
  float                  **max;         /* array of upper bounds */
} CbBox;

typedef struct {
  int                    cbN;           /* number of codebooks */
  CbBox                  **box;         /* array of codebook boxes */
} CbsBox;

typedef struct {
  char            k;                 /* division coordinate index */
  float           h;                 /* position of division hyperplane */
} BBINode;

typedef struct {
  CBX            cbvN;              /* number of Gaussians in this leaf */
  CBX*           cbX;               /* array of indices of Gaussians */
} BBILeaf;

struct BBITree_S {
  char                   *name;      /* name of BBI tree */
  int                    active;     /* on/off switch */
  int                    cbN;        /* number of refering codebooks */
  int                    depthN;     /* depth of BBI Tree */
  BBINode                *node;      /* pointer to array of tree nodes */
  BBILeaf                **leaf;     /* pointer to array of leafs */
  BMem                   *memChunk;
  CBX                    *singletonChunk;
};


CbBox   *bbiCreateCbBox (Codebook *cb, float gamma);
int      bbiFreeCbBox   (CbBox *cbBox);

int  bbiInit   (BBITree *bbi, ClientData cd);
int  bbiDeinit (BBITree *bbi);

int  bbiMake   (BBITree *bbi, CbsBox *cbsBox, int depth, int verbose);
int  bbiSave   (BBITree *bbi, FILE *fp);
int  bbiLoad   (BBITree *bbi, FILE *fp);
int  bbiPrune  (BBITree *bbi, int levelN);

extern int BBI_Init ( );

#endif

