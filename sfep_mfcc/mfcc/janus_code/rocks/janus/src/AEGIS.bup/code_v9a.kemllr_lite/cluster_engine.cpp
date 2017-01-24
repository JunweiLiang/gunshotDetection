#include "cluster_engine.h"

ClusterEngine::ClusterEngine(int dim){
  data_id.clear();

  this->dim=dim;
  max = 1024;
  num_data=0;
  next_id=1;

  data = new Matrix(max, dim);
  distance_matrix = new Matrix(dim, dim);
  distance_matrix->identity();

  cluster_info.addTable("DataID2Cluster");
  cluster_info.addTable("SimilarConstraint");
  cluster_info.addTable("DataID2Location");

  tree_info = new ClusterTree;
  tree_info->left=NULL;
  tree_info->right=NULL;
  srand(time(NULL));
}

ClusterEngine::~ClusterEngine(){
  delTree(tree_info);

  delete data;
  delete distance_matrix;
}

void ClusterEngine::readDistanceMatrix(const char* filename){
  FILE* fp;
  float tmp;
  int i, j;

  if(distance_matrix!=NULL){
    delete distance_matrix;
    distance_matrix = new Matrix(dim, dim);
  }
  fp = fopen(filename, "r");
  if(fp==NULL){
    printf("Error in opening %s\n", filename);
    exit(-1);
  }

  for(i=0;i<dim;i++){
    for(j=0;j<dim;j++){
      fscanf(fp, "%lf", &tmp);
      distance_matrix->setValue(i, j, tmp);
    }
  }
  fclose(fp);
}

void ClusterEngine::addDataPoint(const string& id, Matrix* vec){
  int i, j, k;
  
  if(num_data+1>max){
    max*=2;
    Matrix* tmp = new Matrix(max, dim);
    for(i=0;i<data->getNumRow();i++){
      for(j=0;j<data->getNumCol();j++){
        (*tmp)[i][j]=(*data)[i][j];
      }
    }
    delete data;
    data = tmp;
  }

  data_id.push_back(id);
  label.push_back("UNKNOWN");
  for(i=0;i<dim;i++)
    (*data)[num_data][i]=*vec[0][i];

  num_data++;
}

void ClusterEngine::addDataPoint(const string& id, float* vec){
  int i, j, k;
  
  if(num_data+1>max){
    max*=2;
    Matrix* tmp = new Matrix(max, dim);
    for(i=0;i<data->getNumRow();i++){
      for(j=0;j<data->getNumCol();j++){
        (*tmp)[i][j]=(*data)[i][j];
      }
    }
    delete data;
    data = tmp;
  }

  data_id.push_back(id);
  label.push_back("UNKNOWN");
  for(i=0;i<dim;i++){
    (*data)[num_data][i]=vec[i];
  }

  num_data++;
}

void ClusterEngine::addSimilarConstraint(const string& id1, const string& id2){
  Ptr tmp;

  if(id1==id2)
    return;
  
  tmp = cluster_info.getFirstAnswer("DataID2Location", id1);
  cluster_info.addRelation("SimilarConstraint", id2, tmp);
  tmp = cluster_info.getFirstAnswer("DataID2Location", id2);
  cluster_info.addRelation("SimilarConstraint", id1, tmp);
}

int ClusterEngine::getNumDataPoints(){
  return num_data;
}

void ClusterEngine::buildIndices(){
  int i;
 
  cluster_info.removeAllRelations("DataID2Cluster");
  cluster_info.removeAllRelations("DataID2Location");

//  printf("Building indices..."); 
  for(i=0;i<data_id.size();i++){
    cluster_info.addRelation("DataID2Cluster", data_id[i], (Ptr)&label[i]);
    cluster_info.addRelation("DataID2Location", data_id[i], (Ptr)&data_id[i]);
  }
//  printf("OK\n");
}

