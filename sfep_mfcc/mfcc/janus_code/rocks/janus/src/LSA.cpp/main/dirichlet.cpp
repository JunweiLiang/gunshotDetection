#include <numeric>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cassert>
#include "gamma.h"
#include "dirichlet.h"
#include "util.h"
#include "reader.h"
#include "writer.h"
#include "dmatrix.h"

using namespace std;

#define RANDOM ((double)rand()/(double)RAND_MAX)

#define SMALL_CNT 1.0e-5

Dirichlet::Dirichlet(const double* _alpha, long _size) : size(_size)
{
   alpha = new double[size];
   adapt_alpha = new double[size];
   gamma = new double[size];
   expect_log_theta = new double[size];
   branch_prob = new double[size];
   
   double sum_alpha = 0;

   parent = -1;
   id = -1;
   child = new int[size];
   
   K = 0;
   for (int i=0; i < _size; i++) {
      double alpha_i = 1.0 + RANDOM;
      //double alpha_i = RANDOM;

      if (_alpha)
         alpha_i = _alpha[i];
      
      alpha[i] = alpha_i;
      adapt_alpha[i] = alpha[i];

      sum_alpha += alpha_i;
      
      K -= lgamma(alpha_i);

      child[i] = -1;
      gamma[i] = 0.0;
   }
   
   K += lgamma(sum_alpha);

   psg = 0.0;
   spg = 0;

   sum_map_p = 0;
   sum_map_p_sqr = 0;

   // update branch prob
   copy_array(alpha, branch_prob, size);
   normalize(branch_prob, size);

   topic = 0;
}

void Dirichlet::init_flat()
{
   double sum_alpha = 0;

   K = 0;
   for (int i=0; i < size; i++) {
      double alpha_i = 1.0 + RANDOM;

      alpha[i] = alpha_i;
      adapt_alpha[i] = alpha[i];

      sum_alpha += alpha_i;
      
      K -= lgamma(alpha_i);

      gamma[i] = 0.0;
   }
   
   K += lgamma(sum_alpha);

   // update branch prob
   copy_array(alpha, branch_prob, size);
   normalize(branch_prob, size);
}

void Dirichlet::print() const
{
   for (int i=0; i < size; i++) {
      cout << alpha[i] << " ";
   }
   cout << endl;
}

void Dirichlet::update_K()
{
   double sum_alpha = 0.0;

   K = 0;
   for (int i=0; i < size; i++) {
      sum_alpha += alpha[i];
      
      K -= lgamma(alpha[i]);
   }
   
   K += lgamma(sum_alpha);
}

double* Dirichlet::expect_ltheta()
{
   // compute new phi_t
   const double sum_gamma = accumulate(gamma, gamma+size, 0.0);
   const double digamma_sum_gamma = psi(sum_gamma);
   
   // precompute Eq[log theta_z_jk]
   //cout << "expect_log_theta " << id << " ";
   for (int j=0; j < size; j++) {
      expect_log_theta[j] = psi(gamma[j]) - digamma_sum_gamma;
      //cout << expect_log_theta[j] << " ";
   }
   //cout << endl;
   
   return expect_log_theta;
}

Dirichlet::~Dirichlet()
{
   delete [] alpha;
   delete [] adapt_alpha;
   delete [] expect_log_theta;
   delete [] child;
   delete [] branch_prob;
   delete [] gamma;

   if (spg) delete [] spg;

   if (sum_map_p) delete [] sum_map_p;
   if (sum_map_p_sqr) delete [] sum_map_p_sqr;

   if (topic) delete [] topic;
}

double Dirichlet::log_prob(const double* x, bool arg_in_log)
{
   double value = K;
   
   for (int i=0; i < size; i++) {
      double alpha_i = alpha[i];
      
      if (arg_in_log)
         value += (alpha_i-1.0) * x[i];
      else
         value += (alpha_i-1.0) * log(x[i]);
   }
   
   return value;
}

// method of moment to obtain initial estimates of alpha
void Dirichlet::moment(int M)
{
   // compute variance
   double* emp_mean = new double[size];
   double* emp_sec_moment = new double[size];
   double* est_sum_alpha = new double[size];

//   const double sum_alpha = accumulate(alpha, alpha+size, 0.0);

   for (int k=0; k < size; k++) {
      emp_mean[k] = sum_map_p[k] / M;
      emp_sec_moment[k] = sum_map_p_sqr[k] / M;

      est_sum_alpha[k] = (emp_mean[k] - emp_sec_moment[k]) / (emp_sec_moment[k] - emp_mean[k]*emp_mean[k]);
   }

   // average the log results to obtain the estimated sum of alphas
   double result = 0.0;
   for (int k=0; k < size; k++) {
      //result += log(est_sum_alpha[k]);
      result += est_sum_alpha[k];
   }
   result /= size;
   //result = exp(result);

   cout << result << endl;
 
   // update alpha
   for (int k=0; k < size; k++) {
      cout << k << " " << alpha[k] << " -> ";
      alpha[k] = emp_mean[k] * result;
      cout << alpha[k] << endl;
   }

   delete [] emp_mean;
   delete [] emp_sec_moment;
   delete [] est_sum_alpha;
}

