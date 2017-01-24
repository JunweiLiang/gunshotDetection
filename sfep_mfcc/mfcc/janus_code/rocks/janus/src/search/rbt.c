/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Generic Red and Black Search Tree
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  rbt.c
    Date    :  $Id: rbt.c 151 1998-06-29 23:41:51Z kries $
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
    Revision 1.2  1998/06/29 23:41:51  kries
    corrected FindNode

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *
 
   ======================================================================== */


#include "rbt.h"

/* ------------------------------------------------------------------------
   RBT_Sentinel
   ------------------------------------------------------------------------ */

RBT_Node RBT_Sentinel = { 0, 0, 0, RBT_Black, NULL};

/* ========================================================================
   Initialization and Deinitalization
   ======================================================================== */

static  long RBT_hashKey(ClientData DataHolder, ClientData CD) {  
  return ((RBT*)DataHolder)->hashKey(((RBT*)DataHolder)->DataHolder,((RBT_Node*)CD)->Data);
}

static  int  RBT_hashCmp(ClientData DataHolder, ClientData CD1, ClientData CD2) {  
  return ((RBT*)DataHolder)->hashCmp(((RBT*)DataHolder)->DataHolder,
				     ((RBT_Node*)CD1)->Data,
				     ((RBT_Node*)CD2)->Data);
}

  
/* ------------------------------------------------------------------------
   RBT_Init
   ------------------------------------------------------------------------ */


int RBT_Init(RBT * t,            /* Pointer to the structure initialized */

	     /* The ordering of elements */
	      RBTOrderingFct * is_less,                   /*       != NULL */

	     /* Deinitializing elements in the tree */
	     RBTDeinitFct *data_deinit,                   /* maybe == NULL */
	     ClientData DataHolder,

	     /* Bounded trees */
	     int  capacity,   /* >0 for bounded trees, 0 otherwise         */                             

	     /* In the future we may have special incarnations of RBTs     */
	     TypeInfo * ti,                               /* maybe == NULL */

	     /* If hash-key and compare functions are set up dublication
		can be eliminated
		Equality can be defined fully independently of "is_less"
		Unless both are !=0 this is not activated.
		It is always the smaller element that is eliminated
		in case of conflicts.
	     */
	     HashKeyFn2* hashKey,
	     HashCmpFn2* hashCmp) {

  int i;

  RBT_Sentinel.Left = RBT_NIL; RBT_Sentinel.Right = RBT_NIL;

  t->Root=RBT_NIL;
  t->nodesN=0;
  t->nodesMaxN=capacity;  
  t->is_less = is_less;
  t->data_deinit = data_deinit;
  t->DataHolder = DataHolder;
  t->ti = ti;
  t->nodesReplaced = 0;
  t->nodesPruned = 0;

  t->HighNodeP = t->LowNodeP  = NULL;

  if(is_less==NULL) {
    ERROR("Less function is not specified\n");
    return TCL_ERROR;
  }

  if(capacity>0) {
    if ((t->Nodes = malloc(sizeof(RBT_Node)*capacity)) == 0) {
      ERROR("insufficient memory (TreeInit with capacity %ld)\n",capacity);
	return TCL_ERROR;
      }	
    t->NodesFree = &(t->Nodes[0]);
    for(i=1;i<capacity;i++)
      t->Nodes[i-1].Left = &(t->Nodes[i]);
    t->Nodes[i-1].Left = RBT_NIL;
  }
  else
    t->NodesFree = t->Nodes = NULL;

  t->hashKey = hashKey;
  t->hashCmp = hashCmp;
  
  if(hashKey!=NULL && hashCmp!=NULL) {

    t->DataEq = (Hash*)malloc( sizeof(Hash));
    if(t->DataEq==NULL) {
      ERROR("Cannot allocate DataEq\n");
      RBT_Deinit(t);
      return TCL_ERROR;
    }
    if(hashInit(t->DataEq)!=TCL_OK)
      return TCL_ERROR;
    t->DataEq->hashKey = RBT_hashKey;
    t->DataEq->hashCmp = RBT_hashCmp;
    t->DataEq->DataHolder = (ClientData)t;
  }
  else
    t->DataEq = 0;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
   RBT_Deinit     (+ RBT_NodesDeinit)
   ------------------------------------------------------------------------ */

static void RBT_NodesDeinit(RBT* t,RBT_Node * Current) {

  if(Current!=RBT_NIL) {
     RBT_NodesDeinit(t,Current->Left);
     RBT_NodesDeinit(t,Current->Right);

    if(t->data_deinit)
      t->data_deinit(t->DataHolder,Current->Data);

     if(t->nodesMaxN<=0)
       free(Current);
  }
}

int RBT_Deinit(RBT * t) {

  if(t->DataEq) {
    hashClear(t->DataEq);
    free(t->DataEq);
  }

  RBT_NodesDeinit(t,t->Root); 
  if(t->nodesMaxN>0)
   free(t->Nodes);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   RBT_IsOK
   ------------------------------------------------------------------------ */

#define E(problem) { \
  ERROR("RBT_isOK detected a problem: %s\n",problem); \
  *error_code=TCL_ERROR; \
  return 0; \
} 


/* Returns the black height (well defined if RBT, which is checked)
   Properties 1. and 2. are satisified automagically,
   properties 3 and 4 nee to be checked
   and the checks need to be carried out recursively */

static int RBT_NodeRBT(RBT* t, RBT_Node* Node, int * error_code) {

  int bh;

  if(Node == NULL)
    E("Node is NULL instead of RBT_NIL");
  if(Node == RBT_NIL || Node == NULL)
    return 1;
  
  /* Check that the parents links are set up correctly */
  
  if(Node == t->Root) {
    if(Node->Parent!=NULL)
      E("Root nodes parent is not NULL");
  }
  else if(Node->Parent == NULL) {
    E("Parent initalized to NULL");
  }
  else if((Node->Parent->Left != Node) && (Node->Parent->Right != Node))
    E("The parent does not know about its child");

  /* Check property 4 */

  bh = RBT_NodeRBT(t,Node->Left, error_code);
  if(*error_code!=TCL_OK) return bh;
  if(bh!=RBT_NodeRBT(t,Node->Right, error_code)) {
    if(*error_code!=TCL_OK) return bh;    
    E("Property 4 is violated");
  }
  if(*error_code!=TCL_OK) return bh;    

  if(Node->Color==RBT_Red) { /* Check property 3 */
    if(Node->Left->Color!=RBT_Black || Node->Right->Color!=RBT_Black) {
      E("Property 3 is violated");
    }
  }
  else
    bh++;
  return bh;
}

#undef E
#define E(problem) { \
  ERROR("RBT_isOK detected a problem: %s\n",problem); \
  return TCL_ERROR; \
} 

