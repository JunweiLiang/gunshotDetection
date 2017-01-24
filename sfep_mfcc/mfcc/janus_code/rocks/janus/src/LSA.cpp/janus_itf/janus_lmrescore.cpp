#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>

#include "LMrescore.h"
#include "util.h"

using namespace std;

// from janus
#include "itf.h"
#include "slimits.h"
#include "ngramLM.h"
//#include "lks.h"
// END

// compulsory
#include "janus_lmrescore.h"


/* COMPRESSEDLM is defined in slimits.h and defines whether to use
   the compressed LM (scores are int) or the full float-type LM */
#ifdef COMPRESSEDLM
  #define   CompressScore(score) (signed short int)((score)*LMSCORE_SCALE_FACTOR-0.5)
  #define UnCompressScore(score) (float)(score)/LMSCORE_SCALE_FACTOR
#else
  #define   CompressScore(score) (score)
  #define UnCompressScore(score) (score)
#endif

//////////////////////////////////////////////////////////////////////
// Local variables
static int Initialized = 0;
//////////////////////////////////////////////////////////////////////

static int rescoreLatItf( ClientData cd, int argc, char *argv[])
{
   char *in_lat_file = NULL;
   char *out_lat_file = NULL;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<in Lattice file>", ARGV_STRING, NULL, &in_lat_file,    NULL, "input lattice file (in IBM format)",
      "<out Lattice file>", ARGV_STRING, NULL, &out_lat_file,    NULL, "output lattice file (in IBM format)",
      NULL) != TCL_OK) return TCL_ERROR;

   LMrescore* rescorer = (LMrescore*) cd;

   cout << in_lat_file << " --> " << out_lat_file << endl;
   rescorer->rescore_lat(in_lat_file, out_lat_file);

   return TCL_OK;
}

static int loadItf( ClientData cd, int argc, char *argv[])
{
   char *model_file = NULL;
   int limit_vocab = 1;

   // get arguments
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<model file>", ARGV_STRING, NULL, &model_file,    NULL, "a list of arpa ngram model (1st one being the Dirichlet-tree prior)",
      "-limit", ARGV_INT, NULL, &limit_vocab,    NULL, "limit vocab to a small subset? (0/1)",
      NULL) != TCL_OK) return TCL_ERROR;

   LMrescore* rescorer = (LMrescore*) cd;

   cout << "loading LM " << model_file << endl;
   rescorer->read_topic_model(model_file, limit_vocab);

   return TCL_OK;
}

// print the object
static int PutsItf( ClientData cd, int argc, char *argv[])
{
   //itfAppendResult("value=%d", obj->get_value());

   return TCL_OK;
}

// destroy the object
static int FreeItf(ClientData cd)
{
   LMrescore* obj = (LMrescore*) cd;

   // delete vocab first
   delete obj->get_vocab();

   // delete obj
   delete obj;

   return TCL_OK;
}

// construct the object
static ClientData CreateItf (ClientData cd, int argc, char *argv[])
{
   //int       ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   char      *name = NULL;
   char      *vocab_file = NULL; // reduced vocab for topicLM (usually generated from lattices)
   char      *stopword_file = NULL;
   char      *noise_file = NULL;
   char      *word_mapper_file = NULL;
   char      *rescore_lm_file = NULL;
   int       order = 4;
   int       num_topic = 1;

   if (itfParseArgv("LMrescore", &argc, argv, 0,
      "<name>", ARGV_STRING, NULL, &name, cd, "object name",
      "<vocab file>", ARGV_STRING, NULL, &vocab_file,    NULL, "active vocab file (rescoring/decoding)",
      "<LM order>",   ARGV_INT, NULL, &order,    NULL,   "LM order",
      "<num topic>",  ARGV_INT, NULL, &num_topic,    NULL,   "Number of topics",
      "-stoplist",   ARGV_STRING, NULL, &stopword_file,    NULL,   "list of stopwords",
      "-noise",      ARGV_STRING, NULL, &noise_file,    NULL,   "list of noise words (e.g. [UH} +noise+)",
      "-mapper",     ARGV_STRING, NULL, &word_mapper_file,    NULL,  "word mapper file(e.g. word -> stem)",
      "-lm",         ARGV_STRING, NULL, &rescore_lm_file,    NULL,   "LM file for rescoring",
		   NULL) != TCL_OK) return NULL;

   File vocab_is(vocab_file, "r");
   Vocab* vocab = new Vocab();

   // read vocab file
   vocab->read(vocab_is);

   // use <UNK> (open vocab)
   vocab->unkIsWord() = true;

   // don't apply lowercase
   vocab->toLower() = false;

   // use uppercase <UNK>
   vocab->remove(vocab->unkIndex());
   vocab->unkIndex() = vocab->addWord("<UNK>");

   LMrescore* rescorer = new LMrescore(vocab, order, num_topic);

   if (stopword_file) {
      cout << "loading stoplist " << stopword_file << endl;
      rescorer->load_stoplist(stopword_file);
   }

   if (noise_file) {
      cout << "loading noiselist " << noise_file << endl;
      rescorer->load_noise_vocab(noise_file);
   }

   if (word_mapper_file) {
      cout << "loading word mapper " << word_mapper_file << endl;
      rescorer->load_word_mapper(word_mapper_file);
   }

   return (ClientData) rescorer;
}

// configure the object
static int ConfigureItf (ClientData cd, char *var, char *val)
{
   return TCL_OK;
}

// access sub-objects
static ClientData AccessItf( ClientData cd, char* name, TypeInfo** ti)
{
  return 0;
}

// Add your methods here
static Method method[] =
{
  { "puts",        PutsItf,        "display object info" },
  { "load",        loadItf,        "load interpolated model" },
  { "rescore",     rescoreLatItf,  "rescore lattices (IBM format)" },
  {  NULL,         NULL,           NULL }
};

static TypeInfo objInfo = { "LMrescore", 0, -1, method,
                      CreateItf,    FreeItf,
                      ConfigureItf, AccessItf, NULL,
                     "LMrescore" } ;

// init()
int LMrescore_Init()
{
   if (Initialized) return TCL_OK;

   // register object
   itfNewType( &objInfo);

   Initialized = 1;
   return TCL_OK;
}
