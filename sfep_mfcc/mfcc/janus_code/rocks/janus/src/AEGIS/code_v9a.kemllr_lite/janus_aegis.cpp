#include "janus_aegis.h"

#include "slimits.h"
#include "lks.h"
#include "svmap.h"

#include "mladapt.h"
#include "codebook.h"
#include "distrib.h"
#include "distribStream.h"
#include "phones.h"
#include "amodel.h"
#include "tree.h"
#include "feature.h"
#include "hmm.h"
#include "glat.h"

#include "aegis.h"
#include "ei_kev.h"
#include "ev.h"
#include "rsw_ext.h"
#include "str_util.h"
#include "g-mmie.h"
#include "gdfsa.h"
#include "fmpe.h"

static int Initialized = 0;

static int PutsItf(ClientData cd, int argc, char *argv[]){
  return TCL_OK;
}

static int TestItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  Aegis* aegis = (Aegis*) cd;
  AegisDictionary* adict = aegis->getADictionary();
  HMMSet* hmmset;
  Recognizer* recognizer;
  MemoryManager* mgr;
  EiKEV* ei_kev;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mgr = aegis->getMemoryManager();
  recognizer = aegis->getRecognizer();
  hmmset = aegis->getHMMSet();

  return TCL_OK;
}

static int loadUtterHMMItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  Aegis* aegis = (Aegis*) cd;
  AegisDictionary* adict = aegis->getADictionary();
  HMMSet* hmm_set;
  MemoryManager* mgr;
  RelationInfo* info;
  EiKEV* ei_kev;
  HMM* hmm;
  AegisHMM* ahmm;
  vector<PDF*> pdf_set;
  vector<State*> state_set;
  vector<Edge*> edge_set;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<HMM>", ARGV_OBJECT, NULL, &hmm, "HMM", "name of the HMM",
    NULL) != TCL_OK) return TCL_ERROR;

  StreamArray* staP = &(hmm->stateGraph->snsP->strA);
  DistribSet* dssP = ((DistribStream*) staP->cdA[0])->dssP;
  CodebookSet* cbsP = dssP->cbsP;
  DistribList ds_list = dssP->list;

  mgr = aegis->getMemoryManager();
  info = mgr->getRelationInfo();
  aegis->getPDFSet(pdf_set);
  SenoneList s_list = hmm->stateGraph->snsP->list;
  for(int i=0;i<hmm->stateGraph->stateN;i++){
    Senone* s = &(s_list.itemA[hmm->stateGraph->senoneX[i]]);
    Distrib* ds = &ds_list.itemA[*(s->classXA)];

    PDF* pdf = aegis->getPDF(ds->name);
    State* state = mgr->createState(pdf);

    state_set.push_back(state);
  }
  for(int i=0;i<hmm->stateGraph->stateN;i++){
    for(int j=0;j<hmm->stateGraph->fwdTrN[i];j++){
      int idx = hmm->stateGraph->fwdIdx[i][j];
      //printf("%d %d %d\n", state_set.size(), i, i+idx);
      if(i+idx<state_set.size()){
        Edge* e = mgr->createEdge(state_set[i], state_set[i+idx], 1.0/hmm->stateGraph->fwdTrN[i]);
        edge_set.push_back(e);
      }
    }
  }

  ahmm = mgr->createHMM("ahmm");
  info->addRelation("HMMNameTable", ahmm->getID(), (Ptr) ahmm);

  State* b_state = mgr->createState(NULL);
  Edge* b_edge = mgr->createEdge(b_state, state_set[0], 1.0);
  ahmm->insertStartState(b_state);
  for(int i=0;i<state_set.size();i++){
    ahmm->insertState(state_set[i]);
  }
  State* e_state = mgr->createState(NULL);
  Edge* e_edge = mgr->createEdge(state_set[state_set.size()-1], e_state, 1.0);
  ahmm->insertEndState(e_state);
  
  ahmm->insertEdge(b_edge);
  for(int i=0;i<edge_set.size();i++){
    ahmm->insertEdge(edge_set[i]);
  }
  ahmm->insertEdge(e_edge);

  hmm_set = new HMMSet(aegis->getDim());
  hmm_set->addHMM(ahmm);
  aegis->setHMMSet(hmm_set);

  aegis->initRecognizer();
  
  return TCL_OK;
}

