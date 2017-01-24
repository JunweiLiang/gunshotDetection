/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search global types
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  sglobal.h
    Date    :  $Id: sglobal.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 4.1  2003/08/14 11:20:06  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.16  2003/06/23 13:04:14  fuegen
    removed CFGSet definition (now located in cfg.h)

    Revision 1.1.2.15  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.14  2002/01/14 13:37:00  soltau
    Added Reverse search tree

    Revision 1.1.2.13  2001/09/26 08:39:05  metze
    Renamed interpolLM to metaLM

    Revision 1.1.2.12  2001/09/24 19:29:59  soltau
    Added LatLmNode

    Revision 1.1.2.11  2001/07/12 18:41:34  soltau
    Added XCM

    Revision 1.1.2.10  2001/06/01 08:38:58  metze
    Added InterpolLM

    Revision 1.1.2.9  2001/06/01 08:33:16  fuegen
    added typedef for CFGSet (Context Free Grammar Set)

    Revision 1.1.2.8  2001/05/30 19:22:54  soltau
    Added G'Lat

    Revision 1.1.2.7  2001/05/28 14:47:00  metze
    Added LatticeLM

    Revision 1.1.2.6  2001/05/23 08:18:01  metze
    Added PhraseLM

    Revision 1.1.2.5  2001/04/27 07:38:53  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.1.2.4  2001/03/14 12:12:08  soltau
    renamed backpointer table

    Revision 1.1.2.3  2001/02/02 16:04:28  soltau
    Added SVocab declarations

    Revision 1.1.2.2  2001/02/01 18:40:11  soltau
    Added couple of declarations

    Revision 1.1.2.1  2001/01/29 17:57:07  soltau
    Initial version


   ======================================================================== */

#ifndef _sglobal
#define _sglobal

/* ------------------------------------------------------------------------
    forward declarations from phmm
   ------------------------------------------------------------------------ */

typedef struct PHMM_S    PHMM; 
typedef struct PHMMSet_S PHMMSet;

/* ------------------------------------------------------------------------
    forward declarations from xhmm
   ------------------------------------------------------------------------ */

typedef struct LCM_S     LCM; 
typedef struct LCMSet_S  LCMSet;
typedef struct RCM_S     RCM; 
typedef struct RCMSet_S  RCMSet;
typedef struct XCM_S     XCM; 
typedef struct XCMSet_S  XCMSet;

/* ------------------------------------------------------------------------
    forward declarations from svocab
   ------------------------------------------------------------------------ */

typedef struct SWord_S   SWord;
typedef struct SVocab_S  SVocab;
typedef struct SVMap_S   SVMap;

/* ------------------------------------------------------------------------
    forward declarations from strace
   ------------------------------------------------------------------------ */

typedef struct BP_S      BP;
typedef struct RTok_S    RTok;
typedef struct NTok_S    NTok;
typedef struct STab_S    STab;

/* ------------------------------------------------------------------------
    forward declarations from smem
   ------------------------------------------------------------------------ */

typedef struct RIce_S    RIce;
typedef struct NIce_S    NIce;
typedef struct LIce_S    LIce;
typedef struct XIce_S    XIce;
typedef struct SMem_S    SMem;

/* ------------------------------------------------------------------------
    forward declarations from stree
   ------------------------------------------------------------------------ */

typedef struct SRoot_S   SRoot;
typedef struct SNode_S   SNode;
typedef struct SIPhone_S SIPhone;
typedef struct SDPhone_S SDPhone;
typedef struct STree_S   STree;
typedef struct RNode_S   RNode;
typedef struct RArray_S  RArray;
typedef struct SPass_S   SPass;

/* ------------------------------------------------------------------------
    forward declarations from lmla
   ------------------------------------------------------------------------ */

typedef struct LTREE_S      LTree;
typedef struct LMLA_S       LMLA;
typedef struct LATLMNODE_S  LatLMNode;
typedef struct LINGKS_S     LingKS;

typedef struct NGramLM_S    NGramLM;
typedef struct PhraseLM_S   PhraseLM;
typedef struct MetaLM_S     MetaLM;

/* ------------------------------------------------------------------------
    forward declarations from glat
   ------------------------------------------------------------------------ */

typedef struct GNode_S     GNode;
typedef struct GLink_S     GLink;
typedef struct GLat_S      GLat;


#endif