int RBT_isOK(RBT* t) {
  int i;
  RBT_Node * monkey, *last;
  int error_code = TCL_OK;

  if(t==NULL)
    E("Pointer t is zero");

  /* Check R&B properties and parent pointers */
  
  error_code = TCL_OK;
  RBT_NodeRBT(t,t->Root, &error_code);
  if(error_code!=TCL_OK)
    return error_code;

  /* Count lattice nodes and check ordering */
  
  for(i=0,monkey=RBT_FindLow(t),last=RBT_NIL;
      monkey;
      i++,last=monkey,monkey=RBT_FindNext(t,monkey)) {
    if(last!=RBT_NIL) {
      int is_less = t->is_less(t->DataHolder,last->Data,monkey->Data);
      int is_greater = t->is_less(t->DataHolder,monkey->Data,last->Data);
      
      if(is_less && is_greater)
	E("is_less && is_greater");
      if(is_greater)
	E("wrong ordering");
    }
  }
  if(i!=t->nodesN)
    E("Wrong number of nodes");

  return TCL_OK;
}


/* ========================================================================
   FINDING NODES AND SEARCHING THE TREE
   ======================================================================== */

/* ------------------------------------------------------------------------
   RBT_FindLow(RBT* t)          find lowest element in search tree
   ------------------------------------------------------------------------ */

static RBT_Node *RBT_FindLow_Node(RBT* t,RBT_Node *Current) {
    for(;Current->Left != RBT_NIL;Current=Current->Left);
    return Current;
}

