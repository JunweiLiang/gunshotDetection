#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>

using namespace std;

// from janus
#include "itf.h"
// END

#include "janus_dirtree.h"
#include "dirichlet.h"

//////////////////////////////////////////////////////////////////////
// Local variables
static int Initialized = 0;
//////////////////////////////////////////////////////////////////////

// print the object
static int PutsItf( ClientData cd, int argc, char *argv[])
{
   DirichletTree* obj = (DirichletTree*) cd;

   obj->print();

   return TCL_OK;
}

// destroy the object
static int FreeItf(ClientData cd)
{
   DirichletTree* obj = (DirichletTree*) cd;

   delete obj;

   return TCL_OK;
}

// construct the object
static ClientData CreateItf (ClientData cd, int argc, char *argv[])
{
   int       ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   char      *name = NULL;
   int num_leaf = 0;
   int num_branch = 2;
   double* alpha = 0;

   if ( itfParseArgv( argv[0], &ac, argv, 1,
      "<name>",  ARGV_STRING, NULL, &name,     NULL, "object name X",
      "<K>",     ARGV_INT,    NULL, &num_leaf, NULL, "# of leaf nodes",
      "-b",      ARGV_INT,    NULL, &num_branch, NULL, "# of branches",
      NULL) != TCL_OK) return NULL;

   // build a dirichlet tree with 
   DirichletTree* obj = new DirichletTree(num_leaf, num_branch, alpha);

   return (ClientData) obj;
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

static int ReadItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   DirichletTree*  tree = (DirichletTree*) cd;
   char* param_file = NULL;

   // get arguments
   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<.alpha file>",   ARGV_STRING, NULL, &param_file, NULL, ".alpha param file",
      NULL) != TCL_OK) return TCL_ERROR;

#ifdef BINARY
   FILE *fp = fopen(param_file, "rb");
#else
   FILE *fp = fopen(param_file, "r");
#endif

   if (fp == NULL) {
      cerr << "can't open " << param_file << endl;
   } else {
      tree->read_alpha(fp);
      fclose(fp);
   }

   return TCL_OK;
}

static int WriteItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   DirichletTree*  tree = (DirichletTree*) cd;
   char* param_file = NULL;

   // get arguments
   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<.alpha file>",   ARGV_STRING, NULL, &param_file, NULL, ".alpha param file",
      NULL) != TCL_OK) return TCL_ERROR;

#ifdef BINARY
   FILE *fp = fopen(param_file, "wb");
#else
   FILE *fp = fopen(param_file, "w");
#endif

   if (fp == NULL) {
      cerr << "can't write to " << param_file << endl;
   } else {
      tree->write_alpha(fp);
      fclose(fp);
   }

   return TCL_OK;
}

static int MergeItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   DirichletTree*  tree = (DirichletTree*) cd;
   int k1, k2;

   // get arguments
   if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<k1>",   ARGV_INT, NULL, &k1, NULL, "topic k1",
      "<k2>",   ARGV_INT, NULL, &k2, NULL, "topic k2",
      NULL) != TCL_OK) return TCL_ERROR;

   tree->merge_node(k1, k2);

   return TCL_OK;
}

static int ProbItf( ClientData cd, int argc, char *argv[])
{
   // get pointer to the current object
   int           ac    = argc /* (argc > 1) ? 1 : 0 */ ;
   DirichletTree*  tree = (DirichletTree*) cd;

   double* prob = tree->MAP_prob();

   for (int k=0; k < tree->get_num_topic(); k++) {
      itfAppendResult("%e ", prob[k]);
   }
   itfAppendResult("\n");

   return TCL_OK;
}

// Add your methods here
static Method method[] =
{
  { "puts",        PutsItf,        "display object info" },
  { "read",        ReadItf,        "read object" },
  { "write",       WriteItf,       "write object" },
  { "merge",       MergeItf,       "merge topics" },
  { "prob",        ProbItf,        "prob of topics" },
  {  NULL,    NULL,          NULL }
};

static TypeInfo objInfo = { "DirichletTree", 0, -1, method,
                      CreateItf,    FreeItf,
                      ConfigureItf, AccessItf, NULL,
                     "DirichletTree" } ;

// init()
int DirichletTree_Init()
{
   if (Initialized) return TCL_OK;

   // register object
   itfNewType( &objInfo);

   Initialized = 1;
   return TCL_OK;
}
