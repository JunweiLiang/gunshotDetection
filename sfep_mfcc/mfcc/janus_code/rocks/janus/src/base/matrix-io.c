/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Matrix I/O Module
               ------------------------------------------------------------

    Author  :  Martin Maier
    Module  :  matrix-io.c
    Date    :  $Id: matrix-io.c 3377 2011-02-28 00:42:24Z metze $
    Remarks :  Routines to read or write matrices or vectors in binary
               form can be found in this module. The files start with
	       a magic (4 Bytes) defining the kind of object.
	       The magic for a fmatrix for example is "FMAT". The following
	       integers define the size of the matrix or the vector.
	       In case of a (m x n) matrix m can also be -1 because this
	       information can be retrieved using the file length. On the
	       other hand this makes it possible to append additional rows to
	       the matrix file. Before for data starts 'count' could
	       be stored as float.

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

    $Log$
    Revision 3.10  2007/03/02 14:50:55  fuegen
    gcc clean

    Revision 3.9  2007/03/02 12:59:29  fuegen
    added SSE support (Florian Metze, Friedrich Faubel)

    Revision 3.8  2005/03/04 09:12:45  metze
    Andreas Eller's DMatrix methods added

    Revision 3.7  2003/08/14 11:19:52  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.4.6  2003/07/02 17:09:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.6.4.5  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.6.4.4  2002/02/17 14:46:39  metze
    Proper fileClose in cload/ csave routines

    Revision 3.6.4.3  2002/01/18 13:33:05  metze
    Added fmatrixCLoad and fmatrixCSave to allow for compressed files

    Revision 3.6.4.2  2002/01/16 09:54:46  metze
    Extra checks in fmatrixBLoad and fmatrixBSave

    Revision 3.6.4.1  2001/02/27 18:38:40  soltau
    Fixed clib2.2 compatibility problems

    Revision 3.6  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.5.4.1  2001/01/12 15:16:57  janus
    necessary changes for running janus under WINDOWS

    Revision 3.5  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.4.34.1  2000/11/06 10:50:35  janus
    Made changes to enable compilation under -Wall.

    Revision 3.4  2000/08/16 11:25:15  soltau
    added append mode for fmatrixBload

    Revision 3.3  1997/07/18 20:23:46  tschaaf
    gcc / DFKI - clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.3  96/03/05  14:09:54  rogina
 * made gcc -Wall warnings go away
 * 
 * Revision 1.2  1996/03/02  12:57:51  maier
 * bload & bsave for fvector
 *
 * Revision 1.1  1995/12/08  04:53:16  maier
 * Initial revision
 *

  
   ======================================================================== */

/* ------------------------------------------------------------------------
   includes
   ------------------------------------------------------------------------ */
#include <string.h>
#include <math.h>

#include "common.h"
#include "matrix.h"                   /* includes  itf.h(tcl.h), common.h */
#include "mach_ind_io.h" 


/* ------------------------------------------------------------------------
   static functions
   ------------------------------------------------------------------------ */
static FILE    *openFile(  char *filename, char *mode, int retry, int sleep);
static int     readMagic4(FILE *fp,char* magic);
static long    bytesLeft( FILE *fp);

/* ========================================================================
    Float Vector
   ======================================================================== */

/* ------------------------------------------------------------------------
    BSave
   ------------------------------------------------------------------------ */
FVector* fvectorBSave (FVector *A, char *filename)
{
  FILE    *fp;
  int     total;
  FVector *ret = A; 
  assert(A);

  if ((fp=openFile(filename,"w",1,2)) == NULL) return NULL;
  fwrite("FVEC",sizeof(char),4,fp);
  write_int(fp,A->n);
  write_float(fp,(float)A->count);      /* unfort. there's no write_double */
  
  total = A->n;
  if (write_floats(fp,A->vecA,total) != total) ret = NULL;
  fileClose (filename, fp);
  return ret;
}

