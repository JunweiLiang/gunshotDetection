/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Set of Hme's for acoustic modeling
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hmeSet.c
    Date    :  $Id: hmeSet.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:20  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.1  2002/06/26 12:26:50  fuegen
    go away, boring message

    Revision 3.3  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.40.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  08:58:48  fritsch
 * Initial revision
 * 

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "feature.h"
#include "phones.h"
#include "word.h"
#include "stream.h"
#include "mach_ind_io.h"
#include "modelSet.h"
#include "hme.h"
#include "hmeSet.h"


#define     MODEL_BLOCKSIZE     50
#define     COMMENT_CHAR        ';'
#define     HMESET_MAGIC        3456
#define     STATE_MAGIC         3
#define     MONO_MAGIC          0
#define     CTX_MAGIC           1
#define     EOF_MAGIC           2



/* ======================================================================= */
/*                              HmeModelItem                               */
/* ======================================================================= */

int  hmeModelItemInit (HmeModelItem *item, ClientData cd) {

  item->name = strdup((char*) cd);
  return TCL_OK;
}


int  hmeModelItemDeinit (HmeModelItem *item) {

  if (item->name) {
    free(item->name);
    item->name = NULL;
  }
  return TCL_OK;
}


int  hmeModelItemPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeModelItem    *item = (HmeModelItem *) cd;
  
  itfAppendResult("%s %d %d %d ",item->name,
		  item->tagIdx,item->phoneIdx,item->ctxIdx);
  return TCL_OK;
}


/* ----------------------------------------------------------------------- */
/*  HmeModelItem                                                           */
/* ----------------------------------------------------------------------- */

static Method hmeModelItemMethod[] = 
{
  { "puts",         hmeModelItemPutsItf,        NULL},
  {  NULL,  NULL, NULL } 
};



TypeInfo hmeModelItemInfo = {
                  "HmeModelItem", 0, -1, hmeModelItemMethod, 
                  NULL, NULL,
                  NULL, NULL,
                  itfTypeCntlDefaultNoLink,
                  "Model name to Hme output nodes mapping item\n" } ;




/* ======================================================================= */
/*                             HmeTagMapItem                               */
/* ======================================================================= */

int  hmeTagMapItemInit (HmeModelItem *item, ClientData cd) {

  item->name = strdup((char*) cd);
  return TCL_OK;
}


int  hmeTagMapItemDeinit (HmeModelItem *item) {

  if (item->name) {
    free(item->name);
    item->name = NULL;
  }
  return TCL_OK;
}


int  hmeTagMapItemPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeTagMapItem    *item = (HmeTagMapItem *) cd;
  
  itfAppendResult("%s %d ",item->name,item->index);
  return TCL_OK;
}


/* ----------------------------------------------------------------------- */
/*  HmeTagMapItem                                                          */
/* ----------------------------------------------------------------------- */

static Method hmeTagMapItemMethod[] = 
{
  { "puts",         hmeTagMapItemPutsItf,        NULL},
  {  NULL,  NULL, NULL } 
};



TypeInfo hmeTagMapItemInfo = {
                  "HmeTagMapItem", 0, -1, hmeTagMapItemMethod, 
                  NULL, NULL,
                  NULL, NULL,
                  itfTypeCntlDefaultNoLink,
                  "Tag name to HmeState index mapping item\n" } ;




/* ======================================================================= */
/*                   HmeStateSet implementation functions                  */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  hmeStateSetCreate                                                      */
/* ----------------------------------------------------------------------- */
HmeStateSet  *hmeStateSetCreate (int phonesN) {

  HmeStateSet        *hmeStateSet;
  register int       phoneX;

  hmeStateSet = (HmeStateSet *) malloc (sizeof(HmeStateSet));
  
  if (!hmeStateSet) {
    ERROR("Failed to allocate HmeStateSet object\n");
    return NULL;
  }

  hmeStateSet->name       = NULL;
  hmeStateSet->phonesN    = phonesN;
  hmeStateSet->mono       = NULL;
  hmeStateSet->monoActive = 0;
  hmeStateSet->ctx        = (Hme **) malloc (sizeof(Hme *) * phonesN);
  hmeStateSet->smooth     = (double *) malloc (sizeof(double) * phonesN);
  hmeStateSet->active     = (int *) malloc (sizeof(int) * phonesN);
  for (phoneX=0; phoneX<phonesN; phoneX++) {
    hmeStateSet->ctx[phoneX] = NULL;
    hmeStateSet->smooth[phoneX] = 1.0;
    hmeStateSet->active[phoneX] = 0;
  }
  
  return hmeStateSet;
}



/* ----------------------------------------------------------------------- */
/*  hmeStateSetFree                                                        */
/* ----------------------------------------------------------------------- */
int  hmeStateSetFree (HmeStateSet *hmeStateSet) {

  register int          phoneX;
  
  if (!hmeStateSet) return TCL_OK;

  if (hmeStateSet->name) free(hmeStateSet->name);
  
  if (hmeStateSet->mono) hmeFree(hmeStateSet->mono);
  for (phoneX=0; phoneX<hmeStateSet->phonesN; phoneX++)
    if (hmeStateSet->ctx[phoneX]) hmeFree(hmeStateSet->ctx[phoneX]);

  free(hmeStateSet->ctx);
  free(hmeStateSet->smooth);
  free(hmeStateSet->active);
  free(hmeStateSet);
  
  return TCL_OK;
}



