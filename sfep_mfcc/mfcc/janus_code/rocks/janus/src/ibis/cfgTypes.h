/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar Types
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgTypes.h
    Date    :  $Id: cfgTypes.h 3108 2010-01-30 20:41:26Z metze $
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
    Revision 4.2  2004/07/23 17:25:34  fuegen
    - Extended supported grammar definition file formats:
       - FSM:  AT&T's FSM (finite state machine) text file format
       - PFSG: Probabilistic Finite State Graph format used by the
               SRI-LM toolkit
    - Added support for reading weights specified in the JSGF format
    - Build functionality of grammars expanded by a mode for making
      equally distributed transitions instead of using fixed scores
    - Added support for generating terminal sequences for specific
      rules either randomly or fixed

    Revision 4.1  2003/08/14 11:20:01  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.17  2003/07/15 16:41:28  fuegen
    changed CFGNodeX to UINT per default

    Revision 1.1.2.16  2003/07/02 17:22:23  fuegen
    changed definitions for CFGArcX and CFGPTItemX to larger types

    Revision 1.1.2.15  2003/06/23 13:12:07  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.14  2002/06/14 08:54:34  fuegen
    added a more flexible FSGraph traversing (removed old extend)

    Revision 1.1.2.13  2002/06/06 10:02:32  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.12  2002/05/23 11:44:39  fuegen
    removed bug inserted by CVS

    Revision 1.1.2.11  2002/05/21 15:24:49  fuegen
    added some more defines

    Revision 1.1.2.10  2002/04/18 09:21:44  fuegen
    added type CFGExt

    Revision 1.1.2.9  2002/02/19 16:29:19  fuegen
    added typedefs and defines for CFGGraphLinkXN, renamed CFGArcScore to CFGScore

    Revision 1.1.2.8  2002/02/06 13:21:16  fuegen
    only minor changes

    Revision 1.1.2.7  2002/02/01 14:46:00  fuegen
    changed rule stack handling

    Revision 1.1.2.6  2002/01/30 16:57:17  fuegen
    changed LingKS interface, added LCT cache

    Revision 1.1.2.5  2002/01/25 17:16:29  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.4  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.3  2002/01/25 17:02:36  fuegen
    extended parse tree implementation

    Revision 1.1.2.2  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.1  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _cfgTypes_010924_CF
#define _cfgTypes_010924_CF

#include <limits.h>
#include "slimits.h"

  /* =======================================================================
     Forward declarations of Objects
     ======================================================================= */

  /* context free grammar */
  typedef struct CFGSet_S		CFGSet;
  typedef struct CFG_S			CFG;
  typedef struct CFGArc_S		CFGArc;
  typedef struct CFGNode_S		CFGNode;
  typedef struct CFGRule_S		CFGRule;

  /* cfg lexicon */
  typedef struct CFGLexicon_S		CFGLexicon;
  typedef struct CFGLexiconItem_S	CFGLexiconItem;

  /* cfg rule stack */
  typedef struct CFGRuleStack_S		CFGRuleStack;
  typedef struct CFGRSItem_S		CFGRSItem;

  /* cfg parse tree */
  typedef struct CFGParseTree_S		CFGParseTree;
  typedef struct CFGPTNode_S		CFGPTNode;

  /* cfg traverse */
  typedef struct CFGTraverse_S		CFGTraverse;

  /* ======================================================================
     Type declarations
     ====================================================================== */

  typedef UINT   CFGArcX;
  typedef UINT   CFGNodeX;
  typedef USHORT CFGRuleX;
  typedef UCHAR  CFGX;
  typedef UINT   CFGRSItemX;
  typedef UINT   CFGPTItemX;
  typedef UINT   CFGPTNodeX;

#define CFGArcX_NIL	UINT_MAX
#define CFGArcX_MAX	UINT_MAX-1
#define CFGNodeX_NIL	UINT_MAX
#define CFGNodeX_MAX	UINT_MAX-1
#define CFGRuleX_NIL	USHRT_MAX
#define CFGRuleX_MAX	USHRT_MAX-1
#define CFGX_NIL	UCHAR_MAX
#define CFGX_MAX	UCHAR_MAX-1
#define CFGRSItemX_NIL	UINT_MAX
#define CFGRSItemX_MAX	UINT_MAX-1
#define CFGPTItemX_NIL	UINT_MAX
#define CFGPTItemX_MAX	UINT_MAX-1
#define CFGPTNodeX_NIL	UINT_MAX
#define CFGPTNodeX_MAX	UINT_MAX-1

  /* enumarations */
  typedef UCHAR  CFGBool;
  typedef UCHAR  CFGArcType;
  typedef UCHAR  CFGNodeType;
  typedef UCHAR  CFGRuleType;
  typedef UCHAR  CFGStatus;
  typedef UCHAR  CFGPutsFormat;
  typedef UCHAR  CFGGrammarFormat;
  typedef UCHAR  CFGBuildMode;

  /* others */
  typedef lmScore CFGScore;
  typedef float   CFGWeight;

  /* ======================================================================
     Function declarations
     ====================================================================== */

#endif /* _cfgTypes_010924_CF */

#ifdef __cplusplus
}
#endif