void Dirichlet::grad_ascent(int iter, double M, double learn)
{
   // run moment estimation to get good initial estimates
//   this->moment(M);

   if (M == 0.0) {
      //cerr << "no training samples found => no dirtree model update" << endl;
      return;
   }

   const int nclass = size;

   double* lalpha = new double[nclass];

   // perform parameter transformation
   copy_array(alpha, lalpha, nclass);
   natural_log(lalpha, nclass);

   int i, t;
   double *g, *pg, *palpha;
   double alpha0, palpha0;
   
   /* allocate arrays */
   if ((g = new double[nclass]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate g.\n");
      return;
   }
   if ((pg = new double[nclass]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate pg.\n");
      return;
   }
   if ((palpha = new double[nclass]) == NULL) {
      fprintf(stderr, "ascent:: cannot allocate palpha.\n");
      return;
   }

   for (i = 0; i < nclass; i++) {
      pg[i] = spg[i] - psg;
      //printf("pg[%d] = %f\n", i, pg[i]);
   }
   
   /* main iteration */
   for (t = 0; t < iter; t++) {
      for (i = 0, alpha0 = 0; i < nclass; i++)
         alpha0 += alpha[i];
      palpha0 = psi(alpha0);
      
      // compute gradient[k] over alpha_k
      for (i = 0; i < nclass; i++) {
         g[i] = (M * (palpha0 - psi(alpha[i])) + pg[i]) * alpha[i];
         //printf("g[%d] = %f\n", i, g[i]);
      }

      // determine learning rate
      const double cur_learn = (1.0 - double(t)/double(iter)) * learn / M;
      //printf("learn = %f\n", cur_learn);

      // gradient ascent
      for (i = 0; i < nclass; i++)
         lalpha[i] += cur_learn * g[i];

      // inverse transform alpha
      for (i = 0; i < nclass; i++) {
         //printf("node %d, b=%d: %f->", id, i, alpha[i]);
         alpha[i] = exp(lalpha[i]);
         //printf("%f\n", alpha[i]);
      }
      
      if ((t > 0) && converged(alpha, palpha, nclass, 1.0e-4))
         break;
      else
         copy_array(alpha, palpha, nclass);
   }

/*
   if (t == iter)
      printf("alpha is not converged after %d iterations\n", iter);
*/

   copy_array(alpha, adapt_alpha, nclass);

// yct 8/15/06
   this->update_K();

   for (i = 0; i < nclass; i++) {
      //assert(alpha[i] > 0.0);
      if (alpha[i] <= 0.0) {
         alpha[i] = 1.0e-10;
      }
   }

   delete [] g;
   delete [] pg;
   delete [] palpha;
   delete [] lalpha;
}

// build the tree from bottom-up with simpler structure (to avoid too restrictive but still allow
// certain degree of topic-correlation to be modelled)
void DirichletTree::build(int _num_leaf, int _branch)
{
   srand(time(NULL));

   num_leaf = _num_leaf;
   branch = _branch;

   // determine how many dirichets needed at the leaf level ceil(num_leaf / branch)
   // then just need one more dirichlet to connect all the leaf-level dirichlets
   if ((num_leaf%branch) == 1) {
      cerr << "can't build tree coz num topic%branch == 1" << endl;
      return;
   }

   if (num_leaf == branch) {
      cerr << "DirichletTree.build(): degenerate into single dirichlet " << endl;

      // degenerate to the LDA model (single dirichlet)
      num_node = 1;
      node = new Dirichlet*[num_node];

      Dirichlet* cur_node = new Dirichlet(0, branch);
      cur_node->id = 0;
      cur_node->parent = -1;
      cur_node->parent_branch = -1;
      cur_node->topic = new int[branch];
      node[0] = cur_node;

      topic2leaf = new BranchStruct[num_leaf];

      for (int b=0; b < branch; b++) {
         cur_node->topic[b] = b;
         topic2leaf[b].node = 0;
         topic2leaf[b].branch = b;
      }
   } else {
      num_node = (int) ceil((double)num_leaf / branch) + 1;
   
      node = new Dirichlet*[num_node];
   
      // index 0 is the root dirichlet
      node[0] = new Dirichlet(0, num_node-1);
      Dirichlet* root_node = node[0];
      root_node->id = 0;
   
      topic2leaf = new BranchStruct[num_leaf];
   
      // build the leaf dirichlets
      int k = 0;
      for (int j=1; j < num_node; j++) {
         const int fanout = ((k+branch) <= num_leaf)? branch : (num_leaf - k);
   
         //cout << j << " " << branch << " " << k << endl;
   
         Dirichlet* cur_node = new Dirichlet(0, fanout);
         cur_node->id = j;
         cur_node->parent = 0;
         cur_node->parent_branch = j-1;
         cur_node->topic = new int[fanout];
   
         for (int b=0; b < fanout; b++, k++) {
            cur_node->topic[b] = k;
            topic2leaf[k].node = j;
            topic2leaf[k].branch = b;
         }
   
         node[j] = cur_node;
   
         // connect root node's child to j
         root_node->child[j-1] = j;
      }
   
      //cout << "k = " << k << endl;
   
      // sanity check
      assert(k == num_leaf);
   }

   map_prob = new double[num_leaf];

   // init path leading to each topic k
   topic_path = new path[num_leaf];
   this->num_topic = num_leaf;
   this->trace_path();

   M = 0.0;

   cerr << "DirichletTree.build() created with # node = " << this->num_node << endl;
}

DirichletTree::DirichletTree(int _num_leaf, int _branch, const double* topic_alpha) : name(0), useN(0)
{
   num_leaf = _num_leaf;
   num_node = _num_leaf - 1;
   branch = _branch;
   M = 0.0;

   //srand(time(NULL));

   if (num_leaf <= 1) {
      cerr << "# of leaf should be greater than 1. empty tree is created" << endl;
      node = 0;
      num_node = 0;

      return;
   }

/*
   if (branch != 2) {
      cerr << "# of branches should be 2" << endl;
      exit(-1);
   }
*/

   if (num_leaf == branch) {
      cerr << "DirichletTree.build(): degenerate into single dirichlet " << endl;

      // degenerate to the LDA model (single dirichlet)
      num_node = 1;
      node = new Dirichlet*[num_node];

      Dirichlet* cur_node = new Dirichlet(0, branch);
      cur_node->id = 0;
      cur_node->parent = -1;
      cur_node->parent_branch = -1;
      cur_node->topic = new int[branch];
      node[0] = cur_node;

      topic2leaf = new BranchStruct[num_leaf];

      for (int b=0; b < branch; b++) {
         cur_node->topic[b] = b;
         topic2leaf[b].node = 0;
         topic2leaf[b].branch = b;
      }
   } else {
      num_node = _num_leaf - 1;
   
      node = new Dirichlet*[num_node];
   
      // build the tree structure
      // build the root node
      //node.push_back(new Dirichlet(0, branch));
      node[0] = new Dirichlet(0, branch);
      node[0]->id = 0;
      int num_cover = branch;
   
      int next = 0;
      int cur = 0;
      int j = 1;
   
      while (num_cover < num_leaf) {
         for (int c=0; c < branch; c++, j++) {
            Dirichlet* child_node = new Dirichlet(0, branch);
            next++;
   
            child_node->id = next;
            child_node->parent = cur;
            child_node->parent_branch = c;
            
            node[cur]->child[c] = next;
   
            if (j >= num_node) {
               cerr << "# node exceeded the limit!" << endl;
               exit(-1);
            }
   
            //node.push_back(child_node);
            node[j] = child_node;
   
            num_cover += (branch-1);
   
            if (num_cover >= num_leaf) break;
         }
         cur++;
      }
   
      topic2leaf = new BranchStruct[num_leaf];
   
      // get the leaf node idx
      int k = 0;
      for (int j=0; j < (int) this->num_node; j++) {
         int* topic = new int[node[j]->size];
         node[j]->topic = topic;
   
         // a map from node id to topic id
         fill(topic, topic + node[j]->size, -1);
   
         for (int b=0; b < node[j]->size; b++) {
            if (node[j]->child[b] == -1) {
               //topic2leaf.push_back(pair<int,int>(j,b));
   
               topic[b] = k;
   
               topic2leaf[k].node = j;
               topic2leaf[k].branch = b;
   
               //cout << k << "(" << j << "," << b << ")" << endl;
               k++;
            }
         }
      }

      // sanity check
      assert(k == num_leaf);
   }

   map_prob = new double[num_leaf];

   // init path leading to each topic k
   topic_path = new path[num_leaf];
   this->num_topic = num_leaf;
   this->trace_path();

   // re-init alpha of each node if topic_alpha is provided
   if (topic_alpha) {
      // clear alpha values
      for (int j=0; j < (int) this->num_node; j++) {
         Dirichlet* cur_node = node[j];

         for (int k=0; k < branch; k++)
            cur_node->alpha[k] = 0.0;
      }

      // recompute alpha values
      for (int k=0; k < num_leaf; k++) {
         const path& p = topic_path[k];
   
         for (int j=0; j < (int) p.size(); j++) {
            Dirichlet* cur_node = node[p[j].node];
            const int b = p[j].branch;
   
            cur_node->alpha[b] += topic_alpha[k];
         }
      }
   }

   cerr << "DirichletTree created with # node = " << this->num_node << endl;
   //print();
}

void DirichletTree::clear_gamma()
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      for (int k=0; k < cur_node->size; k++)
         cur_node->gamma[k] = 0.0;
   }
}

