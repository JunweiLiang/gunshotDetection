#include <deque>
#include "network.h"

Network::Network(MemoryManager* mgr){
  this->mgr = mgr;
  
  hmm_list.clear();
  edge_list.clear();
  penalty_list.clear();
  net2st_start.clear();
  net2st_end.clear();
  st2net.clear();
  net_name.clear();

  null_state = mgr->createState(NULL);
  start=end=-1;
  item_count=0;
}

Network::~Network(){
  mgr->removeState(null_state);
}

void Network::clear(){
  hmm_list.clear();
  edge_list.clear();
  penalty_list.clear();
  state_set.clear();
  net2st_start.clear();
  net2st_end.clear();
  st2net.clear();
  net_name.clear();
  adj_list.clear();
  parent_list.clear();
  word_end.clear();
  start=end=-1;
  item_count=0;
}

void Network::addHMM(AegisHMM* hmm){
  vector<State*> hmm_state_set;
  vector<int> empty;
  int idx = adj_list.size();
 
  this->net_name.push_back(hmm->getName());
  hmm_list.push_back(hmm);
  hmm_list[hmm_list.size()-1]->getStateSet(hmm_state_set);
  for(int i=0;i<hmm_state_set.size();i++){
    adj_list.push_back(empty);
    parent_list.push_back(empty);
  }
  for(int j=0;j<hmm_state_set.size();j++){
    state_set.push_back(hmm_state_set[j]);
    if(j!=hmm_state_set.size()-1)
      word_end.push_back(false);
    else
      word_end.push_back(true);

    st2net[state_set.size()-1] = item_count;
    if(j==0){
      net2st_end[item_count] = state_set.size()-1;
      continue;
    }
    if(j==hmm_state_set.size()-1){
      net2st_start[item_count] = state_set.size()-1;
      continue;
    }
  }

  for(int i=0;i<hmm_state_set.size();i++){
    for(int j=0;j<hmm_state_set.size();j++){
      if(hmm_list[hmm_list.size()-1]->hasEdge(hmm_state_set[i], hmm_state_set[j])){
        adj_list[idx+i].push_back(idx+j);
        parent_list[idx+j].push_back(idx+i);
      }
    }
  }

  item_count++;
}

void Network::addHMM(const string& net_name, vector<AegisHMM*> hmm){
  vector<State*> hmm_state_set;
  vector<int> empty;
  int idx;

  this->net_name.push_back(net_name);
  for(int k=0;k<hmm.size();k++){
    idx = adj_list.size();
    hmm_list.push_back(hmm[k]);
    hmm_list[hmm_list.size()-1]->getStateSet(hmm_state_set);
    for(int i=0;i<hmm_state_set.size();i++){
      adj_list.push_back(empty);
      parent_list.push_back(empty);
    }
    for(int j=0;j<hmm_state_set.size();j++){
      state_set.push_back(hmm_state_set[j]);
      if(j==hmm_state_set.size()-1 && k==hmm.size()-1){
        word_end.push_back(true);
      }
      else
        word_end.push_back(false);

      st2net[state_set.size()-1] = item_count;
      if(j==0 && k==0){
        net2st_end[item_count] = state_set.size()-1;
        continue;
      }
      if(j==hmm_state_set.size()-1 && k==hmm.size()-1){
        net2st_start[item_count] = state_set.size()-1;
        continue;
      }
    }

    if(k>0){
      adj_list[idx-1].push_back(idx);
      parent_list[idx].push_back(idx-1);
    }
    for(int i=0;i<hmm_state_set.size();i++){
      for(int j=0;j<hmm_state_set.size();j++){
        if(hmm_list[hmm_list.size()-1]->hasEdge(hmm_state_set[i], hmm_state_set[j])){
          adj_list[idx+i].push_back(idx+j);
          parent_list[idx+j].push_back(idx+i);
        }
      }
    }
  }

  item_count++;
}


