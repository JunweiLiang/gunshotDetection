#ifndef REGTREE_H
#define REGTREE_H

#include <stdio.h>
#include <stdlib.h>

class RegTree{
  public:
    RegTree(int label);
    ~RegTree();

    void setOcc(int occ);
    int getOcc();
    int getLabel();
    void addChildren(RegTree* left, RegTree* right);
  private:
    int label;
    int occ;
    RegTree *left, *right;
};

#endif
