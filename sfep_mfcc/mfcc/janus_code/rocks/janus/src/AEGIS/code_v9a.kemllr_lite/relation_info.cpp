#include "relation_info.h"

RelationInfo::RelationInfo(){
  collection.clear();
  table_type.clear();
}

void RelationInfo::clear(){
  collection.clear();
  table_type.clear();
}

void RelationInfo::destroy(){
  //destroy all objs in relation info
  vector<string> table_list;
  vector<string> key_list;
  vector<Ptr> ans;
  int i, j, k;

  getTableList(table_list);
  for(i=0;i<table_list.size();i++){
    getKeyList(table_list[i], key_list);
    for(j=0;j<key_list.size();j++){
      answer(table_list[i], key_list[j], ans);
      for(k=0;k<ans.size();k++){
        if(table_type[table_list[i]]=="Relation")
          free(ans[k]);
        else if(table_type[table_list[i]]=="AssociateValue")
          deleteObjVal((ObjVal*)ans[k]);
      }
    }
  }

  clear();
}

bool RelationInfo::destroyTable(const string& table_name){
  multiset<Ptr>* tmp;
  vector<Ptr> ans;
  vector<string> key_list;
  int i, j;
  
  if(!hasTable(table_name))
    return false;
  
  getKeyList(table_name, key_list);
  
  for(i=0;i<key_list.size();i++){
    if(table_type[table_name]=="Relation"){
      answer(table_name, key_list[i], ans);

      for(j=0;j<ans.size();j++){
        free(ans[j]);
      }
    }
    else if(table_type[table_name]=="AssociateValue"){
      tmp = retrieve(table_name, key_list[i]);

      for(MULTI_SET_IT it=tmp->begin();it!=tmp->end();it++){
        deleteObjVal((ObjVal*)*it);
      }
    }
  }

  removeTable(table_name);

  return true;
}

bool RelationInfo::addTable(const string& table_name){
  if(hasTable(table_name))
    return false;
  
  collection[table_name]=empty_table;
  table_type[table_name]="Relation";
  
  return true;
}

bool RelationInfo::addTable(const string& table_name, const string& type){
  if(hasTable(table_name))
    return false;
  
  collection[table_name]=empty_table;
  table_type[table_name]=type;
  
  return true;
}

bool RelationInfo::removeTable(const string& table_name){
  if(!hasTable(table_name))
    return false;

  collection.erase(table_name);
  table_type.erase(table_name);

  return true;
}

bool RelationInfo::hasTable(const string& table_name){
  if(collection.find(table_name)==collection.end())
    return false;
  return true;
}

bool RelationInfo::setAssociateValue(const string& table_name, const string& key, Ptr value, int size){
  ObjVal* obj;
  
  if(!hasTable(table_name))
    return false;
  if(table_type[table_name]!="AssociateValue")
    return false;

  if(!hasKey(table_name, key))
    collection[table_name][key]=empty_data;
  if(!collection[table_name][key].empty()){
    obj = (ObjVal*) *collection[table_name][key].begin();
    collection[table_name][key].erase((Ptr)obj);

    deleteObjVal(obj);
  }

  obj = makeObjVal(value, size);

  collection[table_name][key].insert((Ptr)obj);

  return true;
}

bool RelationInfo::getAssociateValue(const string& table_name, const string& key, Ptr value){
  ObjVal* obj;
  
  if(!hasTable(table_name))
    return false;
  if(table_type[table_name]!="AssociateValue")
    return false;
  if(!hasKey(table_name, key))
    return false;

  obj = (ObjVal*) *collection[table_name][key].begin();

  memcpy(value, obj->value, obj->size);

  return true;
}

bool RelationInfo::removeAssociateValue(const string& table_name, const string& key){
  if(!hasKey(table_name, key))
    return false;
  if(table_type[table_name]!="AssociateValue")
    return false;

  deleteObjVal((ObjVal*)*collection[table_name][key].begin());

  collection[table_name][key].clear();
  collection[table_name].erase(key);

  return true;
}

bool RelationInfo::addRelation(const string& table_name, const string& key, Ptr obj){
  if(!hasTable(table_name))
    return false;
  if(table_type[table_name]!="Relation")
    return false;
  if(!hasKey(table_name, key))
    collection[table_name][key]=empty_data;

  collection[table_name][key].insert(obj);
  return true;
}