void Network::addNull(){
  vector<int> empty;
  
  hmm_list.push_back(NULL);
  state_set.push_back(null_state);
  adj_list.push_back(empty);
  parent_list.push_back(empty);
  word_end.push_back(false);
  st2net[state_set.size()-1] = item_count;
  net_name.push_back("!NULL");
  net2st_start[item_count] = state_set.size()-1;
  net2st_end[item_count] = state_set.size()-1;
  item_count++;
}
/*
void Network::link(int source, int target){
  link(source, target, 0.0);
}
*/
void Network::linkNetNode(int source, int target, float penalty){
  pair<int, int> tuple;

  tuple.first = source;
  tuple.second = target;

  adj_list[net2st_start[tuple.first]].push_back(net2st_end[tuple.second]);
  parent_list[net2st_end[tuple.second]].push_back(net2st_start[tuple.first]);

//  printf("link: %d, %d %d\n", parent_list[0].size(), source, target);
  
  edge_list.push_back(tuple);
  penalty_list.push_back(penalty);
}


void Network::setStart(int node_idx){
  start = node_idx;
}

void Network::setEnd(int node_idx){
  end = node_idx;
}

int Network::getStart(){
  return start;
}

int Network::getEnd(){
  return end;
}

void Network::getNetNodeSeq(const vector<int>& state_seq, vector<string>& word_seq){
  string word;
  
  word_seq.clear();
  for(int q=0;q<state_seq.size();q++){
    if(isWordEnd(state_seq[q])){
      word_seq.push_back(net_name[st2net[state_seq[q]]]);
    }
  }
}

int Network::getNumStates(){
  return state_set.size();
}

bool Network::hasEdge(int source, int target){
  for(int i=0;i<edge_list.size();i++){
    if(edge_list[i].first==source && edge_list[i].second==target)
      return true;
  }
  return false;
}

void Network::getStateSet(vector<State*>& state_set){
  state_set = this->state_set;
}

void Network::getEdgeSet(vector<Edge*>& edge_set){
  vector<Edge*> hmm_edge_set;

  edge_set.clear();

  for(int i=0;i<hmm_list.size();i++){
    if(hmm_list[i]==NULL)
      continue;
    else{
      hmm_list[i]->getEdgeSet(hmm_edge_set);
      for(int j=0;j<hmm_edge_set.size();j++){
        edge_set.push_back(hmm_edge_set[j]);
      }
    }
  }
}

AegisHMM* Network::getHMM(int index){
  return hmm_list[index];
}

void Network::getUniqHMMSet(vector<AegisHMM*>& result){
  vector<string> id_set;
  bool flag;

  result.clear();
  for(int i=0;i<hmm_list.size();i++){
    if(hmm_list[i]==NULL)
      continue;
    flag = false;
    for(int j=0;j<id_set.size();j++){
      if(hmm_list[i]->getID()==id_set[j]){
        flag = true;
        break;
      }
    }

    if(!flag){
      result.push_back(hmm_list[i]);
      id_set.push_back(hmm_list[i]->getID());
    }
  }
}

void Network::getUniqRealStateSet(vector<State*>& result, vector<int>& idx){
  vector<string> id_set;
  bool flag;

  result.clear();
  idx.clear();
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]==NULL)
      continue;
    flag = false;
    for(int j=0;j<id_set.size();j++){
      if(state_set[i]->getID()==id_set[j]){
        flag = true;
        break;
      }
    }

    if(!flag){
      result.push_back(state_set[i]);
      idx.push_back(i);
      id_set.push_back(state_set[i]->getID());
    }
  }
}

void Network::buildAdjList(vector<vector<int> >& adj_list){
  adj_list = this->adj_list;
}

void Network::buildAdjRealList(vector<vector<int> >& adj_real_list){
  adj_real_list.clear();
  adj_real_list.resize(state_set.size());

  for(int i=0;i<state_set.size();i++){
    for(int j=0;j<adj_list[i].size();j++){
      if(!state_set[adj_list[i][j]]->isNullNode()){
        adj_real_list[i].push_back(adj_list[i][j]);
      }
    }
  }
}