int fvectorBSaveItf( ClientData cd, int argc, char *argv[])
{
  FVector *A = (FVector*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fvectorBSave(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    BLoad
   ------------------------------------------------------------------------ */
FVector* fvectorBLoad (FVector *A, char *filename)
{
  FILE    *fp;
  int     n,total;
  long    left;
  long    total_size;
  FVector *ret = A; 
  assert(A);

  if ((fp=openFile(filename,"r",1,2)) == NULL) return NULL;
  if (readMagic4(fp,"FVEC")) {

    /* -- read dimensions -- */
    n = read_int(fp);
    A->count = read_float(fp);
    
    /* -- check with file length -- */
    left = bytesLeft(fp);
    total = n;
    total_size = total*sizeof(float);


    if (left < 0L || left != total_size) {
      ERROR("Number of bytes in file = %ld don't match vector dimension:\n"
	    "%d times %d bytes\n",left,n,sizeof(float));
      ret = NULL;
    }

    /* -- load data -- */
    else {
      if (fvectorResize(A,n) < 0 ||
	  read_floats(fp,A->vecA,total) != total) ret = NULL;
    }
  }
  else ret = NULL;

  fileClose (filename, fp);
  return ret;
}

int fvectorBLoadItf( ClientData cd, int argc, char *argv[])
{
  FVector *A = (FVector*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fvectorBLoad(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}


/* ========================================================================
    Float Matrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    BAppend

    This method will append the rows of a matrix A to a matrix file without
    any checking.
    If the file doesn't exist yet it will be created and a header will
    be written allowing other matrices to be appended.
    The matrices created in such manner can be read with the normal
    matrixBLoad routine.
   ------------------------------------------------------------------------ */

FMatrix* fmatrixBAppend (FMatrix *A, char *filename)
{
  FILE    *fp;
  int     i;
  FMatrix *ret = A; 
  assert(A);

  if ((fp=openFile(filename,"a",1,2)) == NULL) return NULL;

  /* if that is the start of the file write header */
  if (ftell(fp) == 0) {
    fwrite("FMAT",sizeof(char),4,fp);
    write_int(fp,-1);
    write_int(fp,A->n);
    write_float(fp,0.0);
  }
  
  for (i = 0; i < A->m; i++)
    if (write_floats (fp, A->matPA[i], A->n) != A->n) 
      ret = NULL;
  /* total = A->m * A->n;
     if (write_floats(fp,A->matPA[0],total) != total) ret = NULL; */
  fileClose (filename, fp);
  return ret;
}

int fmatrixBAppendItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fmatrixBAppend(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    BSave
   ------------------------------------------------------------------------ */
FMatrix* fmatrixBSave (FMatrix *A, char *filename)
{
  FILE    *fp;
#ifndef SSE_OPT
  int      total;
#endif
  FMatrix *ret    = A; 

  assert (A);

  if (!(fp = openFile (filename, "w", 1, 2)))
    return NULL;

  fwrite ("FMAT", sizeof(char), 4, fp);
  write_int (fp,A->m);
  write_int (fp,A->n);
  write_float (fp, (float) A->count);      /* unfort. there's no write_double */

#ifdef SSE_OPT
  if (A->matPA) {
    int i;
    for (i=0; i<A->m; i++)
      if (write_floats(fp, A->matPA[i], A->n) != A->n) ret = NULL;
  }
#else  
  total = A->m * A->n;
  if (A->matPA && write_floats (fp, A->matPA[0], total) != total)
    ret = NULL;
#endif

  fileClose (filename, fp);

  return ret;
}

int fmatrixBSaveItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fmatrixBSave(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    BLoad
   ------------------------------------------------------------------------ */

int fmatrixBLoad_ (FMatrix *A, char *filename, int im, int append);

FMatrix* fmatrixBLoad (FMatrix *A, char *filename, int im) {
  if (fmatrixBLoad_(A,filename,im,0) != TCL_OK) {
    return NULL;
  } else {
    return A;
  }
}

int fmatrixBLoad_ (FMatrix *A, char *filename, int im, int append) {
  FILE    *fp;
  int      m, n, total, m_old, n_old;
  int      left;
  long     total_size;
  /* FMatrix *ret = A;  */

  assert(A);

  m_old = A->m;
  n_old = A->n;

  if (!(fp = openFile (filename, "r", 1, 2))) {
    ERROR("could not open %s\n", filename);
    return TCL_ERROR;
  }


  if (readMagic4(fp,"FMAT")) {

    /* -- read dimensions -- */
    /* fpos_t position;
       fgetpos(fp, &position); */

    m = read_int(fp);
    /* fgetpos(fp, &position); */
    n = read_int(fp);
    /* fgetpos(fp, &position); */
    A->count = read_float(fp);
    /* fgetpos(fp, &position); */
    
    /* -- check with file length -- */
    left = bytesLeft(fp);
    if (n && (m < 0 || im)) m = left/(sizeof(float)*n); /* number of rows wasn't set */
    total = m * n;
    total_size = total*sizeof(float);

    if (!left) {
      WARN ("File empty, matrix unchanged!\n");

    } else if (left < 0 || left != total_size) {
      ERROR ("Number of bytes in file = %ld don't match matrix dimension:\n"
	     "%d x %d times %d bytes\n",left,m,n,sizeof(float));
      return TCL_ERROR;
    } else { /* -- load data -- */
#ifdef SSE_OPT
      if (append == 0) {
	if (fmatrixResize(A,m,n) == TCL_ERROR) {
	  ERROR("Failed to resize Matrix\n");
	  return TCL_ERROR;
	} else {
	  int i;
	  for (i=0; i<m; i++)
	    if (read_floats(fp,A->matPA[i],n) != n) {
	      ERROR("Could not read %d floats from file\n", n);
	      return TCL_ERROR;
	    }
	}
      } else {
	if (n_old != n) {
	  ERROR("Dimension mismatch: n_old= $d != n_new= %d\n",n_old,n);
	  return TCL_ERROR;
	}
	if (fmatrixResize(A,m+m_old,n) < 0) {
	  ERROR("Failed to resize Matrix\n");
	  return TCL_ERROR;
	} else {
	  int i;
          for (i=m_old; i<m_old+m; i++)
	    if (read_floats(fp,A->matPA[i],n) != n) {
	      ERROR("Could not read %d floats from file\n", n);
	      return TCL_ERROR;
	    }
	}
      }
#else
      if (append == 0) {
	if (fmatrixResize(A,m,n) < 0 || read_floats(fp,A->matPA[0],total) != total) {
	  ERROR ("Resize operation failed or could not read %d floats from file\n", total);
	  return TCL_ERROR;
	}
      } else {
	if (n_old != n) {
	  ERROR("Dimension mismatch: n_old= $d != n_new= %d\n",n_old,n);
	  return TCL_ERROR;
	}
	if (fmatrixResize(A,m+m_old,n) < 0 || read_floats(fp,A->matPA[m_old],total) != total) {
	  ERROR ("Resize operation failed or could not read %d floats from file\n", total);
	  return TCL_ERROR;
	}
      }
#endif
    }

  } else {
    ERROR("File is not an FMatrix\n");
    return TCL_ERROR;
  }

  fileClose (filename, fp);
  return TCL_OK;
}

int fmatrixBLoadItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*)cd;
  char *name = NULL;
  int     im = 0;
  int append = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name,   cd, "",
    "-im",        ARGV_INT,    NULL, &im,     cd, "ignore m in file header",
    "-append",    ARGV_INT,    NULL, &append, cd, "append file to matrix",

    NULL) != TCL_OK) return TCL_ERROR;
  if (fmatrixBLoad_(A,name,im,append) != TCL_OK) return TCL_ERROR;
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Compressed Save
   ------------------------------------------------------------------------ */
FMatrix* fmatrixCSave (FMatrix *A, char *filename, char *modus)
{
  FILE    *fp;

  if (!(fp = fileOpen (filename, "w"))) {
    ERROR ("Cannot open file '%s' for writing.\n", filename);
    return NULL;
  }
  
  if (streq (modus, "rl")) {
    /* Run-length compression */
    int      c;
    float    *a, *o;
    float    l;

    fwrite      ("FMRL", sizeof(char), 4, fp);
    write_int   (fp, A->m);
    write_int   (fp, A->n);
    write_float (fp, (float) A->count);
 
    c = 0;
    l = A->matPA[0][0];
    for (a = A->matPA[0], o = a+A->m*A->n; a < o; a++) {
      if ((*a != l && c) || c == 32767) {
	write_short (fp, c);
	write_float (fp, l);
	l = *a;
	c = 1;
      } else
	c++;
    }
    write_short (fp, c);
    write_float (fp, l);

  } else {
    /* Compression (hopefully) achieved through filter */
    int ret = 1;

    fwrite         ("FMAT", sizeof(char), 4, fp);
    write_int      (fp, A->m);
    write_int      (fp, A->n);
    write_float    (fp, (float) A->count);
    if (A->matPA) {
      int i;
      for (i = 0; i < A->m; i++)
	if (write_floats (fp, A->matPA[i], A->n) != A->n) ret = 0;
    }
  }

  fileClose (filename, fp);
  return A;
}

int fmatrixCSaveItf (ClientData cd, int argc, char *argv[])
{
  FMatrix  *A = (FMatrix*) cd;
  char *name  = NULL;
  char *modus = "none";

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name,  cd, "filename",
    "-mode",      ARGV_STRING, NULL, &modus, cd, "extra compression modes: rl, none",
    NULL) != TCL_OK) return TCL_ERROR;

  if (!fmatrixCSave (A, name, modus))
    return TCL_ERROR;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Compressed load
   ------------------------------------------------------------------------ */

FMatrix* fmatrixCLoad (FMatrix *A, char *filename, int append) 
{
  int     m, n, old_m;
  FILE    *fp;
  char    buf[512];

  if (!(fp = fileOpen (filename, "r"))) {
    ERROR ("Cannot open file '%s' for reading.\n", filename);
    return NULL;
  }

  if (!fread (buf, sizeof(char), 4, fp)) {
    ERROR ("File contains no data.\n");
    fileClose (filename, fp);
    return NULL;
  }
  buf[4] = 0;
  m        = read_int   (fp);
  n        = read_int   (fp);
  A->count = read_float (fp);

  if (m < 0) {
    /* we need to find out the length ourselves */
    float* tmpA = malloc (sizeof(float)*n);
    while (!feof (fp)) {
      read_floats (fp, tmpA, n);
      m++;
    }
    free (tmpA);
    fileClose (filename, fp);
    INFO ("fmatrixCLoad: m= %d n=%d\n", m, n);
    if (!(fp = fileOpen (filename, "r"))) {
      ERROR ("Cannot open file '%s' for reading.\n", filename);
      return NULL;
    }
    fread (buf, sizeof(char), 4, fp);
               read_int   (fp);
    n        = read_int   (fp);
    A->count = read_float (fp);
  }
  old_m = (append) ? A->m : 0;
  if (!append && fmatrixResize (A, m, n) == TCL_ERROR) {
    ERROR ("fmatrixCLoad: allocation failure.\n");
    fileClose (filename, fp);
    return NULL;
  } else if (append && (n != A->n || fmatrixResize (A, m+old_m, n) == TCL_ERROR)) {
    ERROR ("fmatrixCLoad: allocation failure.\n");
    fileClose (filename, fp);
    return NULL;    
  }

  if (streq (buf, "FMRL")) {
    /* Run-length compression */
    int   c;
    float *a, *o;
    float l;

    for (a = A->matPA[old_m], o = a+m*n; a < o; ) {
      c = read_short (fp);
      l = read_float (fp);      
      while (c--)
	*a++ = l;
    }

  } else if (streq (buf, "FMAT")) {
    /* Simple memory dump */
    int c;
    for (c = old_m; c < A->m; c++)
      read_floats (fp, A->matPA[c], n);
    
  } else {
    ERROR ("Don't know how to decompress '%s'.\n", buf);
    fileClose (filename, fp);
    return NULL;
  }

  fileClose (filename, fp);
  return A;
}

int fmatrixCLoadItf (ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*) cd;
  char *name = NULL;
  int append = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    "-append",    ARGV_INT,    NULL, &append, cd, "append file to matrix",
    NULL) != TCL_OK) return TCL_ERROR;

  if (!fmatrixCLoad (A, name, append))
    return TCL_ERROR;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Matlab load
   ------------------------------------------------------------------------ */
   
typedef struct {
  int type;
  int mrows;
  int ncols;
  int imagf;
  int namelen;
} MMatrix;

/* type 
     The type flag contains an integer whose decimal digits encode storage information. If
     the integer is represented as MOPT where M is the thousands digit, O is the hundreds
     digit, P is the tens digit, and T is the ones digit, then:

   - M indicates the numeric format of binary numbers on the machine that wrote the file.
     Use this table to determine the number to use for your machine:
     0                      IEEE Little Endian (PC, 386, 486, DEC Risc)
     1                      IEEE Big Endian (Macintosh, SPARC, Apollo,SGI, HP 9000/300,
                            other Motorola)
     2                      VAX D-float
     3                      VAX G-float
     4                      Cray

   - O is always 0 (zero) and is reserved for future use.

   - P indicates which format the data is stored in according to the following table:
     0                     double-precision (64-bit) floating point numbers
     1                     single-precision (32-bit) floating point numbers
     2                     32-bit signed integers
     3                     16-bit signed integers
     4                     16-bit unsigned integers
     5                     8-bit unsigned integers
     The precision used by the save command depends on the size and type of each matrix.
     Matrices with any noninteger entries and matrices with 10,000 or fewer elements are
     saved in floating point formats requiring 8 bytes per real element. Matrices with all
     integer entries and more than 10,000 elements are saved in the following formats,
     requiring fewer bytes per element.
     Element range         Bytes per element
     [0:255]               1
     [0:65535]             2
     [-32767:32767]        2
     [-2^31+1:2^31-1]      4
     other                 8

   - T indicates the matrix type according to the following table:
     0                     Numeric (Full) matrix
     1                     Text matrix
     2                     Sparse matrix
     Note that the elements of a text matrix are stored as floating point numbers between
     0 and 255 representing ASCII-encoded characters.
*/

FMatrix* fmatrixMLoad (FMatrix *A, char *filename, int append, int idx) 
{
  int     i;
  FILE    *fp;
  char    buf[512];
  MMatrix mm;
  float* tmp = NULL;

  if (!(fp = fileOpen (filename, "r"))) {
    ERROR ("Cannot open file '%s' for reading.\n", filename);
    return NULL;
  }

  if (!fread (&mm, sizeof(char), 20, fp)) {
    ERROR ("File contains no data.\n");
    fileClose (filename, fp);
    return NULL;
  }

  if (mm.type < 0    || mm.namelen < 0  || mm.mrows < 0 || mm.ncols < 0 || mm.imagf < 0 ||
      mm.type > 9999 || mm.namelen > 99 || mm.imagf > 1) {
    ERROR ("This does not look like a Matlab file.\n");
    fileClose (filename, fp);
    return NULL;
  } 

  for (i = 0; i < idx && !feof (fp); i++) {
    /* printf ("MMatrix found - %d %d %d %d %d\n", mm.type, mm.mrows, mm.ncols, mm.imagf, mm.namelen); */
    fseek (fp, mm.namelen+(1+mm.imagf)*mm.mrows*mm.ncols*sizeof (float), SEEK_CUR);
    if (feof (fp))
      break;
    fread (&mm, sizeof(char), 20, fp);
  }

  if (feof (fp)) {
    ERROR ("File contains insufficient data.\n");
    fileClose (filename, fp);
    return NULL;
  }
  fread (buf, sizeof(char), mm.namelen, fp);

  INFO ("Matlab '%s' type=%d %d x %d\n", buf, mm.type, mm.mrows, mm.ncols);

  if (mm.imagf)
    WARN ("Imaginary part ignored.\n");
  if (mm.type != 10)
    WARN ("Matrix type %d unheard of.\n", mm.type);
  if (append)
    WARN ("-append ignored.\n");

  if (fmatrixResize (A, mm.mrows, mm.ncols) == TCL_ERROR) {
    ERROR("Failed to resize Matrix\n");
    fileClose (filename, fp);
    return NULL;
  }
  if (!(tmp = malloc (mm.mrows * sizeof (float)))) {
    ERROR ("Failed to allocate memory.\n");
    fileClose (filename, fp);
    return NULL;
  }

  A->count = 0;
  for (i = 0; i < mm.ncols; i++) {
    int j;
    if (fread (tmp, sizeof (float), mm.mrows, fp) != mm.mrows) {
      ERROR("Could not read %d floats from file\n", mm.mrows);
      fileClose (filename, fp);
      return NULL;
    }
    for (j = 0; j < mm.mrows; j++)
      A->matPA[j][i] = tmp[j];
  }

  free (tmp);
  fileClose (filename, fp);
  return A;
}

int fmatrixMLoadItf (ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*) cd;
  char *name = NULL;
  int append = 0;
  int    idx = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name,   cd, "",
    "-idx",       ARGV_INT,    NULL, &idx,    cd, "index of matrix to load",
    "-append",    ARGV_INT,    NULL, &append, cd, "append file to matrix",
    NULL) != TCL_OK) return TCL_ERROR;

  if (!fmatrixMLoad (A, name, append, idx))
    return TCL_ERROR;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ISave (IBM format, by Brian Kingsbury)
   ------------------------------------------------------------------------ */