void DirichletTree::create_accu()
{
   if (is_created) return;

   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      cur_node->spg = new double[cur_node->size];

      // accumulators for method of moments for initializing alphas
      cur_node->sum_map_p = new double[cur_node->size];
      cur_node->sum_map_p_sqr = new double[cur_node->size];

      for (int k=0; k < cur_node->size; k++) {
         cur_node->spg[k] = 0.0;
         cur_node->sum_map_p[k] = 0.0;
         cur_node->sum_map_p_sqr[k] = 0.0;
      }

      cur_node->psg = 0.0;
   }

   M = 0.0;

   is_created = true;
}

void DirichletTree::clear_psi()
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      for (int k=0; k < cur_node->size; k++) {
         cur_node->spg[k] = 0.0;
         cur_node->sum_map_p[k] = 0.0;
         cur_node->sum_map_p_sqr[k] = 0.0;
      }

      cur_node->psg = 0.0;
   }

   M = 0.0;

   is_updated = false;
}

void DirichletTree::accu_psi(double wgt)
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      double* cur_gamma = cur_node->gamma;

      // accumulate sufficient statistics for alpha update
      cur_node->psg += wgt * psi(accumulate(cur_gamma, cur_gamma+cur_node->size, 0.0));

      for (int k = 0; k < cur_node->size; k++) {
         cur_node->spg[k] += wgt * psi(cur_gamma[k]);

         // for method of moments
         cur_node->sum_map_p[k] += cur_node->branch_prob[k];
         cur_node->sum_map_p_sqr[k] += cur_node->branch_prob[k] * cur_node->branch_prob[k];
      }
   }

   //M++;
   M += wgt;
}

void DirichletTree::update_branch_prob()
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      // update (posterior-mean) branch prob
      copy_array(cur_node->gamma, cur_node->branch_prob, cur_node->size);
      normalize(cur_node->branch_prob, cur_node->size);

      //cout << "gamma " << j << " " << cur_node->gamma[0] << " " << cur_node->gamma[1] << endl;
      //cout << "branch " << j << " " << cur_node->branch_prob[0] << " " << cur_node->branch_prob[1] << endl;
   }
}

void DirichletTree::adapt_alpha()
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      copy_array(cur_node->gamma, cur_node->adapt_alpha, cur_node->size);
   }
}

void DirichletTree::subtract_alpha(double forget)
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      double* cur_alpha = cur_node->adapt_alpha;

      // compute new gamma (gamma_{jk})
      for (int k=0; k < cur_node->size; k++) {
         cur_node->gamma[k] -= cur_alpha[k] * forget;

         //printf("gamma[%d] = %f\n", k, cur_node->gamma[k]);

/*
         // floor the posterior count to avoid underflow problem
         if (cur_node->gamma[k] < SMALL_CNT) {
            cur_node->gamma[k] = SMALL_CNT;
         }
*/
      }

      cur_node->expect_ltheta();

/*
      // update (MAP) branch prob
      copy_array(cur_node->gamma, cur_node->branch_prob, branch);
      normalize(cur_node->branch_prob, branch);
*/
   }

   // compute MAP prob
   //this->MAP_prob();
}

void DirichletTree::update_gamma(double forget)
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      double* cur_alpha = cur_node->adapt_alpha;

      // compute new gamma (gamma_{jk})
      for (int k=0; k < cur_node->size; k++) {
         cur_node->gamma[k] += cur_alpha[k] * forget;

         //printf("gamma[%d] = %f\n", k, cur_node->gamma[k]);

/*
         // floor the posterior count to avoid underflow problem
         if (cur_node->gamma[k] < SMALL_CNT) {
            cur_node->gamma[k] = SMALL_CNT;
         }
*/
      }

      cur_node->expect_ltheta();