/* ======================================================================= */
/*                     HmeSet implementation functions                     */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  hmeSetInit                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetInit (HmeSet *hmeSet, char *name, int stateN, Phones *phones,
          	 FeatureSet *fes,
	         char *gateFeatName, int gateDimN,
	         char *expFeatName, int expDimN) {

  register int        phonesN,stateX;

  hmeSet->name        = strdup(name);
  hmeSet->stateN      = stateN;
  hmeSet->divPrior    = 0;
  hmeSet->monoOnly    = 0;
  hmeSet->globSmooth  = 1.0;
  hmeSet->scoreMul    = 1.0;

  /* -------------------------------------------------- */
  /*  get the number of phones and check phones object  */
  /* -------------------------------------------------- */
  phonesN = phones->list.itemN;
  if (phonesN == 0) {
    ERROR("Phones object seems to be empty\n");
    return TCL_ERROR;
  }
  hmeSet->phones = phones;
  link(phones,"Phones");
  
  /* ---------------------------------------- */
  /*  resolve Feature names using FeatureSet  */
  /* ---------------------------------------- */
  hmeSet->fes         = fes;
  hmeSet->gateFeatIdx = fesIndex(fes,gateFeatName,FE_FMATRIX,gateDimN,1);
  hmeSet->expFeatIdx  = fesIndex(fes,expFeatName, FE_FMATRIX,expDimN, 1);

  if ((hmeSet->gateFeatIdx < 0) || (hmeSet->expFeatIdx < 0)) {
    ERROR("Error while resolving FeatureSet name\n");
    return TCL_ERROR;
  }
  
  link(fes,"FeatureSet");

  /* ---------------------- */
  /*  init model name list  */
  /* ---------------------- */
  listInit((List*) &(hmeSet->list), &hmeModelItemInfo,
	   sizeof(HmeModelItem), MODEL_BLOCKSIZE);
  hmeSet->list.init   = (ListItemInit   *) hmeModelItemInit;
  hmeSet->list.deinit = (ListItemDeinit *) hmeModelItemDeinit;

  /* ----------------------- */
  /*  init tag mapping list  */
  /* ----------------------- */
  listInit((List*) &(hmeSet->tagMap), &hmeTagMapItemInfo,
	   sizeof(HmeTagMapItem), 1);
  hmeSet->tagMap.init   = (ListItemInit   *) hmeTagMapItemInit;
  hmeSet->tagMap.deinit = (ListItemDeinit *) hmeTagMapItemDeinit;
  
  /* ------------------------------- */
  /*  init state discriminating Hme  */
  /* ------------------------------- */
  hmeSet->sdh = NULL;
  hmeSet->sdhActive = 0;
  
  /* ---------------------- */
  /*  create HmeStateSet's  */
  /* ---------------------- */
  hmeSet->state = (HmeStateSet **) malloc (sizeof(HmeStateSet *) * stateN);
  for (stateX=0; stateX<stateN; stateX++)
    hmeSet->state[stateX] = hmeStateSetCreate(phonesN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetCreate                                                           */
/* ----------------------------------------------------------------------- */
HmeSet  *hmeSetCreate (char *name, int stateN, Phones *phones,
		       FeatureSet *fes,
	               char *gateFeatName, int gateDimN,
	               char *expFeatName, int expDimN) {

  HmeSet        *hmeSet = (HmeSet *) malloc (sizeof(HmeSet));

  if (!hmeSet || (hmeSetInit(hmeSet,name,stateN,phones,fes,gateFeatName,
			    gateDimN,expFeatName,expDimN) != TCL_OK)) {
    ERROR("Failed to allocate HmeSet object\n");
    return NULL;
  }

  return hmeSet;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetDeinit                                                           */
/* ----------------------------------------------------------------------- */
int  hmeSetDeinit (HmeSet *hmeSet) {

  register int         stateX,phonesN;

  free(hmeSet->name);
  phonesN = hmeSet->phones->list.itemN;

  if (hmeSet->phones) unlink(hmeSet->phones,"Phones");
  if (hmeSet->fes)    unlink(hmeSet->fes,"FeatureSet");

  listDeinit((List *) &(hmeSet->list));
  listDeinit((List *) &(hmeSet->tagMap));
  
  if (hmeSet->sdh) {
    hmeFree(hmeSet->sdh);
    hmeSet->sdh = NULL;
    hmeSet->sdhActive = 0;
  }

  for (stateX=0; stateX<hmeSet->stateN; stateX++)
    hmeStateSetFree(hmeSet->state[stateX]);
  free(hmeSet->state);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFree                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetFree (HmeSet *hmeSet) {

  hmeSetDeinit(hmeSet);
  free(hmeSet);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetTagIndex                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetTagIndex (HmeSet *hmeSet, char *tag) {

  int              stateX,index = -2;
  int              idx;

  if (!hmeSet) return index;
  if (strcmp(tag,"-") == 0) return -1;

  /* --------------------------------------------- */
  /*  search for an entry in the tag mapping list  */
  /* --------------------------------------------- */
  if ((idx = listIndex((List*) &(hmeSet->tagMap), tag, 0)) >= 0) {
    index = hmeSet->tagMap.itemA[idx].index;
    return index;
  }
  
  /* --------------------------------------- */
  /*  search for an existing tag/state name  */
  /* --------------------------------------- */
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    if (hmeSet->state[stateX]->name) {
      if (strcmp(hmeSet->state[stateX]->name,tag) == 0) {
        index = stateX;
	break;
      }
    } else break;
  }

  /* ------------------------------------------ */
  /*  allocate new state name, if enough space  */
  /* ------------------------------------------ */
  if ((index < 0) && (stateX < hmeSet->stateN)) {
    hmeSet->state[stateX]->name = strdup(tag);
    index = stateX;
  }

  return index;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetPhoneIndex                                                       */
/* ----------------------------------------------------------------------- */
int  hmeSetPhoneIndex (HmeSet *hmeSet, char *name) {

  int           index;
  
  if (!hmeSet) return -2;
  if (strcmp(name,"-") == 0) return -1;

  index = phonesIndex(hmeSet->phones,name);
  if (index < 0) index--;
  
  return index;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAdd                                                              */
/* ----------------------------------------------------------------------- */
int  hmeSetAdd (HmeSet *hmeSet, char *tag, char *phone, Hme *hme) {

  int                   tagIdx,phoneIdx;

  tagIdx   = hmeSetTagIndex(hmeSet,tag);
  phoneIdx = hmeSetPhoneIndex(hmeSet,phone);

  if ((tagIdx < -1) || (phoneIdx < -1)) {
    ERROR("invalid tag or phone name\n");
    return TCL_ERROR;
  }
    
  if (hme->gateDimN != hmeSet->fes->featA[hmeSet->gateFeatIdx].dcoeffN)
    WARN(" CAUTION: hme->gateDimN != dcoeffN\n");
  if (hme->expDimN != hmeSet->fes->featA[hmeSet->expFeatIdx].dcoeffN)
    WARN(" CAUTION: hme->expDimN != dcoeffN\n");

  if (tagIdx != -1) {
    
    if (phoneIdx == -1) {
      if (hme->classN != hmeSet->phones->list.itemN)
        WARN(" CAUTION: CI hme->classN != phonesN\n");
      if (hmeSet->state[tagIdx]->mono) {
        hmeFree(hmeSet->state[tagIdx]->mono);
      }
      hmeSet->state[tagIdx]->mono = hme;
      hmeSet->state[tagIdx]->monoActive = 1;
    } else {
      if (hmeSet->state[tagIdx]->ctx[phoneIdx]) {
        hmeFree(hmeSet->state[tagIdx]->ctx[phoneIdx]);
      }    
      hmeSet->state[tagIdx]->ctx[phoneIdx] = hme;
      hmeSet->state[tagIdx]->active[phoneIdx] = 1;
    }
    
  } else {

    if (hme->classN != hmeSet->stateN)
      WARN(" CAUTION: SDH hme->classN != hmeSet->stateN\n");
    if (hmeSet->sdh) {
      hmeFree(hmeSet->sdh);
    }
    hmeSet->sdh = hme;
    hmeSet->sdhActive = 1;
 
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetSave                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetSave (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet       *hmeSS;
  int               phoneX,stateX;
  
  write_int(fp,HMESET_MAGIC);

  write_int(fp,hmeSet->stateN);
  write_int(fp,hmeSet->phones->list.itemN);
  write_int(fp,hmeSet->divPrior);
  write_int(fp,hmeSet->monoOnly);
  write_float(fp,(float) hmeSet->globSmooth);
  write_float(fp,(float) hmeSet->scoreMul);
  
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    write_string(fp,hmeSS->name);
  }

  /* ------------------------------- */
  /*  save state discriminating Hme  */
  /* ------------------------------- */
  if (hmeSet->sdh) {
    write_int(fp,STATE_MAGIC);
    write_int(fp,hmeSet->sdhActive);
    hmeSave(hmeSet->sdh,fp);
  }

  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];

    /* ---------------------- */
    /*  saving monophone Hme  */
    /* ---------------------- */
    if (hmeSS->mono) {
      write_int(fp,MONO_MAGIC);
      write_int(fp,stateX);
      write_int(fp,hmeSS->monoActive);
      hmeSave(hmeSS->mono,fp);
    }

    /* ---------------------- */
    /*  saving context Hme's  */
    /* ---------------------- */
    for (phoneX=0; phoneX<hmeSS->phonesN; phoneX++) {
      if (hmeSS->ctx[phoneX]) {
	write_int(fp,CTX_MAGIC);
	write_int(fp,stateX);
	write_int(fp,phoneX);
	write_int(fp,hmeSS->active[phoneX]);
	write_float(fp,(float) hmeSS->smooth[phoneX]);
	hmeSave(hmeSS->ctx[phoneX],fp);
      }
    }
  }

  write_int(fp,EOF_MAGIC);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetLoad                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetLoad (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet      *hmeSS;
  char             l_name[255];
  int              stateX,phoneX,active,magic;
  double           smooth;
  
  if (read_int(fp) != HMESET_MAGIC) {
    ERROR("did not find HmeSet magic\n");
    return TCL_ERROR;
  }

  if (read_int(fp) != hmeSet->stateN) {
    ERROR("incompatible number of states\n");
    return TCL_ERROR;
  }

  if (read_int(fp) != hmeSet->phones->list.itemN) {
    ERROR("incompatible number of phones\n");
    return TCL_ERROR;
  }
  
  hmeSet->divPrior   = read_int(fp);
  hmeSet->monoOnly   = read_int(fp);
  hmeSet->globSmooth = (double) read_float(fp);
  hmeSet->scoreMul   = (double) read_float(fp);
  
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    read_string(fp,l_name);
    if ((hmeSS->name) && strcmp(hmeSS->name,l_name)){
      WARN("replacing tag name '%s' by '%s' in stateX=%d\n",
	   hmeSS->name,l_name,stateX);
      free(hmeSS->name);
      hmeSS->name = NULL;
    }
    if (!hmeSS->name) hmeSS->name = strdup(l_name);
  }

  do {

    magic = read_int(fp);
    switch (magic) {
      case STATE_MAGIC:
        active = read_int(fp);
	if (hmeSet->sdh) {
	  hmeFree(hmeSet->sdh);
	}
	hmeSet->sdh = hmeLoad(NULL,fp);
	hmeSet->sdhActive = active;
	break;
	
      case MONO_MAGIC:
        stateX = read_int(fp);
	active = read_int(fp);
	hmeSS  = hmeSet->state[stateX];
	if (hmeSS->mono) {
	  hmeFree(hmeSS->mono);
	}
	hmeSS->mono = hmeLoad(NULL,fp);
	hmeSS->monoActive = active;
	break;
	
      case CTX_MAGIC:
        stateX = read_int(fp);
        phoneX = read_int(fp);
	active = read_int(fp);
	smooth = (double) read_float(fp);
	hmeSS  = hmeSet->state[stateX];
	if (hmeSS->ctx[phoneX]) {
	  hmeFree(hmeSS->ctx[phoneX]);
	}
	hmeSS->ctx[phoneX] = hmeLoad(NULL,fp);
	hmeSS->active[phoneX] = active;
	hmeSS->smooth[phoneX] = smooth;
	break;
	
      default: break;
    }

  } while (magic != EOF_MAGIC);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetActivate                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetActivate (HmeSet *hmeSet, int tagIdx, int phoneIdx, int active) {

  HmeStateSet            *hmeSS;
  register int           phoneX,stateX;
  
  if (tagIdx < 0) {

    if (phoneIdx < -1) {

      for (stateX=0; stateX<hmeSet->stateN; stateX++) {
	hmeSS = hmeSet->state[stateX];
	if ((hmeSS->mono && active) || !active)
	  hmeSS->monoActive = active;
        for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
          if ((hmeSS->ctx[phoneX] && active) || (!active))
	    hmeSS->active[phoneX] = active;
      }
      
    } else {

      for (stateX=0; stateX<hmeSet->stateN; stateX++) {
	hmeSS = hmeSet->state[stateX];
        if (phoneIdx == -1) {
	  if ((hmeSS->mono && active) || !active)
	    hmeSS->monoActive = active;
        } else {
          if ((hmeSS->ctx[phoneIdx] && active) || (!active))
	    hmeSS->active[phoneIdx] = active;
	}
      }	
    }
      
  } else {

    hmeSS = hmeSet->state[tagIdx];
    
    if (phoneIdx < -1) {

      if ((hmeSS->mono && active) || !active)
        hmeSS->monoActive = active;
      for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
        if ((hmeSS->ctx[phoneX] && active) || (!active))
          hmeSS->active[phoneX] = active;

    } else {

      if (phoneIdx == -1) {
        if ((hmeSS->mono && active) || !active)
          hmeSS->monoActive = active;
      } else {
        if ((hmeSS->ctx[phoneIdx] && active) || (!active))
          hmeSS->active[phoneIdx] = active;
      }
      
    }

  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetIndex                                                            */
/* ----------------------------------------------------------------------- */
int  hmeSetIndex (ClientData cd, char *name) {

  HmeSet *hmeSet = (HmeSet *) cd;
  
  return listIndex((List*) &(hmeSet->list), name, 0);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetName                                                             */
/* ----------------------------------------------------------------------- */
char  *hmeSetName (ClientData cd, int index) {

  HmeSet *hmeSet = (HmeSet *) cd;

  return (index>=0 && index<hmeSet->list.itemN) ?
         hmeSet->list.itemA[index].name : "(null)";
}



/* ----------------------------------------------------------------------- */
/*  hmeSetDist   --  makes no sense here, therefore empty                  */
/* ----------------------------------------------------------------------- */
int  hmeSetDist (ClientData cd, ModelArray *maP, ModelArray *maQ,
		 ModelArray *maR, float *distP) {

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetReset                                                            */
/* ----------------------------------------------------------------------- */
int  hmeSetReset (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeReset(hmeSet->sdh);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeReset(hmeSS->mono);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeReset(hmeSS->ctx[phoneX]);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetConfHmes                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetConfHmes (HmeSet *hmeSet, int argc, char *argv[]) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeConfHme(hmeSet->sdh,argc,argv);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeConfHme(hmeSS->mono,argc,argv);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeConfHme(hmeSS->ctx[phoneX],argc,argv);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetConfGates                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetConfGates (HmeSet *hmeSet, int argc, char *argv[]) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeConfGates(hmeSet->sdh,argc,argv);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeConfGates(hmeSS->mono,argc,argv);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeConfGates(hmeSS->ctx[phoneX],argc,argv);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetConfExperts                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetConfExperts (HmeSet *hmeSet, int argc, char *argv[]) {

  HmeStateSet             *hmeSS;
  register int            stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeConfExperts(hmeSet->sdh,argc,argv);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeConfExperts(hmeSS->mono,argc,argv);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeConfExperts(hmeSS->ctx[phoneX],argc,argv);
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetScore                                                            */
/* ----------------------------------------------------------------------- */
float  hmeSetScore (HmeSet *hmeSet, int idx, int frameX) {

  HmeStateSet          *hmeSS;
  Hme                  *hme;
  HmeModelItem         item;
  register int         tagIdx,phoneIdx,ctxIdx;
  register double      cdsc,score = 0.0;
  register float       *gf,*ef;

  if ((idx < 0) || (idx >= hmeSet->list.itemN)) {
    ERROR("invalid model index %d\n",idx);
    return 0.0;
  }

  /* --------------------------- */
  /*  get Hme model information  */
  /* --------------------------- */
  item     = hmeSet->list.itemA[idx];
  tagIdx   = item.tagIdx;
  phoneIdx = item.phoneIdx;
  ctxIdx   = item.ctxIdx;

  /* --------------------- */
  /*  get feature vectors  */
  /* --------------------- */
  gf = fesFrame(hmeSet->fes,hmeSet->gateFeatIdx,frameX);
  ef = fesFrame(hmeSet->fes,hmeSet->expFeatIdx,frameX);

  /* ----------------------------------- */
  /*  Evaluate state discriminating Hme  */
  /* ----------------------------------- */
  if (hmeSet->sdh && hmeSet->sdhActive)
    score = hmeScore(hmeSet->sdh,gf,ef,frameX,tagIdx,hmeSet->divPrior);
  
  /* -------------------------------------------- */
  /*  Evaluate context-independent monophone Hme  */
  /* -------------------------------------------- */
  hmeSS  = hmeSet->state[tagIdx];
  hme    = hmeSS->mono;
  if (hme && hmeSS->monoActive)
    score += hmeScore(hme,gf,ef,frameX,phoneIdx,hmeSet->divPrior);
  if (hmeSet->monoOnly)
    return ((float) (hmeSet->scoreMul * score));

  /* -------------------------------- */
  /*  Evaluate context-dependent Hme  */
  /* -------------------------------- */
  hme = hmeSS->ctx[phoneIdx];
  if (hme && hmeSS->active[phoneIdx]) {
    cdsc = hmeScore(hme,gf,ef,frameX,ctxIdx,hmeSet->divPrior);
    score += (hmeSet->globSmooth * hmeSS->smooth[phoneIdx] * cdsc);
  }

  return ((float) (hmeSet->scoreMul * score));
}



/* ----------------------------------------------------------------------- */
/*  hmeSetCreateAccus                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetCreateAccus (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeCreateAccus(hmeSet->sdh);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeCreateAccus(hmeSS->mono);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeCreateAccus(hmeSS->ctx[phoneX]);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFreeAccus                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetFreeAccus (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeFreeAccus(hmeSet->sdh);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeFreeAccus(hmeSS->mono);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeFreeAccus(hmeSS->ctx[phoneX]);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetClearAccus                                                       */
/* ----------------------------------------------------------------------- */
int  hmeSetClearAccus (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeClearAccus(hmeSet->sdh);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeClearAccus(hmeSS->mono);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeClearAccus(hmeSS->ctx[phoneX]);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetSaveAccus                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetSaveAccus (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeSaveAccus(hmeSet->sdh,fp);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeSaveAccus(hmeSS->mono,fp);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeSaveAccus(hmeSS->ctx[phoneX],fp);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetLoadAccus                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetLoadAccus (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;
  
  if (hmeSet->sdh && hmeSet->sdhActive) hmeLoadAccus(hmeSet->sdh,fp);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeLoadAccus(hmeSS->mono,fp);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeLoadAccus(hmeSS->ctx[phoneX],fp);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAccu                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetAccu (HmeSet *hmeSet, int idx, int frameX, float factor) {

  HmeStateSet          *hmeSS;
  Hme                  *hme;
  HmeModelItem         item;
  register int         tagIdx,phoneIdx,ctxIdx;
  register float       *gf,*ef;

  if ((idx < 0) || (idx >= hmeSet->list.itemN)) {
    ERROR("invalid model index %d\n",idx);
    return 0.0;
  }

  if (factor < 1.0) WARN("Using 1.0 instead of %f as path state weight\n",
			 factor);

  /* --------------------------- */
  /*  get Hme model information  */
  /* --------------------------- */
  item     = hmeSet->list.itemA[idx];
  tagIdx   = item.tagIdx;
  phoneIdx = item.phoneIdx;
  ctxIdx   = item.ctxIdx;

  /* --------------------- */
  /*  get feature vectors  */
  /* --------------------- */
  gf = fesFrame(hmeSet->fes,hmeSet->gateFeatIdx,frameX);
  ef = fesFrame(hmeSet->fes,hmeSet->expFeatIdx,frameX);

  /* ------------------------------- */
  /*  accu state discriminating Hme  */
  /* ------------------------------- */
  if (hmeSet->sdh && hmeSet->sdhActive)
    hmeAccu(hmeSet->sdh,gf,ef,tagIdx);

  /* ----------------------- */
  /*  accu CI monophone Hme  */
  /* ----------------------- */
  hmeSS  = hmeSet->state[tagIdx];
  hme    = hmeSS->mono;
  if (hme && hmeSS->monoActive) hmeAccu(hme,gf,ef,phoneIdx);
  if (hmeSet->monoOnly) return TCL_OK;
  
  /* ------------- */
  /*  accu CD Hme  */
  /* ------------- */
  hme = hmeSS->ctx[phoneIdx];
  if (hme && hmeSS->active[phoneIdx]) hmeAccu(hme,gf,ef,ctxIdx);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetUpdate                                                           */
/* ----------------------------------------------------------------------- */
int  hmeSetUpdate (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeUpdate(hmeSet->sdh);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeUpdate(hmeSS->mono);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeUpdate(hmeSS->ctx[phoneX]);
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFrameN                                                           */
/* ----------------------------------------------------------------------- */
int  hmeSetFrameN (HmeSet *hmeSet, int *from, int *shift, int *ready) {

  int          frameX[2];
  
  if (hmeSet->fes == NULL) {
    ERROR("no feature set connected to HmeSet object\n");
    return TCL_ERROR;
  }

  frameX[0] = hmeSet->gateFeatIdx;
  frameX[1] = hmeSet->expFeatIdx;
  
  return (fesFrameN(hmeSet->fes,frameX,2,from,shift,ready));
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFesUsed                                                          */
/* ----------------------------------------------------------------------- */
FeatureSet  *hmeSetFesUsed (HmeSet *hmeSet) {

  return hmeSet->fes;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetCreateTest                                                       */
/* ----------------------------------------------------------------------- */
int  hmeSetCreateTest (HmeSet *hmeSet, int cidataN, int ciskipN,
		       int cddataN, int cdskipN) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive)
    hmeCreateTest(hmeSet->sdh,cidataN,ciskipN);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeCreateTest(hmeSS->mono,cidataN,ciskipN);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX])
	hmeCreateTest(hmeSS->ctx[phoneX],cddataN,cdskipN);
  }
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFreeTest                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetFreeTest (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeTestEnvFree(hmeSet->sdh->testEnv);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeTestEnvFree(hmeSS->mono->testEnv);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeTestEnvFree(hmeSS->ctx[phoneX]->testEnv);
  }
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetSaveTest                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetSaveTest (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeSaveTestEnv(hmeSet->sdh,fp);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeSaveTestEnv(hmeSS->mono,fp);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeSaveTestEnv(hmeSS->ctx[phoneX],fp);
  }
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetLoadTest                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetLoadTest (HmeSet *hmeSet, FILE *fp) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeLoadTestEnv(hmeSet->sdh,fp);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeLoadTestEnv(hmeSS->mono,fp);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeLoadTestEnv(hmeSS->ctx[phoneX],fp);
  }
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetTest                                                             */
/* ----------------------------------------------------------------------- */
int  hmeSetTest (HmeSet *hmeSet) {

  HmeStateSet           *hmeSS;
  register int          stateX,phoneX;

  if (hmeSet->sdh && hmeSet->sdhActive) hmeTest(hmeSet->sdh,0);
  for (stateX=0; stateX<hmeSet->stateN; stateX++) {
    hmeSS = hmeSet->state[stateX];
    if (hmeSS->monoActive) hmeTest(hmeSS->mono,0);
    for (phoneX=0; phoneX<hmeSet->phones->list.itemN; phoneX++)
      if (hmeSS->active[phoneX]) hmeTest(hmeSS->ctx[phoneX],0);
  }
  return TCL_OK;
}




/* ======================================================================= */
/*                         HmeStateSet itf functions                       */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  hmeStateSetPutsItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeStateSetPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeStateSet         *hmeSS = (HmeStateSet *) cd;
  register int        phoneX;

  itfAppendResult("{name %s} {phonesN %d} ",hmeSS->name,hmeSS->phonesN);
  
  if (hmeSS->mono) itfAppendResult("{mono %s %d} ",hmeSS->mono->name,
				   hmeSS->monoActive);
            else   itfAppendResult("{mono N/A %d} ",hmeSS->monoActive);

  itfAppendResult("{ ctx ");
  for (phoneX=0; phoneX<hmeSS->phonesN; phoneX++) {
    if (hmeSS->ctx[phoneX])
      itfAppendResult("{ %s ",hmeSS->ctx[phoneX]->name);
    else
      itfAppendResult("{ N/A ");
      
    itfAppendResult("%f %d } ",hmeSS->smooth[phoneX],hmeSS->active[phoneX]);
  }
  itfAppendResult("} ");
       
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStateSetAccessItf                                                   */
/* ----------------------------------------------------------------------- */
ClientData  hmeStateSetAccessItf (ClientData cd, char *name, TypeInfo **ti) { 

  HmeStateSet        *hmeSS = (HmeStateSet *) cd;
  int                phoneX;
  
  if ( *name == '.') {

    if ( name[1] =='\0') {

      /* -------------------------------------- */
      /*  give information on possible accesses */
      /* -------------------------------------- */
      itfAppendResult("mono ");
      itfAppendResult("ctx(0..%d) ", hmeSS->phonesN-1);

    } else {

      /* ---------------- */
      /*  perform access  */
      /* ---------------- */
      if (!strcmp(name+1,"mono")) {
         *ti = itfGetType("Hme");
         return (ClientData) hmeSS->mono;
      }
      else if (sscanf(name+1,"ctx(%d)",&phoneX)==1) {
        *ti = itfGetType("Hme");
        if (phoneX >= 0 && phoneX < hmeSS->phonesN) 
          return (ClientData) (hmeSS->ctx[phoneX]);
	else
	  return NULL;
      }
    }      
  }

  *ti = NULL;
  return NULL;
}




/* ======================================================================= */
/*                            HmeSet itf functions                         */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeSetCreateItf                                                        */
/* ----------------------------------------------------------------------- */
ClientData  hmeSetCreateItf (ClientData cd, int argc, char *argv[]) {

  int            ac = argc-1;
  int            stateN=0,gateDimN=0,expDimN=0;
  Phones         *phones=NULL;
  FeatureSet     *fes=NULL;
  char           *gateFeatName=NULL;
  char           *expFeatName=NULL;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<stateN>",       ARGV_INT,    NULL, &stateN,       NULL,         "number of phonetic states",
      "<Phones>",       ARGV_OBJECT, NULL, &phones,       "Phones",     "Phones",
      "<FeatureSet>",   ARGV_OBJECT, NULL, &fes,          "FeatureSet", "FeatureSet",
      "<gateFeatName>", ARGV_STRING, NULL, &gateFeatName, NULL,         "name of Feature for gating networks",
      "<gateDimN>" ,    ARGV_INT,    NULL, &gateDimN,     NULL,         "dimension of feature vectors for gating networks",
      "<expFeatName>" , ARGV_STRING, NULL, &expFeatName,  NULL,         "name of Feature for expert networks",
      "<expDimN>" ,     ARGV_INT,    NULL, &expDimN,      NULL,         "dimension of feature vectors for expert networks",
      NULL) != TCL_OK) return NULL;

  return (ClientData) hmeSetCreate(argv[0],stateN,phones,fes,
				   gateFeatName,gateDimN,
                                   expFeatName,expDimN);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetFreeItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetFreeItf (ClientData cd) {

  return  hmeSetFree ((HmeSet *) cd);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetPutsItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeSet       *hmeSet = (HmeSet *) cd;

  itfAppendResult("{name %s} ",hmeSet->name);
  itfAppendResult("{stateN %d} {phonesN %d} {divPrior %d} {monoOnly %d} ",
		  hmeSet->stateN,hmeSet->phones->list.itemN,
		  hmeSet->divPrior,hmeSet->monoOnly);
  itfAppendResult("{sdhActive %d} {globSmooth %f} {scoreMul %f} ",
		  hmeSet->sdhActive,hmeSet->globSmooth,hmeSet->scoreMul);

  return TCL_OK;
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetConfigureItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetConfigureItf (ClientData cd, char *var, char *val) {

  HmeSet       *hmeSet = (HmeSet *) cd;
  
  if (! var) {
    ITFCFG(hmeSetConfigureItf,cd,"stateN");
    ITFCFG(hmeSetConfigureItf,cd,"gateFeatIdx");
    ITFCFG(hmeSetConfigureItf,cd,"expFeatIdx");
    ITFCFG(hmeSetConfigureItf,cd,"divPrior");
    ITFCFG(hmeSetConfigureItf,cd,"monoOnly");
    ITFCFG(hmeSetConfigureItf,cd,"sdhActive");
    ITFCFG(hmeSetConfigureItf,cd,"globSmooth");
    ITFCFG(hmeSetConfigureItf,cd,"scoreMul");
    return TCL_OK;
  }
  ITFCFG_Int    (var,val,"stateN",      hmeSet->stateN,      1);
  ITFCFG_Int    (var,val,"gateFeatIdx", hmeSet->gateFeatIdx, 1);
  ITFCFG_Int    (var,val,"expFeatIdx",  hmeSet->expFeatIdx,  1);
  ITFCFG_Int    (var,val,"divPrior",    hmeSet->divPrior,    0);
  ITFCFG_Int    (var,val,"monoOnly",    hmeSet->monoOnly,    0);
  ITFCFG_Int    (var,val,"sdhActive",   hmeSet->sdhActive,   0);
  ITFCFG_Float  (var,val,"globSmooth",  hmeSet->globSmooth,  0);
  ITFCFG_Float  (var,val,"scoreMul",    hmeSet->scoreMul,    0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAccessItf                                                        */
/* ----------------------------------------------------------------------- */
ClientData  hmeSetAccessItf (ClientData cd, char *name, TypeInfo **ti) { 

  HmeSet             *hmeSet = (HmeSet *) cd;
  int                mapIdx,stateX;
  
  if ( *name == '.') {

    if ( name[1] =='\0') {

      /* -------------------------------------- */
      /*  give information on possible accesses */
      /* -------------------------------------- */
      itfAppendResult("phones fes sdh ");
      if (hmeSet->list.itemN > 0)
	itfAppendResult("map(0..%d) ", hmeSet->list.itemN-1);
      if (hmeSet->tagMap.itemN > 0)
	itfAppendResult("tagMap(0..%d) ", hmeSet->tagMap.itemN-1);
      itfAppendResult("state(0..%d) ", hmeSet->stateN-1);

    } else {

      /* ---------------- */
      /*  perform access  */
      /* ---------------- */
      if (!strcmp(name+1,"phones")) {
         *ti = itfGetType("Phones");
         return (ClientData) hmeSet->phones;
      }
      else if (!strcmp(name+1,"fes")) {
         *ti = itfGetType("FeatureSet");
         return (ClientData) hmeSet->fes;
      }
      else if ((!strcmp(name+1,"sdh")) && (hmeSet->sdh)) {
         *ti = itfGetType("Hme");
         return (ClientData) hmeSet->sdh;
      }
      else if ((hmeSet->list.itemN > 0) &&
	       (sscanf(name+1,"map(%d)",&mapIdx)==1)) {
        *ti = itfGetType("HmeModelItem");
        if (mapIdx >= 0 && mapIdx < hmeSet->list.itemN) 
          return (ClientData) &(hmeSet->list.itemA[mapIdx]);
	else
	  return NULL;
      }
      else if ((hmeSet->tagMap.itemN > 0) &&
	       (sscanf(name+1,"tagMap(%d)",&mapIdx)==1)) {
        *ti = itfGetType("HmeTagMapItem");
        if (mapIdx >= 0 && mapIdx < hmeSet->tagMap.itemN) 
          return (ClientData) &(hmeSet->tagMap.itemA[mapIdx]);
	else
	  return NULL;
      }
      else if (sscanf(name+1,"state(%d)",&stateX)==1) {
        *ti = itfGetType("HmeStateSet");
        if (stateX >= 0 && stateX < hmeSet->stateN)
          return (ClientData) (hmeSet->state[stateX]);
	else
	  return NULL;
      }
    }
  }
  *ti = NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAddItf                                                           */
/* ----------------------------------------------------------------------- */
int  hmeSetAddItf (ClientData cd, int argc, char *argv[]) {

  HmeSet                *hmeSet = (HmeSet *) cd;
  Hme                   *hme;
  int                   ac;
  char                  *tag;
  char                  *phone;

  ac = (argc < 3)?(argc-1):2;
  if ( itfParseArgv (argv[0], &ac, argv+1, 1,
      "<tag>",    ARGV_STRING, NULL, &tag,   NULL, "tag/state name",
      "<phone>",  ARGV_STRING, NULL, &phone, NULL, "phone name ('-' for CI)",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((hme = (Hme *) hmeCreateItf(NULL,argc-3,argv+3)) == NULL) {
    ERROR("couldn't create Hme\n");
    return TCL_ERROR;
  }

  return  hmeSetAdd(hmeSet,tag,phone,hme);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetPlugItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetPlugItf (ClientData cd, int argc, char *argv[]) {

  HmeSet                *hmeSet = (HmeSet *) cd;
  int                   ac = argc-1;
  char                  *tag;
  char                  *phone;
  Hme                   *hme;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<tag>",    ARGV_STRING, NULL, &tag,   NULL, "tag name ('-' for SDH)",
      "<phone>",  ARGV_STRING, NULL, &phone, NULL, "phone name ('-' for CI)",
      "<Hme>",    ARGV_OBJECT, NULL, &hme,  "Hme", "Hme to plug in",
      NULL) != TCL_OK) return TCL_ERROR;

  return  hmeSetAdd(hmeSet,tag,phone,hme);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetReadItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetReadItf (ClientData cd, int argc, char *argv[]) {

  int             ac = argc-1;
  char            *filename;
  int             n;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((n = freadAdd(cd,filename,COMMENT_CHAR,hmeSetAddItf)) < 0) {
    ERROR("freadAdd() failure trying to read description file\n");
    return TCL_ERROR;
  }
  
  itfAppendResult("%d",n);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetSaveItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetSaveItf (ClientData cd, int argc, char *argv[]) {

  HmeSet           *hmeSet = (HmeSet *) cd;
  int              ac = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetSave(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetLoadItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetLoadItf (ClientData cd, int argc, char *argv[]) {

  HmeSet           *hmeSet = (HmeSet *) cd;
  int              ac = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetLoad(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAddMapItf                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetAddMapItf (ClientData cd, int argc, char *argv[]) {

  HmeSet                *hmeSet = (HmeSet *) cd;
  Hme                   *hme;
  int                   ac = argc-1;
  char                  *model;
  char                  *tag;
  char                  *phone;
  int                   contextX = 0;
  int                   tagIdx,phoneIdx,index;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<modelName>", ARGV_STRING, NULL, &model,    NULL, "model name",
      "<tag>",       ARGV_STRING, NULL, &tag,      NULL, "tag name",
      "<phone>",     ARGV_STRING, NULL, &phone,    NULL, "monophone name",
      "-ctx",        ARGV_INT,    NULL, &contextX, NULL, "context node index",
      NULL) != TCL_OK) return TCL_ERROR;

  tagIdx   = hmeSetTagIndex(hmeSet,tag);
  phoneIdx = hmeSetPhoneIndex(hmeSet,phone);

  if ((tagIdx < 0) || (phoneIdx < 0)) {
    ERROR("invalid tag or phone name\n");
    return TCL_ERROR;
  }
    
  if ((index = listIndex((List*) &(hmeSet->list), model, 0)) >= 0) {
    WARN("Hme model '%s' is already in mapping list at index %d\n",
	 model,index);
    return TCL_OK;
  }

  hme = hmeSet->state[tagIdx]->ctx[phoneIdx];
  if ((contextX < 0) || (hme && (contextX >= hme->classN))) {
    ERROR("Invalid context index %d for model '%s'\n",
	  contextX,model);
    return TCL_ERROR;
  }
  
  index = listIndex((List *) &(hmeSet->list), model, 1);
  hmeSet->list.itemA[index].tagIdx   = tagIdx;
  hmeSet->list.itemA[index].phoneIdx = phoneIdx;
  hmeSet->list.itemA[index].ctxIdx   = contextX;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetReadMapItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeSetReadMapItf (ClientData cd, int argc, char *argv[]) {

  int          ac = argc-1;
  char         *filename;
  int          n;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((n = freadAdd(cd,filename,COMMENT_CHAR,hmeSetAddMapItf)) < 0) {
    ERROR("freadAdd() failure trying to read ModelSet mapping file\n");
    return TCL_ERROR;
  }
  
  itfAppendResult("%d",n);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetAddTagMapItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetAddTagMapItf (ClientData cd, int argc, char *argv[]) {

  HmeSet                *hmeSet = (HmeSet *) cd;
  int                   ac = argc-1;
  int                   index,idx;
  char                  *tagName;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<tagName>",  ARGV_STRING, NULL, &tagName,    NULL, "tag name",
      "<tagIndex>", ARGV_INT,    NULL, &index,      NULL, "mapped tag index",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((index < 0) || (index >= hmeSet->stateN)) {
    ERROR("invalid mapped tag index\n");
    return TCL_ERROR;
  }
    
  if ((idx = listIndex((List*) &(hmeSet->tagMap), tagName, 0)) >= 0) {
    WARN("Tag '%s' is already in mapping list at index %d\n",
	 tagName,idx);
    return TCL_OK;
  }
  
  idx = listIndex((List *) &(hmeSet->tagMap), tagName, 1);
  hmeSet->tagMap.itemA[idx].index = index;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetReadTagMapItf                                                    */
/* ----------------------------------------------------------------------- */
int  hmeSetReadTagMapItf (ClientData cd, int argc, char *argv[]) {

  int          ac = argc-1;
  char         *filename;
  int          n;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((n = freadAdd(cd,filename,COMMENT_CHAR,hmeSetAddTagMapItf)) < 0) {
    ERROR("freadAdd() failure trying to read tag mapping file\n");
    return TCL_ERROR;
  }
  
  itfAppendResult("%d",n);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetActivateItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetActivateItf (ClientData cd, int argc, char *argv[]) {

  HmeSet           *hmeSet = (HmeSet *) cd;
  int              ac = argc-1;
  char             *tag = NULL;
  char             *phone = NULL;
  int              tagIdx,phoneIdx;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-tag",   ARGV_STRING, NULL, &tag,   NULL, "tag name",
      "-phone", ARGV_STRING, NULL, &phone, NULL, "phone name",
      NULL) != TCL_OK) return TCL_ERROR;

  if (tag == NULL) {
    tagIdx = -1;
  } else {
    tagIdx = hmeSetTagIndex(hmeSet,tag);
    if (tagIdx < 0) {
      ERROR("invalid tag name\n");
      return TCL_ERROR;
    }
  }
  
  if (phone == NULL) {
    phoneIdx = -2;
  } else {
    phoneIdx = hmeSetPhoneIndex(hmeSet,phone);
    if (phoneIdx < -1) {
      ERROR("invalid phone name\n");
      return TCL_ERROR;
    }
  }
  
  return  hmeSetActivate(hmeSet,tagIdx,phoneIdx,1);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetDeactivateItf                                                    */
/* ----------------------------------------------------------------------- */
int  hmeSetDeactivateItf (ClientData cd, int argc, char *argv[]) {

  HmeSet           *hmeSet = (HmeSet *) cd;
  int              ac = argc-1;
  char             *tag = NULL;
  char             *phone = NULL;
  int              tagIdx,phoneIdx;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-tag",   ARGV_STRING, NULL, &tag,   NULL, "tag name",
      "-phone", ARGV_STRING, NULL, &phone, NULL, "phone name",
      NULL) != TCL_OK) return TCL_ERROR;

  if (tag == NULL) {
    tagIdx = -1;
  } else {
    tagIdx = hmeSetTagIndex(hmeSet,tag);
    if (tagIdx < 0) {
      ERROR("invalid tag name\n");
      return TCL_ERROR;
    }
  }
  
  if (phone == NULL) {
    phoneIdx = -2;
  } else {
    phoneIdx = hmeSetPhoneIndex(hmeSet,phone);
    if (phoneIdx < -1) {
      ERROR("invalid phone name\n");
      return TCL_ERROR;
    }
  }
  
  return  hmeSetActivate(hmeSet,tagIdx,phoneIdx,0);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetSmoothItf                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetSmoothItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *tag,*phone;
  int               tagIdx,phoneIdx;
  float             smooth;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<tag>",    ARGV_STRING, NULL, &tag,    NULL, "tag name",
      "<phone>",  ARGV_STRING, NULL, &phone,  NULL, "phone name",
      "<smooth>", ARGV_FLOAT,  NULL, &smooth, NULL, "smoothing factor",
      NULL) != TCL_OK) return TCL_ERROR;

  tagIdx = hmeSetTagIndex(hmeSet,tag);
  phoneIdx = hmeSetPhoneIndex(hmeSet,phone);

  if ((tagIdx < 0) || (phoneIdx < 0)) {
    ERROR("invalid tag or phone name\n");
    return TCL_ERROR;
  }

  hmeSet->state[tagIdx]->smooth[phoneIdx] = (double) smooth;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetResetItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetResetItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetReset(hmeSet);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetConfHmesItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetConfHmesItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetConfHmes(hmeSet,argc,argv);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetConfGatesItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetConfGatesItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetConfGates(hmeSet,argc,argv);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetConfExpertsItf                                                   */
/* ----------------------------------------------------------------------- */
int  hmeSetConfExpertsItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetConfExperts(hmeSet,argc,argv);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetIndexItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetIndexItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char           *name;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>", ARGV_STRING, NULL, &name, NULL, "name of Hme model",
  NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%d",hmeSetIndex((ClientData) hmeSet,name));
  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetNameItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetNameItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  int            index;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<index>", ARGV_INT, NULL, &index, NULL, "index of Hme model",
  NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%s",hmeSetName((ClientData) hmeSet,index));
  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetScoreItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeSetScoreItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *name      = NULL;
  int               frameX     = 0;
  int               index;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",   ARGV_STRING, NULL, &name,   NULL, "name of a Hme model",
      "<frameX>", ARGV_INT,    NULL, &frameX, NULL, "index of feature vector",
    NULL) != TCL_OK) return TCL_ERROR;

  index = hmeSetIndex((ClientData) hmeSet,name);
  if (index < 0) return TCL_ERROR;

  itfAppendResult("%f",hmeSetScore(hmeSet,index,frameX));  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSetCreateAccusItf                                                   */
/* ----------------------------------------------------------------------- */
int  hmeSetCreateAccusItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetCreateAccus(hmeSet);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetFreeAccusItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetFreeAccusItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetFreeAccus(hmeSet);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetClearAccusItf                                                    */
/* ----------------------------------------------------------------------- */
int  hmeSetClearAccusItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetClearAccus(hmeSet);
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetSaveAccusItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetSaveAccusItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *filename;
  FILE              *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetSaveAccus(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetLoadAccusItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeSetLoadAccusItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *filename;
  FILE              *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetLoadAccus(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeSetAccuItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetAccuItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  char              *name;
  int               frameX,index;
  int               ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",   ARGV_STRING, NULL, &name,   NULL, "name of a Hme model",
      "<frameX>", ARGV_INT,    NULL, &frameX, NULL, "index of feature vector",
    NULL) != TCL_OK) return TCL_ERROR;

  index = hmeSetIndex((ClientData) hmeSet,name);
  if (index < 0) return TCL_ERROR;

  return  hmeSetAccu(hmeSet,index,frameX,1.0);
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetUpdateItf                                                        */
/* ----------------------------------------------------------------------- */
int  hmeSetUpdateItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  return  hmeSetUpdate(hmeSet);
}



/* ----------------------------------------------------------------------- */
/*  hmeSetCreateTestItf                                                    */
/* ----------------------------------------------------------------------- */
int  hmeSetCreateTestItf (ClientData cd, int argc, char *argv[]) {

  HmeSet          *hmeSet = (HmeSet *) cd;
  int             ac      = argc-1;
  int             cidataN = 1000;
  int             ciskipN = 10;
  int             cddataN = 100;
  int             cdskipN = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-cidataN", ARGV_INT, NULL, &cidataN, NULL, "size of CI test buffers",
      "-ciskipN", ARGV_INT, NULL, &ciskipN, NULL, "skipN for CI Hme's",
      "-cddataN", ARGV_INT, NULL, &cddataN, NULL, "size of CD test buffers",
      "-cdskipN", ARGV_INT, NULL, &cdskipN, NULL, "skipN for CD Hme's",
    NULL) != TCL_OK) return TCL_ERROR;

  return  hmeSetCreateTest(hmeSet,cidataN,ciskipN,cddataN,cdskipN);
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetFreeTestItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetFreeTestItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetFreeTest(hmeSet);
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetSaveTestItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetSaveTestItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *filename;
  FILE              *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetSaveTest(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetLoadTestItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeSetLoadTestItf (ClientData cd, int argc, char *argv[]) {

  HmeSet            *hmeSet = (HmeSet *) cd;
  int               ac = argc-1;
  char              *filename;
  FILE              *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (hmeSetLoadTest(hmeSet,fp) == TCL_ERROR)
    return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}

  

/* ----------------------------------------------------------------------- */
/*  hmeSetTestItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeSetTestItf (ClientData cd, int argc, char *argv[]) {

  HmeSet  *hmeSet = (HmeSet *) cd;
  return  hmeSetTest(hmeSet);
}

  

/* ======================================================================= */
/*                             module initialization                       */
/* ======================================================================= */
 

/* ----------------------------------------------------------------------- */
/*  HmeStateSet                                                            */
/* ----------------------------------------------------------------------- */

static Method hmeStateSetMethod[] = 
{
  { "puts",         hmeStateSetPutsItf,        NULL},
  {  NULL,  NULL, NULL } 
};



TypeInfo hmeStateSetInfo = {
                  "HmeStateSet", 0, -1, hmeStateSetMethod, 
                  NULL, NULL,
                  NULL, hmeStateSetAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "Set of CI and CD Hme's for one HMM state tag\n" } ;



/* ----------------------------------------------------------------------- */
/*  HmeSet                                                                 */
/* ----------------------------------------------------------------------- */

static Method hmeSetMethod[] = 
{
  { "puts",         hmeSetPutsItf,        NULL},
  { "add",          hmeSetAddItf,         "Add a new Hme to the HmeSet" },
  { "hme",          hmeSetPlugItf,        "Plug in an existing Hme" },
  { "read",         hmeSetReadItf,        "Read an HmeSet description file" },
  { "save",         hmeSetSaveItf,        "Save HmeSet" },
  { "load",         hmeSetLoadItf,        "Load HmeSet" },
  { "addMap",       hmeSetAddMapItf,      "Add a ModelSet mapping item" },
  { "readMap",      hmeSetReadMapItf,     "Read a ModelSet mapping file" },
  { "addTagMap",    hmeSetAddTagMapItf,   "Add a tag mapping item" },
  { "readTagMap",   hmeSetReadTagMapItf,  "Read a tag mapping file" },
  { "activate",     hmeSetActivateItf,    "Activate specific Hme(s)" },
  { "deactivate",   hmeSetDeactivateItf,  "Deactivate specific Hme(s)" },
  { "smooth",       hmeSetSmoothItf,      "Set smoothing factor for CD Hme" },
  { "reset",        hmeSetResetItf,       "Reset all active Hme's" },
  { "confHmes",     hmeSetConfHmesItf,    "Configure all active Hme's" },
  { "confGates",    hmeSetConfGatesItf,   "Configure all gates in all active Hme's" },
  { "confExperts",  hmeSetConfExpertsItf, "Configure all experts in all active Hme's" },
  { "index",        hmeSetIndexItf,       "Index of a given Hme model" },
  { "name",         hmeSetNameItf,        "Name of a given Hme model index" },
  { "score",        hmeSetScoreItf,       "Score a given Hme model" },
  { "createAccus",  hmeSetCreateAccusItf, "Create training accumulators" },
  { "freeAccus",    hmeSetFreeAccusItf,   "Free training accumulators" },
  { "clearAccus",   hmeSetClearAccusItf,  "Clear training accumulators" },
  { "saveAccus",    hmeSetSaveAccusItf,   "Save training accumulators" },
  { "loadAccus",    hmeSetLoadAccusItf,   "Load and accu training accumulators" },
  { "accu",         hmeSetAccuItf,        "Accumulate training information" },
  { "update",       hmeSetUpdateItf,      "Update Hme's" },
  { "createTest",   hmeSetCreateTestItf,  "Create test environments" },
  { "freeTest",     hmeSetFreeTestItf,    "Free test environments" },
  { "saveTest",     hmeSetSaveTestItf,    "Save test environments" },
  { "loadTest",     hmeSetLoadTestItf,    "Load test environments" },
  { "test",         hmeSetTestItf,        "Test all Hme's with test env" },
  {  NULL,  NULL, NULL }
};



TypeInfo hmeSetInfo = {
                  "HmeSet", 0, -1, hmeSetMethod, 
                  hmeSetCreateItf, hmeSetFreeItf,
                  hmeSetConfigureItf, hmeSetAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "Set of CI and CD Hme's for an HmeStream\n" } ;



static ModelSet  hmeSetModel = 
             { "HmeSet", &hmeSetInfo, &hmeSetInfo,
               (ModelSetIndexFct*) hmeSetIndex,
               (ModelSetNameFct *) hmeSetName,
               (ModelSetDistFct *) hmeSetDist };




static int HmeSetInitialized = 0;

/* ----------------------------------------------------------------------- */
/*  HmeSet_Init                                                            */
/* ----------------------------------------------------------------------- */
int HmeSet_Init () {

  if (HmeSetInitialized) return TCL_OK;

  itfNewType      (&hmeModelItemInfo);
  itfNewType      (&hmeTagMapItemInfo);
  itfNewType      (&hmeStateSetInfo);
  itfNewType      (&hmeSetInfo);
  modelSetNewType (&hmeSetModel);
  
  HmeSetInitialized = 1;
  return TCL_OK;
}