FMatrix* fmatrixISave (FMatrix *A, char *filename)
{
  FILE    *fp;
  FMatrix *ret = A; 
  int i;

  assert (A);

  if (!(fp = openFile (filename, "w", 1, 2)))
    return NULL;

  write_int (fp, A->m); /* no of frames */
  for (i = 0; i < A->m && ret; i++) {
    write_int (fp, A->n); /* no of features */
    if (write_floats(fp, A->matPA[i], A->n) != A->n) ret = NULL;
  }

  fileClose (filename, fp);

  return ret;
}

int fmatrixISaveItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (fmatrixISave(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}

FMatrix* fmatrixILoad (FMatrix *A, char *filename) 
{
  int     m, n, i;
  FILE    *fp;

  if (!(fp = fileOpen (filename, "r"))) {
    ERROR ("Cannot open file '%s' for reading.\n", filename);
    return NULL;
  }

  m        = read_int   (fp);
  n        = read_int   (fp);
  A->count = 0;

  fileClose (filename, fp);
  fp = fileOpen (filename, "r");

  m        = read_int   (fp);
  if (fmatrixResize (A, m, n) == TCL_ERROR) {
    ERROR ("fmatrixILoad: allocation failure.\n");
    fileClose (filename, fp);
    return NULL;
  }

  for (i = 0; i < m; i++) {
    n = read_int (fp);
    read_floats (fp, A->matPA[i], n);
  }

  fileClose (filename, fp);
  return A;
}

int fmatrixILoadItf (ClientData cd, int argc, char *argv[])
{
  FMatrix *A = (FMatrix*) cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name,   cd, "",
    NULL) != TCL_OK) return TCL_ERROR;

  if (!fmatrixILoad (A, name))
    return TCL_ERROR;

  return TCL_OK;
}