/*
      // update (MAP) branch prob
      copy_array(cur_node->gamma, cur_node->branch_prob, branch);
      normalize(cur_node->branch_prob, branch);
*/
   }

   // compute MAP prob
   //this->MAP_prob();
}

void DirichletTree::accu_gamma(double* topic_count)
{
   this->recur_accu_gamma(node[0], topic_count);

   //cout << total << " " << accumulate(topic_count, topic_count + K, 0.0) << endl;
}

// recursively accu gamma
double DirichletTree::recur_accu_gamma(Dirichlet* cur_node, double* topic_count)
{
   const int* child = cur_node->child;
   double total = 0.0;

   // accu the subtrees under the current node
   for (int b=0; b < cur_node->size; b++) {
      if (child[b] == -1) {
         // we arrive at the leave node. use the topic_count corresponding to that node
         const int k = cur_node->topic[b];

         cur_node->gamma[b] += topic_count[k];

         total += topic_count[k];
      } else {
         const double count_b = this->recur_accu_gamma(node[child[b]], topic_count);
   
         cur_node->gamma[b] += count_b;
   
         total += count_b;
      }
   }

   return total;
}

void DirichletTree::accu_gamma(double* topic_count, int K)
{
   for (int k=0; k < K; k++) {
      const path& p = topic_path[k];

      for (int j=0; j < (int) p.size(); j++) {
         Dirichlet* cur_node = node[p[j].node];
         const int b = p[j].branch;

         cur_node->gamma[b] += topic_count[k];
      }
   }

/*
   // sanity check Sum gamma of root node = some topic count
   double sum_input = accumulate(topic_count, topic_count+K, 0.0);
   double sum_root = accumulate(node[0]->gamma, node[0]->gamma + branch, 0.0);

   cout << "sum_input " << sum_input << " sum_root " << sum_root << endl;
*/
}

DirichletTree::~DirichletTree()
{
   for (int j=0; j < (int) this->num_node; j++) {
      delete node[j];
   }

   delete [] node;
   delete [] topic2leaf;
   delete [] topic_path;
   delete [] map_prob;
}

void DirichletTree::trace_path()
{
   for (int k=0; k < num_leaf; k++) {
      //int leaf_idx = topic2leaf[k].first;
      //int branch_idx = topic2leaf[k].second;
      int leaf_idx = topic2leaf[k].node;
      int branch_idx = topic2leaf[k].branch;

      Dirichlet* cur_node = node[leaf_idx];

      path path_k;
      BranchStruct bstruct;

      bstruct.node = leaf_idx;
      bstruct.branch = branch_idx;

      path_k.push_back(bstruct);
      //cout << k << " " << leaf_idx << " " << branch_idx << endl;

      while (1) {
         if (cur_node->parent == -1) break;

         Dirichlet* parent_node = node[cur_node->parent];

         bstruct.node = parent_node->id;
         bstruct.branch = cur_node->parent_branch;

         //path_k.push_back(pair<int,int>(parent_node->id, cur_node->parent_branch));
         path_k.push_back(bstruct);

         //cout << k << " " << parent_node->id << " " << cur_node->parent_branch << endl;

         cur_node = parent_node;
      }

      topic_path[k] = path_k;
   }
}

// prune dirichlet-tree => update path, remove dirichlet nodes, update leaf2node idx
void DirichletTree::merge_node(int k1, int k2)
{
   // check if k1 and k2 come from the same parent. if not, merging is not allowed!
   BranchStruct b1 = topic_path[k1][0];
   BranchStruct b2 = topic_path[k2][0];

   if (b1.node != b2.node) {
      cerr << "merging topic k1 and topic k2 not allowed since they come from different parent nodes! " << b1.node << " " << b2.node << endl;
      return;
   }

   // remove one record from the bottom
   topic_path[k1].erase(topic_path[k1].begin(), topic_path[k1].begin()+1);

   // shift path to the left 
   for (int k=k2+1; k < num_leaf; k++)
      topic_path[k-1] = topic_path[k];


   this->num_topic--;
}

void DirichletTree::get_prior(double* ap, int K, bool norm)
{
   //if (K != num_leaf) {
   if (K != this->num_topic) {
      cerr << "# topic not matched!" << endl;
      exit(-1);
   }

   double max_ap = -999999;

/*
// yct 8/1 recompute E_q[log theta_k]
   for (int j=0; j < this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      cur_node->expect_ltheta();
   }
*/

   for (int k=0; k < K; k++) {
      const path& p = topic_path[k];

      ap[k] = 0.0;

      for (int j=0; j < (int) p.size(); j++) {
         Dirichlet* cur_node = node[p[j].node];
         const int b = p[j].branch;

         ap[k] += cur_node->expect_log_theta[b];
      }

      if (ap[k] > max_ap) max_ap = ap[k];

   }

   if (norm) {
      // subtract ap by max to avoid overflow
      for (int k=0; k < K; k++)
         ap[k] = exp(ap[k] - max_ap);
   }
/*
 else {
      for (int k=0; k < K; k++)
         ap[k] = exp(ap[k]);
   }
*/
}

// compute MAP topic prob based on the posterior counts
double* DirichletTree::MAP_prob()
{
   for (int k=0; k < num_topic; k++) {
      const path& p = topic_path[k];

      map_prob[k] = 1.0;

      for (int j=0; j < (int) p.size(); j++) {
         const Dirichlet* cur_node = node[p[j].node];
         const int b = p[j].branch;

         //map_prob[k] *= cur_node->gamma[b];
         map_prob[k] *= cur_node->branch_prob[b];
      }
   }

   // normalize
   //normalize(map_prob, num_topic);

/*
   // sanity check (should sum to one)
   double sum_p = accumulate(map_prob, map_prob+num_topic, 0.0);

   if (sum_p < 0.999 or sum_p > 1.001)
      cerr << "Warning: sum topic prob != 1.0 " << sum_p << endl;
*/

   return map_prob;
}

