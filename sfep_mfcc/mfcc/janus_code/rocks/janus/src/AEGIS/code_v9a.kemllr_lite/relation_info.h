#ifndef RELATION_INFO_H
#define RELATION_INFO_H

#include <stdio.h>
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace std;

#define Ptr char*
#define MULTI_SET_IT multiset<Ptr>::iterator

struct ObjValType{
  Ptr value;
  int size;
};
typedef struct ObjValType ObjVal;

class RelationInfo{
  public:
    RelationInfo();

    void destroy();
    void clear();

    bool addTable(const string& table_name);
    bool addTable(const string& table_name, const string& type);
    bool removeTable(const string& table_name);
    bool destroyTable(const string& table_name);
    bool hasTable(const string& table_name);

    bool setAssociateValue(const string& table_name, const string& key, Ptr value, int size);
    bool getAssociateValue(const string& table_name, const string& key, Ptr value);
    bool removeAssociateValue(const string& table_name, const string& key);
    
    bool addRelation(const string& table_name, const string& key, Ptr obj);
    bool removeRelation(const string& table_name, const string& key, Ptr obj);
    bool removeAllRelations(const string& table_name, const string& key);
    bool removeAllRelations(const string& table_name);
    bool hasRelation(const string& table_name, const string& key, Ptr obj);
    
    bool hasKey(const string& table_name, const string& key);

    int getNumKeys(const string& table_name);
    int getNumAnswers(const string& table_name, const string& key);
    int getTotalCount(const string& table_name);
    int getMaxSize(const string& table_name);
    string getTableType(const string& table_name);

    void getTableList(vector<string>& table_list);
    void getKeyList(const string& table_name, vector<string>& key_list);

    Ptr getFirstAnswer(const string& table_name, const string& key);
    bool answer(const string& table_name, const string& key, vector<Ptr>& answer);
    multiset<Ptr>* retrieve(const string& table_name, const string& key);

  protected:
    ObjVal* makeObjVal(Ptr value, int size);
    void deleteObjVal(ObjVal* obj);

  private:
    map<string, map<string, multiset<Ptr> > > collection;
    map<string, string> table_type;

    map<string, multiset<Ptr> > empty_table;
    multiset<Ptr> empty_data;
};

#endif
