#include "reg_tree.h"

RegTree::RegTree(int label){
  this->label = label;
  occ = 0;
  left = right = NULL;
}

RegTree::~RegTree(){
  if(left!=NULL)
    delete left;
  if(right!=NULL)
    delete right;
}

void RegTree::setOcc(int occ){
  this->occ = occ;
}

int RegTree::getOcc(){
  if(left==NULL && right==NULL)
    return occ;
  return left->getOcc() + right->getOcc();
}

int RegTree::getLabel(){
  return label;
}

void RegTree::addChildren(RegTree* left, RegTree* right){
  this->left = left;
  this->right = right;
}