static int AdaptItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  Aegis* aegis = (Aegis*) cd;
  HMMSet* hmmset;
  Network* fa_net;
  Recognizer* recognizer;
  MemoryManager* mgr;
  EiKEV* ei_kev;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  Transcription result;
  vector<string> garbage, triph_seq, sentence;
  float** obs;
  int t_max;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  aegis->getGarbagePhone(garbage);
  triph_seq.push_back("ahmm");

  feature = &feature_set->featA[1];
  obs = feature->data.fmat->matPA;
  t_max = feature->data.fmat->m;

  ei_kev = aegis->getKEMLLR();
  ei_kev->adapt(obs, t_max, triph_seq);

  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static int AdaptRSWItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  Aegis* aegis = (Aegis*) cd;
  vector<Gaussian*> gaussian;
  HMMSet* hmmset;
  Network* fa_net;
  Recognizer* recognizer;
  MemoryManager* mgr;
  EV* ev;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  Transcription result;
  vector<string> garbage, triph_seq, sentence;
  float** obs;
  int t_max;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  aegis->getGarbagePhone(garbage);
  triph_seq.push_back("ahmm");

  feature = &feature_set->featA[1];
  obs = feature->data.fmat->matPA;
  t_max = feature->data.fmat->m;
  aegis->getAllGaussians(gaussian);
  
  ev = aegis->getRSW();
  ev->robustAdapt(obs, t_max);
  //ev->updateRobustModel(gaussian);
  ev->updateRobustModel();

  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static int AdaptRSWExtItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  Aegis* aegis = (Aegis*) cd;
  vector<Gaussian*> gaussian;
  HMMSet* hmmset;
  Network* fa_net;
  Recognizer* recognizer;
  MemoryManager* mgr;
  RSWExt* rsw_ext;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  Transcription result;
  vector<string> garbage, triph_seq, sentence;
  float** obs;
  int t_max;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  aegis->getGarbagePhone(garbage);
  triph_seq.push_back("ahmm");

  feature = &feature_set->featA[1];
  obs = feature->data.fmat->matPA;
  t_max = feature->data.fmat->m;
  aegis->getAllGaussians(gaussian);
  
  rsw_ext = aegis->getRSWExt();
  rsw_ext->robustAdapt(obs, t_max);
  rsw_ext->updateRobustModel();

  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}


static int initKEMLLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* mllr_transform;
  Aegis* aegis = (Aegis*) cd;
  EiKEV* ei_kev;
  MemoryManager* mgr = aegis->getMemoryManager();
  Recognizer* recognizer = aegis->getRecognizer();
  HMMSet* hmmset = aegis->getHMMSet();
  int dim, num_spkrs, num_eigen_vectors;
  int num_reg_groups;
  float learning_rate, threshold, beta;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<mllr transform>", ARGV_STRING, NULL, &mllr_transform, NULL, "mllr transform file",
    "<num spkrs>", ARGV_INT, NULL, &num_spkrs, NULL, "num spkrs",
    "<num eigenvectors>", ARGV_INT, NULL, &num_eigen_vectors, NULL, "num eigenvectors",
    "<num reg groups>", ARGV_INT, NULL, &num_reg_groups, NULL, "num regression classes (only 1 is supported)",
    "<learning rate>", ARGV_FLOAT, NULL, &learning_rate, NULL, "learning rate",
    "<threshold>", ARGV_FLOAT, NULL, &threshold, NULL, "threshold",
    "<beta>", ARGV_FLOAT, NULL, &beta, NULL, "beta",
    NULL) != TCL_OK) return TCL_ERROR;

  dim = aegis->getDim();
  //ei_kev = new EiKEV(num_spkrs, mgr, recognizer, hmmset, 1, dim, num_eigen_vectors, 0.00001, 0.00050, 0.001);
  ei_kev = new EiKEV(num_spkrs, mgr, recognizer, hmmset, num_reg_groups, dim, num_eigen_vectors, learning_rate, threshold, beta);
  ei_kev->load(mllr_transform);
  ei_kev->init();

  aegis->setKEMLLR(ei_kev);

  return TCL_OK;
}

static int freeKEMLLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* mllr_transform;
  Aegis* aegis = (Aegis*) cd;
  EiKEV* ei_kev;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  ei_kev = aegis->getKEMLLR();
  aegis->setKEMLLR(NULL);

  delete ei_kev;

  return TCL_OK;
}

static int initFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;
  SenoneSet* senone_set;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  FeatureSet* feature_set;
  SVMap* svmap;
  float E=2.0, scale;
  int dim;
  int num_classes = 256;
  int num_idx=1, den_idx=2;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<SenoneSet>", ARGV_OBJECT, NULL, &senone_set, "SenoneSet", "name of the codebook set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<SVMap>", ARGV_OBJECT, NULL, &svmap, "SVMap", "name of the feature set",
    "<scale>", ARGV_FLOAT, NULL, &scale, "", "scale",
    "<dim>", ARGV_INT, NULL, &dim, "", "dim",
    "-E", ARGV_FLOAT, NULL, &E, "", "E",
    "-num_classes", ARGV_INT, NULL, &num_classes, "", "E",
    "-num_idx", ARGV_INT, NULL, &num_idx, "", "num_idx",
    "-den_idx", ARGV_INT, NULL, &den_idx, "", "den_idx",
    NULL) != TCL_OK) return TCL_ERROR;


  fmpe = new FMPE(senone_set, codebook_set, distrib_set, feature_set, svmap, E, scale, dim, num_classes);

  fmpe->setCBAIdx(num_idx, den_idx);
  aegis->setFMPE(fmpe);

  return TCL_OK;
}

static int accuFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;
  GLat* glat;
  char* refNodeLstStr=NULL;
  int* refNodeLst=NULL;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "<refNodeList>",ARGV_STRING,NULL,&refNodeLstStr,NULL, "ref. node list",
    NULL) != TCL_OK) return TCL_ERROR;

  int slen=(int)strlen(refNodeLstStr);
  if(slen<=0) {
    ERROR("accuGMMIEItf: Not reference node information.\n");
    return TCL_ERROR;
  }
  int refNodeN=0;
  int i;
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]=='_') refNodeN++;
  }
  refNodeN++;
  if(NULL == (refNodeLst=(int *)calloc(refNodeN,sizeof(int)))) {
    ERROR("accuGMMIEItf: Unable to allocate memory.\n");
    return TCL_ERROR;
  }
  int tempc=refNodeN=0;
  char temp[20];
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]!='_')
      temp[tempc++]=refNodeLstStr[i];
    else {
      temp[tempc]='\n';
      temp[tempc+1]=0;
      sscanf(temp,"%d",&refNodeLst[refNodeN++]);
      tempc=0;
    }
  }
  temp[tempc]='\n';
  temp[tempc+1]=0;
  sscanf(temp,"%d",&refNodeLst[refNodeN++]);

  fmpe = aegis->getFMPE();
  fmpe->accu(glat, refNodeLst, refNodeN);

  return TCL_OK;
}

