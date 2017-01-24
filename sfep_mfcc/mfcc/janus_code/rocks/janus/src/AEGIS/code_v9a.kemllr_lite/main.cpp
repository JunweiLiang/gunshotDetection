#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "HTKparser.h"
#include "readHTKformat.h"
#include "transcription.h"
#include "dictionary.h"
#include "cache.h"
#include "math_engine.h"
#include "mlf.h"
#include "hmmset.h"
#include "str_util.h"
#include "accumulator.h"

using namespace std;

void usage();
int getOpt(int argc, char* argv[], RelationInfo* config);
char* getVal(RelationInfo* config, char option[]);
void readHMMList(RelationInfo* config, vector<string>& all_classes);
void readFilelist(RelationInfo* config, vector<string>& filelist);
void readFilelist(RelationInfo* config, const char* listname, vector<string>& filelist);
void readRealLength(MFCCReader* reader, vector<int>& filelist);
void readHTKConfig(RelationInfo* config);

int main(int argc, char* argv[]){
  MemoryManager* mgr;
  RelationInfo* info;
  RelationInfo* config;
  HTKParser* parser, *xform_parser;
  Recognizer *recognizer;
  Network *network, *xform_network;
  AegisDictionary* dict;
  MFCCReader *reader;
  MasterLabel mlf;
  vector<Transcription> transcription;
  vector<string> all_classes, filelist, testlist;
  vector<int> real_length;
  int dim, window_size, target_rate;
  float learning_rate, barrier_weight, delta;
  char *in_transfile, *out_transfile, *objcons;
  string out_mlf;
  bool is_diag;

  if(argc==1){
    usage();
    exit(-1);
  }

  info = new RelationInfo();
  config = new RelationInfo();
  mgr = new MemoryManager(info);
  parser = new HTKParser(mgr);
  reader = new MFCCReader();

  getOpt(argc, argv, config);

  printf("parsing HMM\n");
  parser->parse(getVal(config, "Model"));
  network = new Network(mgr);
  dim = parser->getVecSize();
  config->setAssociateValue("Config", "dim", (Ptr)&dim, sizeof(int)); //dimension of the observations
  
  HMMSet hmmset = parser->makeHMMSet(); //equivalent to a mmf file

  printf("reading AegisDictionary\n");
  dict = new AegisDictionary();
  dict->parse(getVal(config, "Dict")); //read a dictionary

  //uncomment this one if you want to use arbitrary network in HTK format
  //printf("parsing Network\n");
  //parser->parseLattice(getVal(config, "Lattice"), dict, network);

  printf("initiating Recognizer\n");
  recognizer = new Recognizer(mgr, config); //create a recognizer, no network yet

  reader->readFileList(getVal(config, "Filelist")); //read a MFCC filelist
  readFilelist(config, filelist);
  readHMMList(config, all_classes); //read a HMM list

  printf("reading MLF...\n");
  mlf.readMLF(getVal(config, "TrainMLF")); //read a MLF
  printf("converting MLF...\n");
  mlf.createTranscription(filelist, transcription); //transform it into an internal format
/*
  for(int u=0;u<reader->getTotalUtterance();u++){
    Network* fa_net;
    CacheTable* mix_occ;
    vector<State*> state_set;
    Transcription phone_trans = dict->convertToPhoneLevel(transcription[u]);
    
    reader->readData(u); //load an utterance

    fa_net = new Network(mgr);
    recognizer->concatHMM(phone_trans.getSeq(), *fa_net); //make a network for force alignment
    recognizer->setNetwork(fa_net); //request the recognizer to use that network
    fa_net->getStateSet(state_set); //get a vector of all states in the network

    mix_occ = new CacheTable(3, reader->getTotalFrames(), fa_net->getNumStates(), hmmset.getNumMix());
    recognizer->computeLogMixOcc(reader->getX(), reader->getTotalFrames(), mix_occ); //compute gamma

    //change the format if you dun like this one
    printf("utterance: %d\n", u);
    for(int t=0;t<reader->getTotalFrames();t++){
      printf("time: %d\n", t);
      for(int j=0;j<state_set.size();j++){
        if(state_set[j]->isNullNode()){
          printf("null state: %d\n", j); //dun do anything if it's a null state
          continue;
        }
        HMM* hmm = (HMM*) info->getFirstAnswer("StateInHMM", state_set[j]->getID()); //this state belongs to which hmm?
        printf("real state(%s): %d\n", hmm->getName().c_str(), j);
        MultiMix* mm = (MultiMix*) state_set[j]->getPDF(); //get a multiple mixture pdf
        for(int m=0;m<mm->numMixture();m++){
          Gaussian* g = mm->getGaussian(m); //get a gaussian
          printf("mix: %d = %e\n", m, mix_occ->get(t, j, m)); //print the gamma in log scale
        }
      }
    }
    
    delete fa_net;
    delete mix_occ;
  }
*/ 


  FILE* fp = fopen(getVal(config, "TestMLF"), "w");
  fprintf(fp, "#!MLF!#\n");
  for(int u=0;u<reader->getTotalUtterance();u++){
    Network* fa_net;
    CacheTable* mix_occ;
    vector<State*> state_set;
    Transcription phone_trans = dict->convertToPhoneLevel(transcription[u]);
    vector<string> word_seq = transcription[u].getSeq();
    Transcription result;
    
    reader->readData(u); //load an utterance

    fa_net = new Network(mgr);
    recognizer->forceAlign(phone_trans.getSeq(), reader->getX(), reader->getTotalFrames(), result, *fa_net);
    vector<int> state_seq, time;

    fprintf(fp, "\"%s\"\n", filelist[u].c_str());
    int pre=0, cur=0;
    for(int i=0;i<word_seq.size();i++){
      vector<string> sym_seq;
      dict->getSymSeq(word_seq[i], sym_seq);
      cur+=sym_seq.size()-1;
      fprintf(fp, "%d %d %s\n", result.getDuration(pre).first, result.getDuration(cur).second, word_seq[i].c_str());
      cur++;
      pre = cur;
    }
    fprintf(fp, ".\n");

    delete fa_net;
  }
  fclose(fp);
  
  delete reader;
  delete recognizer;
  delete dict;
  delete parser;
  delete network;
  delete mgr;
  delete config;
  delete info;

  return 0;
}