RBT_Node *RBT_FindLow(RBT* t) {
  return t->LowNodeP; /* */
  return t->Root == RBT_NIL ? NULL : RBT_FindLow_Node(t,t->Root);
}

/* ------------------------------------------------------------------------
   RBT_FindHigh(RBT* t)          find highest element in search tree
   ------------------------------------------------------------------------ */

static RBT_Node *RBT_FindHigh_Node(RBT* t,RBT_Node *Current) {
  for(;Current->Right != RBT_NIL;Current=Current->Right);
  return Current;
}

RBT_Node *RBT_FindHigh(RBT* t) {
  return t->HighNodeP; /* */
  return t->Root == RBT_NIL ? NULL : RBT_FindHigh_Node(t,t->Root);
}


/* ------------------------------------------------------------------------
   RBT_FindNext              find next node in prefix traversal
   ------------------------------------------------------------------------ */

RBT_Node *RBT_FindNext(RBT* t, RBT_Node* Current) {
  RBT_Node* Parent;

  if(Current==NULL)        	return NULL;
  if(Current->Right!=RBT_NIL) 	return RBT_FindLow_Node(t,Current->Right);

  for(Parent=Current->Parent;
      Parent!=NULL && Parent->Right==Current;
      Current=Parent,Parent=Parent->Parent);

  return Parent;
}

/* ------------------------------------------------------------------------
   RBT_FindPrev              find prev node in prefix traversal
   ------------------------------------------------------------------------ */

RBT_Node *RBT_FindPrev(RBT* t, RBT_Node* Current) {
  RBT_Node* Parent;

  if(Current==NULL)        	return NULL;
  if(Current->Left!=RBT_NIL) 	return RBT_FindHigh_Node(t,Current->Left);

  for(Parent=Current->Parent;
      Parent!=NULL && Parent->Left==Current;
      Current=Parent,Parent=Parent->Parent);

  return Parent;
}

/* ------------------------------------------------------------------------
   RBT_FindNode              find smallest >= Data
   ------------------------------------------------------------------------ */

RBT_Node *RBT_FindNode(RBT* t, ClientData Data) {

  RBT_Node *Current     = t->Root;
  RBT_Node *LastFulfill = NULL;

  while(Current != RBT_NIL) {
    if(t->is_less(t->DataHolder, Current->Data, Data))
      Current = Current->Right;   /* Look right, Current + left subtree contain NO fulfillers */
    else {
      LastFulfill = Current;   /* Current is >= Data and smaller than Lastfullfill */
      Current = Current->Left; /* Look left, all better fulfiller must be there !! */
    }
  }
  return LastFulfill;  /* No new fulfillers in this subtree */
}


/* ========================================================================
   AUXILIARY FUNCTIONS -- MAINLY TO MAINTAIN RED-BLACK PROPERTIES
   ======================================================================== */

/* ------------------------------------------------------------------------
   RBT_RotateLeft      rotate node X to the left
   ------------------------------------------------------------------------ */

static void RBT_RotateLeft(RBT* t, RBT_Node *X) {

    RBT_Node *Y = X->Right;

    /* establish X->Right link */
    X->Right = Y->Left;
    if (Y->Left != RBT_NIL) Y->Left->Parent = X;

    /* establish Y->Parent link */
    if (Y != RBT_NIL) Y->Parent = X->Parent;
    if (X->Parent) {
        if (X == X->Parent->Left)
            X->Parent->Left = Y;
        else
            X->Parent->Right = Y;
    } else {
        t->Root = Y;
    }

    /* link X and Y */
    Y->Left = X;
    if (X != RBT_NIL) X->Parent = Y;
}

/* ------------------------------------------------------------------------
   RBT_RotateRight      rotate node X to the right
   ------------------------------------------------------------------------ */

static void RBT_RotateRight(RBT* t, RBT_Node *X) {

    RBT_Node *Y = X->Left;

    /* establish X->Left link */
    X->Left = Y->Right;
    if (Y->Right != RBT_NIL) Y->Right->Parent = X;

    /* establish Y->Parent link */
    if (Y != RBT_NIL) Y->Parent = X->Parent;
    if (X->Parent) {
        if (X == X->Parent->Right)
            X->Parent->Right = Y;
        else
            X->Parent->Left = Y;
    } else {
        t->Root = Y;
    }

    /* link X and Y */
    Y->Right = X;
    if (X != RBT_NIL) X->Parent = Y;
}