static int setStatusFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;
  int status=0;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<status>",ARGV_INT,NULL,&status,NULL, "enable/disable",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->setStatus(status);
  return TCL_OK;
}

static int clearAccuFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->clearAccu();
  return TCL_OK;
}

static int storeFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->storeFeatures();
  return TCL_OK;
}

static int transformFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->transformFeatures();
  return TCL_OK;
}

static int updateFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  FMPE* fmpe;
  float learning_rate = 0.000000001;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "-learn", ARGV_FLOAT, NULL, &learning_rate, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->update(learning_rate);
  return TCL_OK;
}

static int saveFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->saveTransforms(header);
  return TCL_OK;
}

static int loadFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->loadTransforms(header);
  return TCL_OK;
}

static int saveAccuFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->saveAccu(header);
  return TCL_OK;
}

static int loadAccuFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->loadAccu(header);
  return TCL_OK;
}

static int saveClusterFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* filename=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->saveCluster(filename);
  return TCL_OK;
}

static int loadClusterFMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* filename=NULL;
  Aegis* aegis = (Aegis*) cd;  
  FMPE* fmpe;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  fmpe = aegis->getFMPE();
  fmpe->loadCluster(filename);
  return TCL_OK;
}


static int initGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  SenoneSet* senone_set;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  FeatureSet* feature_set;
  SVMap* svmap;
  float scale;
  float E=2.0;
  int num_classes = 256;
  int use_fsa=0;
  int ref_lock=1;
  int mmi_init=0;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<SenoneSet>", ARGV_OBJECT, NULL, &senone_set, "SenoneSet", "name of the codebook set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<SVMap>", ARGV_OBJECT, NULL, &svmap, "SVMap", "name of the feature set",
    "<num_classes>", ARGV_INT, NULL, &num_classes, "", "num_classes",
    "<use_fsa>", ARGV_INT, NULL, &use_fsa, "", "use_fsa",
    "<scale>", ARGV_FLOAT, NULL, &scale, "", "scale",
    "<ref_lock>", ARGV_INT, NULL, &ref_lock, "", "ref_lock",
    "-E", ARGV_FLOAT, NULL, &E, "", "E",
    "-mmi_init", ARGV_INT, NULL, &mmi_init, "", "mmi_init",
    NULL) != TCL_OK) return TCL_ERROR;

  if(!use_fsa)
    gdfsa = new GDFSA(senone_set, codebook_set, distrib_set, feature_set, svmap, num_classes, false, scale, ref_lock, E, mmi_init);
  else
    gdfsa = new GDFSA(senone_set, codebook_set, distrib_set, feature_set, svmap, num_classes, true, scale, ref_lock, E, mmi_init);
  aegis->setGDFSA(gdfsa);

  return TCL_OK;
}

static int accuClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->accuCluster();
  return TCL_OK;
}

static int kmeanClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->kmeanCluster();
  return TCL_OK;
}

static int clearAccuClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->clearAccuCluster();
  return TCL_OK;
}


static int updateClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->updateCluster();
  return TCL_OK;
}

static int saveAccuClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  char* filename = NULL;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->saveAccuCluster(filename);
  return TCL_OK;
}

static int loadAccuClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  char* filename = NULL;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->loadAccuCluster(filename);
  return TCL_OK;
}

static int saveClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  char* filename = NULL;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->saveCluster(filename);
  return TCL_OK;
}

static int loadClusterGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  char* filename = NULL;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<filename>", ARGV_STRING, NULL, &filename, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->loadCluster(filename);
  return TCL_OK;
}


static int accuGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  GLat* glat;
  char* refNodeLstStr=NULL;
  int* refNodeLst=NULL;
  char* in_file = NULL;
  char* out_file = NULL;
  float learning_rate = 0.0001;
  float c_avg = 0.0;
  float base_score = 0.0;
  float cur_score = 0.0;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "<refNodeList>",ARGV_STRING,NULL,&refNodeLstStr,NULL, "ref. node list",
    "<in>",ARGV_STRING,NULL,&in_file,NULL, "ref. node list",
    "<out>",ARGV_STRING,NULL,&out_file,NULL, "ref. node list",
    "<c_avg>",ARGV_FLOAT,NULL,&c_avg,NULL, "ref. node list",
    "<base_score>",ARGV_FLOAT,NULL,&base_score,NULL, "ref. node list",
    "<cur_score>",ARGV_FLOAT,NULL,&cur_score,NULL, "ref. node list",
    "-learn",ARGV_FLOAT,NULL,&learning_rate,NULL, "ref. node list",
    NULL) != TCL_OK) return TCL_ERROR;

  int slen=(int)strlen(refNodeLstStr);
  if(slen<=0) {
    ERROR("accuGDLRItf: Not reference node information.\n");
    return TCL_ERROR;
  }
  int refNodeN=0;
  int i;
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]=='_') refNodeN++;
  }
  refNodeN++;
  if(NULL == (refNodeLst=(int *)calloc(refNodeN,sizeof(int)))) {
    ERROR("accuGDLRItf: Unable to allocate memory.\n");
    return TCL_ERROR;
  }

  int tempc=refNodeN=0;
  char temp[20];
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]!='_')
      temp[tempc++]=refNodeLstStr[i];
    else {
      temp[tempc]='\n';
      temp[tempc+1]=0;
      sscanf(temp,"%d",&refNodeLst[refNodeN++]);
      tempc=0;
    }
  }
  temp[tempc]='\n';
  temp[tempc+1]=0;
  sscanf(temp,"%d",&refNodeLst[refNodeN++]);

  gdfsa = aegis->getGDFSA();
  float score = gdfsa->accu(glat, refNodeLst, refNodeN, learning_rate, c_avg, base_score, cur_score, in_file, out_file);
  itfAppendResult("%e", score);

  free(refNodeLst);

  return TCL_OK;
}