void Network::buildParentList(vector<vector<int> >& parent_list){
  parent_list = this->parent_list;
}

void Network::buildTranTable(map<int, map<int, float> >& log_tran_prob){
  map<int, float> entry;
  int s, t;
  Edge* e;
  map<State*, map<State*, float> > cache;
  map<State*, map<State*, bool> > flag;
  vector<Edge*> edge_set;

  log_tran_prob.clear();
  
  for(int j=0;j<hmm_list.size();j++){
    if(hmm_list[j]==NULL)
      continue;
    hmm_list[j]->getEdgeSet(edge_set);
    for(int k=0;k<edge_set.size();k++){
      e = edge_set[k];
      cache[e->getSource()][e->getTarget()] = log(e->getProb());
      flag[e->getSource()][e->getTarget()] = true;
    }
  }

  for(int s=0;s<state_set.size();s++){
//    printf("s: %d %d\n", s, state_set.size());
    entry.clear();
    for(int t=0;t<adj_list[s].size();t++){
      if(flag[state_set[s]][state_set[adj_list[s][t]]]){
        entry[adj_list[s][t]] = cache[state_set[s]][state_set[adj_list[s][t]]];
      }
    }
    log_tran_prob[s] = entry;
  }

  for(int i=0;i<edge_list.size();i++){
//    printf("ckpt1: %d %d %lf %lf\n", edge_list[i].first, edge_list[i].second, penalty_list[i],log_tran_prob[net2st_start[edge_list[i].first]][net2st_end[edge_list[i].second]]);
    log_tran_prob[net2st_start[edge_list[i].first]][net2st_end[edge_list[i].second]] += penalty_list[i];
  }
}

bool Network::determineBeginEnd(){
  bool s_flag = false, e_flag=false;

  for(int i=0;i<parent_list.size();i++){
    if(parent_list[i].size()==0){
      setStart(i);
      s_flag = true;
      break;
    }
  }
  for(int i=0;i<adj_list.size();i++){
    if(adj_list[i].size()==0){
      setEnd(i);
      e_flag = true;
      break;
    }
  }

  return s_flag && e_flag;
}

bool Network::isWordEnd(int node_idx){
  return word_end[node_idx];
}

void Network::topSort(vector<State*>& set, vector<int>& null_idx){
  vector<bool> visited;
  int i, j, k, indegree, null_amt=0;
  vector<vector<int> > null_adj_list;
  vector<int> order;
  deque<int> queue;

  set.clear();
  null_idx.clear();
  null_adj_list.resize(state_set.size());
  visited.resize(state_set.size());
  order.resize(state_set.size());

  for(i=0;i<state_set.size();i++){
    order[i] = -1;
  }
  
  for(i=0;i<state_set.size();i++){
    if(state_set[i]->isNullNode()){
      null_amt++;
      for(int j=0;j<adj_list[i].size();j++){
        if(state_set[adj_list[i][j]]->isNullNode()){
          null_adj_list[i].push_back(adj_list[i][j]);
        }
      }
      indegree = 0;
      for(int j=0;j<parent_list[i].size();j++){
        if(state_set[parent_list[i][j]]->isNullNode()){
          indegree++;
        }
      }
      if(indegree==0){
        queue.push_back(i);
        order[i] = 0;
        visited[i] = true;
      }
    }
  }

  while(!queue.empty()){
    int node = queue[0];

//    printf("null_amt: %d\n", null_amt);
    
    queue.pop_front();
    set.push_back(state_set[node]);
    null_idx.push_back(node);
    null_amt--;
    for(int j=0;j<null_adj_list[node].size();j++){
      if(visited[null_adj_list[node][j]])
        continue;
      queue.push_back(null_adj_list[node][j]);
      order[null_adj_list[node][j]] = order[node]+1;
      visited[null_adj_list[node][j]] = true;
    }
  }
}
