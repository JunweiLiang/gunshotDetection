#include <vector>
#include <map>
#include "janus_hash.h"

using namespace std;

void* create_long_double_hash()
{
   return (void*) new map<long,double>();
}

void set_int_double_hash(void* table, long key, double value)
{
   map<long,double>* tab = (map<long,double>*) table;

   (*tab)[key] = value;
}

double get_int_double_hash(void* table, long key, int* isfound)
{
   map<long,double>* tab = (map<long,double>*) table;
   map<long,double>::const_iterator iter = tab->find(key);

   if (iter != tab->end()) {
      *isfound = 1;
      return iter->second;
   } else {
      *isfound = 0;
      return 0.0;
   }
}

void clear_long_double_hash(void* table)
{
   map<long,double>* tab = (map<long,double>*) table;

   tab->clear();
}

void* create_II2I_hash()
{
   return (void*) new map<int,map<int, int> >();
}

void set_II2I_hash(void* table, int k1, int k2, int value)
{
   map<int,map<int, int> >* tab = (map<int,map<int, int> >*) table;

   (*tab)[k1][k2] = value;
}

int get_II2I_hash(void* table, int k1, int k2, int* isfound)
{
   map<int,map<int, int> >* tab = (map<int,map<int, int> >*) table;
   map<int,map<int, int> >::const_iterator iter1 = tab->find(k1);

   if(iter1 == tab->end()){
      *isfound = 0;
      return -1;
   }

   map<int, int>::const_iterator iter2 = iter1->second.find(k2);
   
   if(iter2 == iter1->second.end()){
      *isfound = 0;
      return -1;
   }

   *isfound = 1;
   return iter2->second;
}

void clear_II2I_hash(void* table)
{
   map<int,map<int, int> >* tab = (map<int,map<int, int> >*) table;

   tab->clear();
}

void* create_III2I_hash()
{
   return (void*) new map<int,map<int, map<int, int> > >();
}

void set_III2I_hash(void* table, int k1, int k2, int k3, int value)
{
   map<int,map<int, map<int, int> > >* tab = (map<int,map<int, map<int, int> > >*) table;

   (*tab)[k1][k2][k3] = value;
}

int get_III2I_hash(void* table, int k1, int k2, int k3, int* isfound)
{
   map<int,map<int, map<int, int> > >* tab = (map<int,map<int, map<int, int> > >*) table;
   map<int,map<int, map<int, int> > >::const_iterator iter1 = tab->find(k1);

   if(iter1 == tab->end()){
      *isfound = 0;
      return -1;
   }

   map<int, map<int, int> >::const_iterator iter2 = iter1->second.find(k2);
   
   if(iter2 == iter1->second.end()){
      *isfound = 0;
      return -1;
   }

   map<int, int>::const_iterator iter3 = iter2->second.find(k3);

   if(iter3 == iter2->second.end()){
      *isfound = 0;
      return -1;
   }

   *isfound = 1;
   return iter3->second;
}

void clear_III2I_hash(void* table)
{
   map<int,map<int, int> >* tab = (map<int,map<int, int> >*) table;

   tab->clear();
}

void* create_III2I_vector_hash(){
  return (void*) new vector<vector<vector<int> > >();
}

void set_III2I_vector_hash(void* table, int k1, int k2, int k3, int value){
  vector<vector<vector<int> > >* tab1 = (vector<vector<vector<int> > >*) table;
  if(tab1->size()==0 || k1>tab1->size()-1){
    tab1->resize(k1+1);
  }

  vector<vector<int> >* tab2 = &((*tab1)[k1]);
  if(tab2->size()==0 || k2>tab2->size()-1){
    tab2->resize(k2+1);
  }

  vector<int>* tab3 = &((*tab2)[k2]);
  if(tab3->size()==0 || k3>tab3->size()-1){
    int idx = tab3->size();
    tab3->resize(k3+1);
    for(int i=idx;i<tab3->size();i++){
      (*tab3)[i] = -1;
    }
  }
  (*tab3)[k3] = value;
}

int get_III2I_vector_hash(void* table, int k1, int k2, int k3, int* isfound){
  vector<vector<vector<int> > >& tab = *((vector<vector<vector<int> > >*) table);

  if(tab.size()==0 || tab.size()-1<k1){
    *isfound = 0;
    return -1;
  }
  if(tab[k1].size()==0 || tab[k1].size()-1<k2 || tab[k1][k2].size()-1<k3){
    *isfound = 0;
    return -1;
  }
  if(tab[k1][k2].size()==0 || tab[k1][k2].size()-1<k3 || tab[k1][k2][k3]==-1){
    *isfound = 0;
    return -1;
  }

  *isfound = 1;
  return tab[k1][k2][k3];
}

void clear_III2I_vector_hash(void* table){
  vector<vector<vector<int> > >& tab = *((vector<vector<vector<int> > >*) table);

  tab.clear();
}


