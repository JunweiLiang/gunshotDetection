#ifndef MEMMGR_H
#define MEMMGR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include "pdf.h"
#include "gaussian.h"
#include "multimix.h"
#include "state.h"
#include "edge.h"
#include "ahmm.h"
#include "relation_info.h"
#include "linear_algebra.h"

using namespace std;

class AegisHMM;

class MemoryManager{
  public:
    MemoryManager();
    MemoryManager(RelationInfo* info);

    RelationInfo* getRelationInfo();

    DiagGaussian* createDiagGaussian(int dim);
    FullGaussian* createFullGaussian(int dim);
    MultiMix* createMultiMix(int dim);
    Edge* createEdge(State* source, State* target, float weight);
    State* createState(PDF* pdf);
    AegisHMM* createHMM(const string& name);

    State* duplicateState(State* state);

    bool hasObj(const string& id, Ptr obj);
    bool hasObj(const string& id, const string& table_name, Ptr obj);

    bool removeDiagGaussian(DiagGaussian* g);
    bool removeFullGaussian(FullGaussian* g);
    bool removeMultiMix(MultiMix* mm);
    bool removeEdge(Edge* edge);
    bool removeState(State* state);
    bool removeHMM(AegisHMM* hmm);
    bool removeObj(const string& id);
    bool removeObj(const string& id, const string& table_name);

    string keyType(const string& id);
  private:
    RelationInfo* info;
    int pdf_count;
    int edge_count;
    int state_count;
    int hmm_count;
};

#endif
