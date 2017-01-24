/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Simple List Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  SimpleList.h
    Date    :  $Id: SimpleNode.h 2844 2008-11-25 12:00:00Z wolfel $

   ========================================================================

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

#ifndef SIMPLE_LIST_YET_INCLUDED
    #define SIMPLE_LIST_YET_INCLUDED

/*-----------------------------------------------------------------------------
/  INCLUDES
/----------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "SimpleNode.h"

/*===================================================================================
/  struct
===================================================================================*/
struct SimpleList
{

    /*-------------------------------------------------------------------------
    /  VARIABLES
    /------------------------------------------------------------------------*/
    SimpleNode*   head;
    SimpleNode*   tail;

    /*-------------------------------------------------------------------------
    /  INTERNAL VARIABLES
    /------------------------------------------------------------------------*/
    SimpleNode*   currentNode;

    /*-------------------------------------------------------------------------
    /  FUNCTIONS
    /------------------------------------------------------------------------*/
    void        (*removeNode)(struct SimpleList* list, SimpleNode* node);
    void        (*addNode)(struct SimpleList* list, SimpleNode* node);
    SimpleNode* (*getFirst)(struct SimpleList* list);
    SimpleNode* (*getCurrent)(struct SimpleList* list);
    SimpleNode* (*getNext)(struct SimpleList* list);
    SimpleNode* (*search)(struct SimpleList* list, void* item, int (*searchF)(void* a, void* b));
    void        (*print)(struct SimpleList* list);

} typedef SimpleList;

/*===================================================================================
/  functions
===================================================================================*/
SimpleList* createSimpleList();

#endif