/* ========================================================================
    Int Matrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    BSave
   ------------------------------------------------------------------------ */
IMatrix* imatrixBSave (IMatrix *A, char *filename)
{
  FILE    *fp;
  int     total;
  IMatrix *ret = A; 
  assert(A);

  if ((fp=openFile(filename,"w",1,2)) == NULL) return NULL;
  fwrite("IMAT",sizeof(char),4,fp);
  write_int(fp,A->m);
  write_int(fp,A->n);
  
  total = A->m * A->n;
  if (write_ints(fp,A->matPA[0],total) != total) ret = NULL;
  fileClose (filename, fp);
  return ret;
}

int imatrixBSaveItf( ClientData cd, int argc, char *argv[])
{
  IMatrix *A = (IMatrix*)cd;
  char *name = NULL;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;
  if (imatrixBSave(A,name) == NULL) return TCL_ERROR;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    BLoad
   ------------------------------------------------------------------------ */
IMatrix* imatrixBLoad (IMatrix *A, char *filename, int im)
{
  FILE    *fp;
  int     m,n,total;
  long    left;
  long    total_size;
  IMatrix *ret = A; 
  assert(A);

  if ((fp=openFile(filename,"r",1,2)) == NULL) return NULL;
  if (readMagic4(fp,"IMAT")) {

    /* -- read dimensions -- */
    m = read_int(fp);
    n = read_int(fp);
    
    /* -- check with file length -- */
    left = bytesLeft(fp);
    if (m<0 || im) m = left/(sizeof(int)*n); /* number of rows wasn't set */
    total = m * n;
    total_size = total*sizeof(int);
    if (left < 0 || left != total_size) {
      ERROR("Number of bytes in file = %ld don't match matrix dimension:\n"
	    "%d x %d times %d bytes\n",left,m,n,sizeof(int));
      ret = NULL;
    }

    /* -- load data -- */
    else {
      if (imatrixResize(A,m,n) < 0 ||
	  read_ints(fp,A->matPA[0],total) != total) ret = NULL;
    }
  }
  else ret = NULL;

  fileClose (filename, fp);
  return ret;
}

int imatrixBLoadItf( ClientData cd, int argc, char *argv[])
{
  IMatrix *A = (IMatrix*)cd;
  char *name = NULL;
  int     im = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    "-im",        ARGV_INT,    NULL, &im,   cd, "ignore m in file header",
    NULL) != TCL_OK) return TCL_ERROR;
  if (imatrixBLoad(A,name,im) == NULL) return TCL_ERROR;
  return TCL_OK;
}

