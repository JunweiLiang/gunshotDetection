/*
feature.c
an implementation of feature matrix.
$Id: feature.c,v 1.10 2004/11/02 02:12:03 dmochiha Exp $

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lsafeature.h"
#include "writer.h"
#include "reader.h"
#include "util.h"
#define  BUFSIZE  65536
static int n_fields (char *line);
static int n_phrases (char *line);
static int file_lines (FILE *fp);
static int isspaces (char *s);

document *
feature_matrix (const char *file_name, int *maxid, int *maxlen, int *num_doc, double *num_word)
{
   document *d;
   int  n;
   int num_phrase = 0;
   FILE *fp;
   char line[BUFSIZE];
   *maxid  = -1;
   *maxlen =  0;
   
   if ((fp = fopen(file_name, "r")) == NULL)
      return NULL;
   *num_doc = file_lines(fp);
   
   d = new document[*num_doc+1];
   d[*num_doc].len = -1;
   
   *num_word = 0;
   n = 0;
   while (fgets(line, sizeof(line), fp))
   {
      int i, len;
      char *cp, *sp, *lp = line;
      
      if (isspaces(line))
         continue;
      
      len = n_fields(line);

      // obtain # of phrases (if any)
      d[n].num_phrase = num_phrase = n_phrases (line);

// Start Wilson Oct 23 2008
      if (num_phrase > 0) {
         printf("%d phrases found in doc %d\n", num_phrase, n);
         // allocate array
         d[n].phrase_size = new int[num_phrase];

         set_int_array(d[n].phrase_size, num_phrase, 0);
      }
// End Wilson Oct 23 2008

      if (len > *maxlen)
         *maxlen = len;
      if (!(len > 0)) {
         fprintf(stderr, "feature_matrix: suspicious line:\n%s",
            line);
         exit(1);
      }
      
      d[n].id  = new int[len];
      d[n].cnt = new double[len];
      d[n].len = len;

      if ((d[n].id == NULL) || (d[n].cnt == NULL))
         return NULL;
      
      i = 0;
      int cur_phrase_id = -1;
      while (*lp)
      {
         int id;
         double cnt;
         if ((cp = strchr(lp, ':')) == NULL)
            break;
         if ((sp = strpbrk(cp + 1, " \t\n")) == NULL)
            break;

         *cp = '\0';
         *sp = '\0';
         id  = atoi(lp) - 1;	/* zero origin */
         cnt = atof(cp + 1);
         if (id >= *maxid)
            *maxid = id + 1;
         d[n].id[i]  = id;
         d[n].cnt[i] = cnt;
         i++;

         //printf("found %d %f\n", id, cnt);

         *num_word += cnt;

// Start Wilson Oct 23 2008
         if (*(sp+1) == '|') {
            // begin of phrase boundary at word i
            //printf("phrase boundary starts at word %d\n", i);
            if (d[n].phrase_start == -1)
               d[n].phrase_start = i;
            else
               d[n].phrase_size[cur_phrase_id]++;

            cur_phrase_id++;

            lp  = sp + 3;
         } else {
            lp  = sp + 1;
            if (cur_phrase_id >= 0) {
               // count # of words in the current phrase
               d[n].phrase_size[cur_phrase_id]++;
            }
         }
// End Wilson Oct 23 2008
      }
      n++;
   }
   fclose(fp);
   return(d);
}

document* read_feature_matrix(const char* file_name, int& num_doc)
{
   document *m = 0;
   FILE *fp;

   num_doc = 0;

   if ((fp = fopen(file_name, "rb")) == NULL) {
      printf("can't read from %s\n", file_name);
      return 0;
   }

   fread(&num_doc, sizeof(int), 1, fp);
   m = new document[num_doc];

   for (int d=0; d < num_doc; d++) {
      document* dp = m+d;

      fread(&(dp->len), sizeof(int), 1, fp);

      dp->id  = new int[dp->len];
      dp->cnt = new double[dp->len];

      read_int_vector(fp, dp->id, dp->len);
      read_vector(fp, dp->cnt, dp->len);
   }

   fclose(fp);

   return m;
}

void write_feature_matrix(const char* file_name, const document* m, int num_doc)
{
   FILE *fp;

   if ((fp = fopen(file_name, "wb")) == NULL) {
      printf("can't write to %s\n", file_name);
      return;
   }

   fwrite(&num_doc, sizeof(int), 1, fp);

   for (int d=0; d < num_doc; d++) {
      const document* dp = m+d;

      fwrite(&(dp->len), sizeof(int), 1, fp);
      write_int_vector(fp, dp->id, dp->len);
      write_vector(fp, dp->cnt, dp->len);
   }

   fclose(fp);
}

void
free_feature_matrix (document *matrix)
{
   delete [] matrix;
}

static int
file_lines (FILE *fp)
{
   int n = 0;
   char buf[BUFSIZE];
   
   while (fgets(buf, sizeof(buf), fp))
   {
      if (!isspaces(buf))
         n++;
   }
   rewind(fp);
   
   return n;
}

static int
n_fields (char *line)
{
   int n = 0;
   char *cp, *sp, *lp = line;
   
   while (*lp)
   {
      if ((cp = strchr(lp, ':')) == NULL)
         break;
      if ((sp = strpbrk(cp + 1, " \t\n")) == NULL)
         break;
      lp = sp + 1;
      n++;
   }
   
   return n;
}

static int
n_phrases (char *line)
{
   int n = 0;
   char *cp, *lp = line;
   
   while (*lp)
   {
      if ((cp = strchr(lp, '|')) == NULL)
         break;
      lp = cp + 1;
      n++;
   }
   
   return n;
}

static int
isspaces (char *s)	/* return 1 if s consists of only white spaces */
{
   char *c = s;
   while (*c)
   {
      if (!isspace(*c))
         return 0;
      c++;
   }
   return 1;
}

document::document(int max_buf)
{
   len = 0;
   id  = new int[max_buf];
   cnt = new double[max_buf];

   // word pos for the phrase section
   phrase_start = -1;

   // # of words in each phrase
   phrase_size = NULL;

   // number of phrases in the phrase section
   num_phrase = 0;
}

document::~document()
{
   len = 0;
   delete [] id;
   delete [] cnt;

   if (phrase_size) 
      delete [] phrase_size;
}

document::document()
{
   len = 0;
   id  = 0;
   cnt = 0;

   // word pos for the phrase section
   phrase_start = -1;

   // # of words in each phrase
   phrase_size = NULL;

   // number of phrases in the phrase section
   num_phrase = 0;
}

/* $Id: feature.c,v 1.10 2004/11/02 02:12:03 dmochiha Exp $ */
