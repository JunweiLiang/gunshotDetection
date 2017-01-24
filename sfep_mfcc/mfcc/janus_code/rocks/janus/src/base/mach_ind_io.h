/*#############################################################################################################################
 #
 #  mach_ind_io.h:	Header file for machine independent binary IO routines defined in mach_ind_io.c
 #
 ##############################################################################################################################
 #
 # DESCRIPTION :        See mach_ind_io.c for some more (minimal) documentation.
 #
 # NOTE:	You _have_ to call init_mach_ind_io() during initialization of your program,
 #              which causes this module to check for the type of machine the program is running on.
 #              After initialization the module memorizes this machinetype and converts all
 #              read/written numbers accordingly. The resulting files are in the same format as 
 #              on a SUN workstation, eg. the integer 16909060 = 0x01020304 would be written as
 #              the following four succeeding bytes: 0x01 0x02 0x03 0x04 into a file.
 #
 # HISTORY:
 #	30 May 91  arthurem   created from jmt's routines.c and lpnn.h files for his LPNN system
 #  	14 Jun 91  arthurem   modified procedure declarations for changes (see mach_ind_io.c)
 #	 1 Jun 93  hhild      moved local stuff from .h into .c file
 #      16 Jul 93  sloboda    changed all headers to ANSI-C format, added old read/write functions
 #                            for backward compatibility and added Joe's new floatbyte routines.
 #                            Changed return value of init_mach_ind_io()
 #      22 Jul 93  hhild      added non-ansi prototypes (some machines need it ...)
 #       2.Aug 93  sloboda    added functions for float <--> floatbyte conversion
 #       7.Aug 93  sloboda    changed #ifdef ANSI_C to #ifdef __STDC__
 #      14.Aug 93  sloboda    minor change
 #      24.Nov 93  sloboda    added two new routines for reading/writing "scaled vectors"
 #
 #== RCS INFO: ================================================================================================================
 #
 # $Id: mach_ind_io.h 512 2000-08-27 15:31:32Z jmcd $
 #
 # $Log$
 # Revision 3.4  2000/08/27 15:31:24  jmcd
 # Merging branch 'jtk-00-08-24-jmcd'.
 #
 # Revision 3.3.4.1  2000/08/25 22:18:38  jmcd
 # Added extern "C" statements to all header files; compiled main() in
 # C++ to ensure proper initialization of C++ libraries.
 #
 # Revision 3.3  1997/07/31 07:54:02  kemp
 # Added #include <stdio.h>
 #
 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.2  95/12/07  03:59:47  maier
    read_scaled_vectors_range()
    
    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 # Revision 1.4  1994/11/30  21:08:30  kemp
 # Added prototypes for the change-in-memory functions.
 #
 # Revision 1.3  1993/12/03  17:03:13  janus
 # introduced read/write routines for scaled vectors
 #
 # Revision 1.2  1993/08/14  21:39:07  janus
 # minor change
 #
 # Revision 1.1  1993/08/07  15:51:41  janus
 # Initial revision
 #
 #
 ############################################################################################################################*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MACH_IND_IO_H_                 /* we only want to include each header file once ! */
#define _MACH_IND_IO_H_

#include <stdio.h>

typedef  unsigned char   UBYTE;         /* type for reading/writing floatbytes */


/*#############################################################################################################################
 #
 #   FUNCTION DECLARATIONS  
 #
 ############################################################################################################################*/

#ifdef __STDC__                  /* are we using an ANSI C compiler? if so, the compiler allows type checking: */

void  init_mach_ind_io( void );

UBYTE float_to_ubyte( float f );
float ubyte_to_float( UBYTE u );

float read_float( FILE *fp );
int   read_floats(FILE *fp, float *whereto, int count );

float read_floatbyte( FILE *fp );
int   read_floatbytes(FILE *fp, float *whereto, int count );

int   read_int( FILE *fp );
int   read_ints(FILE *fp, int *whereto, int count );

short read_short( FILE *fp );
int   read_shorts(FILE *fp, short *whereto, int count);

int   read_string( FILE *f, char *str);

int   read_scaled_vectors( FILE *fp, float **whereto,  int* coeffNP, int *vectorNP );
int   read_scaled_vectors_range( FILE *fp, float **whereto, int *coeffNP, int *vectorNP, int from, int to);
int   write_scaled_vectors(FILE *fp, float *wherefrom, int  coeffN,  int  vectorN );

void  write_float( FILE *fp, float f);
int   write_floats(FILE *fp, float *wherefrom, int count);

void  write_floatbyte( FILE *fp, float f);
int   write_floatbytes(FILE *fp, float *wherefrom, int count);

void  write_int( FILE *fp, int i);
int   write_ints(FILE *fp, int *wherefrom, int count);

void  write_short( FILE *fp, short s);
int   write_shorts(FILE *fp, short *wherefrom, int count);

int   write_string(FILE *f, char *str);

int   set_machine( int new_machine );

int   check_byte_swap( short *buf, int bufN );       /* See if adc data should be swapped */

void  buf_byte_swap( short *buf, int bufN );         /* Swap bytes for each short in an adc data buffer */

int short_memorychange(short *buf, int bufN);
int float_memorychange(float *buf, int bufN);
int int_memorychange(int *buf, int bufN);
int change_short(short *x);
int change_int(int *x);
int change_float(float *x);

#else

void  init_mach_ind_io();

int short_memorychange();
int float_memorychange();
int int_memorychange();

int change_short();
int change_int();
int change_float();

UBYTE float_to_ubyte();
float ubyte_to_float();

float read_float();
int   read_floats();

float read_floatbyte();
int   read_floatbytes();

int   read_int( );
int   read_ints();

short read_short();
int   read_shorts();

int   read_string();

int   read_scaled_vectors();
int   read_scaled_vectors_range();
int   write_scaled_vectors();

void  write_float();
int   write_floats();

void  write_floatbyte();
int   write_floatbytes();

void  write_int();
int   write_ints();

void  write_short();
int   write_shorts();

int   write_string();

int   set_machine();

int   check_byte_swap();

void  buf_byte_swap();

#endif /* __STDC__ */

/*************************************************************************************************************************/

#endif 	/* _MACH_IND_IO_H_ */

#ifdef __cplusplus
}
#endif
