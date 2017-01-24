/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: several aux functions for semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_common.h
    Date    :  $Id: cbn_common.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 4.2  2000/08/27 15:31:08  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.2.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 4.1  2000/08/24 13:32:25  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 1.1.2.1  2000/08/24 00:17:51  jmcd
    Fixed the hairy elbows Hagen left for me to find.


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _cbn_common
#define _cbn_common

#include "itf.h"
#include "matrix.h"

#include <stdio.h>

struct CBNew;


/* --- Interface --- */

int print_int_vector (FILE* fp,int* vect,int dim);
int scan_int_vector (FILE* fp,int* vect,int dim);
int write_int_vector (FILE* fp,int* vect,int dim);
int read_int_vector (FILE* fp,int* vect,int dim);

int print_char_vector (FILE* fp,char* vect,int dim);
int scan_char_vector (FILE* fp,char* vect,int dim);
int write_char_vector (FILE* fp,char* vect,int dim);
int read_char_vector (FILE* fp,char* vect,int dim);
int discard_char_vector (FILE* fp,int dim);

int print_float_vector (FILE* fp,float* vect,int dim);
int scan_float_vector (FILE* fp,float* vect,int dim);
int write_float_vector (FILE* fp,float* vect,int dim);
int read_float_vector (FILE* fp,float* vect,int dim);
int read_float_vector_incr (FILE* fp,float* vect,int dim);
int discard_float_vector (FILE* fp,int dim);

int print_float_matrix (FILE* fp,FMatrix* mat);
int scan_float_matrix (FILE* fp,FMatrix* mat);
int read_float_matrix (FILE* fp,FMatrix* mat);
int write_float_matrix (FILE* fp,FMatrix* mat);
int read_float_matrix_incr (FILE* fp,FMatrix* mat);
int discard_float_matrix (FILE* fp,int m,int n);
int print_symm_float_matrix (float* tmat,int dim);

int print_double_vector (FILE* fp,double* vect,int dim);
int scan_double_vector (FILE* fp,double* vect,int dim);
int write_double_vector (FILE* fp,double* vect,int dim);
int read_double_vector (FILE* fp,double* vect,int dim);
int read_double_vector_incr (FILE* fp,double* vect,int dim);
int discard_double_vector (FILE* fp,int dim);

int print_double_matrix (FILE* fp,DMatrix* mat);
int scan_double_matrix (FILE* fp,DMatrix* mat);
int read_double_matrix (FILE* fp,DMatrix* mat);
int write_double_matrix (FILE* fp,DMatrix* mat);
int read_double_matrix_incr (FILE* fp,DMatrix* mat);
int discard_double_matrix (FILE* fp,int m,int n);
int print_symm_double_matrix (double* tmat,int dim);

int write_double_vector_dep (FILE* fp,double* vect,int dim);
int read_double_vector_dep (FILE* fp,double* vect,int dim);
int read_double_vector_incr_dep (FILE* fp,double* vect,int dim);
int discard_double_vector_dep (FILE* fp,int dim);
int read_double_matrix_dep (FILE* fp,DMatrix* mat);
int write_double_matrix_dep (FILE* fp,DMatrix* mat);
int read_double_matrix_incr_dep (FILE* fp,DMatrix* mat);
int discard_double_matrix_dep (FILE* fp,int m,int n);

int print_line (FILE* fp,char* str);
int scan_line (FILE* fp,char* str,int buflen);

int free_accus_gaussian (struct CBNew* cd,int k);
int allocate_accus_gaussian (struct CBNew* cd,int k);
int free_accus (struct CBNew* cd);
int allocate_accus (struct CBNew* cd);
int convert_accus (struct CBNew* cd);

void diag_ml_estimation (struct CBNew* cd,int k,double* mle);
int reestimate_diag (struct CBNew* cd,float* diag,int k);
double eval_kl_gaussian (struct CBNew* cd,int k);

int getString (ClientData cd,char* key,ClientData result,int *argcP,
		      char *argv[]);

#endif

#ifdef __cplusplus
}
#endif