static int transferGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->transferTransformation();
  return TCL_OK;
}

static int setStatusGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;
  int status=0;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<status>", ARGV_INT, NULL, &status, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  if(status==0)
    gdfsa->setStatus(false);
  else
    gdfsa->setStatus(true);
  return TCL_OK;
}


static int saveOrigFeatGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->storeFeatures();
  return TCL_OK;
}

static int saveFSAFeatGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->storeFSAFeatures();
  return TCL_OK;
}

static int transformGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->transformFeatures();
  return TCL_OK;
}

static int saveGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->saveTransformation(header);
  return TCL_OK;
}

static int loadGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->loadTransformation(header);
  return TCL_OK;
}

static int saveAccuGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->saveAccu(header);
  return TCL_OK;
}

static int loadAccuGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->loadAddAccu(header);
  return TCL_OK;
}

static int clearTransformGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->clearTransformation();
  return TCL_OK;
}

static int updateGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->update();
  return TCL_OK;
}

static int restoreOrigFeatGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->restoreFeatures();

  return TCL_OK;
}

static int clearAccuGDFSAItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDFSA* gdfsa;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdfsa = aegis->getGDFSA();
  gdfsa->clearAccu();

  return TCL_OK;
}


static int initGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;
  SenoneSet* senone_set;
  MLAdapt* mladapt;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  FeatureSet* feature_set;
  SVMap* svmap;
  float scale, E=0.0;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<SenoneSet>", ARGV_OBJECT, NULL, &senone_set, "SenoneSet", "name of the codebook set",
    "<MLAdapt>", ARGV_OBJECT, NULL, &mladapt, "MLAdapt", "name of the codebook set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<SVMap>", ARGV_OBJECT, NULL, &svmap, "SVMap", "name of the feature set",
    "<scale>", ARGV_FLOAT, NULL, &scale, "", "scale",
    "-E", ARGV_FLOAT, NULL, &E, "", "scale",
    NULL) != TCL_OK) return TCL_ERROR;

  gdlr = new GDLR(senone_set, mladapt, codebook_set, distrib_set, feature_set, svmap, scale, E);
  aegis->setGDLR(gdlr);

  return TCL_OK;
}

static int accuGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;
  GLat* glat;
  char* refNodeLstStr=NULL;
  int* refNodeLst=NULL;
  char* in_file = NULL;
  char* out_file = NULL;
  float learning_rate = 0.0001;
  float c_avg = 0.0;
  float base_score=0.0, cur_score=0.0;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "<refNodeList>",ARGV_STRING,NULL,&refNodeLstStr,NULL, "ref. node list",
    "<in>",ARGV_STRING,NULL,&in_file,NULL, "ref. node list",
    "<out>",ARGV_STRING,NULL,&out_file,NULL, "ref. node list",
    "<c_avg>",ARGV_FLOAT,NULL,&c_avg,NULL, "ref. node list",
    "<base_score>",ARGV_FLOAT,NULL,&base_score,NULL, "ref. node list",
    "<cur_score>",ARGV_FLOAT,NULL,&cur_score,NULL, "ref. node list",
    "-learn",ARGV_FLOAT,NULL,&learning_rate,NULL, "ref. node list",
    NULL) != TCL_OK) return TCL_ERROR;

  int slen=(int)strlen(refNodeLstStr);
  if(slen<=0) {
    ERROR("accuGDLRItf: Not reference node information.\n");
    return TCL_ERROR;
  }
  int refNodeN=0;
  int i;
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]=='_') refNodeN++;
  }
  refNodeN++;
  if(NULL == (refNodeLst=(int *)calloc(refNodeN,sizeof(int)))) {
    ERROR("accuGDLRItf: Unable to allocate memory.\n");
    return TCL_ERROR;
  }

  int tempc=refNodeN=0;
  char temp[20];
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]!='_')
      temp[tempc++]=refNodeLstStr[i];
    else {
      temp[tempc]='\n';
      temp[tempc+1]=0;
      sscanf(temp,"%d",&refNodeLst[refNodeN++]);
      tempc=0;
    }
  }
  temp[tempc]='\n';
  temp[tempc+1]=0;
  sscanf(temp,"%d",&refNodeLst[refNodeN++]);

  gdlr = aegis->getGDLR();
  float score = gdlr->accu(glat, refNodeLst, refNodeN, learning_rate, c_avg, base_score, cur_score, in_file, out_file);
  itfAppendResult("%e", score);

  free(refNodeLst);

  return TCL_OK;
}