/* ------------------------------------------------------------------------
   RBT_InsertFixup 
        maintain red-black tree balance  after inserting node X 
   ------------------------------------------------------------------------ */

static void RBT_InsertFixup(RBT* t, RBT_Node *X) {

    /* check red-black properties */
    while (X != t->Root && X->Parent->Color == RBT_Red) {
        /* we have a violation */
        if (X->Parent == X->Parent->Parent->Left) {
            RBT_Node *Y = X->Parent->Parent->Right;
            if (Y->Color == RBT_Red) {

                /* uncle is red */
                X->Parent->Color = RBT_Black;
                Y->Color = RBT_Black;
                X->Parent->Parent->Color = RBT_Red;
                X = X->Parent->Parent;
            } else {

                /* uncle is black */
                if (X == X->Parent->Right) {
                    /* make X a left child */
                    X = X->Parent;
                    RBT_RotateLeft(t,X);
                }

                /* recolor and rotate */
                X->Parent->Color = RBT_Black;
                X->Parent->Parent->Color = RBT_Red;
                RBT_RotateRight(t,X->Parent->Parent);
            }
        } else {

            /* mirror image of above code */
            RBT_Node *Y = X->Parent->Parent->Left;
            if (Y->Color == RBT_Red) {

                /* uncle is red */
                X->Parent->Color = RBT_Black;
                Y->Color = RBT_Black;
                X->Parent->Parent->Color = RBT_Red;
                X = X->Parent->Parent;
            } else {

                /* uncle is black */
                if (X == X->Parent->Left) {
                    X = X->Parent;
                    RBT_RotateRight(t,X);
                }
                X->Parent->Color = RBT_Black;
                X->Parent->Parent->Color = RBT_Red;
                RBT_RotateLeft(t,X->Parent->Parent);
            }
        }
    }
    t->Root->Color = RBT_Black;
}

/* ------------------------------------------------------------------------
   RBT_DeleteFixup
       maintain red-black tree balance after deleting node X
   ------------------------------------------------------------------------ */

static void RBT_DeleteFixup(RBT* t, RBT_Node *X) {

    while (X != t->Root && X->Color == RBT_Black) {
        if (X == X->Parent->Left) {
            RBT_Node *W = X->Parent->Right;
            if (W->Color == RBT_Red) {
                W->Color = RBT_Black;
                X->Parent->Color = RBT_Red;
                RBT_RotateLeft (t,X->Parent);
                W = X->Parent->Right;
            }
            if (W->Left->Color == RBT_Black && W->Right->Color == RBT_Black) {
                W->Color = RBT_Red;
                X = X->Parent;
            } else {
                if (W->Right->Color == RBT_Black) {
                    W->Left->Color = RBT_Black;
                    W->Color = RBT_Red;
                    RBT_RotateRight (t,W);
                    W = X->Parent->Right;
                }
                W->Color = X->Parent->Color;
                X->Parent->Color = RBT_Black;
                W->Right->Color = RBT_Black;
                RBT_RotateLeft (t,X->Parent);
                X = t->Root;
            }
        } else {
            RBT_Node *W = X->Parent->Left;
            if (W->Color == RBT_Red) {
                W->Color = RBT_Black;
                X->Parent->Color = RBT_Red;
                RBT_RotateRight (t,X->Parent);
                W = X->Parent->Left;
            }
            if (W->Right->Color == RBT_Black && W->Left->Color == RBT_Black) {
                W->Color = RBT_Red;
                X = X->Parent;
            } else {
                if (W->Left->Color == RBT_Black) {
                    W->Right->Color = RBT_Black;
                    W->Color = RBT_Red;
                    RBT_RotateLeft (t,W);
                    W = X->Parent->Left;
                }
                W->Color = X->Parent->Color;
                X->Parent->Color = RBT_Black;
                W->Left->Color = RBT_Black;
                RBT_RotateRight (t,X->Parent);
                X = t->Root;
            }
        }
    }
    X->Color = RBT_Black;
}


