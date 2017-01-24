#ifndef DIRICHLET_H
#define DIRICHLET_H

#include <vector>
#include <cstdio>
#include <algorithm>

class Dirichlet
{
private:
   double* alpha;
   double* adapt_alpha;
   double* gamma;
   double* expect_log_theta;
   long size;
   double K;
   int parent;
   int parent_branch; // [0,B-1]
   int id;
   int* child;

   double* branch_prob;

   // accumulators for updating alpha. will accumulate over all training document
   double psg;
   double* spg;

   // accumulators for method of moments for initializing alphas
   double* sum_map_p;
   double* sum_map_p_sqr;

   // branch -> topic (applies to leaf nodes)
   int* topic;

protected:
   void grad_ascent(int iter, double M, double learn);
   void moment(int M);
   
public:
   Dirichlet(const double* _alpha, long _size);
   ~Dirichlet();
   
   double get_alpha(int i) const { return alpha[i]; }
   
   double log_prob(const double* x, bool arg_in_log = false);
   double* expect_ltheta();

   void update_K();

   void print() const;

   void init_flat();

   friend class DirichletTree;
   friend class DirichletNet;
};

struct BranchStruct
{
   int node;
   int branch;

   BranchStruct() : node(-1), branch(-1) {}
   BranchStruct(const BranchStruct& s) : node(s.node), branch(s.branch) {}
   ~BranchStruct() {}
};

struct PathStruct
{
   BranchStruct* br;
   int size;

   PathStruct() : br(0), size(0) {}
   ~PathStruct() { delete [] br; }
};

class Prior
{
protected:
   bool is_created;
   bool is_updated;

public:
   Prior() : is_created(false), is_updated(false) {}
   virtual ~Prior() {}

   virtual void accu_gamma(double* topic_count) = 0;
   virtual void update_gamma(double forget) = 0;
   virtual void subtract_alpha(double forget) = 0;
   virtual void clear_gamma() = 0;
   virtual void get_prior(double* ap, int num_topic, bool norm=true) = 0;
   virtual void create_accu() = 0;
   virtual void clear_psi() = 0;
   virtual void accu_psi(double wgt = 1.0) = 0;
   virtual void reset() = 0;
   virtual void write_alpha(FILE *ap) = 0;
   virtual void read_alpha(FILE *ap) = 0;
   virtual double* MAP_prob() = 0;
   virtual void estimate_alpha(int iter, double learn) = 0;
   virtual void update_branch_prob() = 0;
   virtual void adapt_alpha() = 0;
   virtual void write_accum(FILE *ap) = 0;
   virtual void add_accum(FILE *ap) = 0;
   virtual double var_ll() const = 0;
   virtual void init_flat() = 0;
};

class DirichletTree : public Prior
{
public:
   typedef std::vector<BranchStruct> path;

private:
// compulsory for janus module: must start as the 1st and 2nd variable
   char* name;
   int useN;
// end

   //typedef struct PathStruct path;

   int num_leaf;
   int branch;

   //std::vector<Dirichlet*> node;
   Dirichlet** node;

   // <leaf node,branch> idx for each topic
   //std::vector<std::pair<int,int> > topic2leaf;
   BranchStruct* topic2leaf;

   path* topic_path;
   int num_topic;

   double* map_prob;

   // number of training documents
   double M;

   int num_node;

protected:
   void trace_path();

   double recur_accu_gamma(Dirichlet* cur_node, double* topic_count);

public:
   DirichletTree() {}

   DirichletTree(int _num_leaf, int _branch=2, const double* topic_alpha=0);

   // build dirichlettree based on a standard LDA model
   //DirichletTree(double* alpha, double** beta, int V, int K);

   //DirichletTree(int _num_leaf, int _branch=2);
   ~DirichletTree();

   // # of topics K (accumulate q in the E-step)
   void accu_gamma(double* topic_count, int K);

   //void accu_gamma(double* topic_count);
   void accu_gamma(double* topic_count);

   void create_accu();
   // for alpha update later
   void accu_psi(double wgt = 1.0);

   // reestimate gamma values for each node
   void update_gamma(double forget = 1.0);

   // remove the effect from alpha
   void subtract_alpha(double forget = 1.0);

   void clear_gamma();
   void clear_psi();

   // get exp{Eq[log theta_k]} in ap
   // if (norm==false), then we only compute Eq[log theta_k]
   void get_prior(double* ap, int K, bool norm=true);

   // compute MAP topic prob based on the posterior counts
   double* MAP_prob();

   // gradient ascent over the alpha parameters of each node
   void estimate_alpha(int iter, double learn);

   void write_alpha(FILE *ap);
   void read_alpha(FILE *ap);

   // reset adapt_alpha to background alpha
   void reset();
   void update_branch_prob();
   void adapt_alpha();

   void write_accum(FILE *ap);
   void add_accum(FILE *ap);

   // different method to construct the dirichlet tree (simpler one)
   void build(int _num_leaf, int _branch);

   // compute variational likelihood bound (partial term)
   double var_ll() const;

   int get_num_leaf() const { return num_leaf; }

   void print() const;

   void init_flat();

// Mar 25 08: copy gammas between dirichlet tree
   void copy_gamma(const DirichletTree* src_tree);

   void copy_alpha(const DirichletTree* src_tree);

   path* get_topic_path() { return topic_path; }

   void merge_node(int k1, int k2);

   int get_num_topic() const { return num_topic; }
};

class DirichletNet : public Prior
{
private:
   int K;

   // 2-d matrix to Dirichlet*
   int depth;
   Dirichlet*** dir_net;
   int*         num_node;

   double** alpha;
   double** beta;

   double* posterior;

   // variational posterior prob at the fully-connected layer
   double** Q;

   // number of training documents
   double M;

protected:
   void forward();
   void MAP_forward();
   void backward(double* topic_count);

public:
   DirichletNet(int num_leaf);
   ~DirichletNet();

   void accu_gamma(double* topic_count);
   void update_gamma(double forget);
   void subtract_alpha(double forget) {}
   void clear_gamma();
   void get_prior(double* ap, int num_topic, bool norm=true);
   void create_accu();
   void clear_psi();
   void accu_psi(double wgt = 1.0);
   void reset();
   void write_alpha(FILE *ap);
   void read_alpha(FILE *ap);
   double* MAP_prob();
   void estimate_alpha(int iter, double learn);
   void update_branch_prob();
   void adapt_alpha();
   void write_accum(FILE *ap);
   void add_accum(FILE *ap);
   double var_ll() const;

   void init_flat() {}
};

#endif