void ClusterEngine::changeLabel(vector<ClusterTree*>& tree_list, vector<string>& cluster_id, vector<string>& label, string l1, string l2){
  string key;
  bool flag=false;
  
  for(int i=0;i<tree_list.size();i++){
    key = "Tree"+int2String(i);
    if(tree_list[i]->label==l1){
      if(!cluster_info.hasKey("Cache", key)){
        tree_list[i]->label=l2;
        cluster_info.setAssociateValue("Cache", key, (Ptr)&flag, sizeof(bool));
      }
    }
  }
  for(int i=0;i<cluster_id.size();i++){
    key = "Cluster"+int2String(i);
    if(cluster_id[i]==l1){
      if(!cluster_info.hasKey("Cache", key)){
        cluster_id[i]=l2;
        cluster_info.setAssociateValue("Cache", key, (Ptr)&flag, sizeof(bool));
      }
    }
  }
  for(int i=0;i<label.size();i++){
    key = "Label"+int2String(i);
    if(label[i]==l1){
      if(!cluster_info.hasKey("Cache", key)){
        label[i]=l2;
        cluster_info.setAssociateValue("Cache", key, (Ptr)&flag, sizeof(bool));
      }
    }
  }
}

int ClusterEngine::getSmallestLabel(ClusterTree* tree){
  int min=-1;
  int left_val, right_val;

  if(tree==NULL)
    return -1;
  
  min = atoi(tree->label.c_str());
  left_val = getSmallestLabel(tree->left);
  right_val = getSmallestLabel(tree->right);

  if(left_val!=-1){
    if(left_val<min)
      min = left_val;
  }
  if(right_val!=-1){
    if(right_val<min)
      min = right_val;
  }
  return min;
}

int ClusterEngine::getLargestLabel(ClusterTree* tree){
  int max=-1;
  int left_val, right_val;

  if(tree==NULL)
    return -1;
  
  max = atoi(tree->label.c_str());
  left_val = getLargestLabel(tree->left);
  right_val = getLargestLabel(tree->right);

  if(left_val!=-1){
    if(left_val>max)
      max = left_val;
  }
  if(right_val!=-1){
    if(right_val>max)
      max = right_val;
  }
  return max;
}

void ClusterEngine::getAllClusterID(ClusterTree* root, vector<int>& cluster_id){
  if(root==NULL)
    return;
  if(root->left==NULL && root->right==NULL)
    cluster_id.push_back(atoi(root->label.c_str()));
  getAllClusterID(root->left, cluster_id);
  getAllClusterID(root->right, cluster_id);
}

int ClusterEngine::getClusterNo(const string& data_id){
  vector<int> cluster_id;
  int target;

  getAllClusterID(tree_info, cluster_id);
  sort(cluster_id.begin(), cluster_id.end());

  target = inCluster(data_id);
  for(int i=0;i<cluster_id.size();i++){
    if(target==cluster_id[i])
      return i;
  }
  return -1;
}

void ClusterEngine::renumberingClusterID(vector<ClusterTree*>& tree_list){
  int min_label, max_label, tmp, order;
  vector<int> swap_info;

  cluster_info.addTable("Cache", "AssociateValue");

  for(int i=0;i<tree_list.size();i++){
    order = 0;
    for(int j=0;j<tree_list.size();j++){
      if(i!=j){
        if(atoi(tree_list[i]->label.c_str()) > atoi(tree_list[j]->label.c_str())){
          order++;
        }
      }
    }
    swap_info.push_back(order);    
  }

  for(int i=0;i<swap_info.size();i++){
    changeLabel(tree_list, cluster_id, label, tree_list[i]->label, int2String(swap_info[i]+1));
  }

  cluster_info.destroyTable("Cache");

  min_label = getSmallestLabel(tree_info);
  max_label = getLargestLabel(tree_info);

  renumberingTree(tree_info, max_label);

  for(int i=0;i<cluster_id.size();i++){
    tmp = atoi(cluster_id[i].c_str());
    cluster_id[i] = int2String(max_label - tmp + 1);
  }
  for(int i=0;i<label.size();i++){
    tmp = atoi(label[i].c_str());
    label[i] = int2String(max_label - tmp + 1);
  }
}

void ClusterEngine::renumberingTree(ClusterTree* tree, int largest){
  int tmp;
  
  if(tree==NULL)
    return;
  tmp = atoi(tree->label.c_str());
  tree->label = int2String(largest - tmp + 1);
  renumberingTree(tree->left, largest);
  renumberingTree(tree->right, largest);
}

