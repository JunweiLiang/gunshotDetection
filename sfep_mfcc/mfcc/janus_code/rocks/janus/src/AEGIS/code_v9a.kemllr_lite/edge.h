#ifndef EDGE_H
#define EDGE_H

#include <string>
#include "state.h"

using namespace std;

class Edge{
  public:
    Edge();
    Edge(const string& id);
    Edge(const string& id, State* source, State* target, const float& probability);

    string getID();

//    void link(State* source, State* target, const float& probability);
    
    void setProb(const float& probability);
    float getProb();

    State* getSource();
    State* getTarget();
    void setSource(State* source);
    void setTarget(State* target);

  private:
    string id;
    float probability;
    State* source;
    State* target;
};

#endif
