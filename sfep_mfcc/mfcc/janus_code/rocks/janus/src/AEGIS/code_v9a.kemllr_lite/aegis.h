#ifndef Aegis_H
#define Aegis_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <omp.h>
#include "memmgr.h"
#include "relation_info.h"
#include "recognizer.h"
#include "HTKparser.h"
#include "readHTKformat.h"
#include "transcription.h"
#include "adictionary.h"
#include "cache.h"
#include "math_engine.h"
#include "mlf.h"
#include "hmmset.h"
#include "ei_kev.h"
#include "ev.h"
#include "rsw_ext.h"
#include "str_util.h"
#include "accumulator.h"
#include "topo.h"
#include "amodel.h"
#include "senones.h"
#include "phones.h"
#include "word.h"
#include "distrib.h"
#include "codebook.h"
#include "tree.h"
#include "dictionary.h"
#include "mpe.h"
#include "g-mmie.h"
#include "gdlr.h"
#include "gdfsa.h"
#include "fmpe.h"
#include "gdt.h"

using namespace std;

class Aegis{
  public:
    Aegis(int argc, char* argv[]);
    ~Aegis();
    
    void readHMMList(RelationInfo* config, vector<string>& all_classes);
    void readFilelist(RelationInfo* config, vector<string>& filelist);
    void readFilelist(RelationInfo* config, const char* filelist, vector<string>& filelist);
    void readHTKConfig(RelationInfo* config);
    
    void exportPDFSet(CodebookSet* codebook_set, DistribSet* distrib_set);
    void importPDFSet(CodebookSet* codebook_set, DistribSet* distrib_set);
    void updatePDFSet(CodebookSet* codebook_set, DistribSet* distrib_set);
    void importPhonesSet(PhonesSet* phone_set, bool tri_phone=true);
    void importHMMSet(Tree* distrib_tree);
    void importDictionary(Dictionary* jdict);
    void initRecognizer();

    void defineGarbagePhone();
    bool isGarbage(const string& phone);
    void getGarbagePhone(vector<string>& garbage);

    void setKEMLLR(EiKEV* ei_kev);
    EiKEV* getKEMLLR();
    void setRSW(EV* ev);
    EV* getRSW();
    void setRSWExt(RSWExt* rsw_ext);
    RSWExt* getRSWExt();
    void setMPE(MPE* mpe);
    MPE* getMPE();
    void setFMPE(FMPE* fmpe);
    FMPE* getFMPE();
    void setGMMIE(GMMIE* gmmie);
    GMMIE* getGMMIE();
    void setGDLR(GDLR* gdlr);
    GDLR* getGDLR();
    void setGDFSA(GDFSA* gdfsa);
    GDFSA* getGDFSA();
    void setGDT(GDT* gdt);
    GDT* getGDT();

    void backupMeans();
    void reloadBackup();

    AegisDictionary* getADictionary();
    HMMSet* getHMMSet();
    void setHMMSet(HMMSet* hmm_set);
    Recognizer* getRecognizer();
    MemoryManager* getMemoryManager();
    PDF* getPDF(const string& ds_name);
    void getPDFSet(vector<PDF*>& pdf_set);
    void getAllGaussians(vector<Gaussian*>& gaussian);
    int getDim();
    int setNumThreads(int num_threads);
    
    int getOpt(int argc, char* argv[], RelationInfo* config);
    char* getVal(RelationInfo* config, char option[]);
  private:
    int getNodeX(char* ph_pos, TreeNodeList* list);
    MemoryManager* mgr;
    RelationInfo* info;
    RelationInfo* config;
    HTKParser* parser;
    Recognizer *recognizer;
    Network *network;
    AegisDictionary* dict;
    MFCCReader *reader;
    EiKEV* ei_kev;
    EV* ev;
    RSWExt* rsw_ext;
    MPE* mpe;
    GMMIE* gmmie;
    GDLR* gdlr;
    GDFSA* gdfsa;
    GDT* gdt;
    FMPE* fmpe;
    MasterLabel mlf;
    vector<Transcription> transcription;
    vector<string> all_classes, filelist, garbage;
    
    vector<PDF*> pdf_set;
    vector<Gaussian*> gaussian;
    vector<Matrix> mean_backup;
    map<string, int> ds_name2pdf_idx;
    vector<int> ds_idx2pdf_idx;
    HMMSet* hmm_set;
    AegisDictionary* adict;

    int num_threads;
    int dim;
    float learning_rate;
    string version;
};


#endif