static int linkGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->linkMLAdapt();

  return TCL_OK;
}

static int transformGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->transformModel();

  return TCL_OK;
}

static int saveGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->saveTransformation(header);
  return TCL_OK;
}

static int loadGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->loadTransformation(header);
  return TCL_OK;
}

static int saveAccuGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->saveAccu(header);
  return TCL_OK;
}

static int loadAccuGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* header;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
        "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
        "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
        NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->loadAccu(header);
  return TCL_OK;
}

static int clearAccuGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->clearAccu();

  return TCL_OK;
}

static int updateGDLRItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GDLR* gdlr;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gdlr = aegis->getGDLR();
  gdlr->update();

  return TCL_OK;
}

static int initGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  SenoneSet* senone_set;
  CodebookSet* codebook_set;
  CodebookSet* base_codebook_set;
  DistribSet* distrib_set;
  FeatureSet* feature_set;
  SVMap* svmap;
  float scale;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<SenoneSet>", ARGV_OBJECT, NULL, &senone_set, "SenoneSet", "name of the codebook set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<CodebookSet>", ARGV_OBJECT, NULL, &base_codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    "<FeatureSet>", ARGV_OBJECT, NULL, &feature_set, "FeatureSet", "name of the feature set",
    "<SVMap>", ARGV_OBJECT, NULL, &svmap, "SVMap", "name of the feature set",
    "<scale>", ARGV_FLOAT, NULL, &scale, "", "scale",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = new GMMIE(senone_set, codebook_set, base_codebook_set, distrib_set, feature_set, svmap, scale);
  aegis->setGMMIE(gmmie);

  return TCL_OK;
}

static int accuGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  GLat* glat;
  char* refNodeLstStr=NULL;
  int* refNodeLst=NULL;
  char* in_file = NULL;
  char* out_file = NULL;
  float learning_rate = 0.0001;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "<refNodeList>",ARGV_STRING,NULL,&refNodeLstStr,NULL, "ref. node list",
    "<in>",ARGV_STRING,NULL,&in_file,NULL, "ref. node list",
    "<out>",ARGV_STRING,NULL,&out_file,NULL, "ref. node list",
    "-learn",ARGV_FLOAT,NULL,&learning_rate,NULL, "ref. node list",
    NULL) != TCL_OK) return TCL_ERROR;

  int slen=(int)strlen(refNodeLstStr);
  if(slen<=0) {
    ERROR("accuGMMIEItf: Not reference node information.\n");
    return TCL_ERROR;
  }
  int refNodeN=0;
  int i;
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]=='_') refNodeN++;
  }
  refNodeN++;
  if(NULL == (refNodeLst=(int *)calloc(refNodeN,sizeof(int)))) {
    ERROR("accuGMMIEItf: Unable to allocate memory.\n");
    return TCL_ERROR;
  }

  int tempc=refNodeN=0;
  char temp[20];
  for(i=0;i<slen;i++) {
    if(refNodeLstStr[i]!='_')
      temp[tempc++]=refNodeLstStr[i];
    else {
      temp[tempc]='\n';
      temp[tempc+1]=0;
      sscanf(temp,"%d",&refNodeLst[refNodeN++]);
      tempc=0;
    }
  }
  temp[tempc]='\n';
  temp[tempc+1]=0;
  sscanf(temp,"%d",&refNodeLst[refNodeN++]);

  gmmie = aegis->getGMMIE();
  float score = gmmie->accu(glat, refNodeLst, refNodeN, learning_rate, in_file, out_file);
  //INFO("score: %e\n", score);
  itfAppendResult("%e", score);

  free(refNodeLst);

  return TCL_OK;
}

static int setScaleGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  float scale=0.0;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<scale>", ARGV_FLOAT, NULL, &scale, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->setScale(scale);

  return TCL_OK;
}

static int clearAccuGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->clearAccu();

  return TCL_OK;
}

static int saveAccuGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  char* header;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->saveAccu(header);

  return TCL_OK;
}

static int loadAccuGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  char* header;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->loadAccu(header);

  return TCL_OK;
}

static int loadAddAccuGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
  char* header;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<header>", ARGV_STRING, NULL, &header, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->loadAddAccu(header);

  return TCL_OK;
}

static int updateGMMIEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GMMIE* gmmie;
 
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  gmmie = aegis->getGMMIE();
  gmmie->update();

  return TCL_OK;
}

static int initMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* mllr_transform;
  Aegis* aegis = (Aegis*) cd;
  MPE* mpe;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = new MPE();
  aegis->setMPE(mpe);

  return TCL_OK;
}

static int alphaBetaMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  float scale = 1.0;
  float tmp;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "-scale", ARGV_FLOAT, NULL, &scale, NULL, "mystic scale",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  mpe->resetMPE();
  mpe->setAllNodesStatus(glat, DFS_INIT);
  mpe->gnodeComputeMPEAlpha(glat, glat->endP, scale);
  mpe->gnodeComputeMPEBeta(glat, glat->startP, scale);

  return TCL_OK;
}

