#ifndef CLUSTER_ENGINE_H
#define CLUSTER_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include "memmgr.h"
#include "relation_info.h"
#include "linear_algebra.h"
#include "str_util.h"

#define DELTA 0.001
#define THRESHOLD 0

using namespace std;

struct ClusterTreeType{
  string label;
  struct ClusterTreeType* left;
  struct ClusterTreeType* right;
};
typedef struct ClusterTreeType ClusterTree;

class ClusterEngine{
  public:
    ClusterEngine(int dim);
    ~ClusterEngine();

    void readDistanceMatrix(const char* filename);

    void addDataPoint(const string& data_id, Matrix* vec);
    void addDataPoint(const string& data_id, float* vec);
    void addSimilarConstraint(const string& id1, const string& id2);
    int getNumDataPoints();

    void buildIndices();

    void hierarchicalClustering(const vector<int>& init_label, int init_num_clusters, int num_children);
    void clustering(int num_clusters, int iteration);
    void kMeanClustering(vector<Matrix>& mean, int num_clusters, int iteration);
    vector<Matrix> splitClusters();

    int getNumClusters();
    //multiset<Ptr>* getCluster(int cluster, bool get_id);
    int inCluster(const string& data_id);
    int getClusterNo(const string& data_id);
    void getAllClusterID(ClusterTree* root, vector<int>& cluster_id);

    Matrix getClusterMean(const string& cluster_id);
    Matrix getClusterVar(const string& cluster_id);
    int getClusterSize(const string& cluster_id);
    string getClusterID(int index);

    void renumberingClusterID(vector<ClusterTree*>& tree_list);
    void renumberingTree(ClusterTree* tree, int largest);
    void changeLabel(vector<ClusterTree*>& tree_list, vector<string>& cluster_id, vector<string>& label, string l_from, string l_to);
    ClusterTree* getClusterInfo();

    ClusterTree* makeLeaf(const string& label);
    void addChildren(ClusterTree* tree_info, const string& id, const string& left, const string& right);
    void delTree(ClusterTree* tree_info);
    int getSmallestLabel(ClusterTree* tree);
    int getLargestLabel(ClusterTree* tree);

    float clusterDistance(const string& cluster_id1, const string& cluster_id2);
    float distance(const Matrix& x, const Matrix& y);

    string int2String(int num);
  private:

    RelationInfo cluster_info;
    vector<string> data_id;
    vector<string> cluster_id;
    vector<string> tree_node;
    ClusterTree* tree_info;

    vector<string> label;

    Matrix* data;
    Matrix* distance_matrix;

    int next_id;
    int dim;
    int num_data;
    int max;
};

#endif