/* ========================================================================
    Double Matrix
   ======================================================================== */

int dmatrixBLoad(DMatrix* A, const char* filename)
{
        int m, n;
        double d;
        FILE* pFile;

        if((pFile = fopen(filename, "rb")) == 0)
        {
                ERROR("unable to open file %s\n", filename);
                return TCL_ERROR;
        }

        fread(&m, sizeof(m), 1, pFile);
        fread(&n, sizeof(n), 1, pFile);
        dmatrixResize(A, m, n);
        fread(&d, sizeof(d), 1, pFile);
        A->count = d;
        fread(A->matPA[0], sizeof(double), A->m * A->n, pFile);

        fclose(pFile);

        return TCL_OK;
}

int dmatrixBLoadItf(ClientData cd, int argc, char *argv[])
{
        DMatrix *A = (DMatrix*)cd;
        char *filename = NULL;
        argc--;

        if (itfParseArgv( argv[0], &argc, argv+1, 0,
                "<filename>", ARGV_STRING, NULL, &filename,   cd, "",
                NULL) != TCL_OK) return TCL_ERROR;

        dmatrixBLoad(A, filename);

        return TCL_OK;
}

int dmatrixBSave(DMatrix* A, const char* filename)
{
        int m, n;
        double d;
        FILE *pFile;

        if((pFile = fopen(filename, "wb")) == 0)
        {
                ERROR("unable to open file %s\n", filename);
                return TCL_ERROR;
        }

        m = A->m; fwrite(&m, sizeof(m), 1, pFile);
        n = A->n; fwrite(&n, sizeof(n), 1, pFile);
        d = A->count; fwrite(&d, sizeof(d), 1, pFile);
        fwrite(A->matPA[0], sizeof(double), A->m * A->n, pFile);

        fclose(pFile);

        return TCL_OK;
}