void ClusterEngine::hierarchicalClustering(const vector<int>& init_label, int init_num_clusters, int num_children){
  int nearest_idx1, nearest_idx2, tree_idx1, tree_idx2;
  float nearest=-1.0, d;
  vector<string> result, result_id;
  vector<string> key_list;
  vector<ClusterTree*> tree_list;
  ClusterTree* tmp_tree;
  string key;
  char field[STRMAX];

  cluster_info.addTable("Cache", "AssociateValue");

  next_id = init_num_clusters + 1;

  result = label;
  cluster_id.clear();
  tree_list.clear();
  
  for(int i=0;i<init_num_clusters;i++){
    cluster_id.push_back(int2String(i+1));
  }
  for(int i=0;i<label.size();i++){
    label[i] = int2String(init_label[i]);
  }
  
  buildIndices();

  if(getNumClusters()==num_children){
    result = label;
    result_id = cluster_id;
    for(int i=0;i<cluster_id.size();i++){
      tmp_tree = makeLeaf(cluster_id[i]);
      tree_list.push_back(tmp_tree);
    }
  }
 
  while(getNumClusters()>1){
    nearest=-1.0;
    for(int i=0;i<getNumClusters();i++){
      for(int j=i+1;j<getNumClusters();j++){
        key = cluster_id[i]+":"+cluster_id[j];
        if(cluster_info.hasKey("Cache", key))
          cluster_info.getAssociateValue("Cache", key, (Ptr)&d);
        else{
          d = clusterDistance(cluster_id[i], cluster_id[j]);
          cluster_info.setAssociateValue("Cache", key, (Ptr)&d, sizeof(float));
        }

        if(nearest==-1.0 || d < nearest){
          nearest_idx1 = i;
          nearest_idx2 = j;
          nearest = d;
        }
      }
    }
    cluster_id.push_back(int2String(next_id));
    printf("next_id: %d\n", next_id);
    fflush(stdout);

    for(int i=0;i<label.size();i++){
      if(label[i]==cluster_id[nearest_idx1] || label[i]==cluster_id[nearest_idx2])
        label[i] = int2String(next_id);
    }

    if(getNumClusters()-1<=num_children){
      tree_idx1 = -1;
      for(int i=0;i<tree_list.size();i++){
        if(tree_list[i]->label==cluster_id[nearest_idx1]){
          tree_idx1 = i;
          break;
        }
      }
      tree_idx2 = -1;
      for(int i=0;i<tree_list.size();i++){
        if(tree_list[i]->label==cluster_id[nearest_idx2]){
          tree_idx2 = i;
          break;
        }
      }
      tmp_tree = makeLeaf(int2String(next_id));
      tmp_tree->left = tree_list[tree_idx1];
      tmp_tree->right = tree_list[tree_idx2];

      cluster_info.getKeyList("Cache", key_list);
      for(int i=0;i<key_list.size();i++){
        extractField(key_list[i].c_str(), 0, ":", field);
        if(tmp_tree->left->label==string(field) || tmp_tree->right->label==string(field)){
          cluster_info.removeAssociateValue("Cache", key_list[i]);
        }
        extractField(key_list[i].c_str(), 1, ":", field);
        if(tmp_tree->left->label==string(field) || tmp_tree->right->label==string(field)){
          cluster_info.removeAssociateValue("Cache", key_list[i]);
        }
      }

      tree_list.push_back(tmp_tree);
    }

    swap(cluster_id[nearest_idx1], cluster_id[cluster_id.size()-1]);
    cluster_id.pop_back();
    swap(cluster_id[nearest_idx2], cluster_id[cluster_id.size()-1]);
    cluster_id.pop_back();

    if(getNumClusters()==num_children){
      result = label;
      result_id = cluster_id;
      if(tree_list.empty()){
        for(int i=0;i<cluster_id.size();i++){
          tmp_tree = makeLeaf(cluster_id[i]);
          tree_list.push_back(tmp_tree);
        }
      }
    }
    next_id++;
  }

  cluster_info.destroyTable("Cache");

  label = result;
  cluster_id = result_id;

  tree_info = tree_list[tree_list.size()-1];

  renumberingClusterID(tree_list);
}