/* ========================================================================
   INSERTION AND DELETION
   ======================================================================== */


/* ------------------------------------------------------------------------
   RBT_InsertNode_      allocate node for Data and insert in tree
                        returns NULL as an error condition
   ------------------------------------------------------------------------ */

RBT_Node *RBT_InsertNode_(RBT * t, ClientData Data, RBT_Node * X) {
  RBT_Node *Current, *Parent;
  HashHandle h;
  int  moveLeft, moveRight;


  /* Try to insert the node in the hash-array */
  
  /* The tree might be attached with an equality relation on the nodes
     If the new node to be entered is equal to a node in the tree
     the following happens:
     newNode < oldNode --> newNode is not entered
     newNode > oldNode --> oldNode is removed, new Node is entered       
  */

  if(t->DataEq) {
    RBT_Node Node;
    
    Node.Data = Data;
    
    if(NULL != (h = hashItem(t->DataEq,(ClientData)&Node,0))) {
      RBT_Node* NodeP = *(RBT_Node**)hashRecall(t->DataEq,h);
      t->nodesReplaced++;
      if(t->is_less(t->DataHolder, Data, NodeP->Data))  { /* Return the old node and deinialize Data */
	/* INFO("is less deletes new item\n"); */
	if(t->data_deinit)
	  t->data_deinit(t->DataHolder,Data);
	free(X);
	return NodeP;
      }
      /* The older entry is worse --> delete it */ 
      /* INFO("is less deletes old item\n"); */
      RBT_DeleteNode(t,NodeP,0);
    }
  }

  /* The tree may come with a maximal capacity constraint.
     If the tree has a capacity constraints we first have to check whether
     the new node should be inserted at all and eventually delete the
     worst element in the tree */

  if((t->nodesMaxN<=t->nodesN)  && (t->nodesMaxN>0)) {


    /* Tree has reached maximal size -->
       either the new data is to bad to be entered --> it is discared
       it is good enough --> the lowest node has to go
    */

    RBT_Node* low = RBT_FindLow(t);
    t->nodesPruned++;
   if(t->nodesMaxN<t->nodesN) /* just checking ... */
      FATAL("Internal RBT: Bounded tree grew over its allowed size");

    if(t->is_less(t->DataHolder, Data, low->Data)) { /* the node is pruned away due to capacity constraints */
      if(t->data_deinit)  
	t->data_deinit(t->DataHolder,Data);	
      free(X);
      return RBT_NIL; 
    }
    RBT_DeleteNode(t,low,0);
    if(t->nodesN!=t->nodesMaxN-1)
      FATAL("Internal RBT: Bounded tree not shrinkable ?");
  }

  /* find where node belongs and count left/right moves*/
  Current = t->Root;
  Parent = 0;

  moveLeft=moveRight=0;
  while (Current != RBT_NIL) {
    Parent = Current;
    if(t->is_less(t->DataHolder, Data, Current->Data)) {
      Current =  Current->Left;     moveLeft++;
    } else {
      Current =  Current->Right;    moveRight++;
    }
  }

  /* setup new node */
  if(t->nodesMaxN>0) {
    free(X);
    X = t->NodesFree;
    if (X  == NULL) {
      ERROR("bounded tree constraints violated (InsertNode)\n");
      return NULL;
    }	
    t->NodesFree = X->Left;
  }
  else if(X==0)
    if ((X = malloc (sizeof(RBT_Node))) == 0) {
      ERROR("insufficient memory (InsertNode)\n");
      return NULL;
    }	
  t->nodesN++;

  if(moveLeft==0) {      t->HighNodeP = X; }
  if(moveRight==0) {     t->LowNodeP  = X; }

  X->Data = Data;
  X->Parent = Parent;
  X->Left = RBT_NIL;
  X->Right = RBT_NIL;
  X->Color = RBT_Red;

  /* insert node in tree */
  if(Parent) {
    if(t->is_less(t->DataHolder, Data, Parent->Data))
      Parent->Left = X;
    else
      Parent->Right = X;
  } else {
    t->Root = X;
  }

  RBT_InsertFixup(t,X);
  if(t->DataEq)
     if(NULL == (h = hashItem(t->DataEq,(ClientData)X,1)))
         ERROR("Could not enter hashitem\n");

  /* Checking for this new feature, turn on old versions of the
     repective procedures to check

  if(RBT_FindLow(t)!=t->LowNodeP) {
    INFO("Low node incorrect in insertNode, fixing it\n");
    t->LowNodeP = RBT_FindLow(t);
  }
  if(RBT_FindHigh(t)!=t->HighNodeP) {
    INFO("High node incorrect in insertNode, fixing it\n");
    t->HighNodeP = RBT_FindHigh(t);
  }
  */

  return(X);
}