static int computeCAvgMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  double c_avg;
  int count, num_ref;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();

  c_avg = mpe->glatComputeMPECq(glat);
  count = mpe->getNumContext(glat);
  num_ref = mpe->getNumRefPhones(glat);

  itfAppendResult("%e %d %d\n", c_avg, count, num_ref);

  return TCL_OK;
}

static int getNumRefPhones(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  int num_ref;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  num_ref = mpe->getNumRefPhones(glat);

  itfAppendResult("%d\n", num_ref);

  return TCL_OK;
}

static int posterioriMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  float tmp;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();

  float c_avg = mpe->glatComputeMPECq(glat);
  mpe->glatComputeMPEGamma(glat, c_avg);

//  mpe->printMPEStatus(glat);

  return TCL_OK;
}

static int adjustScoresBMMIItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  float scale = 1.0;
  float boost_factor = 0.5;
  float tmp;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "-b", ARGV_FLOAT, NULL, &boost_factor, NULL, "mystic scale",
    "-scale", ARGV_FLOAT, NULL, &scale, NULL, "mystic scale",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  mpe->resetMPE();
  mpe->glatBMMIAdjustScores(glat, boost_factor, scale);

  return TCL_OK;
}


static int objectiveMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  double c_avg=0.0;
  float scale = 1.0;
  float result;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    "-scale", ARGV_FLOAT, NULL, &scale, NULL, "mystic scale",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  result = mpe->objectiveVal(glat, scale);
  itfAppendResult("%e", result);

  return TCL_OK;
}

static int printMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  float result;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  mpe->printMPEStatus(glat);

  return TCL_OK;
}

static int printMPESummaryItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  GLat* glat;
  MPE* mpe;
  float result;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<GLat>", ARGV_OBJECT, NULL, &glat, "GLat", "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  mpe->printMPESummary(glat);

  return TCL_OK;
}


static int freeMPEItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  MPE* mpe;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mpe = aegis->getMPE();
  aegis->setMPE(NULL);

  delete mpe;

  return TCL_OK;
}


static int initRSWItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* mllr_transform;
  Aegis* aegis = (Aegis*) cd;
  EV* ev;
  MemoryManager* mgr = aegis->getMemoryManager();
  Recognizer* recognizer = aegis->getRecognizer();
  HMMSet* hmmset = aegis->getHMMSet();
  int dim, num_spkrs;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<mllr transform>", ARGV_STRING, NULL, &mllr_transform, NULL, "mllr transform file",
    "<num spkrs>", ARGV_INT, NULL, &num_spkrs, NULL, "num spkrs",
    NULL) != TCL_OK) return TCL_ERROR;

  dim = aegis->getDim();
  ev = new EV(mgr, recognizer, hmmset, dim, num_spkrs);
  ev->load(mllr_transform);

  aegis->setRSW(ev);
  aegis->backupMeans();

  return TCL_OK;
}

static int initRSWExtItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* mllr_transform;
  Aegis* aegis = (Aegis*) cd;
  RSWExt* rsw_ext;
  MemoryManager* mgr = aegis->getMemoryManager();
  Recognizer* recognizer = aegis->getRecognizer();
  HMMSet* hmmset = aegis->getHMMSet();
  int dim, num_spkrs, num_eigen_vectors;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<mllr transform>", ARGV_STRING, NULL, &mllr_transform, NULL, "mllr transform file",
    "<num spkrs>", ARGV_INT, NULL, &num_spkrs, NULL, "num spkrs",
    "<num eigen vecs>", ARGV_INT, NULL, &num_eigen_vectors, NULL, "num spkrs",
    NULL) != TCL_OK) return TCL_ERROR;

  dim = aegis->getDim();
  rsw_ext = new RSWExt(mgr, recognizer, hmmset, dim, num_spkrs, num_eigen_vectors);
  rsw_ext->load(mllr_transform);

  aegis->setRSWExt(rsw_ext);
  aegis->backupMeans();

  return TCL_OK;
}


static int freeRSWItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  EV* ev;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  ev = aegis->getRSW();
  aegis->setRSW(NULL);

  delete ev;

  return TCL_OK;
}

static int freeRSWExtItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  RSWExt* rsw_ext;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  rsw_ext = aegis->getRSWExt();
  aegis->setRSWExt(NULL);

  delete rsw_ext;

  return TCL_OK;
}


