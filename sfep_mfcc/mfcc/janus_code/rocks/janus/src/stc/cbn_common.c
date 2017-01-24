/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_common.c
    Date    :  $Id: cbn_common.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 1.5  2003/08/14 11:20:24  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.3  2002/11/21 17:10:52  fuegen
    windows code cleaning

    Revision 1.4.20.2  2002/03/03 18:16:39  soltau
    cleand blockDeinit

    Revision 1.4.20.1  2001/11/19 14:46:27  metze
    Go away, boring message!

    Revision 1.4  2000/11/14 12:01:10  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 1.3.30.1  2000/11/06 10:50:24  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/09/08 13:22:43  janus
    Merging branch 'jtk-00-08-27-hyu'.

    Revision 1.2.12.1  2000/09/07 23:55:34  janus
    Changes necessary to compile at CMU.

    Revision 1.3  2000/08/24 14:34:03  soltau
    Fixed wrong dmatrixFree call

    Revision 1.2  2000/08/22 16:44:59  soltau
    special memory allocation full covariance accu's

    Revision 1.1  2000/08/16 09:08:53  soltau
    Initial revision


   ======================================================================== */


#include "cbn_common.h"
#include "cbn_global.h"
#include "cbnew.h"
#include "cbn_parmat.h"
#include "cbn_assert.h"

#include "mach_ind_io.h"

extern int free_accus_gaussian (CBNew*,int);  /* predecl. */

/* -------------------------------------------------------------------
   I/O functions
   Note the naming conventions: 'print' / 'scan' use Tcl list format.
   'print' can also print into the itf result string (using 'fp' = 0).
   'read' / 'write' use the binary machine-independant format of
   the 'mach_ind_io' module. 'discard' performs as 'read', but the
   object is discarded, not returned. The suffix 'incr' for 'read'
   functions means the object is loaded incrementally, i.e. the stored
   object is added to the target object.
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   print_int_vector
   Prints integer array into file. If 'fp' = 0, the target is the itf
   result string.
   ------------------------------------------------------------------- */