void DirichletTree::estimate_alpha(int iter, double learn)
{
   if (is_updated) return;

   //cerr << "total doc = " << M << endl;
   // perform gradient ascent for each node in the tree
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      cur_node->grad_ascent(iter, M, learn);
      //cur_node->moment(M);
   }

   is_updated = true;
}

void DirichletTree::write_alpha(FILE *ap)
{
   for (int j=0; j < (int) this->num_node; j++) {
      const Dirichlet* cur_node = node[j];

      write_vector(ap, cur_node->alpha, cur_node->size);
   }

   // write topic_path
#ifdef BINARY
   // number of topics
   fwrite(&(this->num_topic), sizeof(int), 1, ap);
   cout << "writing Num topic = " << this->num_topic << endl;
   for (int k=0; k < this->num_topic; k++) {
      const vector<BranchStruct>& path_k = topic_path[k];

      // number of items in path_k
      const int path_k_size = path_k.size();
      fwrite(&(path_k_size), sizeof(int), 1, ap);
      cout << "Num items in " << k << " is " << path_k_size << " ";
      
      for (int p=0; p < path_k_size; p++) {
         fwrite(&(path_k[p]), sizeof(BranchStruct), 1, ap);
         cout << path_k[p].node << "," << path_k[p].branch << " ";
      }
      cout << endl;
   }
#else
#endif
}

void DirichletTree::read_alpha(FILE *ap)
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      read_vector(ap, cur_node->alpha, cur_node->size);

      cur_node->update_K();
   }

   this->reset();

   // read topic_path if file is not at the end
   // write topic_path
#ifdef BINARY
   // number of topics
   int num_read = fread(&(this->num_topic), sizeof(int), 1, ap);

   if (num_read == 0) {
      cout << "no path items found" << endl;
      return;
   }

   cout << "reading Num topic = " << this->num_topic << endl;
   for (int k=0; k < this->num_topic; k++) {
      vector<BranchStruct>& path_k = topic_path[k];

      // clear items
      path_k.clear();

      // number of items in path_k
      int path_k_size = 0;
      fread(&(path_k_size), sizeof(int), 1, ap);
      cout << "Num items in " << k << " is " << path_k_size << " ";
      
      for (int p=0; p < path_k_size; p++) {
         BranchStruct bs;
         fread(&bs, sizeof(BranchStruct), 1, ap);

         path_k.push_back(bs);

         cout << path_k[p].node << "," << path_k[p].branch << " ";
      }
      cout << endl;
   }
#else
#endif
}

// reset adapt_alpha to background alpha
void DirichletTree::reset()
{
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      copy_array(cur_node->alpha, cur_node->adapt_alpha, cur_node->size);

      // update branch prob
      copy_array(cur_node->alpha, cur_node->branch_prob, cur_node->size);
      normalize(cur_node->branch_prob, cur_node->size);
   }

   // clear posterior count
   this->clear_gamma();
}

void DirichletTree::write_accum(FILE *ap)
{
   write_vector(ap, &M, 1);

   for (int j=0; j < (int) this->num_node; j++) {
      const Dirichlet* cur_node = node[j];

      write_vector(ap, &(cur_node->psg), 1);
      write_vector(ap, cur_node->spg, cur_node->size);
   }
}

void DirichletTree::add_accum(FILE *ap)
{
   double tmp_num_doc = 0;

   // read # of Doc processed
   read_vector (ap, &tmp_num_doc, 1);

   M += tmp_num_doc;

   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      double tmp_psg = 0.0;
      double* tmp_spg = new double[cur_node->size];

      read_vector(ap, &tmp_psg, 1);
      read_vector(ap, tmp_spg, cur_node->size);

      // accumulate to the cur_node's acc
      cur_node->psg += tmp_psg;
      for (int k=0; k < cur_node->size; k++)
         cur_node->spg[k] += tmp_spg[k];

      delete [] tmp_spg;
   }
}