int dmatrixBSaveItf(ClientData cd, int argc, char *argv[])
{
        DMatrix *A = (DMatrix*)cd;
        char *filename = NULL;
        argc--;

        if (itfParseArgv( argv[0], &argc, argv+1, 0,
                "<filename>", ARGV_STRING, NULL, &filename,   cd, "",
                NULL) != TCL_OK) return TCL_ERROR;

        dmatrixBSave(A, filename);

        return TCL_OK;
}


/* ========================================================================
   Static functions
   ======================================================================== */
/* ---------------------------------------------------------
   openFile
   --------------------------------------------------------- */

static FILE *openFile(char *filename, char *mode, int retry, int time)
{
	int count = 0;
	FILE *fp = NULL;
	while (count <= retry) {
		if ((fp=fileOpen(filename,mode))==NULL) {
			WARN("%d. try\n",count+1);
			sleep(time);
			count++;
		}
		else break;
	}
	if (count>retry) {
		ERROR("could not open %s\n",filename);
	}
	else if (count!=0)
		WARN("\nLoading of feature file %s was successful only after %d attempts\n",
			filename,count+1);
	return fp;
}

/* ---------------------------------------------------------
   readMagic4

   Read a 4 byte long magic from file (fp) and compare it
   to the given 'magic'. If they are equal return 1, if not
   return 0 and set back the file pointer.
   --------------------------------------------------------- */
static int readMagic4(FILE *fp,char* magic)
{
  static char *fmagic = NULL;
  long        current = ftell(fp);
	size_t      t = 4;
  
	fseek(fp,current,SEEK_SET);

  if (fmagic == NULL) fmagic = (char*)malloc(5 * sizeof(char));
	fmagic[4] = '\0';
	t = fread (fmagic, sizeof(char), 4, fp);

  if (  t != 4   ||
       strncmp(magic,fmagic,4)) {
    fseek(fp,current,SEEK_SET);
    SERROR("Couldn't find magic %s in file\n",magic);
    return 0;
  }

  return 1;
}

/* ---------------------------------------------------------------------------
   bytesLeft

   Returns the number of bytes left in file.
   ------------------------------------------------------------------------- */
static long bytesLeft(FILE* fp) {

   fpos_t position;
   long   left;

   if ( fgetpos(fp, &position) != 0 ) return -1;
   left = ftell (fp);

   fseek(fp, (long)0, SEEK_END);
   left = ftell (fp) - left;

   if ( fsetpos(fp, &position) != 0 ) return -3;

   return left;
}