int print_int_vector (FILE* fp,int* vect,int dim)
{
  register int i;

#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (!fp) {
    itfAppendResult ("{ ");
    for (i = 0; i < dim; i++)
      itfAppendResult ("%d ",vect[i]);
    itfAppendResult ("}\n");
  } else {
    fprintf (fp,"{ ");
    for (i = 0; i < dim; i++)
      fprintf (fp,"%d ",vect[i]);
    fprintf (fp,"}\n");
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_int_vector
   Reads integer array from file (Tcl list format). The vector size
   must be known in advance.
   ------------------------------------------------------------------- */
int scan_int_vector (FILE* fp,int* vect,int dim)
{
  register int i;
  char dummy;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (fscanf (fp," %c",&dummy) != 1 || dummy != '{') return TCL_ERROR;
  for (i = 0; i < dim; i++)
    if (fscanf (fp,"%d",&vect[i]) != 1) return TCL_ERROR;
  if (fscanf (fp," %c \n",&dummy) != 1 || dummy != '}') return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_int_vector
   Writes integer array into file.
   ------------------------------------------------------------------- */
int write_int_vector (FILE* fp,int* vect,int dim)
{
#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (write_ints (fp,vect,dim) != dim) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_int_vector
   Reads integer array from file.
   ------------------------------------------------------------------- */
int read_int_vector (FILE* fp,int* vect,int dim)
{
#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (read_ints (fp,vect,dim) != dim) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   print_char_vector
   Prints char array into file. If 'fp' = 0, the target is the itf
   result string.
   ------------------------------------------------------------------- */
int print_char_vector (FILE* fp,char* vect,int dim)
{
  register int i;

#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (!fp) {
    itfAppendResult ("{ ");
    for (i = 0; i < dim; i++)
      itfAppendResult ("%d ",(int) vect[i]);
    itfAppendResult ("}\n");
  } else {
    fprintf (fp,"{ ");
    for (i = 0; i < dim; i++)
      fprintf (fp,"%d ",vect[i]);
    fprintf (fp,"}\n");
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_char_vector
   Reads char array from file (Tcl list format). The vector size
   must be known in advance.
   ------------------------------------------------------------------- */
int scan_char_vector (FILE* fp,char* vect,int dim)
{
  register int i;
  int temp;
  char dummy;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (fscanf (fp," %c",&dummy) != 1 || dummy != '{') return TCL_ERROR;
  for (i = 0; i < dim; i++) {
    if (fscanf (fp,"%d",&temp) != 1) return TCL_ERROR;
    vect[i] = (char) temp;
  }
  if (fscanf (fp," %c \n",&dummy) != 1 || dummy != '}') return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_char_vector
   Writes char array into file.
   ------------------------------------------------------------------- */
int write_char_vector (FILE* fp,char* vect,int dim)
{
  register int i;

#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  for (i = 0; i < dim; i++) write_int (fp,(int) vect[i]);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_char_vector
   Reads char array from file.
   ------------------------------------------------------------------- */
int read_char_vector (FILE* fp,char* vect,int dim)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (i = 0; i < dim; i++)
    vect[i] = (char) read_int (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_char_vector
   Reads char array from file, but doesn't return the entries.
   ------------------------------------------------------------------- */
int discard_char_vector (FILE* fp,int dim)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (dim > 0);
#endif

  for (i = 0; i < dim; i++) read_int (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   print_float_vector
   Prints float vector into file (Tcl list format). If 'fp' = 0, the
   target is the itf result string.
   ------------------------------------------------------------------- */
int print_float_vector (FILE* fp,float* vect,int dim)
{
  register int j;

#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (!fp) {
    itfAppendResult ("{ ");
    for (j = 0; j < dim; j++)
      itfAppendResult ("%f ",vect[j]);
    itfAppendResult ("}\n");
  } else {
    fprintf (fp,"{ ");
    for (j = 0; j < dim; j++)
      fprintf (fp,"%f ",vect[j]);
    fprintf (fp,"}\n");
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_float_vector
   Reads float vector from file (Tcl list format).
   ------------------------------------------------------------------- */
int scan_float_vector (FILE* fp,float* vect,int dim)
{
  register int j;
  char dummy;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (fscanf (fp," %c",&dummy) != 1 || dummy != '{') return TCL_ERROR;
  for (j = 0; j < dim; j++)
    if (fscanf (fp,"%f",&vect[j]) != 1) return TCL_ERROR;
  if (fscanf (fp," %c \n",&dummy) != 1 || dummy != '}') return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_float_vector
   Writes float array into file.
   ------------------------------------------------------------------- */
int write_float_vector (FILE* fp,float* vect,int dim)
{
#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (write_floats (fp,vect,dim) != dim) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_float_vector
   Reads float array from file.
   ------------------------------------------------------------------- */
int read_float_vector (FILE* fp,float* vect,int dim)
{
#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (read_floats (fp,vect,dim) != dim) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_float_vector_incr
   Reads float array from file and adds it to the target object.
   ------------------------------------------------------------------- */
int read_float_vector_incr (FILE* fp,float* vect,int dim)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (i = 0; i < dim; i++)
    vect[i] += read_float (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_float_vector
   Reads float array from file. The array is not returned.
   ------------------------------------------------------------------- */
int discard_float_vector (FILE* fp,int dim)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (dim > 0);
#endif

  for (i = 0; i < dim; i++) read_float (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   print_float_matrix
   Prints FMatrix into file (Tcl list format). If 'fp' = 0, the target
   is the itf result string.
   ------------------------------------------------------------------- */
int print_float_matrix (FILE* fp,FMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (mat);
  assert (mat->m && mat->n);
#endif

  for (i = 0; i < mat->m; i++)
    if (print_float_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_float_matrix
   Reads FMatrix from file (Tcl list format). The stored matrix must
   have the same size as the target matrix object.
   ------------------------------------------------------------------- */
int scan_float_matrix (FILE* fp,FMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m && mat->n);
#endif

  for (i = 0; i < mat->m; i++)
    if (scan_float_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_float_matrix
   ------------------------------------------------------------------- */
int read_float_matrix (FILE* fp,FMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    if (read_float_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_float_matrix
   ------------------------------------------------------------------- */
int write_float_matrix (FILE* fp,FMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    if (write_float_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_float_matrix_incr
   Adds stored matrix to target matrix.
   ------------------------------------------------------------------- */
int read_float_matrix_incr (FILE* fp,FMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    read_float_vector_incr (fp,mat->matPA[i],mat->n);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_float_matrix
   ------------------------------------------------------------------- */
int discard_float_matrix (FILE* fp,int m,int n)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (m > 0 && n > 0);
#endif

  for (i = 0; i < m; i++)
    discard_float_vector (fp,n);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   print_symm_float_matrix
   Converts upper triangular matrix into symmetric matrix and prints
   it into itf result string.
   ------------------------------------------------------------------- */
int print_symm_float_matrix (float* tmat,int dim)
{
  FMatrix* hmat;
  register int i,j,dcnt,ret;

#ifdef DEBUG
  assert (tmat);
  assert (dim > 0);
#endif

  if (!(hmat = fmatrixCreate (dim,dim))) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = dcnt = 0; i < dim; i++)
    for (j = i; j < dim; j++,dcnt++)
      hmat->matPA[i][j] = hmat->matPA[j][i] = tmat[dcnt];
  ret = fmatrixPutsItf ((ClientData) hmat,0,0);
  fmatrixDestroy (hmat);

  return ret;
}

/* -------------------------------------------------------------------
   print_double_vector
   Prints double vector into file (Tcl list format). If 'fp' = 0, the
   target is the itf result string.
   ------------------------------------------------------------------- */
int print_double_vector (FILE* fp,double* vect,int dim)
{
  register int j;

#ifdef DEBUG
  assert (vect);
  assert (dim > 0);
#endif

  if (!fp) {
    itfAppendResult ("{ ");
    for (j = 0; j < dim; j++)
      itfAppendResult ("%lf ",vect[j]);
    itfAppendResult ("}\n");
  } else {
    fprintf (fp,"{ ");
    for (j = 0; j < dim; j++)
#ifdef LINUX
      fprintf (fp,"%f ",vect[j]);
#else
      fprintf (fp,"%lf ",vect[j]);
#endif
    fprintf (fp,"}\n");
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_double_vector
   Reads double vector from file (Tcl list format).
   ------------------------------------------------------------------- */
int scan_double_vector (FILE* fp,double* vect,int dim)
{
  register int j;
  char dummy;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if (fscanf (fp," %c",&dummy) != 1 || dummy != '{') return TCL_ERROR;
  for (j = 0; j < dim; j++)
    if (fscanf (fp,"%lf",&vect[j]) != 1) return TCL_ERROR;
  if (fscanf (fp," %c \n",&dummy) != 1 || dummy != '}') return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_double_vector
   Writes double array into file.
   ATTENTION: The entries are converted to float first!!
   ------------------------------------------------------------------- */
int write_double_vector (FILE* fp,double* vect,int dim)
{
  register int j;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (j = 0; j < dim; j++) write_float (fp,(float) vect[j]);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_vector
   Reads double array from file.
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int read_double_vector (FILE* fp,double* vect,int dim)
{
  register int j;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (j = 0; j < dim; j++) vect[j] = (double) read_float (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_vector_incr
   Reads double array from file and adds it to the target object.
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int read_double_vector_incr (FILE* fp,double* vect,int dim)
{
  register int j;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (j = 0; j < dim; j++)
    vect[j] += (double) read_float (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_double_vector
   Reads double array from file. The array is not returned.
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int discard_double_vector (FILE* fp,int dim)
{
  return discard_float_vector (fp,dim);
}

/* -------------------------------------------------------------------
   print_double_matrix
   Prints FMatrix into file (Tcl list format). If 'fp' = 0, the target
   is the itf result string.
   ------------------------------------------------------------------- */
int print_double_matrix (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (mat);
  assert (mat->m && mat->n);
#endif

  for (i = 0; i < mat->m; i++)
    if (print_double_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_double_matrix
   Reads FMatrix from file (Tcl list format). The stored matrix must
   have the same size as the target matrix object.
   ------------------------------------------------------------------- */
int scan_double_matrix (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m && mat->n);
#endif

  for (i = 0; i < mat->m; i++)
    if (scan_double_vector (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_matrix
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int read_double_matrix (FILE* fp,DMatrix* mat)
{
  register int i,j;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    for (j = 0; j < mat->n; j++)
      mat->matPA[i][j] = (double) read_float (fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_double_matrix
   ATTENTION: The entries are converted to float first!!
   ------------------------------------------------------------------- */
int write_double_matrix (FILE* fp,DMatrix* mat)
{
  register int i,j;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    for (j = 0; j < mat->n; j++)
      write_float (fp,(float) mat->matPA[i][j]);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_matrix_incr
   Adds stored matrix to target matrix.
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int read_double_matrix_incr (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    read_double_vector_incr (fp,mat->matPA[i],mat->n);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_double_matrix
   ATTENTION: The stored entries are floats!!
   ------------------------------------------------------------------- */
int discard_double_matrix (FILE* fp,int m,int n)
{
  return discard_float_matrix (fp,m,n);
}

/* -------------------------------------------------------------------
   print_symm_double_matrix
   Converts upper triangular matrix into symmetric matrix and prints
   it into itf result string.
   ------------------------------------------------------------------- */
int print_symm_double_matrix (double* tmat,int dim)
{
  DMatrix* hmat;
  register int i,j,dcnt,ret;

#ifdef DEBUG
  assert (tmat);
  assert (dim > 0);
#endif

  if (!(hmat = dmatrixCreate (dim,dim))) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = dcnt = 0; i < dim; i++)
    for (j = i; j < dim; j++,dcnt++)
      hmat->matPA[i][j] = hmat->matPA[j][i] = tmat[dcnt];
  ret = dmatrixPutsItf ((ClientData) hmat,0,0);
  dmatrixFree(hmat);

  return ret;
}

/* -------------------------------------------------------------------
   write_double_vector_dep (DEBUG)
   ------------------------------------------------------------------- */
int write_double_vector_dep (FILE* fp,double* vect,int dim)
{
#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  fwrite (vect,sizeof(double),dim,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_vector_dep (DEBUG)
   ------------------------------------------------------------------- */
int read_double_vector_dep (FILE* fp,double* vect,int dim)
{
#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  if ((int)fread (vect,sizeof(double),dim,fp) != dim)
    return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_vector_incr_dep (DEBUG)
   ------------------------------------------------------------------- */
int read_double_vector_incr_dep (FILE* fp,double* vect,int dim)
{
  register int j;
  double temp;

#ifdef DEBUG
  assert (fp);
  assert (vect);
  assert (dim > 0);
#endif

  for (j = 0; j < dim; j++) {
    if (fread (&temp,sizeof(double),1,fp) != 1) return TCL_ERROR;
#ifdef DEBUG
    if (temp > 10e-20)
#endif
      vect[j] += temp;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_double_vector_dep (DEBUG)
   ------------------------------------------------------------------- */
int discard_double_vector_dep (FILE* fp,int dim)
{
  register int j;
  double temp;

#ifdef DEBUG
  assert (fp);
  assert (dim > 0);
#endif

  for (j = 0; j < dim; j++)
    if (fread (&temp,sizeof(double),1,fp) != 1) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_matrix_dep (DEBUG)
   ------------------------------------------------------------------- */
int read_double_matrix_dep (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    if ((int)fread (mat->matPA[i],sizeof(double),mat->n,fp) != mat->n)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   write_double_matrix_dep (DEBUG)
   ------------------------------------------------------------------- */
int write_double_matrix_dep (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    fwrite (mat->matPA[i],sizeof(double),mat->n,fp);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   read_double_matrix_incr_dep (DEBUG)
   ------------------------------------------------------------------- */
int read_double_matrix_incr_dep (FILE* fp,DMatrix* mat)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (mat);
  assert (mat->m > 0 && mat->n > 0);
#endif

  for (i = 0; i < mat->m; i++)
    if (read_double_vector_incr_dep (fp,mat->matPA[i],mat->n) == TCL_ERROR)
      return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   discard_double_matrix_dep (DEBUG)
   ------------------------------------------------------------------- */
int discard_double_matrix_dep (FILE* fp,int m,int n)
{
  register int i;

#ifdef DEBUG
  assert (fp);
  assert (m > 0 && n > 0);
#endif

  for (i = 0; i < m; i++)
    if (discard_double_vector_dep (fp,n) == TCL_ERROR) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   print_line
   Prints text line into file, followed by CR. If 'fp' = 0, the target
   is the itf result string.
   ------------------------------------------------------------------- */
int print_line (FILE* fp,char* str)
{
  if (fputs (str,fp) == EOF ||
      fputc ('\n',fp) == EOF) return TCL_ERROR;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   scan_line
   Reads text line into file. The CR at the end of the line is cut
   off.
   ------------------------------------------------------------------- */
int scan_line (FILE* fp,char* str,int buflen)
{
  int ret = TCL_OK;

  if (fgets (str,buflen,fp) == 0)
    ret = TCL_ERROR;
  else if (str[strlen (str) - 1] == '\n')
    str[strlen (str) - 1] = 0;

  return ret;
}


/* -------------------------------------------------------------------
   Memory management of codebook accus
   ------------------------------------------------------------------- */

/* block based memory allocation for full covariance accumulators
   because Linux cannot handle a huge number of small memory requests
   Here we allocate max. 100 blocks of 67 megabytes = 6.7G
*/

#define USE_BLK_MALLOC

#define BLK_SIZE  67108864
#define BLOCK_MAX 100

#define BYTE char

typedef struct {
  BYTE*  mem;
  BYTE*  ptr;
  size_t size;
} mem_block;

static int blockN = -1;
static mem_block blockA[BLOCK_MAX];

static void blockInit(mem_block* block) {
    if (NULL == block) {
        ERROR("blockInit: init failure\n");
        return;
    }
    if (NULL == (block->mem=malloc(BLK_SIZE+1))) {
        ERROR("blockInit: allocation failure\n");
        return;
    }
    block->ptr  = block->mem;
    block->size = 0;
}

static void blockDeinit(mem_block* block) {
  if (block->mem) free(block->mem);  
  block->ptr  = NULL;
  block->mem  = NULL;
  block->size = 0;
}

static void* malloc_from_block(mem_block* block,size_t request) {
  BYTE* ptr = block->ptr;
  if (block->size + request > BLK_SIZE) return NULL;
  block->ptr  = block->ptr  + request;
  block->size = block->size + request;
  return (void*) ptr;
}

static void* mallocBLK(size_t request) {
  int blockX;
  if (request > BLK_SIZE) {
    ERROR("allocation failure\n");
    return NULL;
  }
  if (request == 0) {
    return NULL;
  }
  if (blockN == -1) {
    blockN++;
    blockInit(blockA+blockN);
  }
  for (blockX=0;blockX<=blockN;blockX++) {
    void* ptr = malloc_from_block(blockA+blockX,request);
    if (ptr != NULL) return ptr;
  }
  if (blockN < BLOCK_MAX-1) {
    blockN++;
    blockInit(blockA+blockN);
    return  malloc_from_block(blockA+blockN,request);
  }
  ERROR("allocation failure\n");
  return NULL;
}

static void freeBLK() {
 int blockX;
 for (blockX=0;blockX<=blockN;blockX++) {
   blockDeinit(blockA+blockX);
 }
 blockN= -1;
}


/* -------------------------------------------------------------------
   free_accus_gaussian_int
   Phase: working.
   Deallocates all existing accus of Gaussian. Internal function.
   ------------------------------------------------------------------- */
static void free_accus_gaussian_int (CBNew* cd,double*** blockp,double** diagp)
{
  FREE(*diagp);
  if (*blockp) {
#ifndef USE_BLK_MALLOC
    FREE((*blockp)[0]);
#endif
    FREE(*blockp);
  }
}

/* -------------------------------------------------------------------
   allocate_accus_gaussian_int
   Phase: working.
   Allocates the 'accu_cov' fields for Gaussian 'k'. Existing accus
   are deallocated first. The new fields are cleared. The pointers
   to the fields are returned via 'blockp' and 'diagp', but not
   written into 'accu_cov'. Internal function.
   See 'CBNew_accu' comments in cbnew.h for information about the
   subphases (of working phase) determined by 'train_parmats' flag of
   the CBNewSet.
   ------------------------------------------------------------------- */
static int allocate_accus_gaussian_int (CBNew* cd,int k,double*** blockp,
					double** diagp)
{
  register int i,bound,d;
  int cb_num_dim;
  char train_parmats = cd->cbnew_set->train_parmats;

  if (*blockp || *diagp)
    free_accus_gaussian_int (cd,blockp,diagp);

  cb_num_dim = cd->cbnew_set->num_dim;
  if ((train_parmats != 0) && cd->parmat[k].mat) {
    GETMEM(*blockp,double*,cd->parmat[k].mat->num_block);
    /* Calc. allocation size */
    for (i = bound = 0; i < cd->parmat[k].mat->num_block; i++) {
      d = cd->parmat[k].mat->list[i]->m;
      bound += (d*(d+1))/2;
      (*blockp)[i] = 0;
    }
    /* Allocate arrays and init. pointers. Clear arrays */
#ifndef USE_BLK_MALLOC
    GETMEM((*blockp)[0],double,bound); 
#else
    (*blockp)[0] = (double*) mallocBLK(bound*sizeof(double));
#endif
    for (i = 0; i < bound; i++)
      (*blockp)[0][i] = 0.00;
    for (i = 0; i < cd->parmat[k].mat->num_block - 1; i++) {
      d = cd->parmat[k].mat->list[i]->m;
      (*blockp)[i+1] = (*blockp)[i] + (d*(d+1))/2;
    }
    d = cb_num_dim - cd->parmat[k].mat->num_rel;  /* size of 'diag' vector */
  } else
    d = cb_num_dim;
  if (d > 0) {
    /* Allocate and clear diag vector */
    GETMEM(*diagp,double,d);
    for (i = 0; i < d; i++)
      (*diagp)[i] = 0.00;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   free_accus_int
   Phase: all.
   Frees accu structures of codebook. Internal function.
   ------------------------------------------------------------------- */
static void free_accus_int (CBNew* cd,double* gamma_sum,DMatrix* mean,
			    CBNew_accu* cov)
{
  register int i;

  FREE(gamma_sum);
  FREEDMAT(mean);
  if (cov) {
    for (i = 0; i < cd->num_dens; i++)
      free_accus_gaussian_int (cd,&(cov[i].block),&(cov[i].diag));
    FREE(cov);
  }
}

/* -------------------------------------------------------------------
   free_accus_gaussian
   Phase: all.
   See 'free_accus_gaussian_int'.
   ------------------------------------------------------------------- */
int free_accus_gaussian (CBNew* cd,int k)
{
  assert (cd);
  assert (k >= 0 && k < cd->num_dens);

  free_accus_gaussian_int (cd,&(cd->accu_cov[k].block),
			   &(cd->accu_cov[k].diag));

  return TCL_OK;
}

/* -------------------------------------------------------------------
   allocate_accus_gaussian
   Phase: working.
   See 'allocate_accus_gaussian_int'.
   ------------------------------------------------------------------- */
int allocate_accus_gaussian (CBNew* cd,int k)
{
#ifdef DEBUG
  ASSERT_CB(cd,0);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
  assert (k >= 0 && k < cd->num_dens);
  assert (cd->active[k]);
#endif

  return allocate_accus_gaussian_int (cd,k,&(cd->accu_cov[k].block),
				      &(cd->accu_cov[k].diag));
}

/* -------------------------------------------------------------------
   free_accus
   Phase: all.
   See 'free_accus_int'.
   ------------------------------------------------------------------- */
int free_accus (CBNew* cd)
{
#ifdef DEBUG
  assert (cd);
#endif

  free_accus_int (cd,cd->accu_gamma_sum,cd->accu_mean,cd->accu_cov);
  cd->accu_gamma_sum = 0;
  cd->accu_mean = 0;
  cd->accu_cov = 0;

#ifdef USE_BLK_MALLOC
  freeBLK();
#endif

  return TCL_OK;
}

/* -------------------------------------------------------------------
   allocate_accus
   Phase: working.
   Allocates and clears all accus not already existing. Existing accus
   are not altered. The 'accu_flag' is not altered. Existing
   covariance accus of inactive Gaussians are deallocated.
   ------------------------------------------------------------------- */
int allocate_accus (CBNew* cd)
{
  register int i,cb_num_dim;

#ifdef DEBUG
  ASSERT_CB(cd,0);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
#endif

  cb_num_dim = cd->cbnew_set->num_dim;
  if (!cd->accu_gamma_sum) {
    GETMEM(cd->accu_gamma_sum,double,cd->num_dens);
    for (i = 0; i < cd->num_dens; i++)
      cd->accu_gamma_sum[i] = 0.00;
  }
  if (!cd->accu_mean) {
    GETDMAT(cd->accu_mean,cd->num_dens,cb_num_dim);
    dmatrixInitConst (cd->accu_mean,0.00);
  }
  if (!cd->accu_cov) {
    GETMEM(cd->accu_cov,CBNew_accu,cd->num_dens);
    for (i = 0; i < cd->num_dens; i++) {
      cd->accu_cov[i].diag = 0;
      cd->accu_cov[i].block = 0;
    }
  }
  for (i = 0; i < cd->num_dens; i++) {
    if (!cd->active[i])
      free_accus_gaussian_int (cd,&(cd->accu_cov[i].block),
			       &(cd->accu_cov[i].diag));
    else if (cd->accu_cov[i].block == 0 && cd->accu_cov[i].diag == 0) {
      if (allocate_accus_gaussian_int (cd,i,&(cd->accu_cov[i].block),
				       &(cd->accu_cov[i].diag)) == TCL_ERROR)
	return TCL_ERROR;
    }
  }

  return TCL_OK;
}


/* -------------------------------------------------------------------
   Miscellaneous stuff
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   diag_ml_estimation
   Phase: working. 'train_parmats' must be set.
   Computes the ML estimate of the relevant components of the
   diagonal variance matrix of Gaussian 'k'. In 'mle' we return
   this vector, multiplied by the exp. number of samples of the
   Gaussian (accuGammaS).
   The estimate is not affected by floor values (and therefore can't
   be used directly!). Note that mle[j] / accuGammaS corr. to
   diag[dim_vect[j]], dim_vect the dimension index vector of the
   parmat.
   ATTENTION: The Gaussian must be active ('active' entry = 1) and
   must have a parameter matrix! The 'train_parmats' flag of the
   CBNewSet must be set.
   ------------------------------------------------------------------- */
void diag_ml_estimation (CBNew* cd,int k,double* mle)
{
  register int i,i2,j,l,dcnt,d,icum,cb_num_dim;
  float* pmat_row;
  double* cov;
  double sum,factor;
  int* dim_vect;

#ifdef DEBUG
  assert (k >= 0 && k < cd->num_dens);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
  assert (cd->cbnew_set->train_parmats != 0);
  assert (cd->accu_mean);
  assert (cd->active[k]);
  assert (cd->parmat[k].mat);
  assert (mle);
#endif

  cb_num_dim = cd->cbnew_set->num_dim;
  dim_vect = cd->parmat[k].mat->dim_vect;
  if (!cd->parmat[k].mat->def) {
    for (i = icum = 0; i < cd->parmat[k].mat->num_block; i++,icum += d) {
      d = cd->parmat[k].mat->list[i]->m;
      cov = cd->accu_cov[k].block[i];
      for (i2 = 0; i2 < d; i2++) {
	pmat_row = cd->parmat[k].mat->list[i]->matPA[i2];
	sum = 0.00;
	for (j = dcnt = 0; j < d; j++) {
	  factor = (double) pmat_row[j];
	  sum += (factor * factor * cov[dcnt++]);
	  factor *= 2.00;
	  for (l = j+1; l < d; l++,dcnt++)
	    sum += (factor * ((double) pmat_row[l]) * cov[dcnt]);
	}
	mle[icum+i2] = sum;
      }
    }
  } else {
    /* The parmat is default init. with I, so we can ignore it. */
    for (i = icum = 0; i < cd->parmat[k].mat->num_block; i++,icum += d) {
      d = cd->parmat[k].mat->list[i]->m;
      cov = cd->accu_cov[k].block[i];
      for (i2 = dcnt = 0; i2 < d; i2++) {
	mle[icum+i2] = cov[dcnt];
	dcnt += (d - i2);
      }
    }
  }
}

/* -------------------------------------------------------------------
   reestimate_diag
   Phase: working. 'train_parmats' must be set.
   The "relevant" components of diagonal variance matrix of Gaussian
   'k' are reestimated.
   'diag' points to the 'k'-th row of the 'diag' matrix of 'cd'.
   The codebook pars may be inconsistent, so we don't use ASSERT_CB.
   See cbn_global.h for desc. of floor and threshold constants.
   ATTENTION: The Gaussian must be active ('active' entry = 1).
   ATTENTION: The helper array 'temp_samp' is used during reestimation!
   ------------------------------------------------------------------- */
int reestimate_diag (CBNew* cd,float* diag,int k)
{
  register int i;
  double beta,temp;
  double* mle;
  int* dim_vect;

#ifdef DEBUG
  assert (k >= 0 && k < cd->num_dens);
  assert (diag == cd->diag->matPA[k]);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
  assert (cd->cbnew_set->train_parmats != 0);
  assert (cd->accu_mean);
  assert (cd->active[k]);
#endif

  beta = cd->accu_gamma_sum[k];
  if (beta >= DIAG_UPDATE_THRES && cd->parmat[k].mat) {
    dim_vect = cd->parmat[k].mat->dim_vect;
    mle = cd->temp_samp;  /* We use 'temp_samp' to temp. store the MLE */
    diag_ml_estimation (cd,k,mle);
    for (i = 0; i < cd->parmat[k].mat->num_rel; i++) {
      if ((temp = mle[i] / beta) < DIAG_COMP_FLOOR)
	temp = DIAG_COMP_FLOOR;
      diag[dim_vect[i]] = (float) temp;
    }
  }
  cd->internal_flag = 0;  /* internals no longer valid */

  return TCL_OK;
}

/* -------------------------------------------------------------------
   eval_kl_gaussian
   Phase: working.
   Accus of corr. codebook must be locked.
   The determinant of the assoc. parameter matrices must be 1.
   Note: If a Gaussian has >= DIAG_UPDATE_THRES samples (see
   cbn_global.h), we can assume its 'diag' vector to be the MLE of
   the last recently accum. data. This fact can be used to save a lot
   of time during KL crit. evaluation (see below).
   ATTENTION: We use the helper array 'temp_samp' during the
   evaluation!
   The result is not normalized. To get the KL crit. of the whole
   system, sum the results over all Gaussians and divide by total
   number of samples.
   ATTENTION: The Gaussian must be active!
   ------------------------------------------------------------------- */
double eval_kl_gaussian (CBNew* cd,int k)
{
  register int j,cb_num_dim,num_rel;
  double sum,temp,beta;
  double* mle;
  int* dim_vect;

#ifdef DEBUG
  ASSERT_CB(cd,0);
  assert (k >= 0 && k < cd->num_dens);
  assert (cd->cbnew_set->phase == WORKING_PHASE);
  assert (cd->accu_flag == ACCUS_LOCKED);
  assert (cd->active[k]);
#endif

  cb_num_dim = cd->cbnew_set->num_dim;
  mle = cd->temp_samp;
  beta = cd->accu_gamma_sum[k];
  for (j = 0,temp = 0.00; j < cb_num_dim; j++)
    temp += log ((double) cd->diag->matPA[k][j]);
  sum = ((double) (- cd->distrib[k]) - 0.50 * temp) * beta;
  if (beta >= DIAG_UPDATE_THRES)
    sum -= (0.50 * beta * ((double) cb_num_dim));
  else {
    temp = 0.00;
    if ((cd->cbnew_set->train_parmats != 0) && cd->parmat[k].mat) {
      dim_vect = cd->parmat[k].mat->dim_vect;
      diag_ml_estimation (cd,k,mle);
      num_rel = cd->parmat[k].mat->num_rel;
      for (j = 0; j < num_rel; j++)
	temp += (mle[j] / (double) cd->diag->matPA[k][dim_vect[j]]);
      for (j = num_rel; j < cb_num_dim; j++)
	temp += (cd->accu_cov[k].diag[j-num_rel] /
		 (double) cd->diag->matPA[k][dim_vect[j]]);
    } else {
      /* Either there's no parmat or the parmat is kept fixed during
	 training. In both cases, the 'diag' accu contains the variance
	 stats in the transformed domain (which is the original domain
	 if there's no parmat)! */
      for (j = 0; j < cb_num_dim; j++)
	temp += (cd->accu_cov[k].diag[j] / (double) cd->diag->matPA[k][j]);
    }
    sum -= (0.50 * temp);
  }

  return sum;
}

/* -------------------------------------------------------------------
   getString
   Custom function for string parsing
   ARGV_STRING treats string with leading '@' in a way we don't want,
   so we've to replace it.
   ------------------------------------------------------------------- */
int getString (ClientData cd,char* key,ClientData result,int *argcP,
		      char *argv[])
{
  if (*argcP < 1) return -1;
  *((char**) result) = argv[0];
  (*argcP)--;

  return 0;
}