void ClusterEngine::clustering(int num_clusters, int iteration){
  vector<Matrix> mean;
  int i, j, k;
  int size;
  vector<Ptr> ans;
 
  cluster_id.clear();
  tree_node.clear();
  
  mean.push_back(data->getRowVec(0));
  cluster_id.push_back(int2String(1));
  tree_node.push_back(int2String(1));
  
  tree_info->label= tree_node[0];
  next_id++;
 
  for(i=1;i<=num_clusters;i++){
//    printf("NumClusters: %d\n", i);

    kMeanClustering(mean, i, iteration);

    size=mean.size();

    if(i<num_clusters){
//      printf("splitting...\n");
      mean = splitClusters();

      if(size==mean.size()){
//        printf("Cannot split\n");
        break;
      }
      else
        size = mean.size();
    }

  }
}

vector<Matrix> ClusterEngine::splitClusters(){
  vector<Matrix> mean, var, result;
  vector<bool> flag;
  vector<Ptr> ans;
  float max_var, tmp;
  int max_idx=-1;
  int i, j, k;
  int num_clusters;
  bool code;

  num_clusters = getNumClusters();

  flag.resize(num_clusters);
  mean.resize(num_clusters);
  var.resize(num_clusters);

  for(i=0;i<num_clusters;i++){
    mean[i]=getClusterMean(cluster_id[i]);
    var[i]=getClusterVar(cluster_id[i]);

    flag[i]=true;
  }

  for(i=0;i<num_clusters;i++){
    if(getClusterSize(cluster_id[i])<=THRESHOLD){
      flag[i]=false;
    }
  }

  for(i=0;i<num_clusters;i++){
    if(flag[i]){
      tmp = (var[i].transpose()*var[i])[0][0];
      if(max_idx==-1 || tmp > max_var){
        max_var=tmp;
        max_idx=i;
      }
    }
  }

  for(i=0;i<num_clusters;i++){
    if(i!=max_idx)
      result.push_back(mean[i]);
  }

  if(max_idx==-1){
    return result;
  }
  
  cluster_id.push_back(int2String(next_id++));
  cluster_id.push_back(int2String(next_id++));
  tree_node.push_back(cluster_id[cluster_id.size()-2]);
  tree_node.push_back(cluster_id[cluster_id.size()-1]);

//  printf("parent: %s, child1: %s, child2: %s\n", cluster_id[max_idx].c_str(), tree_node[tree_node.size()-2].c_str(), tree_node[tree_node.size()-1].c_str());
/*
  code=tree_info.addRelation("ClusterTree", cluster_id[max_idx], (Ptr) &tree_node[tree_node.size()-2]);
  code=tree_info.addRelation("ClusterTree", cluster_id[max_idx], (Ptr) &tree_node[tree_node.size()-1]);

  code=tree_info.removeRelation("ClusterTree", cluster_id[max_idx], (Ptr) empty);
  
  code=tree_info.addRelation("ClusterTree", tree_node[tree_node.size()-2], (Ptr)empty);
  code=tree_info.addRelation("ClusterTree", tree_node[tree_node.size()-1], (Ptr)empty);
*/
  addChildren(tree_info, cluster_id[max_idx], tree_node[tree_node.size()-2], tree_node[tree_node.size()-1]);
  
  swap(cluster_id[max_idx], cluster_id[cluster_id.size()-1]);
  cluster_id.pop_back();

  result.push_back(mean[max_idx]+var[max_idx].mul(DELTA));
  result.push_back(mean[max_idx]-var[max_idx].mul(DELTA));

  return result;
}

ClusterTree* ClusterEngine::makeLeaf(const string& label){
  ClusterTree* leaf;

  leaf = new ClusterTree();
  leaf->label = label;
  leaf->left = NULL;
  leaf->right = NULL;

  return leaf;
}

void ClusterEngine::addChildren(ClusterTree* tree_info, const string& id, const string& leftc, const string& rightc){
  if(tree_info==NULL)
    return;
  
  if(tree_info->label==id){
    tree_info->left = new ClusterTree();
    tree_info->left->label=leftc;
    tree_info->left->left=NULL;
    tree_info->left->right=NULL;

    tree_info->right = new ClusterTree();
    tree_info->right->label=rightc;
    tree_info->right->left=NULL;
    tree_info->right->right=NULL;

    return;
  }  
  addChildren(tree_info->left, id, leftc, rightc);
  addChildren(tree_info->right, id, leftc, rightc);
}

void ClusterEngine::delTree(ClusterTree* tree){
  if(tree==NULL)
    return;
  delTree(tree->left);
  delTree(tree->right);
  delete tree;
}

