/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featurePLP.c
    Date    :  $Id: featurePLP.c 2515 2004-09-16 14:36:37Z metze $
    Remarks :

   ========================================================================

    Part of this software was developed by

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

    $Log$
    Revision 3.5  2004/09/16 14:36:00  metze
    Made code 'x86_64'-clean

    Revision 3.4  2003/08/14 11:19:56  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.20.2  2002/11/20 09:38:20  soltau
    Included "common.h"

    Revision 3.3.20.1  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  2000/11/14 12:25:53  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.2.34.1  2000/11/06 10:50:28  janus
    Made changes to enable compilation under -Wall.

    Revision 3.2  1997/07/09 17:50:51  westphal
    *** empty log message ***


   ======================================================================== */
/*
   Changes of the original code (MM):
   =================================== */
/* set values to 0.0 in alloc_fvec()   */
/* function free_fvec(), also define in plp.h */

/*************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      alloc_fvec(): allocate an fvec structure         *
 *                              (float vector and int length) and        *
 *                              return a pointer to it                   *
 *                                                                       *
 *                      alloc_fmat(): allocate an fmat structure         *
 *                              (float matrix and int nrows and ncols)   *
 *                              and return a pointer to it               *
 *                                                                       *
 *                      fvec_copy(): copy an fvec to an fvec             *
 *                                                                       *
 *                      norm_fvec(): normalize fvec by scaling factor    *
 *                                                                       *
 *                      fmat_x_fvec(): matrix-vector multiply            *
 *                                                                       *
 *                      fvec_check(): bounds check for fvec reference    *
 *                                                                       *
 ************************************************************************/

#include "common.h"
#include "featurePLP.h"


/* MM: */
void free_fvec(struct fvec *vecptr) {
   if (vecptr) {
      if (vecptr->values) free(vecptr->values);
      free(vecptr);
   }
}
void free_fmat(struct fmat *mptr) {
   int i;
   if (mptr) {
      if (mptr->values) {
	 for(i=0; i<mptr->nrows; i++) free(mptr->values[i]);	   
	 free(mptr->values);
      }
      free(mptr);
   }
}


/* Allocate fvec, and return pointer to it. */
struct fvec *alloc_fvec( int veclength )
{
	struct fvec *vecptr;
	int i;
	

	vecptr = (struct fvec *)malloc (sizeof( struct fvec) );
	if(vecptr == (struct fvec *)NULL)
	{
		fprintf(stderr,"Can't allocate %d bytes for fvec\n",
                                (int) sizeof( struct fvec) );
		exit(-1);
	}
	vecptr->length = veclength;
	vecptr->values = (float *)malloc ((veclength) * sizeof(float) );
	if(vecptr->values == (float *)NULL)
	{
		fprintf(stderr,"Can't allocate %d bytes for vector\n",
                                (veclength) * (int) sizeof(float) );
		exit(-1);
	}
	for(i=0; i<veclength; i++) vecptr->values[i] = 0.0;
	
    	return (vecptr);
} 

/* Allocate fmat, and return pointer to it. */
struct fmat * alloc_fmat(int nrows, int ncols)
{
        struct fmat *mptr;
        int i;

        mptr = (struct fmat *)malloc(sizeof(struct fmat ));
        if(mptr == (struct fmat *)NULL)
        {
                fprintf(stderr,"Cant malloc an fmat\n");
                exit(-1);
        }

        mptr->values = (float **)malloc(nrows * sizeof(float *));
        if(mptr->values == (float **)NULL)
        {
                fprintf(stderr,"Cant malloc an fmat ptr array\n");
                exit(-1);
        }
        for(i=0; i<nrows; i++)
        {
                mptr->values[i] = (float *)malloc(ncols * sizeof(float ));
                if(mptr->values[i] == (float *)NULL)
                {
                        fprintf(stderr,"Cant malloc an fmat array\n");
                        exit(-1);
                }
        }
        mptr->nrows = nrows;
        mptr->ncols = ncols;

        return( mptr );
}

/* Multiply a float matrix (in fmat structure) times a
	float vector (in fvec structure) */
void fmat_x_fvec( struct fmat * mat, struct fvec * invec,
                        struct fvec *outvec )
{
        int i, j;

        if(mat->ncols != invec->length)
        {
                fprintf(stderr,"input veclength neq mat ncols\n");
                exit(-1);
        }
        if(mat->nrows != outvec->length)
        {
                fprintf(stderr,"output veclength neq mat nrows\n");
                exit(-1);
        }

        for(i=0; i<outvec->length; i++)
        {
                outvec->values[i] = mat->values[i][0] * invec->values[0];
                for(j=1; j<invec->length; j++)
                {
                        outvec->values[i]
                                += mat->values[i][j] * invec->values[j];
                }
        }
}

/* Normalize a float vector by a scale factor */
void norm_fvec ( struct fvec *fptr, float scale)
{
	int i;

	for(i=0; i<fptr->length; i++)
	{
		fptr->values[i] /= scale;
	}
}

/* Copy a vector into an equal length fvec (first one into
	second one). Currently kills
	the program if you attempt to copy incompatible length
	fvecs. */
void fvec_copy(char *funcname, struct fvec *fvec_1, struct fvec *fvec_2 )
{
	int i;

	if(fvec_1->length != fvec_2->length)
	{
		fprintf(stderr,"Cannot copy an fvec into one of ");
		fprintf(stderr,"unequal length\n");
		fprintf(stderr,"\tThis was tried in function %s\n", funcname);
		exit(-1);
	}
	for(i=0; i<fvec_1->length; i++)
	{
		fvec_2->values[i] = fvec_1->values[i];
	}
}

/* Routine to check that it is OK to access an array element.
Use this in accordance with your level of paranoia; if truly
paranoid, use it before every array reference. If only moderately
paranoid, use it once per loop with the indices set to the
largest value they will achieve in the loop. You don't need to use this
at all, of course. 

	The routine accesses a global character array that is supposed
to hold the name of the calling function. Of course if you
write a new function and don't update this value, this fature won't work. */

void fvec_check( char *funcname, struct fvec *vec, int index )
{
	if((index >= vec->length) || (index < 0))
	{
		fprintf(stderr,"Tried to access %dth elt, array length=%d\n",
			index + 1, vec->length);
		fprintf(stderr,"\tIn routine %s\n", funcname);
		fflush(stderr);
		exit(-1);
	}
}