/*
// build dirichlettree based on a standard LDA model
DirichletTree::DirichletTree(double* alpha, double** beta, int V, int K)
{
   // build distance matrix 
   const int max_topic = 2*K - 1;
   double **dist_matrix = dmatrix(max_topic, max_topic);
   double **new_beta = dmatrix(V, max_topic);
   double **log_new_beta = dmatrix(V, max_topic);
   double *all_alpha = new double[max_topic];
   bool* exists = new bool[max_topic];

   // at most K-1 merges incrementally. so total topics introduced including K = K + K-1 = 2K-1
   set_dmatrix(dist_matrix, max_topic, max_topic, 0.0);

   for (int v=0; v < V; v++)
      for (int i=0; i < K; i++) {
         new_beta[v][i] = beta[v][i];
         log_new_beta[v][i] = log(beta[v][i]);
      }

   for (int i=0; i < max_topic; i++)
      exists[i] = false;

   for (int i=0; i < K; i++) {
      all_alpha[i] = alpha[i];
      exists[i] = true;
   }

   // init matrix (only upper triangle is needed)
   cout << "start init topic dist_matrix" << endl;
   for (int v=0; v < V; v++) {
      const double* beta_v = new_beta[v];
      const double* log_beta_v = log_new_beta[v];

      for (int i=0; i < K; i++) {
         const double beta_vi = beta_v[i];
         const double log_beta_vi = log_beta_v[i];

         for (int j=i+1; j < K; j++) {
            const double beta_vj = beta_v[j];
            const double log_beta_vj = log_beta_v[j];

            dist_matrix[i][j] += beta_vi * (log_beta_vi - log_beta_vj) + beta_vj * (log_beta_vj - log_beta_vi);
         }
      }
   }
   cout << "finish init topic dist_matrix" << endl;

   // start with K topics
   int curK = K;
   // there are only K-1 merges to form the tree
   while (1) {
      // find the closest pair and merge => introduce a new "topic"
      double min_dist = INFINITY;
      int min_i = -1, min_j = -1;
      for (int i=0; i < curK; i++) {
         if (!exists[i]) continue;

         for (int j=i+1; j < curK; j++) {
            if (!exists[j]) continue;

            if (dist_matrix[i][j] < min_dist) {
               min_i = i;
               min_j = j;
               min_dist = dist_matrix[i][j];
            }
         }
      }

      // no objects exists
      if (min_i == -1 && min_j == -1) break;

      cout << curK << ", merging " << min_i << " " << min_j << " dist=" << min_dist << endl;
   
      // merge p_min_i and p_min_j by introducing new p(v)
      const double w_i = all_alpha[min_i] / (all_alpha[min_i] + all_alpha[min_j]);
   
      for (int v=0; v < V; v++) {
         double* beta_v = new_beta[v];
         double* log_beta_v = log_new_beta[v];
   
         //beta_v[curK] = w_i * beta_v[min_i] + (1.0 - w_i) * beta_v[min_j];
         beta_v[curK] =  (beta_v[min_i] + beta_v[min_j]) * 0.5;
         log_beta_v[curK] = log(beta_v[curK]);
      }
      // add new topic into all_alpha
      all_alpha[curK] = all_alpha[min_i] + all_alpha[min_j];

      // erase min_i and min_j for further consideration
      exists[min_i] = false;
      exists[min_j] = false;
      exists[curK] = true;
   
      // insert new topic and compute new distance matrix from K+1 matrix[K+1][*]
      for (int v=0; v < V; v++) {
         const double* beta_v = new_beta[v];
         const double* log_beta_v = log_new_beta[v];
         const double new_pv_v = beta_v[curK];
         const double log_new_pv_v = log_beta_v[curK];
   
         for (int i=0; i < curK; i++) {
            const double beta_vi = beta_v[i];
            const double log_beta_vi = log_beta_v[i];
   
            if (exists[i])
               dist_matrix[i][curK] += beta_vi * (log_beta_vi - log_new_pv_v) + new_pv_v * (log_new_pv_v - log_beta_vi);
         }
      }
      curK++;
   }
   cout << "merging completed!" << endl;

   free_dmatrix(dist_matrix, max_topic);
   free_dmatrix(new_beta, V);
   free_dmatrix(log_new_beta, V);
   delete [] exists;
}
*/

double DirichletTree::var_ll() const
{
   double total = 0.0;

   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* cur_node = node[j];
      const double* cur_alpha = cur_node->alpha;
      const double* cur_gamma = cur_node->gamma;

      Dirichlet f_theta(cur_alpha, cur_node->size);
      Dirichlet q_theta(cur_gamma, cur_node->size);

      double* ap = new double[cur_node->size];
      const double sum_gamma = accumulate(cur_gamma, cur_gamma+cur_node->size, 0.0);
      const double psi_sum_gamma = psi(sum_gamma);
      for (int k=0; k < cur_node->size; k++) {
         ap[k] = psi(cur_gamma[k]) - psi_sum_gamma;
      }

      const double term_theta = f_theta.log_prob(ap, true) - q_theta.log_prob(ap, true);

      total += term_theta;

      delete [] ap;

/*
      const double term1 = cur_node->K;

      const double sum_gamma = accumulate(cur_gamma, cur_gamma+cur_node->size, 0.0);
      const double psi_sum_gamma = psi(sum_gamma);

      double term2 = lgamma(sum_gamma);
      for (int k=0; k < cur_node->size; k++) {
         const double expect_lb = (psi(cur_gamma[k]) - psi_sum_gamma);

         total += (cur_alpha[k] - cur_gamma[k]) * expect_lb;

         term2 -= lgamma(cur_gamma[k]);
      }

      total += (term1 - term2);
*/
   }

   return total;
}

void DirichletTree::print() const
{
   cout << "# dirichlet node = " << num_node << endl;
   cout << "# leaf = " << num_leaf << endl;
   cout << "# topic = " << num_topic << endl;
   cout << "# branch = " << branch << endl;

/*
   for (int j=0; j < this->num_node; j++) {
      Dirichlet* cur_node = node[j];

      cout << "node " << j << " ";
      cur_node->print();
   }
*/
}

void DirichletTree::init_flat()
{
   for (int i=0; i < num_node; i++) {
      node[i]->init_flat();
   }
}

// Mar 25 08
void DirichletTree::copy_alpha(const DirichletTree* src_tree)
{
   // copy alpha value node by node
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* src_cur_node = src_tree->node[j];
      Dirichlet* cur_node = node[j];

      copy_array(src_cur_node->alpha, cur_node->alpha, cur_node->size);
   }
}
void DirichletTree::copy_gamma(const DirichletTree* src_tree)
{
   // copy gamma value node by node
   for (int j=0; j < (int) this->num_node; j++) {
      Dirichlet* src_cur_node = src_tree->node[j];
      Dirichlet* cur_node = node[j];

      copy_array(src_cur_node->gamma, cur_node->gamma, cur_node->size);
   }
}

DirichletNet::DirichletNet(int num_leaf) : K(num_leaf)
{
   int init_num = 1;
   int next = 0;
   const int branch = 2;

   // determine # of layers we need to cover the leaf nodes
   depth = (int) floor(log((double)num_leaf) / log(2.0));

   // establish a "tree" structure, but it is fully connected
   dir_net = new Dirichlet**[depth];
   num_node = new int[depth+1];
   alpha = new double*[depth+1];
   beta = new double*[depth];

   for (int d=0; d < depth; d++) {
      num_node[d] = init_num;

// we use tree structure for internal node, then fully connected one for leaf layer
      //int num_branch = (d < depth-1)? init_num * 2 : num_leaf;
      int num_branch = (d < depth-1)? branch : num_leaf;

      //cout << d << " # branch = " << num_branch << endl;

      dir_net[d] = new Dirichlet*[init_num];
      for (int i=0; i < init_num; i++) {
         //cout << d << " " << i << endl;

         dir_net[d][i] = new Dirichlet(0, num_branch);

         dir_net[d][i]->id = next;
         next++;
      }

      alpha[d] = new double[init_num];
      beta[d] = new double[init_num];

      //init_num = num_branch;
      init_num *= branch;
   }
   alpha[depth] = new double[num_leaf];
   beta[depth] = new double[num_leaf];
   num_node[depth] = num_leaf;

   // tmp buffer
   posterior = new double[K];
   Q = dmatrix(K, num_node[depth-1]);

   for (int d=0; d < depth; d++) {
      cout << d << " # node = " << num_node[d] << endl;
   }
}