void readHTKConfig(RelationInfo* config){
  FILE* fp;
  char line[STRMAX], buf[STRMAX];
  int tmp;

  fp = fopen(getVal(config, "HTKConfig"), "r");

  while(1){
    fgets(line, STRMAX, fp);
    if(feof(fp))
      break;

    if(line[strlen(line)-1]=='\n')
      line[strlen(line)-1] = '\0';

    if(numField(line, " \t\n=")<2)
      continue;

    extractField(line, 0, " \t\n", buf);
    if(strcmp(buf, "WINDOWSIZE")==0){
      extractField(line, 2, " \t\n", buf);
      tmp = atoi(buf);

      config->setAssociateValue("Config", "WindowSize", (Ptr)&tmp, sizeof(tmp));
    }
    else if(strcmp(buf, "TARGETRATE")==0){
      extractField(line, 2, " \t\n", buf);
      tmp = atoi(buf);

      config->setAssociateValue("Config", "TargetRate", (Ptr)&tmp, sizeof(tmp));
    }
  }

  fclose(fp);
}

void readRealLength(MFCCReader* reader, vector<int>& real_length){
  real_length.clear();

  for(int i=0;i<reader->getTotalUtterance();i++){
    reader->readData(i);
    real_length.push_back(reader->getTotalFrames());
  }
}

void readFilelist(RelationInfo* config, const char* listname, vector<string>& filelist){
  FILE* fp;
  char buf[STRMAX];
  
  filelist.clear();
  fp = fopen(getVal(config, (char*)listname), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    filelist.push_back(buf);
  }

  fclose(fp);
}

void readFilelist(RelationInfo* config, vector<string>& filelist){
  FILE* fp;
  char buf[STRMAX];
  
  filelist.clear();
  fp = fopen(getVal(config, "Filelist"), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    filelist.push_back(buf);
  }

  fclose(fp);
}

void readHMMList(RelationInfo* config, vector<string>& all_classes){
  FILE* fp;
  char buf[STRMAX];
  
  all_classes.clear();
  fp = fopen(getVal(config, "Phone"), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    all_classes.push_back(buf);
  }

  fclose(fp);
}