void ClusterEngine::kMeanClustering(vector<Matrix>& mean, int num_clusters, int iteration){
  float min_d, tmp;
  int min_idx;
  int i, j, k;
  bool code;

  for(i=0;i<iteration;i++){
    //printf("iter: %d, num_data: %d\n", i, num_data);
    for(j=0;j<num_data;j++){
      min_idx=-1;
      for(k=0;k<cluster_id.size();k++){
        tmp = distance(mean[k].transpose(), data->getRowVec(j).transpose());
        //printf("k: %d, tmp: %lf\n", k, tmp);
        if(min_idx==-1 || min_d > tmp){
          min_idx=k;
          min_d=tmp;
        }
      }
      label[j]=cluster_id[min_idx];
    }
    for(k=0;k<num_clusters;k++){
      mean[k] = getClusterMean(cluster_id[k]);
    }
  }
}

int ClusterEngine::getNumClusters(){
  return cluster_id.size();
}

ClusterTree* ClusterEngine::getClusterInfo(){
  return tree_info;
}
/*
multiset<Ptr>* ClusterEngine::getCluster(int cluster, bool get_id){
  if(get_id){
    return cluster_info.retrieve("Cluster2DataID", int2String(cluster));
  }
  return cluster_info.retrieve("Cluster2Data", int2String(cluster));
}
*/

int ClusterEngine::inCluster(const string& id){
  string result;
  int i;

  result = *((string*)cluster_info.getFirstAnswer("DataID2Cluster", id));
  
  return atoi(result.c_str());
}

float ClusterEngine::distance(const Matrix& x, const Matrix& y){
  float result;
/*
  distance_matrix->print();
  x.print();
  y.print();
*/
  result = ((x-y).transpose()*(*distance_matrix)*(x-y))[0][0];

  return result;
}

float ClusterEngine::clusterDistance(const string& cluster_id1, const string& cluster_id2){
  Matrix m1, m2, d;
  float result;
/*
  m1 = getClusterMean(cluster_id1);
  m2 = getClusterMean(cluster_id2);
*/
  d = getClusterMean(cluster_id1) - getClusterMean(cluster_id2);

  //result = distance(m1.transpose(), m2.transpose());
  result = (d.transpose()*d)[0][0];

  return result;
}

int ClusterEngine::getClusterSize(const string& cluster_id_str){
  int i, size=0;

  for(i=0;i<label.size();i++){
    if(label[i]==cluster_id_str)
      size++;
  }
  return size;
}

Matrix ClusterEngine::getClusterMean(const string& cluster_id){
  Matrix mean(1, dim);
  int i, j, count=0;

  for(i=0;i<label.size();i++){
    if(label[i]!=cluster_id)
      continue;
    for(j=0;j<dim;j++){
      mean[0][j]+=(*data)[i][j];
    }
    count++;
  }
  
  if(count==0){
    printf("Error in getClusterMean %s\n", cluster_id.c_str());
    exit(-1);
  }
  
  for(j=0;j<dim;j++){
    mean[0][j]/=count;
  }

  return mean;
}

Matrix ClusterEngine::getClusterVar(const string& cluster_id_str){
  vector<Ptr> ans;
  Matrix var(1, dim);
  Matrix mean(1, dim);
  Matrix tmp(1, dim);
  int i, j;

  for(i=0;i<label.size();i++){
    if(label[i]==cluster_id_str)
      ans.push_back((Ptr)((*data)[i]));
  }

  if(ans.size()==0){
    printf("Error in getClusterMean %s\n", cluster_id_str.c_str());
    exit(-1);
  }

  mean = getClusterMean(cluster_id_str);

  for(i=0;i<ans.size();i++){
    for(j=0;j<dim;j++)
      tmp[0][j] = ((float*)ans[i])[j];
    for(j=0;j<dim;j++)
      var[0][j] = (tmp[0][j] - mean[0][j])*(tmp[0][j] - mean[0][j]);
    var = var.mul(1.0/ans.size());
  }

  return var;
}

string ClusterEngine::getClusterID(int index){
  return cluster_id[index];
}

string ClusterEngine::int2String(int num){
  char buf[STRMAX];

  sprintf(buf, "%d", num);

  return string(buf);
}