DirichletNet::~DirichletNet()
{
   for (int d=0; d < depth; d++) {
      for (int i=0; i < num_node[d]; i++)
         delete dir_net[d][i];

      delete [] dir_net[d];
      delete [] alpha[d];
      delete [] beta[d];
   }
   delete [] alpha[depth];
   delete [] beta[depth];

   free_dmatrix(Q, num_node[depth-1]);

   delete [] num_node;
   delete [] alpha;
   delete [] beta;
   delete [] dir_net;

   delete [] posterior;
}

void DirichletNet::MAP_forward()
{
   //static double* tmpbuffer = new double[K];

   // init alpha variables
   for (int i=0; i < num_node[0]; i++)
      alpha[0][i] = 1.0;

   // run from depth to 1
   for (int d=1; d <= depth; d++) {
      Dirichlet** layer = dir_net[d-1];

/*
      // sanity check on sum branches
      for (int i=0; i < num_node[d-1]; i++) {
         double sum_b = accumulate(layer[i]->branch_prob, layer[i]->branch_prob + layer[i]->size, 0.0);

         cout << "sum branch = " << d << " " << i << " " << sum_b << endl;
      }
*/

      for (int j=0; j < num_node[d];j++) {
         alpha[d][j] = 0.0;

         if (d == depth) {
            for (int i=0; i < num_node[d-1]; i++) {
               alpha[d][j] += alpha[d-1][i] * layer[i]->branch_prob[j];
            }
         } else {
            int i = j / 2;
            alpha[d][j] += alpha[d-1][i] * layer[i]->branch_prob[j % 2];
         }
         //alpha[d][j] = log_add(tmpbuffer, num_node[d-1]);
      }

/*
      double sum_alpha = accumulate(alpha[d], alpha[d]+num_node[d], 0.0);
      cout << "sum alpha = " << d << " " << sum_alpha << endl;
*/
   }
}

// perform VB-forward step
void DirichletNet::forward()
{
   static double* tmpbuffer = new double[K];

/*
   // init alpha variables
   for (int i=0; i < num_node[0]; i++)
      alpha[0][i] = 1.0;

   // run from depth to 1
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++)
         for (int j=0; j < num_node[d+1];j++)
            alpha[d+1][j] += alpha[d][i] * exp(layer[i]->expect_log_theta[j]);
   }
*/

   // init alpha variables
   for (int i=0; i < num_node[0]; i++)
      alpha[0][i] = 0.0;

   // run from depth to 1
   for (int d=1; d <= depth; d++) {
      Dirichlet** layer = dir_net[d-1];

      for (int j=0; j < num_node[d];j++) {
         if (d == depth) {
            for (int i=0; i < num_node[d-1]; i++) {
               tmpbuffer[i] = alpha[d-1][i] + layer[i]->expect_log_theta[j];
            }
            alpha[d][j] = log_add(tmpbuffer, num_node[d-1]);
         } else {
            int i = j / 2;
            alpha[d][j] = alpha[d-1][i] + layer[i]->expect_log_theta[j % 2];
         }
      }
   }

/*
   for (int d=0; d < depth; d++) {
      for (int i=0; i < num_node[d]; i++)
         cout << alpha[d][i] << " ";
      cout << endl;
   }
*/
}

void DirichletNet::backward(double* topic_count)
{
   // init beta variables
   for (int k=0; k < K; k++)
      beta[depth][k] = topic_count[k];

   // run from depth to 1
   for (int d=depth; d > 0; d--) {
      Dirichlet** layer = dir_net[d-1];

      for (int i=0; i < num_node[d-1]; i++)
         for (int j=0; j < num_node[d]; j++)
            beta[d-1][i] = beta[d][j] * exp(layer[i]->expect_log_theta[j]);
   }
}

void DirichletNet::accu_gamma(double* topic_count)
{
   // perform VB forward procedure
   this->forward();
   //this->backward(topic_count);

   static double* tmpcount = new double[K];
   double sum_count = 0.0;

   // init count
   for (int i=0; i < K; i++) {
      tmpcount[i] = topic_count[i];
      //cout << tmpcount[i] << " ";
   }
   //cout << endl;

   sum_count = accumulate(tmpcount, tmpcount+K, 0.0);

   //cout << "start accu_gamma" << endl;
   // propagate counts bottom-up. split it according to the posterior prob of the incoming nodes from the upper layer
   for (int d=depth-1; d >=0 ; d--) {
      Dirichlet** layer = dir_net[d];

      // layer d+1 -> layer d
      if (d == depth-1) {
         for (int j=0; j < num_node[d+1]; j++) {
            for (int i=0; i < num_node[d]; i++) {
               //Dirichlet* cur_node = layer[i];
   
               posterior[i] = alpha[d][i] + layer[i]->expect_log_theta[j];
            }
   
            double z = log_add(posterior, num_node[d]);
   
            //normalize(posterior, num_node[d]);
   
            for (int i=0; i < num_node[d]; i++) {
               Dirichlet* cur_node = layer[i];
   
               //cout << posterior[i] << " ";
               // store variational posterior
               Q[j][i] = exp(posterior[i] - z);
   
               // split tmpcount[j] according to posterior prob over its incoming arcs
               cur_node->gamma[j] += tmpcount[j] * Q[j][i];
            }
            //cout << endl;
         }
      } else {
         for (int j=0; j < num_node[d+1]; j++) {
            const int i = j / 2;
            Dirichlet* cur_node = layer[i];

            cur_node->gamma[j%2] += tmpcount[j];
         }
      }

      // update tmpcount vector for layer d by summing it's own gamma collected
      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         //for (int j=0; j < cur_node->size; j++)
         //   tmpcount[j] = 0.0;
         tmpcount[i] = 0.0;

         for (int j=0; j < cur_node->size; j++)
            tmpcount[i] += cur_node->gamma[j];
      }

      sum_count = accumulate(tmpcount, tmpcount+num_node[d], 0.0);

      //cout << "depth = " << d << " count = " << sum_count << endl;
   }
}