bool RelationInfo::removeRelation(const string& table_name, const string& key, Ptr obj){
  int size;
  
  if(!hasKey(table_name, key))
    return false;

  size = collection[table_name][key].size();
  
  collection[table_name][key].erase(obj);
  if(collection[table_name][key].empty()){
    collection[table_name].erase(key);
  }

  return true;
}

bool RelationInfo::removeAllRelations(const string& table_name, const string& key){
  if(!hasKey(table_name, key))
    return false;
  collection[table_name][key].clear();
  collection[table_name].erase(key);

  return true;
}

bool RelationInfo::removeAllRelations(const string& table_name){
  vector<string> key_list;
  bool result=true;

  getKeyList(table_name, key_list);
  for(int i=0;i<key_list.size();i++){
    result = result && removeAllRelations(table_name, key_list[i]);
  }

  return result;
}

bool RelationInfo::hasRelation(const string& table_name, const string& key, Ptr obj){
  if(!hasKey(table_name, key))
    return false;

  if(collection[table_name][key].find(obj)!=collection[table_name][key].end())
    return true;
  return false;
}

bool RelationInfo::hasKey(const string& table_name, const string& key){
  if(!hasTable(table_name))
    return false;
  if(collection[table_name].find(key)==collection[table_name].end())
    return false;

  return true;
}

int RelationInfo::getMaxSize(const string& table_name){
  if(!hasTable(table_name))
    return 0;
  return collection[table_name].size();
}

int RelationInfo::getNumKeys(const string& table_name){
  if(!hasTable(table_name))
    return 0;

  return collection[table_name].size();
}

int RelationInfo::getNumAnswers(const string& table_name, const string& key){
  if(!hasTable(table_name))
    return 0;
  if(!hasKey(table_name, key))
    return 0;

  return retrieve(table_name, key)->size();
}

int RelationInfo::getTotalCount(const string& table_name){
  vector<string> key_list;
  int result=0;
  
  if(!hasTable(table_name))
    return 0;

  getKeyList(table_name, key_list);
  for(int i=0;i<key_list.size();i++){
    result+=getNumAnswers(table_name, key_list[i]);
  }

  return result;
}

string RelationInfo::getTableType(const string& table_name){
  if(!hasTable(table_name))
    return "";
  return table_type[table_name];
}

void RelationInfo::getTableList(vector<string>& table_list){
  map<string, map<string, multiset<Ptr> > >::iterator it;
  
  table_list.clear();

  for(it=collection.begin();it!=collection.end();it++){
    table_list.push_back((*it).first);
  }
}

void RelationInfo::getKeyList(const string& table_name, vector<string>& key_list){
  map<string, multiset<Ptr> >::iterator it;

  key_list.clear();
  for(it=collection[table_name].begin();it!=collection[table_name].end();it++){
    key_list.push_back((*it).first);
  }
}

Ptr RelationInfo::getFirstAnswer(const string& table_name, const string& key){
  vector<Ptr> ans;

  ans.clear();
  answer(table_name, key, ans);

  if(ans.size()==0)
    return NULL;

  if(table_type[table_name]=="AssociateValue"){
    return ((ObjVal*)ans[0])->value;
  }

  return ans[0];
}

bool RelationInfo::answer(const string& table_name, const string& key, vector<Ptr>& ans){
  multiset<Ptr>::iterator it;
  int i;
  
  ans.clear();

  if(!hasKey(table_name, key))
    return false;

  ans.resize(collection[table_name][key].size());

  i=0;

  if(table_type[table_name]=="Relation"){
    for(it=collection[table_name][key].begin();it!=collection[table_name][key].end();++it){
      ans[i++]=*it;
    }
  }
  else if(table_type[table_name]=="AssociateValue"){
    for(it=collection[table_name][key].begin();it!=collection[table_name][key].end();++it){
      ans[i++]=((ObjVal*)*it)->value;
    }
  }
  else{
    printf("RelationInfo: answer(): Unknown table type\n");
    exit(-1);
  }

  return true;
}

multiset<Ptr>* RelationInfo::retrieve(const string& table_name, const string& key){
  if(!hasKey(table_name, key))
    return NULL;
  return &collection[table_name][key];
}

ObjVal* RelationInfo::makeObjVal(Ptr value, int size){
  ObjVal* obj;
  
  obj = (ObjVal*) malloc(sizeof(ObjVal));
  obj->size = size;
  obj->value = (Ptr) malloc(size);
  memcpy(obj->value, value, size);

  return obj;
}

void RelationInfo::deleteObjVal(ObjVal* obj){
  free(obj->value);
  free(obj);
}
