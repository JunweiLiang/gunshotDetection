/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set Maintenance Functions
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  questions.c
    Date    :  $Id: questions.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.4  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.3.20.4  2003/06/03 14:51:21  metze
    Removed warning

    Revision 4.3.20.3  2002/08/27 08:42:26  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.3.20.2  2001/11/19 14:46:55  metze
    Go away, boring message!

    Revision 4.3.20.1  2001/03/17 17:36:17  soltau
    Fixed memory leak in questionsDefine

    Revision 4.3  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.2.30.2  2000/11/08 17:29:17  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.2.30.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/02/09 10:41:01  soltau
    Fixed bug in questionCompress

    Revision 4.1  2000/01/12 10:12:49  fuegen
    remove bug for AND and OR questions (answers were wrong)

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.3  1997/07/16 11:29:01  rogina
    I'm sure that at the beginning it was possible to use compound questions.
    I have no explanation for why last years Praktikum in KA could work with
    compound questions. The way it was implemented was absolutely wrong. Hm,
    I believe I've fixed it now.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.13  96/02/26  06:11:29  finkem
    if there is no WE Tag use WB in the sense of WordBoundary instead
    of wor begin
    
    Revision 1.12  1996/01/31  05:00:06  finkem
    finallay questions on tags and word-type argument for questions

    Revision 1.11  1996/01/05  17:19:11  finkem
    removed ref to old ERR function

    Revision 1.10  1995/10/30  00:23:19  finkem
    new questionAsk

    Revision 1.9  1995/09/06  07:37:00  kemp
    *** empty log message ***

    Revision 1.8  1995/08/27  22:34:08  finkem
    *** empty log message ***

    Revision 1.7  1995/08/18  08:23:19  finkem
    *** empty log message ***

    Revision 1.6  1995/08/17  17:10:38  rogina
    *** empty log message ***

    Revision 1.5  1995/08/14  12:21:56  finkem
    *** empty log message ***

    Revision 1.4  1995/07/27  18:44:11  rogina
    *** empty log message ***

    Revision 1.3  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:47  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

char questionsRCSVersion[]= 
           "@(#)1$Id: questions.c 2390 2003-08-14 11:20:32Z fuegen $";

#include "common.h"
#include "itf.h"
#include "questions.h"
#include "array.h"
#include "word.h"

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo questionSetInfo;
extern TypeInfo questionInfo;

static QuestionSet questionSetDefault;

/* ========================================================================
    Question
   ======================================================================== */

static int questionPutsItf (ClientData clientData, int argc, char *argv[]);
static int questionConfigureItf (ClientData cd, char *var, char *val);

/* ------------------------------------------------------------------------
    questionCompress  remove spaces/tabs from questionSet
   ------------------------------------------------------------------------ */

char* questionCompress(char* name)
{
  char* p = name;
  char* c = name;
  int   i = -1;
  
  if (name == NULL) return name;
  if (name[0] == 0) return name;

  while (*p) {
    if ( *p == ' ' || *p == '\t') { p++; if (i>0) i = 0; }
    else {
      if (!i) {  *c++ = ' ';}
      *c++ = *p++; 
      i    = 1;
    }
  }
  *c++ = 0;
  return name;
}

/* ------------------------------------------------------------------------
    questionInit    initialize a question
   ------------------------------------------------------------------------ */