void DirichletNet::update_gamma(double forget)
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* node = layer[i];

         double* cur_alpha = node->adapt_alpha;

         for (int j=0; j < node->size; j++)
            node->gamma[j] += cur_alpha[j] * forget;

         node->expect_ltheta();
      }
   }
}

void DirichletNet::clear_gamma()
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* node = layer[i];

/*
         double* cur_alpha = node->alpha;

         // init with prior alpha
         for (int j=0; j < node->size; j++)
            node->gamma[j] = cur_alpha[j];

         // compute prior E
         node->expect_ltheta();
*/

         // reset to zero
         for (int j=0; j < node->size; j++)
            node->gamma[j] = 0.0;
      }
   }
}

void DirichletNet::get_prior(double* ap, int num_topic, bool norm)
{
   assert(num_topic == K);

/*
   // re-do forward step
   this->forward();
*/

   Dirichlet** layer = dir_net[depth-1];

   for (int k=0; k < K; k++) {
      double val = 0.0;
      for (int i=0; i < num_node[depth-1]; i++) {
         Dirichlet* node = layer[i];

         val += Q[k][i] * (alpha[depth-1][i] + node->expect_log_theta[k] - log(Q[k][i]));
      }
      ap[k] = val;
   }

   if (norm) {
      const double max_val = *max_element(ap, ap + K);

      for (int k=0; k < K; k++)
         ap[k] = exp(ap[k] - max_val);
   } else {
      for (int k=0; k < K; k++)
         ap[k] = exp(ap[k]);
   }

/*
   const double max_val = *max_element(alpha[depth], alpha[depth] + K);

   // re-do forward step
   //forward();

   for (int k=0; k < K; k++) {
      if (norm)
         ap[k] = exp(alpha[depth][k] - max_val);
      else
         ap[k] = exp(alpha[depth][k]);
   }
*/
}

void DirichletNet::create_accu()
{
   if (is_created) return;

   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         cur_node->spg = new double[cur_node->size];

         for (int j=0; j < cur_node->size; j++)
            cur_node->spg[j] = 0.0;

         cur_node->psg = 0.0;
      }
   }

   M = 0.0;

   is_created = true;
}

void DirichletNet::clear_psi()
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* node = layer[i];

         for (int j=0; j < node->size; j++)
            node->spg[j] = 0.0;

         node->psg = 0.0;
      }
   }

   M = 0.0;

   is_updated = false;
}

void DirichletNet::accu_psi(double wgt)
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];
         double* cur_gamma = cur_node->gamma;
   
         // accumulate sufficient statistics for alpha update
         cur_node->psg += wgt * psi(accumulate(cur_gamma, cur_gamma+cur_node->size, 0.0));
   
         for (int k = 0; k < cur_node->size; k++) {
            cur_node->spg[k] += wgt * psi(cur_gamma[k]);
         }
      }
   }

   //M++;
   M += wgt;
}

// reset adapt_alpha to background alpha
void DirichletNet::reset()
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         copy_array(cur_node->alpha, cur_node->adapt_alpha, cur_node->size);

         // update branch prob
         copy_array(cur_node->alpha, cur_node->branch_prob, cur_node->size);
         normalize(cur_node->branch_prob, cur_node->size);
      }
   }

   // clear posterior count
   this->clear_gamma();
}

void DirichletNet::write_alpha(FILE *ap)
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         write_vector(ap, cur_node->alpha, cur_node->size);
      }
   }
}

void DirichletNet::read_alpha(FILE *ap)
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         read_vector(ap, cur_node->alpha, cur_node->size);

         cur_node->update_K();
      }
   }
   this->reset();
}

double* DirichletNet::MAP_prob()
{
   MAP_forward();

   for (int k=0; k < K; k++) {
      //posterior[k] = exp(alpha[depth][k]);
      posterior[k] = alpha[depth][k];
   }

   //normalize(posterior, K);

/*
   for (int k=0; k < K; k++) {
      cout << posterior[k] << " ";
   }
   cout << endl;
*/

   return posterior;
}

void DirichletNet::estimate_alpha(int iter, double learn)
{
   if (is_updated) return;

   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         cur_node->grad_ascent(iter, M, learn);
      }
   }

   is_updated = true;
}

void DirichletNet::update_branch_prob()
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];

         // update (posterior-mean) branch prob
         copy_array(cur_node->gamma, cur_node->branch_prob, cur_node->size);
         normalize(cur_node->branch_prob, cur_node->size);
      }
   }
}

void DirichletNet::adapt_alpha()
{
   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];
         copy_array(cur_node->gamma, cur_node->adapt_alpha, cur_node->size);
      }
   }
}

void DirichletNet::write_accum(FILE *ap)
{
   write_vector(ap, &M, 1);

   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];
         write_vector(ap, &(cur_node->psg), 1);
         write_vector(ap, cur_node->spg, cur_node->size);
      }
   }
}

void DirichletNet::add_accum(FILE *ap)
{
   double tmp_num_doc = 0;

   // read # of Doc processed
   read_vector (ap, &tmp_num_doc, 1);

   M += tmp_num_doc;

   for (int d=0; d < depth; d++) {
      Dirichlet** layer = dir_net[d];

      for (int i=0; i < num_node[d]; i++) {
         Dirichlet* cur_node = layer[i];
         double tmp_psg = 0.0;
         double* tmp_spg = new double[cur_node->size];
   
         read_vector(ap, &tmp_psg, 1);
         read_vector(ap, tmp_spg, cur_node->size);
   
         // accumulate to the cur_node's acc
         cur_node->psg += tmp_psg;
         for (int k=0; k < cur_node->size; k++)
            cur_node->spg[k] += tmp_spg[k];
   
         delete [] tmp_spg;
      }
   }
}

double DirichletNet::var_ll() const
{
   return 0.0;
}
