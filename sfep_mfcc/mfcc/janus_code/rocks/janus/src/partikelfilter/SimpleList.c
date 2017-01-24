/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Simple List Module
               ------------------------------------------------------------

    Author  :  Matthias Woelfel and Friedrich Faubel
    Module  :  SimpleList.c
    Date    :  $Id: SimpleList.c 2844 2008-11-25 12:00:00Z wolfel $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

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

    Revision 1.1  2008/11/25  12:00:00  wolfel
    Initial revision

   ======================================================================== */

/*-----------------------------------------------------------------------------
    INCLUDES
  ---------------------------------------------------------------------------*/
#include "SimpleList.h"

/*===================================================================================
/  searchSortedSimpleList
===================================================================================*/
SimpleNode* searchSortedSimpleList(struct SimpleList* list, void* item, int (*searchF)(void* a, void* b))
{
    SimpleNode* node  = (*list).currentNode;
    if (node==0) node = (*list).head;
    while ((node!=0) && (searchF((*node).cargo,item)==1)) node = (*node).nextNode;
    while ((node!=0) && (searchF((*node).cargo,item)==-1)) node = (*node).previousNode;
    if ((node!=0) && (searchF((*node).cargo,item)==0)) (*list).currentNode = node;

    return(node);
}

/*===================================================================================
/  getFirstFromSimpleList
===================================================================================*/
SimpleNode* getFirstFromSimpleList(struct SimpleList* list)
{
    (*list).currentNode = (*list).head;
    return((*list).currentNode);
}

/*===================================================================================
/  getNextFromSimpleList
===================================================================================*/
SimpleNode* getNextFromSimpleList(struct SimpleList* list)
{
    if ((*list).currentNode!=0) (*list).currentNode = (*(*list).currentNode).nextNode;
    return((*list).currentNode);
}

/*===================================================================================
/  getNextFromSimpleList
===================================================================================*/
SimpleNode* getCurrentFromSimpleList(struct SimpleList* list)
{
    return((*list).currentNode);
}

/*===================================================================================
/  addNodeToSimpleList
===================================================================================*/
void addNodeToSimpleList(struct SimpleList* list, SimpleNode* node)
{
    SimpleNode* tail = (*list).tail;
    SimpleNode* head = (*list).head;

    /*-------------------------------------------------------------------------
     
      -----------------------------------------------------------------------*/
    if (tail==0) {
        head = node;
        (*node).previousNode = 0;
    } else {
        (*tail).nextNode      = node;
        (*node).previousNode  = tail;
    }

    tail = node;
    (*node).nextNode   = 0;

    (*list).tail = tail;
    (*list).head = head;
}

/*===================================================================================
/  removeFromSimpleList
===================================================================================*/
void removeNodeFromSimpleList(SimpleList* list, SimpleNode* node)
{
    SimpleNode*   head = (*list).head;
    SimpleNode*   tail = (*list).tail;

    /*-------------------------------------------------------------------------
      catch:  node == head
      -----------------------------------------------------------------------*/
    if ((*node).previousNode==0) head = (*node).nextNode;
    else {
        /*-------------------------------------------------------------------------
           catch:  node == tail
          -----------------------------------------------------------------------*/
        if ((*node).nextNode==0) {
            tail = (*node).previousNode;
            (*(*node).previousNode).nextNode = 0;
        } else {
            (*(*node).nextNode).previousNode = (*node).previousNode;
            (*(*node).previousNode).nextNode = (*node).nextNode;
        }
    }

    (*node).nextNode = 0;
    (*node).previousNode = 0;
    (*list).head = head;
    (*list).tail = tail;
}

/*===================================================================================
/  printSimpleList
===================================================================================*/
void printSimpleList(SimpleList* list)
{
    SimpleNode*   node;
    char*         cargo;

    node = (*list).head;
    while (node!=0) {
        cargo = (*node).cargo;
        printf("%s ", cargo);
        node = (*node).nextNode;
    }
    printf("\n");
}

/*===================================================================================
  createSimpleList
===================================================================================*/
SimpleList* createSimpleList()
{
    SimpleList* list = (SimpleList*) malloc(sizeof(SimpleList));
    (*list).head = 0;
    (*list).tail = 0;
    (*list).currentNode = 0;
    (*list).getFirst    = getFirstFromSimpleList;
    (*list).getNext     = getNextFromSimpleList;
    (*list).getCurrent  = getCurrentFromSimpleList;
    (*list).search      = searchSortedSimpleList;
    (*list).addNode     = addNodeToSimpleList;
    (*list).removeNode  = removeNodeFromSimpleList;
    (*list).print       = printSimpleList;

    return(list);
}
