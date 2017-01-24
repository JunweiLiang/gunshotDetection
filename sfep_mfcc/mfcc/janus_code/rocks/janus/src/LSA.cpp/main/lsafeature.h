/*
feature.h
a header file for feature matrix.
$Id: feature.h,v 1.3 2004/11/01 06:49:06 dmochiha Exp $
*/
#ifndef LDA_FEATURE_H
#define LDA_FEATURE_H
#include <stdio.h>

class document 
{
public:
   int    len;
   int    *id;
   double *cnt;

   // word pos for the phrase section
   int    phrase_start;

   // # of words in each phrase
   int    *phrase_size;

   // number of phrases in the phrase section
   int    num_phrase;
   
public:
   document(int max_buf);
   document();
   ~document();
};

extern document *feature_matrix (const char *file_name, int *maxid, int *maxlen, int *num_doc, double *num_word);
extern void free_feature_matrix(document *matrix);
extern void write_feature_matrix(const char* file_name, const document* m, int num_doc);
document* read_feature_matrix(const char* file_name, int& num_doc);

#endif
