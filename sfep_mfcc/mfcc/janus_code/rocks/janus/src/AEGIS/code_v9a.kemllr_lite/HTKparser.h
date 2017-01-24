#ifndef HTK_PARSER_H
#define HTK_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "memmgr.h"
#include "relation_info.h"
#include "network.h"
#include "adictionary.h"
#include "ahmm.h"
#include "hmmset.h"
#include "str_util.h"
#include "reg_tree.h"

using namespace std;

class HTKParser{
  public:
    HTKParser(MemoryManager* mgr);
    ~HTKParser();

    void parse(char* filename);
    void parseXForm(char* filename);
    string readHMM(FILE* fp);
    void readStateMacro(FILE* fp);
    void readTransitionMacro(FILE* fp);
    string readXFormHMM(FILE* fp);
    void readXFormStateMacro(FILE* fp);
    void readXFormTransitionMacro(FILE* fp);
    string readRegressionTree(FILE* fp);
    void readVarFloor(FILE* fp);

    void parseLattice(const string& lattice_file, Network* network);
    void parseLattice(const string& lattice_file, AegisDictionary* dict, Network* network);

    HMMSet makeHMMSet();

    RelationInfo* retrieveInfo();
    AegisHMM* retrieveHMM(string name);

    int getVecSize();
    vector<PDF*> getAllPDFs();
  private:
    MemoryManager* mgr;
    RelationInfo* info;
    RelationInfo macro;

    int vec_size;
};

#endif