char* getVal(RelationInfo* config, char option[]){
  char* val=NULL;

  config->getAssociateValue("Config", option, (Ptr)&val);

  return val;
}

int getOpt(int argc, char* argv[], RelationInfo* config){
  int tmp;
  float tmpf;
  float tmpd;

  config->clear();
  config->addTable("Config", "AssociateValue");

  for(int i=1;i<argc;i+=2){
    if(strcmp(argv[i], "-I")==0){
      config->setAssociateValue("Config", "TrainMLF", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-i")==0){
      config->setAssociateValue("Config", "TestMLF", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-H")==0){
      config->setAssociateValue("Config", "Model", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-O")==0){
      config->setAssociateValue("Config", "OutModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-X")==0){
      config->setAssociateValue("Config", "XFormModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Y")==0){
      config->setAssociateValue("Config", "OutXFormModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-A")==0){
      config->setAssociateValue("Config", "OrigModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-B")==0){
      config->setAssociateValue("Config", "XFormOrigModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-f")==0){
      config->setAssociateValue("Config", "FEParam", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-of")==0){
      config->setAssociateValue("Config", "OutFEParam", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-it")==0){
      config->setAssociateValue("Config", "InTrans", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ot")==0){
      config->setAssociateValue("Config", "OutTrans", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-S")==0){
      config->setAssociateValue("Config", "Filelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Spkr")==0){
      config->setAssociateValue("Config", "SpkrFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrTrans")==0){
      config->setAssociateValue("Config", "SpkrTransformation", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrAdapt")==0){
      config->setAssociateValue("Config", "SpkrAdaptFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrTest")==0){
      config->setAssociateValue("Config", "SpkrTestFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrModel")==0){
      config->setAssociateValue("Config", "SAModellist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrDeptModel")==0){
      config->setAssociateValue("Config", "SDModellist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-F")==0){
      config->setAssociateValue("Config", "FBFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-M")==0){
      config->setAssociateValue("Config", "MFCCFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ObjF")==0){
      config->setAssociateValue("Config", "ObjFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ConstF")==0){
      config->setAssociateValue("Config", "ConstFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-DC")==0){
      config->setAssociateValue("Config", "ObjCons", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-C")==0){
      config->setAssociateValue("Config", "HTKConfig", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-D")==0){
      config->setAssociateValue("Config", "Dict", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-P")==0){
      config->setAssociateValue("Config", "Phone", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-p")==0){
      tmpd = atof(argv[i+1]);
      config->setAssociateValue("Config", "InsertionPenalty", (Ptr)&tmpd, sizeof(float));
    }
    else if(strcmp(argv[i], "-t")==0){
      tmpd = atof(argv[i+1]);
      config->setAssociateValue("Config", "Beam", (Ptr)&tmpd, sizeof(float));
    }
    else if(strcmp(argv[i], "-e")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "LearningRate", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-b")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "Beta", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-d")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "Delta", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-L")==0){
      config->setAssociateValue("Config", "Lattice", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-NUMREGC")==0){
      config->setAssociateValue("Config", "NumRegGroup", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-NUMEIGEN")==0){
      config->setAssociateValue("Config", "NumEigenVec", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ADAPT")==0){
      config->setAssociateValue("Config", "Adapt", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SMOOTH")==0){
      config->setAssociateValue("Config", "Smooth", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-COV")==0){
      config->setAssociateValue("Config", "Covariance", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SigSlope")==0){
      config->setAssociateValue("Config", "SigSlope", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SigOff")==0){
      config->setAssociateValue("Config", "SigOff", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Thres")==0){
      config->setAssociateValue("Config", "Thres", (Ptr)&argv[i+1], sizeof(char*));
    }
    else{
      printf("Unknown option\n");
      return -1;
    }
  }

  return 0;
}

void usage(){
  printf("-I\tMLF\n");
  printf("-i\tOutput MLF\n");
  printf("-H\tModel\n");
  printf("-f\tFEParam\n");
  printf("-of\tOutput FEParam\n");
  printf("-S\tFile list\n");
  printf("-F\tFFT file list\n");
  printf("-C\tHTK Config file\n");
  printf("-D\tAegisDictionary\n");
  printf("-P\tHMM list\n");
  printf("-p\tInsertion penalty\n");
  printf("-e\tLearning rate\n");
  printf("-L\tLattice\n");
}