static int ResetItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  int num_words;
  char word[STRMAX];
  Aegis* aegis = (Aegis*) cd;
  EiKEV* ei_kev;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  ei_kev = aegis->getKEMLLR();
  ei_kev->restoreModel();

  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static int RSWResetItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  int num_words;
  char word[STRMAX];
  Aegis* aegis = (Aegis*) cd;
  EV* ev;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  ev = aegis->getRSW();

  aegis->reloadBackup();
  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static int RSWExtResetItf(ClientData cd, int argc, char *argv[]){
  char* method;
  char* text;
  FeatureSet* feature_set;
  Feature* feature;
  int num_words;
  char word[STRMAX];
  Aegis* aegis = (Aegis*) cd;
  RSWExt* rsw_ext;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  rsw_ext = aegis->getRSWExt();

  aegis->reloadBackup();
  aegis->exportPDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static int freeUtterHMMItf(ClientData cd, int argc, char *argv[]){
  char* method;
  Aegis* aegis = (Aegis*) cd;
  MemoryManager* mgr;
  RelationInfo* info;
  Recognizer* recognizer;
  HMMSet* hmmset;
  vector<State*> state_set;
  vector<Edge*> edge_set;
  
  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<method>", ARGV_STRING, NULL, &method, NULL, "method name",
    NULL) != TCL_OK) return TCL_ERROR;

  mgr = aegis->getMemoryManager();
  info = mgr->getRelationInfo();
  recognizer = aegis->getRecognizer();
  hmmset = aegis->getHMMSet();
  hmmset->getStateSet(state_set);
  hmmset->getEdgeSet(edge_set);

  delete recognizer;

  for(int i=0;i<state_set.size();i++){
    mgr->removeState(state_set[i]);
  }
  for(int i=0;i<edge_set.size();i++){
    mgr->removeEdge(edge_set[i]);
  }
  for(int i=0;i<hmmset->getNumHMM();i++){
    AegisHMM* ahmm = hmmset->getHMM(i);
    info->removeRelation("HMMNameTable", ahmm->getName(), (Ptr) ahmm);
    mgr->removeHMM(ahmm);
  }
 
  delete hmmset;
  aegis->setHMMSet(NULL);
  
  return TCL_OK;
}


static int FreeItf(ClientData cd){
  return TCL_OK;
}

static int UpdatePDFItf(ClientData cd, int argc, char* argv[]){
  char* name = NULL;
  Aegis* aegis = (Aegis*) cd;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<name>", ARGV_STRING, NULL, &name, NULL, "object name",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    NULL) != TCL_OK) return TCL_ERROR;

  aegis->updatePDFSet(codebook_set, distrib_set);

  return TCL_OK;
}

static ClientData CreateItf(ClientData cd, int argc, char* argv[]){
  char* name = NULL;
  CodebookSet* codebook_set;
  DistribSet* distrib_set;
  PhonesSet* phones_set;
  SenoneSet* senone_set;  
  Tree* distrib_tree;
  AModelSet* amodel_set;
  Dictionary* jdict;
  int num_threads = 1;

  if(itfParseArgv(argv[0], &argc, argv, 1,
    "<name>", ARGV_STRING, NULL, &name, NULL, "object name",
    "<CodebookSet>", ARGV_OBJECT, NULL, &codebook_set, "CodebookSet", "name of the codebook set",
    "<DistribSet>", ARGV_OBJECT, NULL, &distrib_set, "DistribSet", "name of the distrib set",
    "<PhonesSet>", ARGV_OBJECT, NULL, &phones_set, "PhonesSet", "name of the phones set",
    "<Tree>", ARGV_OBJECT, NULL, &distrib_tree, "Tree", "name of the topo set",
    "<Dictionary>", ARGV_OBJECT, NULL, &jdict, "Dictionary", "name of the dictionary",
    "-threads", ARGV_INT, NULL, &num_threads, NULL, "no of threads",
    NULL) != TCL_OK) return NULL;

  Aegis* aegis = new Aegis(argc, argv);
  aegis->setNumThreads(num_threads);

//  aegis->importPDFSet(codebook_set, distrib_set);
//  aegis->importPhonesSet(phones_set, true);
//  aegis->importDictionary(jdict);
//  aegis->importHMMSet(distrib_tree);
  
//  aegis->initRecognizer();

  return (ClientData) aegis;
}

static int ConfigureItf(ClientData cd, char* var, char* val){
  return TCL_OK;
}

static ClientData AccessItf(ClientData cd , char* name, TypeInfo** ti){
  return 0;
}

