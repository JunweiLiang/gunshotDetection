#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "pdf.h"

using namespace std;

class State{
  public:
    State();
    State(const string& id);
    State(const string& id, PDF* pdf);

    string getID();

    void setPDF(PDF* pdf);
    PDF* getPDF();

    bool isNullNode();

    float logBj(const float* x);
  private:
    string id;
    PDF* pdf;
};

#endif