/* ------------------------------------------------------------------------
   RBT_InsertNode(RBT * t, ClientData Data)
   ------------------------------------------------------------------------ */

int RBT_InsertNode(RBT * t, ClientData Data) {  
  RBT_Node * ret = RBT_InsertNode_(t,Data,0);
  if(ret==NULL)
    return TCL_ERROR;
  else	
    return TCL_OK;
}


/* ------------------------------------------------------------------------
   RBT_DeleteNode
         deletes node Z and deinializes the ClientData unless no_deinit is set
   ------------------------------------------------------------------------ */

/* The old version copies data around --> stupid and destroys the indexability of
   tree nodes */

void RBT_DeleteNodeOld(RBT* t, RBT_Node *Z, int no_deinit) {
    RBT_Node *X, *Y;
    HashHandle h;

    if (!Z || Z == RBT_NIL) return;

    if(Z == t->HighNodeP) t->HighNodeP = RBT_FindPrev(t, Z);
    if(Z == t->LowNodeP)  t->LowNodeP = RBT_FindNext(t, Z);
    
    /* If equality was required the element has to be deleted from
       the hash */

    if(t->DataEq) {
      h = hashItem(t->DataEq,(ClientData)Z,0);
      if(NULL==h)
	ERROR("Element not in hash -- invariant violated\n");
      else
	hashDelete(t->DataEq,h,0);
    }

    /* The data needs to be deinitalized */

    if(t->data_deinit && !no_deinit)
      t->data_deinit(t->DataHolder,Z->Data);

    if (Z->Left == RBT_NIL || Z->Right == RBT_NIL) {
        /* Y has a RBT_NIL node as a child */
        Y = Z;
    } else {
        /* find tree successor with a RBT_NIL node as a child */
        Y = Z->Right;
        while (Y->Left != RBT_NIL) Y = Y->Left;
    }

    /* X is Y's only child */
    if (Y->Left != RBT_NIL)
        X = Y->Left;
    else
        X = Y->Right;

    /* remove Y from the parent chain */
    X->Parent = Y->Parent;
    if (Y->Parent)
        if (Y == Y->Parent->Left)
            Y->Parent->Left = X;
        else
            Y->Parent->Right = X;
    else
        t->Root = X;

    if (Y != Z) {      /* Record the move of data from Y-->Z */
      RBT_Node ** nodeP;
      h = hashItem(t->DataEq,(ClientData)Y,0);
      if(NULL==h)
	ERROR("Element not in hash -- invariant violated\n");
      else {

	nodeP = (RBT_Node **)hashRecall(t->DataEq,h);
	*nodeP = Z;
	/* 
	hashDelete(t->DataEq,h,0);
	Z->Data = Y->Data;
	hashItem(t->DataEq,(ClientData)Z,1);
	*/
      }
    }

    if (Y->Color == RBT_Black)
        RBT_DeleteFixup (t,X);


    /* Delete the node from all data-structures */

    /* The number of nodes is smaller */

    t->nodesN--;

    /* Reclaim the memory of the node */

    if(t->nodesMaxN>0) { /* Maintain free list */
      Y->Left      = t->NodesFree;
      t->NodesFree = Y;
    }
    else                 /* Heap based dealloc */
      free (Y);
}