static Method method[] =
{
  { "puts", PutsItf, "display object info"},
  { "test", TestItf, "for testing purposes only"},
  { "loadUtterHMM", loadUtterHMMItf, "for testing purposes only"},
  { "freeUtterHMM", freeUtterHMMItf, "for testing purposes only"},
  { "initKEMLLR", initKEMLLRItf, "for testing purposes only"},
  { "freeKEMLLR", freeKEMLLRItf, "for testing purposes only"},
  { "initRSW", initRSWItf, "for testing purposes only"},
  { "freeRSW", freeRSWItf, "for testing purposes only"},
  { "initRSWExt", initRSWExtItf, "for testing purposes only"},
  { "freeRSWExt", freeRSWExtItf, "for testing purposes only"},
  { "initGDFSA", initGDFSAItf, "for testing purposes only"},
  { "kmeanClusterGDFSA", kmeanClusterGDFSAItf, "for testing purposes only"},
  { "clearAccuClusterGDFSA", clearAccuClusterGDFSAItf, "for testing purposes only"},
  { "accuClusterGDFSA", accuClusterGDFSAItf, "for testing purposes only"},
  { "updateClusterGDFSA", updateClusterGDFSAItf, "for testing purposes only"},
  { "saveAccuClusterGDFSA", saveAccuClusterGDFSAItf, "for testing purposes only"},
  { "loadAccuClusterGDFSA", loadAccuClusterGDFSAItf, "for testing purposes only"},
  { "saveClusterGDFSA", saveClusterGDFSAItf, "for testing purposes only"},
  { "loadClusterGDFSA", loadClusterGDFSAItf, "for testing purposes only"},
  { "saveOrigFeatGDFSA", saveOrigFeatGDFSAItf, "for testing purposes only"},
  { "saveFSAFeatGDFSA", saveFSAFeatGDFSAItf, "for testing purposes only"},
  { "restoreOrigFeatGDFSA", restoreOrigFeatGDFSAItf, "for testing purposes only"},
  { "transformGDFSA", transformGDFSAItf, "for testing purposes only"},
  { "setStatusGDFSA", setStatusGDFSAItf, "for testing purposes only"},
  { "accuGDFSA", accuGDFSAItf, "for testing purposes only"},
  { "saveAccuGDFSA", saveAccuGDFSAItf, "for testing purposes only"},
  { "loadAccuGDFSA", loadAccuGDFSAItf, "for testing purposes only"},
  { "loadGDFSA", loadGDFSAItf, "for testing purposes only"},
  { "saveGDFSA", saveGDFSAItf, "for testing purposes only"},
  { "updateGDFSA", updateGDFSAItf, "for testing purposes only"},
  { "clearAccuGDFSA", clearAccuGDFSAItf, "for testing purposes only"},
  { "clearTransformGDFSA", clearTransformGDFSAItf, "for testing purposes only"},
  { "transferGDFSA", transferGDFSAItf, "for testing purposes only"},
  { "initGDLR", initGDLRItf, "for testing purposes only"},
  { "accuGDLR", accuGDLRItf, "for testing purposes only"},
  { "clearAccuGDLR", clearAccuGDLRItf, "for testing purposes only"},
  { "saveAccuGDLR", saveAccuGDLRItf, "for testing purposes only"},
  { "loadAccuGDLR", loadAccuGDLRItf, "for testing purposes only"},
  { "saveGDLR", saveGDLRItf, "for testing purposes only"},
  { "loadGDLR", loadGDLRItf, "for testing purposes only"},
  { "linkGDLR", linkGDLRItf, "for testing purposes only"},
  { "transformGDLR", transformGDLRItf, "for testing purposes only"},
  { "clearAccuGDLR", clearAccuGDLRItf, "for testing purposes only"},
  { "updateGDLR", updateGDLRItf, "for testing purposes only"},
  { "initGMMIE", initGMMIEItf, "for testing purposes only"},
  { "accuGMMIE", accuGMMIEItf, "for testing purposes only"},
  { "setScaleGMMIE", setScaleGMMIEItf, "for testing purposes only"},
  { "clearAccuGMMIE", clearAccuGMMIEItf, "for testing purposes only"},
  { "saveAccuGMMIE", saveAccuGMMIEItf, "for testing purposes only"},
  { "loadAccuGMMIE", loadAccuGMMIEItf, "for testing purposes only"},
  { "loadAddAccuGMMIE", loadAddAccuGMMIEItf, "for testing purposes only"},
  { "updateGMMIE", updateGMMIEItf, "for testing purposes only"},
  { "initFMPE", initFMPEItf, "for testing purposes only"},
  { "accuFMPE", accuFMPEItf, "for testing purposes only"},
  { "transformFMPE", transformFMPEItf, "for testing purposes only"},
  { "updateFMPE", updateFMPEItf, "for testing purposes only"},
  { "loadFMPE", loadFMPEItf, "for testing purposes only"},
  { "saveFMPE", saveFMPEItf, "for testing purposes only"},
  { "storeFMPE", storeFMPEItf, "for testing purposes only"},
  { "setStatusFMPE", setStatusFMPEItf, "for testing purposes only"},
  { "loadAccuFMPE", loadAccuFMPEItf, "for testing purposes only"},
  { "saveAccuFMPE", saveAccuFMPEItf, "for testing purposes only"},
  { "loadClusterFMPE", loadClusterFMPEItf, "for testing purposes only"},
  { "saveClusterFMPE", saveClusterFMPEItf, "for testing purposes only"},
  { "clearAccuFMPE", clearAccuFMPEItf, "for testing purposes only"},
  { "initMPE", initMPEItf, "for testing purposes only"},
  { "alphaBetaMPE", alphaBetaMPEItf, "for testing purposes only"},
  { "computeCAvgMPE", computeCAvgMPEItf, "for testing purposes only"},
  { "num_ref_phones", getNumRefPhones, "for testing purposes only"},
  { "posterioriMPE", posterioriMPEItf, "for testing purposes only"},
  { "adjustScoresBMMI", adjustScoresBMMIItf, "for testing purposes only"},
  { "printMPE", printMPEItf, "for testing purposes only"},
  { "printMPESummary", printMPESummaryItf, "for testing purposes only"},
  { "objMPE", objectiveMPEItf, "for testing purposes only"},
  { "freeMPE", freeMPEItf, "for testing purposes only"},
  { "adapt", AdaptItf, "for testing purposes only"},
  { "adaptRSW", AdaptRSWItf, "for testing purposes only"},
  { "adaptRSWExt", AdaptRSWExtItf, "for testing purposes only"},
  { "update", UpdatePDFItf, "for testing purposes only"},
  { "reset", ResetItf, "for testing purposes only"},
  { "resetRSW", RSWResetItf, "for testing purposes only"},
  { "resetRSWExt", RSWExtResetItf, "for testing purposes only"},
  { NULL, NULL, NULL}
};

static TypeInfo objInfo = { "Aegis", 0, -1, method, CreateItf, FreeItf, ConfigureItf, AccessItf, NULL, "Aegis" };

int Aegis_Init(){
  if (Initialized) return TCL_OK;

  itfNewType(&objInfo);

  Initialized = 1;

  return TCL_OK;
}
