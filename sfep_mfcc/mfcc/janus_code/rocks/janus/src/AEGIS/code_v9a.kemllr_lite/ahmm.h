#ifndef AegisHMM_H
#define AegisHMM_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "cluster_engine.h"
#include "state.h"
#include "edge.h"
#include "multimix.h"
#include "gaussian.h"
#include "relation_info.h"

class ClusterEngine;

class AegisHMM{
  public:
    AegisHMM();
    AegisHMM(const string& id, const string& name);

    string getID();
    string getName();
   
    void insertStartState(State* state);
    void insertEndState(State* state);
    void setStartState(int s_index);
    void setEndState(int e_index);

    bool determineBeginEnd();

    void insertState(State* state);
    void insertEdge(Edge* edge);
    
    void getStateSet(vector<State*>& set);
    void getEdgeSet(vector<Edge*>& set);
    void getEmittingEdgeSet(State* state, vector<Edge*>& edge_set);

    void toNormalForm(AegisHMM* normal);

    State* getState(int index);
    Edge* getEdge(State* source, State* target);

    int getInDegree(State* state);
    int getOutDegree(State* state);

    State* getStartState();
    State* getEndState();

    int getStateNum(State* s);
    int getNumState();
    int getNumNullState();
    int getNumEdge();

    void topSort(vector<State*>& set);

    bool hasNullLoop();
    bool hasState(State* state);
    bool hasEdge(State* source, State* target);

    void printFile(FILE* fp);
//    void print();
//    void print(RelationInfo* cluster_info);
//    void printFile(FILE* fp, ClusterEngine* eng);
  private:
    string id;
    string name;
    vector<State*> state_set;
    vector<Edge*> edge_set;

    State* start;
    State* end;
};

#endif
