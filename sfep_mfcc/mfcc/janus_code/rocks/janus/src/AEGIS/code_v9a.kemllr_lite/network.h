#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <map>
#include <string>
#include "ahmm.h"
#include "relation_info.h"

using namespace std;

class Network{
  public:
    Network(MemoryManager* mgr);
    ~Network();

    void clear();
    void addHMM(AegisHMM* hmm);
    void addHMM(const string& net_name, vector<AegisHMM*> hmm);
    void addNull();
    void linkNetNode(int source, int target, float penalty=0.0);
    void setStart(int node_idx);
    void setEnd(int node_idx);
    int getStart();
    int getEnd();

    AegisHMM* getHMM(int index);
    void getUniqHMMSet(vector<AegisHMM*>& result);
    void getUniqRealStateSet(vector<State*>& result, vector<int>& idx);
    void getNetNodeSeq(const vector<int>& state_seq, vector<string>& word_seq);

    int getNumStates();
    bool hasEdge(int source, int target);
    bool isWordEnd(int node_idx);

    void getStateSet(vector<State*>& state_set);
    void getEdgeSet(vector<Edge*>& edge_set);
    void buildAdjList(vector<vector<int> >& adj_list);
    void buildAdjRealList(vector<vector<int> >& adj_list);
    void buildParentList(vector<vector<int> >& parent_list);
//    void buildTranTable(vector<vector<float> >& log_tran_prob);
    void buildTranTable(map<int, map<int, float> >& log_tran_prob);

    void topSort(vector<State*>& null_set, vector<int>& null_idx);
//    void getReal(vector<State*>& real_set, vector<int>& real_idx);
    bool determineBeginEnd();
  private:
    MemoryManager* mgr;

    vector<AegisHMM*> hmm_list;
    vector<pair<int, int> > edge_list;
    vector<float> penalty_list;
    vector<State*> state_set;
    map<int, int> net2st_start, net2st_end, st2net;
    vector<string> net_name;
    vector<vector<int> > adj_list, adj_real_list;
    vector<vector<int> > parent_list;
    int start, end, item_count;
    vector<bool> word_end;

    State* null_state;
};

#endif