int questionInit( Question* QU, char* name)
{
  assert( QU);

  QU->name         =  strdup( name);
  QU->charFunc     =  NULL;
  QU->tagsBitSet   =  NULL;

  QU->clauseN      =      0;
  QU->leftContext  =  99999;
  QU->rightContext = -99999;
  QU->qsetP        =  NULL;

  QU->tagOperation =  questionSetDefault.tagOperation;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    questionAlloc  allocate the data structures to hold the compiled
                   version of the question
   ------------------------------------------------------------------------ */

int questionAlloc( Question* QU, QuestionSet* QS)
{
  int   clauseX;
  int   len, phonesN;
  char* p;

  QU->clauseN      =      1;
  QU->leftContext  =  99999;
  QU->rightContext = -99999;
  QU->qsetP        =     QS;
  QU->tagOperation =  QS->tagOperation;

  for ( p = QU->name; *p != '\0'; p++) {
    int  pos;
    char dummy;

    if (sscanf( p, "%d%c", &pos, &dummy) == 2 && dummy == '=') {
      if (pos < QU->leftContext)  QU->leftContext  = pos;
      if (pos > QU->rightContext) QU->rightContext = pos;
      p = strchr(p,'=')+1;
    }
  }
  for ( p = QU->name; *p != '\0'; p++) if (*p == '|') QU->clauseN++;
  
  if ((len = (QU->rightContext - QU->leftContext)) < 0) {
    if ( strlen( QU->name) > 0) {
         ERROR("Couldn't parse question '%s'.\n", QU->name);
         return TCL_ERROR;
    }
    return TCL_OK;
  }
  QU->charFunc   = (char***)malloc(sizeof(char**) * QU->clauseN);
  QU->tagsBitSet =   (int**)malloc(sizeof( int* ) * QU->clauseN);

  if ( QU->charFunc==NULL || QU->tagsBitSet==NULL) { 
    if ( QU->charFunc) free( QU->charFunc); 
    ERROR( "Can't allocate space for question '%s'.\n", QU->name);
    return TCL_ERROR;
  }

  QU->charFunc[0]   = (char**)malloc(sizeof(char*) * QU->clauseN * (1+len));
  QU->tagsBitSet[0] = (int  *)malloc(sizeof(int)   * QU->clauseN * (1+len));

  if ( QU->charFunc[0]==NULL || QU->tagsBitSet[0]==NULL) { 
    if (  QU->charFunc[0]) free( QU->charFunc[0]); 
    free( QU->charFunc);   free( QU->tagsBitSet);
    ERROR( "Can't allocate space for question '%s'.\n", QU->name);
    return TCL_ERROR;
  }

  for ( clauseX = 1; clauseX < QU->clauseN; clauseX++) {
    QU->charFunc[clauseX]   = QU->charFunc[  clauseX-1] + (len + 1);
    QU->tagsBitSet[clauseX] = QU->tagsBitSet[clauseX-1] + (len + 1);
  }

  memset( QU->charFunc[0],   0, sizeof(char*) * QU->clauseN * (1+len));
  memset( QU->tagsBitSet[0], 0, sizeof(int)   * QU->clauseN * (1+len));

  phonesN            =  phonesNumber(QS->phones);
  QU->charFunc[0][0] = (char*)malloc( sizeof(char) * QU->clauseN * (len+1) *
                                    ((phonesN-1) / 8 + 1));

  memset( QU->charFunc[0][0], 0xff, QU->clauseN * (len+1) * ((phonesN-1)/8+1));

  if (! QU->charFunc[0][0]) { 
    ERROR("Can't allocate characteristic functions for '%s'.\n", QU->name); 
    return TCL_ERROR;
  }

  for ( clauseX = 0; clauseX < QU->clauseN; clauseX++) {
    int contextX;

    if ( clauseX) QU->charFunc[clauseX][0] =  QU->charFunc[clauseX-1][0] + 
                                             (len+1) * ((phonesN-1)/8+1) *
                                              sizeof(char);
    for ( contextX  = QU->leftContext+1; 
          contextX <= QU->rightContext; contextX++) {

      QU->charFunc[clauseX][contextX-QU->leftContext] = 
          QU->charFunc[clauseX][contextX-QU->leftContext-1] + 
         (phonesN - 1)/8 + 1;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    questionDeinit  deinitialize question structure
   ------------------------------------------------------------------------ */

int questionDeinit( Question* QU)
{
  assert(QU);

  if (      QU->charFunc) {
    if (    QU->charFunc[0]) {
      if (  QU->charFunc[0][0]) free( QU->charFunc[0][0]);
      free( QU->charFunc[0]);
    }
    free(   QU->charFunc);
    QU->charFunc = NULL;
  }
  if (    QU->tagsBitSet) {
    if (  QU->tagsBitSet[0]) free( QU->tagsBitSet[0]);
    free( QU->tagsBitSet);
    QU->tagsBitSet = NULL;
  }
  if ( QU->name) { free(QU->name); QU->name = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    questionDefine
   ------------------------------------------------------------------------ */

#define SETBIT(c,i)   ((c)[i/8] |= (       (1 << (i%8))))
#define TESTBIT(c,i)  ((c)[i/8] &  (       (1 << (i%8))))
#define UNSETBIT(c,i) ((c)[i/8] &= (0xff ^ (1 << (i%8))))

int questionDefine( Question* QU, char* name, QuestionSet* QS)
{
  int   clauseX;
  int   retCode  = TCL_OK;
  char* p;
  char* cFunc    = NULL;
  int   phonesN, len;

  assert( QS && QU);

  if ( questionAlloc( QU, QS) != TCL_OK) {
    ERROR("Can't define question '%s'.\n", QU->name);
    return TCL_ERROR;
  }
  if (! QU->charFunc) return TCL_OK;

  phonesN  =  phonesNumber(QS->phones);
  len      =  QU->rightContext - QU->leftContext;
  cFunc    = (char*)malloc(sizeof(char)*(1+phonesN));

  memset( cFunc, '-', phonesN); cFunc[phonesN] = '\0';

  for ( clauseX = 0, p = QU->name; *p != '\0';) {
    int  pos;
    char set[120];

    if (*p == '|') { clauseX++; p++; }

    if ( sscanf( p, "%d=%s", &pos, set) == 2) {
      int idx, i;

      if ((idx = phonesSetIndex(QS->phonesSet,set)) > -1) {

        if ( phonesCharFunc(QS->phones,QS->phonesSet->list.itemA+idx,cFunc)) {
          ERROR("PhonesSet '%s' in '%s' is not subset of general PhonesSet\n",
                 set, QU->name);
          retCode = TCL_ERROR;
        }
        for ( i = 0; i < phonesN; i++) {
          if ( cFunc[i] == '-') 
            UNSETBIT(QU->charFunc[clauseX][pos-QU->leftContext],i);
        }
      }
      else if ((idx = phonesIndex(   QS->phones,   set)) > -1) {

        memset( QU->charFunc[clauseX][pos-QU->leftContext], 0,
              ( phonesN-1)/8+1);
        SETBIT(QU->charFunc[clauseX][pos-QU->leftContext],idx);
      }
      else if ((idx = tagsIndex(   QS->tags,   set)) > -1) {
        QU->tagsBitSet[clauseX][pos - QU->leftContext] |= 1 << idx;
      }
      else {
        ERROR( "Can't compile '%d=%s' in question '%s'.\n", pos, set, name);
        retCode = TCL_ERROR;
      }
      p = strchr(p,(int)'=');
      for ( ; *p != '\0' && *p != ' ' && *p != '\t'; p++);
      for ( ; *p == '\t' || *p == ' '              ; p++);
    }
    else {
      ERROR("Cannot parse question (%s) \'%s\'\n", p, QU->name);
      return TCL_ERROR;
    }
  }
  if (cFunc) free(cFunc);
  return retCode;
}

/* ------------------------------------------------------------------------
    questionPuts  print info on a single question
   ------------------------------------------------------------------------ */

static int questionPutsItf (ClientData clientData, int argc, char *argv[])
{
  Question* QU      = (Question*)clientData;
  int       phonesN =  phonesNumber(QU->qsetP->phones);
  int       cX, clauseX, tagX;

  itfAppendResult("{%s} {", QU->name ? QU->name : ""); 

  for (clauseX=0; clauseX<QU->clauseN; clauseX++) {
    itfAppendResult("\n{{");
    for ( cX  = QU->leftContext; cX <= QU->rightContext; cX++) {
      int i;
      itfAppendResult(" {%+3d ", cX);
      for ( i = 0; i < phonesN; i++) {
        if ( TESTBIT(QU->charFunc[clauseX][cX-QU->leftContext],i)) {
          itfAppendResult( " %s", phonesName(QU->qsetP->phones,i)); 
	}
      }
      itfAppendResult("}");
    }
    itfAppendResult("} {");

    for ( cX  = QU->leftContext; cX <= QU->rightContext; cX++) {
      if ( QU->tagsBitSet[clauseX][cX-QU->leftContext]) {
        itfAppendResult(" {%+d", cX);
        for (tagX=0; tagX<8* (int)sizeof(int); tagX++) {
          if ( QU->tagsBitSet[clauseX][cX-QU->leftContext] & 1 << tagX)
	    itfAppendResult(" %s",tagsName(QU->qsetP->tags,tagX));
        }
        itfAppendResult("}");
      }
    }
    itfAppendResult("}}");
  }
  itfAppendResult("}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    questionConfigureItf
   ------------------------------------------------------------------------ */

static int questionConfigureItf (ClientData cd, char *var, char *val)
{
  Question *QU = (Question*)cd;
  if (QU==NULL) return TCL_ERROR;

  if (var==NULL) {
    ITFCFG (questionConfigureItf,cd,"leftContext");
    ITFCFG (questionConfigureItf,cd,"rightContext");
    ITFCFG (questionConfigureItf,cd,"clauseN");
    ITFCFG (questionConfigureItf,cd,"tagOperation");
    return TCL_OK;
  }
  ITFCFG_Int (var,val,"leftContext",  QU->leftContext,  1);
  ITFCFG_Int (var,val,"rightContext", QU->rightContext, 1);
  ITFCFG_Int (var,val,"clauseN",      QU->clauseN,      1);
  ITFCFG_Int (var,val,"tagOperation", QU->tagOperation, 0);

  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    questionAnswer answers a question with respect to a word (i.e. a 
                   tagged phone string) either yes=1, don't_know=0 or
                   no=-1. If a padding phone is defined, the answer
                   don't know will only occur in critical situations e.g.
                   where there is a phone/tag set mismatch.
   ------------------------------------------------------------------------ */

int questionAnswer (Question* QU, Word* wP, int  left, int right)
{
  int* phoneA = wP->phoneA;
  int* tagA   = wP->tagA;
  int  contextX, clauseX;

  if ( wordCheck( wP, QU->qsetP->phones, QU->qsetP->tags) != TCL_OK) return 0;

  if ( QU->qsetP->padPhoneX < 0) {

    if (  QU->leftContext  < left || 
          QU->rightContext > right) return 0;
    if (! QU->charFunc)             return 1;

    for ( clauseX=0; clauseX < QU->clauseN; clauseX++) {
      for ( contextX  = QU->leftContext; 
            contextX <= QU->rightContext; contextX++) {
	  int tag, qTag;

	if ( !TESTBIT (QU->charFunc[clauseX][contextX-QU->leftContext],
		       phoneA[contextX-left])) break;

	qTag = QU->tagsBitSet[clauseX][contextX-QU->leftContext];
	tag  = tagA[contextX-left];
	    
	if ( QU->tagOperation ) {
	    /* disjunction */
	    if ( qTag && tagA && (((qTag & tag) != qTag) || !tag) ) break;
	} else {
	    /* conjunction (this part is buggy!!!) */
	    /* example: qTag=11, tag=10 ==> ((qTag & tag) != tag) = 0
	       this produces a break an a result 1 */
	    if ( qTag && tagA && (((qTag & tag) != tag)  || !tag) ) break;
	}

      }
      if ( contextX > QU->rightContext) break;
    }
    if ( clauseX < QU->clauseN) return  1; 
    else                        return -1;
  }
  else {

    /* phone padding */

    if (! QU->charFunc) return 1;

    for ( clauseX=0; clauseX < QU->clauseN; clauseX++) {
      for ( contextX  = QU->leftContext; 
            contextX <= QU->rightContext; contextX++) {

        int phoneX = ( contextX < left || contextX > right) ? 
                       QU->qsetP->padPhoneX :
                       phoneA[contextX-left];
        int tag    = ( contextX < left || contextX > right) ?
	               0 : tagA[contextX-left];
	int qTag;

	if ( !TESTBIT (QU->charFunc[clauseX][contextX-QU->leftContext],
		       phoneX)) break;

	qTag = QU->tagsBitSet[clauseX][contextX-QU->leftContext];
	
	if ( QU->tagOperation ) {
	    /* disjunction */
	    if ( qTag && tagA && (((qTag & tag) != qTag) || !tag) ) break;
	} else {
	    /* conjunction (this part is buggy!!!) */
	    /* example: qTag=11, tag=10 ==> ((qTag & tag) != tag) = 0
	       this produces a break an a result 1 */
	    if ( qTag && tagA && (((qTag & tag) != tag)  || !tag) ) break;
	}
      }
      if ( contextX > QU->rightContext ) break;
    }
    if ( clauseX < QU->clauseN ) return  1; 
    else                       return -1;
  }
}

static int questionAnswerItf( ClientData clientData, int argc, char *argv[])
{
  Question* QU    = (Question*)clientData;
  int       ac    =  argc - 1;
  int       left  =  0,
            right =  0;
  Word      word;

  wordInit( &word, QU->qsetP->phones, QU->qsetP->tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tagged phones>", ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",   ARGV_INT, NULL, &left,  NULL,"left  context offset",
      "<rightContext>",  ARGV_INT, NULL, &right, NULL,"right context offset",
       NULL) != TCL_OK || word.itemN != (right-left+1)) {
       wordDeinit(&word); return TCL_ERROR;
  }

  itfAppendResult( "%d", questionAnswer(QU, &word, left, right));
  wordDeinit(&word);
  return TCL_OK;
}

/* ========================================================================
    QuestionSet
   ======================================================================== */

/* ------------------------------------------------------------------------
    Create/Init QuestionSet
   ------------------------------------------------------------------------ */

int questionSetInit( QuestionSet* questionSet, char *name, 
                   Phones *phones, PhonesSet *phonesSet, Tags *tags)
{
  questionSet->name         = strdup(name);
  questionSet->useN         = 0;

  /* must be defined before listInit */
  questionSet->tagOperation = questionSetDefault.tagOperation;

  listInit((List*)&(questionSet->list), &questionInfo, sizeof(Question),
                    questionSetDefault.list.blkSize);

  questionSet->list.init    = (ListItemInit  *)questionInit;
  questionSet->list.deinit  = (ListItemDeinit*)questionDeinit;

  questionSet->phones       =  phones;
  questionSet->phonesSet    =  phonesSet;
  questionSet->tags         =  tags;
  questionSet->padPhoneX    =  questionSetDefault.padPhoneX;
  questionSet->commentChar  =  questionSetDefault.commentChar;

  link( questionSet->phones,    "Phones");
  link( questionSet->phonesSet, "PhonesSet");
  link( questionSet->tags,      "Tags");

  return TCL_OK;
}

QuestionSet *questionSetCreate( char *name, Phones *phones, PhonesSet *phonesSet, 
                                        Tags *tags)
{
  QuestionSet *questionSet = (QuestionSet*)malloc(sizeof(QuestionSet));

  if (! questionSet || questionSetInit( questionSet, name, phones, phonesSet,
                                    tags) != TCL_OK) {
     if ( questionSet) free( questionSet);
     ERROR("Failed to allocate QuestionSet object '%s'.\n", name);
     return NULL; 
  }
  return questionSet;
}

static ClientData questionSetCreateItf( ClientData clientData, 
                                      int argc, char *argv[]) 
{ 
  Phones      *phones    =  NULL;
  PhonesSet   *phonesSet =  NULL;
  Tags        *tags      =  NULL;
  int          padPhoneX = -1;
  QuestionSet* qsetP     =  NULL;
  char* name = NULL;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the question set",
      "<phones>",   ARGV_OBJECT,NULL,&phones,   "Phones",   "set of phones",
      "<phonesSet>",ARGV_OBJECT,NULL,&phonesSet,"PhonesSet","set of phone set",
      "<tags>",     ARGV_OBJECT,NULL,&tags,     "Tags",     "set of tags",
      "-padPhone",  ARGV_INT,   NULL,&padPhoneX,NULL,"padding phone index",
       NULL) != TCL_OK) return NULL;

  if ( (qsetP = questionSetCreate(name,phones,phonesSet,tags)) ) {
    qsetP->padPhoneX = padPhoneX;
  }
  return (ClientData)qsetP;
}

/* ------------------------------------------------------------------------
    Free QuestionSet
   ------------------------------------------------------------------------ */

int questionSetLinkN( QuestionSet* questionSet)
{
  int  useN = listLinkN((List*)&(questionSet->list)) - 1;
  if ( useN < questionSet->useN) return questionSet->useN;
  else                         return useN;
}

int questionSetDeinit( QuestionSet* questionSet)
{
  if (questionSet==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if ( questionSetLinkN( questionSet)) {
    SERROR("QuestionSet '%s' still in use by other objects.\n", questionSet->name);
    return TCL_ERROR;
  }
  if ( questionSet->name) { free( questionSet->name); questionSet->name = NULL; }

  unlink( questionSet->phones,    "Phones");
  unlink( questionSet->phonesSet, "PhonesSet");
  unlink( questionSet->tags,      "Tags");

  return listDeinit((List*)&(questionSet->list));
}

int questionSetFree(QuestionSet* questionSet)
{
  if (questionSetDeinit(questionSet)!=TCL_OK) return TCL_ERROR;
  free(questionSet);
  return TCL_OK;
}

static int questionSetFreeItf (ClientData clientData)
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  return questionSetFree( questionSet);
}

/* ------------------------------------------------------------------------
    questionSetConfigureItf
   ------------------------------------------------------------------------ */

static int questionSetConfigureItf(ClientData cd, char *var, char *val)
{
  QuestionSet *QU = (QuestionSet*)cd;
  if (! QU)  QU = &questionSetDefault;

  if (var==NULL) {

    ITFCFG (questionSetConfigureItf,cd,"useN");
    ITFCFG (questionSetConfigureItf,cd,"phones");
    ITFCFG (questionSetConfigureItf,cd,"phonesSet");
    ITFCFG (questionSetConfigureItf,cd,"tags");
    ITFCFG (questionSetConfigureItf,cd,"padPhone");
    ITFCFG (questionSetConfigureItf,cd,"commentChar");
    ITFCFG (questionSetConfigureItf,cd,"tagOperation");

    return listConfigureItf((ClientData)&(QU->list), var, val);
  }

  ITFCFG_Int    (var,val,"useN",         QU->useN,                     1);
  ITFCFG_Int    (var,val,"padPhone",     QU->padPhoneX,                0);
  ITFCFG_Char   (var,val,"commentChar",  QU->commentChar,              0);
  ITFCFG_Object (var,val,"phones",       QU->phones,   name,Phones,    1);
  ITFCFG_Object (var,val,"phonesSet",    QU->phonesSet,name,PhonesSet, 1);
  ITFCFG_Object (var,val,"tags",         QU->tags,     name,Tags,      1);

  if ( !strcmp (var, "tagOperation") ) {
      int i;

      if ( !val ) {
	  itfAppendElement ("%i", QU->tagOperation);
	  return TCL_OK;
      }

      QU->tagOperation = atoi(val);

      for (i=0; i<QU->list.itemN; i++)
	  QU->list.itemA[i].tagOperation = QU->tagOperation;
      
      return TCL_OK;
  }

  return listConfigureItf((ClientData)&(QU->list), var, val);   
}

/* ------------------------------------------------------------------------
    questionSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData questionSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  QuestionSet* questionSet = (QuestionSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement( "phones");
       itfAppendElement( "phonesSet");
       itfAppendElement( "tags");
       *ti = NULL;
    }
    else { 
      if (! strcmp( name+1, "phones")) {
        *ti = itfGetType("Phones");
        return (ClientData)questionSet->phones;
      }
      if (! strcmp( name+1, "phonesSet")) {
        *ti = itfGetType("PhonesSet");
        return (ClientData)questionSet->phonesSet;
      }
      else if (! strcmp( name+1, "tags")) {
        *ti = itfGetType("Tags");
        return (ClientData)questionSet->tags;
      }
    }
  }
  return listAccessItf((ClientData)&(questionSet->list),name,ti);
}

/* ------------------------------------------------------------------------
    questionSetPuts
   ------------------------------------------------------------------------ */

static int questionSetPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  return listPutsItf((ClientData)&(questionSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    questionSetIndex
   ------------------------------------------------------------------------ */

int questionSetIndex( QuestionSet* questionSet, char* name) 
{
  return listIndex((List*)&(questionSet->list), questionCompress(name), 0);
}

static int questionSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  return listName2IndexItf((ClientData)&(questionSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    questionSetName
   ------------------------------------------------------------------------ */

char* questionSetName( QuestionSet* questionSet, int i) 
{ 
  return (i < 0) ? "(null)" : questionSet->list.itemA[i].name;
}

static int questionSetNameItf( ClientData clientData, int argc, char *argv[] )
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  return listIndex2NameItf((ClientData)&(questionSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    questionSetNumber
   ------------------------------------------------------------------------ */

int questionSetNumber( QuestionSet* questionSet) { return questionSet->list.itemN; }

/* ------------------------------------------------------------------------
    questionSetAdd
   ------------------------------------------------------------------------ */

int questionSetAdd( QuestionSet* questionSet, char* name)
{
  int idx = questionSetIndex( questionSet, name);

  if ( idx < 0) {
    idx = listNewItem((List*)&(questionSet->list), name);
    
    if ( questionDefine( questionSet->list.itemA + idx, name, questionSet) !=
         TCL_OK) {
      listDelete((List*)&(questionSet->list), name);
      return -1;
    }
  }
  return idx;
}

static int questionSetAddItf ( ClientData clientData, int argc, char *argv[] )
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  char*      name      =  NULL;
  int        ac        =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<question>",ARGV_STRING,NULL,&name,NULL,"question string",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  return ( questionSetAdd( questionSet, name) < 0) ? TCL_ERROR : TCL_OK;
}

/* ------------------------------------------------------------------------
    questionSetDeleteItf
   ------------------------------------------------------------------------ */

static int questionSetDeleteItf (ClientData cd, int argc, char *argv[])
{
  QuestionSet* questionSet  = (QuestionSet*)cd;
  return listDeleteItf((ClientData)&(questionSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    questionSetRead
   ------------------------------------------------------------------------ */

int questionSetRead( QuestionSet *questionSet, char *filename)
{
  FILE *fp;
  int   rc = TCL_OK;
  char  line[4000];
  int   n = 0;

  if (! (fp=fileOpen(filename,"r"))) return -1;

  while (1) {
    int   argc; char **argv;
    char* p;

    strcpy(line,"add \"");

    if ( fscanf(fp,"%[^\n]",&(line[5]))!=1) break; 
    else fscanf(fp,"%*c");

    if ( line[5] == questionSet->commentChar) continue; 

    for (p=&(line[5]); *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 
    strcat(line,"\"");

    if ( Tcl_SplitList (itf,line,&argc,&argv) == TCL_OK) {
      questionSetAddItf ((ClientData)questionSet, argc, argv);
      n++;
      free(argv);
    }
    else rc =TCL_ERROR;
  }
  fileClose( filename, fp);
  return (rc != TCL_OK) ? -1 : n;
}

static int questionSetReadItf ( ClientData clientData, int argc, char *argv[] )
{
  QuestionSet* questionSet = (QuestionSet*)clientData;
  char*      fname     =  NULL;
  int        ac        =  argc-1, n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of question set file", NULL) != TCL_OK)
    return TCL_ERROR;

  INFO("Reading QuestionSet '%s' from file '%s'.\n", questionSet->name, fname);

  itfAppendResult( "%d", n = questionSetRead( questionSet, fname));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    questionSetWrite
   ------------------------------------------------------------------------ */

int questionSetWrite( QuestionSet *questionSet, char *filename)
{
  Question* question   = questionSet->list.itemA;
  int       questionSetN = questionSet->list.itemN;
  FILE     *fp;

  if (! (fp=fileOpen(filename,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n",
                questionSet->commentChar);
  fprintf( fp, "%c  Name            : %s\n", questionSet->commentChar,
                questionSet->name);
  fprintf( fp, "%c  Type            : QuestionSet\n", questionSet->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n", questionSet->commentChar,
                questionSet->list.itemN);
  fprintf( fp, "%c  Date            : %s", questionSet->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                questionSet->commentChar);

  for ( ; questionSetN > 0; questionSetN--, question++) {
     fprintf(fp,"%s\n", question->name);
  }
  fileClose( filename, fp);
  return TCL_OK;
}

static int questionSetWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of questionSet file", NULL) != TCL_OK)
    return TCL_ERROR;

  return questionSetWrite((QuestionSet*)clientData, fname);
}

/* ------------------------------------------------------------------------
    questionSetAnswer
   ------------------------------------------------------------------------ */

int questionSetAnswer( QuestionSet *QS, int index, Word* wP,
                                        int left, int right)
{
  return questionAnswer( QS->list.itemA+index,wP, left, right);
}

/* ================================================= */
/* infix to postfix conversion (works on any string) */
/* ================================================= */

static char *infix2UPN (char *infix, char *UPN)
{
  char *iP = infix, *endP;
  int  level = 0;

  for (iP=infix; *iP!='\0'; iP++)   /* search for low priority operators: logical OR: '|' */
      switch (*iP)
      { case '(': level++; break;
        case ')': level--; if (level<0) { ERROR("Syntax error in question"); return NULL; } break;
        case '|': if (level==0) 
	  { *iP='\0'; 
	    if (infix2UPN(infix,UPN)==NULL || infix2UPN(iP+1, UPN)==NULL) return NULL; 
	    return strcat(UPN,"| ");
	  } 
      }

  for (iP=infix; *iP!='\0'; iP++)   /* search for middle priority operators: logical AND: '&' */
      switch (*iP)
      { case '(': level++; break;
        case ')': level--; if (level<0) { ERROR("Syntax error in question"); return NULL; } break;
        case '&': if (level==0) 
	  { *iP='\0'; 
	    if (infix2UPN(infix,UPN)==NULL || infix2UPN(iP+1 ,UPN)==NULL) return NULL; 
	    return strcat(UPN,"& "); 
	  } 
      }

  for (iP=infix; *iP!='\0'; iP++)   /* search for middle priority operators: logical unary NOT: '~' */
      switch (*iP)
      { case '(': level++; break;
        case ')': level--; if (level<0) { ERROR("Syntax error in question"); return NULL; } break;
        case '~': if (level==0) 
	  { *iP='\0'; 
	    if (infix2UPN(iP+1,UPN)==NULL) return NULL;
	    return strcat(UPN,"~ ");
	  } 
      }

  if (level) { ERROR("Syntax error in question"); return NULL; } 

  /* if no operators were found, remove parentheses */

  endP = infix+strlen(infix)-1;

  if (*infix=='(' && *endP==')') { *endP='\0'; infix2UPN(infix+1,UPN); } 
  else { strcat (UPN,infix); strcat(UPN," "); }

  return UPN;
}

char *questionInfix2UPN (char* infix)
{
  static char UPN[240];
  char inf[120], *p1, *p2;

  UPN[0]='\0';

  if (strlen(infix)>120) { ERROR("Question too long"); return NULL; }

  for (p1=infix, p2=inf; *p1!='\0'; p1++)  /* copy all but white space */
    if (*p1>' ') *p2++ = *p1;
  *p2='\0';

  infix2UPN(inf,UPN);
  return UPN;
}

int i2u ( ClientData cd, Tcl_Interp *interp, int argc, char *argv[] )
{
  printf("%s\n",questionInfix2UPN(argv[1]));
  return TCL_OK;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method questionMethod[] =
{
  { "puts",   questionPutsItf,    "display one single question"           },
  { "answer", questionAnswerItf,  "answer a question for a named context" },
  { NULL, NULL, NULL }
} ;

TypeInfo questionInfo = { "Question", 0, -1, questionMethod, 
                                  NULL, NULL, 
                                  questionConfigureItf, NULL, itfTypeCntlDefaultNoLink,
      "A 'Question' object is a definition of a single question." };

static Method questionSetMethod[] = 
{
  { "puts",   questionSetPutsItf,   "displays the contents of a questionSet object" },
  { "add",    questionSetAddItf,    "add a new question to a questionSet object"    },
  { "delete", questionSetDeleteItf, "remove a question from a questionSet object"   },
  { "read",   questionSetReadItf,   "read questionSet from a file"                  },
  { "write",  questionSetWriteItf,  "write questionSet into a file"                 },
  { "index",  questionSetIndexItf,  "return the index of a named question"        },
  { "name",   questionSetNameItf,   "return the name of an indexed question"      },
  { NULL, NULL, NULL }
} ;

TypeInfo questionSetInfo = { "QuestionSet", 0, -1, questionSetMethod, 
                                   questionSetCreateItf, questionSetFreeItf, 
                                   questionSetConfigureItf, questionSetAccessItf, NULL, 
  "A 'QuestionSet' object is a set of characteristic function definitions and a set of questionSet." };

static int questionSetInitialized = 0;

int Questions_Init ()
{
  if (! questionSetInitialized) {
    if (Tags_Init()   != TCL_OK) return TCL_ERROR;
    if (Phones_Init() != TCL_OK) return TCL_ERROR;

    questionSetDefault.name            =  NULL;
    questionSetDefault.useN            =  0;
    questionSetDefault.list.itemN      =  0;
    questionSetDefault.list.blkSize    =  50;
    questionSetDefault.phones          =  NULL;
    questionSetDefault.phonesSet       =  NULL;
    questionSetDefault.tags            =  NULL;
    questionSetDefault.padPhoneX       = -1;
    questionSetDefault.commentChar     = ';';
    questionSetDefault.tagOperation    =  1;

    itfNewType(&questionInfo);
    itfNewType(&questionSetInfo);
 
    Tcl_CreateCommand (itf,"i2u",i2u, 0, 0 );
    questionSetInitialized = 1;
  }
  return TCL_OK;
}