void RBT_DeleteNode(RBT* t, RBT_Node *Z, int no_deinit) {
    RBT_Node *X, *Y;
    HashHandle h;
    RBT_Color ycolor;

    if (!Z || Z == RBT_NIL) return;

    if(Z == t->HighNodeP) t->HighNodeP = RBT_FindPrev(t, Z);
    if(Z == t->LowNodeP)  t->LowNodeP = RBT_FindNext(t, Z);
    
    /* If equality was required the element has to be deleted from
       the hash */

    if(t->DataEq) {
      h = hashItem(t->DataEq,(ClientData)Z,0);
      if(NULL==h)
	ERROR("Element not in hash -- invariant violated\n");
      else
	hashDelete(t->DataEq,h,0);
    }

    /* The data needs to be deinitalized */

    if(t->data_deinit && !no_deinit)
      t->data_deinit(t->DataHolder,Z->Data);

    if (Z->Left == RBT_NIL || Z->Right == RBT_NIL) {
        /* Y has a RBT_NIL node as a child */
        Y = Z;
    } else {
        /* find tree successor with a RBT_NIL node as a child */
        Y = Z->Right;
        while (Y->Left != RBT_NIL) Y = Y->Left;
    }

    /* X is Y's only child */
    if (Y->Left != RBT_NIL)
        X = Y->Left;
    else
        X = Y->Right;

    /* remove Y from the parent chain */
    X->Parent = Y->Parent;
    if (Y->Parent) {
        if (Y == Y->Parent->Left)
            Y->Parent->Left = X;
        else
            Y->Parent->Right = X;
    }
    else
        t->Root = X;    

    ycolor = Y->Color;
    if (Y != Z) {   /* Now copy the pointers from Z to Y */   

      if(X->Parent == Z) /* Oh my dear, a really special case, has cost me a day */
	X->Parent = Y;

      Y->Parent = Z->Parent;
      Y->Left = Z->Left;   if(Y->Left!=RBT_NIL)  Y->Left->Parent  = Y;
      Y->Right = Z->Right; if(Y->Right!=RBT_NIL) Y->Right->Parent = Y;
      Y->Color = Z->Color;
      if(Y->Parent == 0) /* Changing root node */
	t->Root = Y;
      else if(Y->Parent->Left==Z) /* Changing parent, left */	
	Y->Parent->Left = Y;
      else if(Y->Parent->Right==Z) /* Changing parent, right */	
	Y->Parent->Right = Y;
      else 
	FATAL("Parents not properly declared\n");
    }    


    if (ycolor == RBT_Black)
        RBT_DeleteFixup (t,X);

    /* Delete the node from all data-structures */

    /* The number of nodes is smaller */

    t->nodesN--;

    /* Reclaim the memory of the node */

    if(t->nodesMaxN>0) { /* Maintain free list */
      Z->Left      = t->NodesFree;
      t->NodesFree = Z;
    }
    else                 /* Heap based dealloc */
      free (Z);

    /* Checking, in case this new feature is causing trouble 
    if(RBT_FindLow(t)!=t->LowNodeP) {
      INFO("Low node incorrect in deleteNode, fixing it\n");
      t->LowNodeP = RBT_FindLow(t);
    }
    if(RBT_FindHigh(t)!=t->HighNodeP) {
      INFO("High node incorrect in deleteNode, fixing it\n");
      t->HighNodeP = RBT_FindHigh(t);
    }    
    */
}

/* ------------------------------------------------------------------------
   RBT_PopLowNode RBT_PopHighNode
         eliminate the highest or lowest node and return the client data
   ------------------------------------------------------------------------ */

ClientData RBT_PopLowNode(RBT* t, int no_deinit) {
  RBT_Node* Z = RBT_FindLow(t);
  ClientData ret;
  if(Z == RBT_NIL) {
    ERROR("RBT was empty, PopLow is invalid");
    return (ClientData)0;
  }
  ret = Z->Data;
  RBT_DeleteNode(t, Z, no_deinit);
  return ret;
}
  
ClientData RBT_PopHighNode(RBT* t, int no_deinit) {
  RBT_Node* Z = RBT_FindHigh(t);
  ClientData ret;
  if(Z == RBT_NIL) {
    ERROR("RBT was empty, PopLow is invalid");
    return (ClientData)0;
  }
  ret = Z->Data;
  RBT_DeleteNode(t, Z, no_deinit);
  return ret;
}
  



