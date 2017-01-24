#include <fstream>
#include "ei_kev.h"

EiKEV::EiKEV(int num_spkrs, MemoryManager* mgr, Recognizer* recognizer, HMMSet* hmmset, int num_reg_groups, int dim, int num_eigen_vectors, const float& learning_rate, const float& threshold, const float& beta){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->recognizer = recognizer;
  this->hmmset = hmmset;
  this->reader = NULL;
  this->num_spkrs = num_spkrs;
  this->num_reg_groups = num_reg_groups;
  this->dim = dim;
  this->num_eigen_vectors = num_eigen_vectors;
  this->kernel_val = NULL;
  this->beta = beta;
  this->learning_rate = learning_rate;
  this->init_rate = learning_rate;
  this->threshold = threshold;

  kernel.resize(num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    kernel[r] = new HeteroDirectSumKernel();
  }
  hmmset->getStateSet(state_set);

  getAllGaussians();
  
  eigen_vector.resize(num_reg_groups);
  eigen_value.resize(num_reg_groups);

  weight = Matrix(num_reg_groups, num_eigen_vectors);
  weight_zero = Matrix(1, num_reg_groups);
  for(int i=0;i<num_reg_groups;i++){
    weight_zero[0][i] = 0.5;
  }
  
  term_a_gau = new CacheTable(2, dim, gaussian.size());
  term_b_gau = new CacheTable(3, dim, gaussian.size(), num_eigen_vectors);
  term_a_zero = new CacheTable(2, num_reg_groups, dim);
  term_b_zero = new CacheTable(3, num_reg_groups, dim, num_eigen_vectors);

  sim_gau = new CacheTable(2, dim, gaussian.size());
  sim_zero = new CacheTable(2, num_reg_groups, dim);
  partial_sim_zero = new CacheTable(3, num_reg_groups, dim, num_eigen_vectors);
}

EiKEV::EiKEV(int num_spkrs, MemoryManager* mgr, int num_reg_groups, int dim, int num_eigen_vectors, const float& learning_rate, const float& threshold, const float& beta){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->recognizer = NULL;
  this->hmmset = NULL;
  this->reader = NULL;
  this->num_spkrs = num_spkrs;
  this->num_reg_groups = num_reg_groups;
  this->dim = dim;
  this->num_eigen_vectors = num_eigen_vectors;
  this->kernel_val = NULL;
  this->beta = beta;
  this->learning_rate = learning_rate;
  this->init_rate = learning_rate;
  this->threshold = threshold;

  kernel.resize(num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    kernel[r] = new HeteroDirectSumKernel();
  }
  
  eigen_vector.resize(num_reg_groups);
  eigen_value.resize(num_reg_groups);

  weight = Matrix(num_reg_groups, num_eigen_vectors);
  weight_zero = Matrix(1, num_reg_groups);
  for(int i=0;i<num_reg_groups;i++){
    weight_zero[0][i] = 0.5;
  }
  
  term_a_gau = new CacheTable(2, dim, gaussian.size());
  term_b_gau = new CacheTable(3, dim, gaussian.size(), num_eigen_vectors);
  term_a_zero = new CacheTable(2, num_reg_groups, dim);
  term_b_zero = new CacheTable(3, num_reg_groups, dim, num_eigen_vectors);

  sim_gau = new CacheTable(2, dim, gaussian.size());
  sim_zero = new CacheTable(2, num_reg_groups, dim);
  partial_sim_zero = new CacheTable(3, num_reg_groups, dim, num_eigen_vectors);
}


EiKEV::~EiKEV(){
  if(kernel_val!=NULL)
    delete kernel_val;
  delete term_a_gau;
  delete term_b_gau;
  delete term_a_zero;
  delete term_b_zero;
  delete sim_gau;
  //delete partial_sim_gau;
  delete sim_zero;
  delete partial_sim_zero;
  //delete partial_invf;
  for(int r=0;r<num_reg_groups;r++){
    delete kernel[r];
  }

//  delete base_kernel;
  for(int i=0;i<base_kernel.size();i++)
    delete base_kernel[i];
}

void EiKEV::loadUtterHMM(Recognizer* recognizer, HMMSet* hmmset){
  this->recognizer = recognizer;
  this->hmmset = hmmset;
  hmmset->getStateSet(state_set);

  getAllGaussians();
}

Matrix EiKEV::makeGradientVec(Matrix& partial_q, Matrix& partial_w0){
  Matrix vec(num_reg_groups*num_eigen_vectors+num_reg_groups, 1);
  
  for(int r=0;r<num_reg_groups;r++){
    for(int i=0;i<num_eigen_vectors;i++){
      vec[r*num_eigen_vectors+i][0] = partial_q[r][i];
    }
    vec[num_reg_groups*num_eigen_vectors+r][0] = partial_w0[0][r];
  }

  return vec;
}

Matrix EiKEV::makeParameterVec(){
  Matrix vec(num_reg_groups*num_eigen_vectors+num_reg_groups, 1);

  for(int r=0;r<num_reg_groups;r++){
    for(int i=0;i<num_eigen_vectors;i++){
      vec[r*num_eigen_vectors+i][0] = weight[r][i];
    }
    vec[num_reg_groups*num_eigen_vectors+r][0] = weight_zero[0][r];
  }

  return vec;
}

void EiKEV::updateParameters(Matrix& vec){
  for(int r=0;r<num_reg_groups;r++){
    for(int i=0;i<num_eigen_vectors;i++){
      weight[r][i] = vec[r*num_eigen_vectors+i][0];
    }
    weight_zero[0][r] = vec[num_reg_groups*num_eigen_vectors+r][0];
    if(weight_zero[0][r]<0.0)
      weight_zero[0][r] = 0.0;
    if(weight_zero[0][r]>1.0)
      weight_zero[0][r] = 1.0;
   }
}

void EiKEV::initWeight(){
  Matrix id_trans(dim, dim+1);
  vector<vector<Matrix> > id_constituent;
  float tmp1=0.0, tmp2=0.0, tmp3=0.0;
  
  weight = Matrix(num_reg_groups, num_eigen_vectors);

  for(int d=0;d<dim;d++)
    id_trans[d][d+1]=1.0;

  id_constituent.resize(num_reg_groups);
  for(int k=0;k<num_reg_groups;k++){
    for(int d=0;d<dim;d++){
//      id_constituent[k].push_back(id_trans.getRowVec(d).transpose());
      Matrix tmp(dim+1, 1);
      for(int j=0;j<dim+1;j++){
        tmp[j][0] = (id_trans[d][j]-const_mean[k][d][j][0])/const_std[k][d][j][0];
      }
      id_constituent[k].push_back(tmp);
    }
  }

  for(int k=0;k<num_reg_groups;k++){
    tmp1=tmp2=tmp3=0.0;
    for(int i=0;i<num_spkrs;i++){
      tmp1 += kernel[k]->eval(id_constituent[k], const_norm[k][i]);
      for(int j=0;j<num_spkrs;j++){
        //tmp3 += kernel[k]->eval(const_norm[k][i], const_norm[k][j]);
        tmp3 += kernel_mat[k][i][j];
      }
    }
    tmp1/=(float)num_spkrs;
    tmp3/=(float)num_spkrs*num_spkrs;

    for(int m=0;m<num_eigen_vectors;m++){
      for(int i=0;i<num_spkrs;i++){
        float factor = eigen_vector[k][i][m]*inv_sqrt_eigen_value[k][0][m];

        tmp2=0.0;
        for(int j=0;j<num_spkrs;j++){
          //tmp2 += kernel[k]->eval(const_norm[k][i], const_norm[k][j]);
          tmp2 += kernel_mat[k][i][j];
        }
        tmp2/=(float)num_spkrs;
      
        weight[k][m] += factor*(kernel[k]->eval(id_constituent[k], const_norm[k][i]) - tmp1 - tmp2 + tmp3);
      }
    }
    /*
    for(int m=0;m<num_eigen_vectors;m++){
      printf("weight (%d): %d = %f\n", k, m, weight[k][m]);
    }
    */
  }
}

void EiKEV::load(char* trans_file){
  FILE* fp;
  Matrix vec(dim+1, 1);
  MLLR* mllr;

  fp = fopen(trans_file, "r");
/*
  mllr = new MLLR(mgr, recognizer, dim, num_mix, true);  
  mllr->setRegGrouping(num_reg_groups);
  label = mllr->getGroupInfo();
*/
  mllr_transformation.clear();
  constituent.clear();
  constituent.resize(num_reg_groups);
  const_norm.clear();
  const_norm.resize(num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    constituent[r].resize(num_spkrs);
    const_norm[r].resize(num_spkrs);
  }
  
  const_mean.clear();
  const_mean.resize(num_reg_groups);
  const_var.clear();
  const_var.resize(num_reg_groups);
  const_std.clear();
  const_std.resize(num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    const_mean[r].resize(dim);
    const_var[r].resize(dim);
    const_std[r].resize(dim);
    for(int j=0;j<dim;j++){
      const_mean[r][j] = Matrix(dim+1, 1);
      const_var[r][j] = Matrix(dim+1, 1);
      const_std[r][j] = Matrix(dim+1, 1);
    }
  }
  
  for(int i=0;i<num_spkrs;i++){
    for(int r=0;r<num_reg_groups;r++){
      for(int j=0;j<dim;j++){
        for(int k=0;k<dim+1;k++){
          fscanf(fp, "%f", &vec[k][0]);
        }
        constituent[r][i].push_back(vec);
        const_norm[r][i].push_back(vec);
      }
    }
  }

  for(int i=0;i<num_spkrs;i++){
    for(int r=0;r<num_reg_groups;r++){
      for(int j=0;j<dim;j++){
        const_mean[r][j] += constituent[r][i][j]*(1.0/num_spkrs);
      }
    }
  }
  for(int i=0;i<num_spkrs;i++){
    for(int r=0;r<num_reg_groups;r++){
      for(int j=0;j<dim;j++){
        for(int k=0;k<dim+1;k++){
          const_var[r][j][k][0] += ( pow(constituent[r][i][j][k][0], (float)2.0) - pow(const_mean[r][j][k][0], (float)2.0) )*(1.0/num_spkrs);
        }
      }
    }
  }
  for(int r=0;r<num_reg_groups;r++){
    for(int j=0;j<dim;j++){
      for(int k=0;k<dim+1;k++){
        const_std[r][j][k][0] = sqrt(const_var[r][j][k][0]);
      }
    }
  }

  for(int i=0;i<num_spkrs;i++){
    for(int r=0;r<num_reg_groups;r++){
      for(int j=0;j<dim;j++){
        for(int k=0;k<dim+1;k++){
          const_norm[r][i][j][k][0] = (constituent[r][i][j][k][0] - const_mean[r][j][k][0]) / sqrt(const_var[r][j][k][0]);
        }
      }
    }
  }

  for(int r=0;r<num_reg_groups;r++){
    for(int j=0;j<dim;j++){
      Matrix cov(dim+1, 1);

      for(int k=0;k<dim+1;k++){
        cov[k][0] = const_std[r][j][k][0];
      }
      Kernel* bk = new NormalizedGaussianKernel(beta, cov);
      base_kernel.push_back(bk);
      ((HeteroDirectSumKernel*)kernel[r])->addKernel(bk);
    }
  }
  
  fclose(fp);

  //delete mllr;
}

void EiKEV::prepare(float** x, int t_max, vector<string>& adapt_trans){
  MLLR* mllr;
  Matrix id(dim, dim+1);
  vector<Matrix> tmp;

  for(int d=0;d<dim;d++)
    id[d][d] = 1.0;
  
  mllr_transformation.clear();
  label.clear();

  mllr = new MLLR(mgr, recognizer, dim, num_mix, true);
  mllr->setRegGrouping(num_reg_groups);
  //mllr->adapt(x, t_max, adapt_trans);
  vector<Matrix> col = mllr->getTransformations();
  mllr_transformation.push_back(col);

  label = mllr->getGroupInfo();

  for(int j=0;j<gaussian.size();j++){
    gaussian[j]->setMeanVec(old_mean[j]);
  }
  delete mllr;

  for(int k=0;k<num_reg_groups;k++){
    tmp.push_back(id);
  }
  mllr_transformation.push_back(tmp);

  constituent.clear();
  constituent.resize(num_reg_groups);
  for(int k=0;k<num_reg_groups;k++){
    constituent[k].resize(num_spkrs+1);
    for(int i=0;i<num_spkrs;i++){
      for(int l=0;l<dim;l++){
        constituent[k][i].push_back(mllr_transformation[i][k].getRowVec(l).transpose());
      }
    }  
  }
  
  for(int k=0;k<num_reg_groups;k++){
    for(int d=0;d<dim;d++){
      constituent[k][num_spkrs].push_back(id.getRowVec(d).transpose());
    }
  }
  //num_spkrs++;

  for(int i=0;i<num_spkrs;i++){
    for(int k=0;k<num_reg_groups;k++){
      for(int d=0;d<dim;d++){
        constituent[k][i][d].transpose().print();
      }
    }
  }
}

void EiKEV::computeTransformation(){
  Matrix tmp(dim, 1);
  vector<State*> state_set;

  hmmset->getStateSet(state_set);
  mean_trans.clear();
  mean_trans.resize(gaussian.size());

  for(int i=0;i<gaussian.size();i++){
    Gaussian* g = gaussian[i];
    Matrix mean_off = makeMeanOff(g->getMeanVec());
    Matrix tmp = mean_off;
    int reg_group = label[g->getID()];
    mean_trans[i] = Matrix(dim, 1);
    for(int r=0;r<dim;r++){
      for(int d=0;d<dim+1;d++){
        tmp[d][0] = mean_off[d][0]*const_std[reg_group][r][d][0];
      }
      float kval = sim_gau->get(r, i);
      mean_trans[i][r][0] = (-1.0/beta)*log(kval)+(1.0/beta)*log(sim_zero->get(reg_group, r))-(tmp.transpose()*mean_off)[0][0];
      mean_trans[i][r][0] *= -0.5;
      mean_trans[i][r][0] += (const_mean[reg_group][r].transpose()*mean_off)[0][0];
    }
  }
}

void EiKEV::computeKernelMatrix(){
  Matrix ones(num_spkrs, 1), id(num_spkrs, num_spkrs), h_mat;

  id.identity();
  for(int i=0;i<num_spkrs;i++){
    ones[i][0] = 1.0;
  }
  h_mat = id - (ones*ones.transpose())*(1.0/num_spkrs);

  centered_kernel_mat.clear();
  centered_kernel_mat.resize(num_reg_groups);
  kernel_mat.clear();
  kernel_mat.resize(num_reg_groups);
  kr_mat.clear();
  kr_mat.resize(num_reg_groups);
  for(int k=0;k<num_reg_groups;k++){
    kr_mat[k].resize(dim);
    for(int l=0;l<dim;l++){
      kr_mat[k][l] = Matrix(num_spkrs, num_spkrs);
    }
    for(int i=0;i<num_spkrs;i++){
      for(int j=i;j<num_spkrs;j++){
        for(int l=0;l<dim;l++){
          kr_mat[k][l][i][j] = base_kernel[k*dim+l]->eval(const_norm[k][i][l], const_norm[k][j][l]);
          kr_mat[k][l][j][i] = kr_mat[k][l][i][j];
        }
      }
    }
  }
  for(int k=0;k<num_reg_groups;k++){
    kernel_mat[k] = Matrix(num_spkrs, num_spkrs);
    for(int l=0;l<dim;l++){
      kernel_mat[k] += kr_mat[k][l];
    }
    centered_kernel_mat[k] = h_mat*kernel_mat[k]*h_mat;
  }
}

void EiKEV::kernelPCA(){
  computeKernelMatrix();

  for(int k=0;k<num_reg_groups;k++){
    centered_kernel_mat[k].save("k.mat");
//    MathEngine::extEigenDecomp(centered_kernel_mat[k], eigen_value[k], eigen_vector[k]);
    MathEngine::dEigenDecomp(centered_kernel_mat[k], eigen_value[k], eigen_vector[k]);
//    eigen_value[k].load("eigen_val.mat");
//    eigen_vector[k].load("eigen_vec.mat");
  }

  inv_sqrt_eigen_value = eigen_value;
  for(int k=0;k<num_reg_groups;k++){
    for(int m=0;m<num_eigen_vectors;m++){
      inv_sqrt_eigen_value[k][0][m] = 1.0/sqrt(eigen_value[k][0][m]);
    }
  }


//  eigen_value[0].print();
//  eigen_value[1].print();
//  eigen_vector[0].getColVec(num_eigen_vectors-1).transpose().print();
//  eigen_vector[1].getColVec(0).transpose().print();
//  fflush(stdout);
}

Matrix EiKEV::computeGradient(float** x, int t_max, vector<string>& adapt_trans){
  Matrix gradient, partial_q(num_reg_groups, num_eigen_vectors), partial_w0(1, num_reg_groups), tmp;
  Network* fa_net;
  
  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);
  tmp = computePartialQ(fa_net, x, t_max);
  for(int r=0;r<num_reg_groups;r++){
    for(int k=0;k<num_eigen_vectors;k++){
      partial_q[r][k] += tmp[r][k]*(1.0-weight_zero[0][r]);
    }
    partial_w0[0][r] += tmp[r][num_eigen_vectors];
  }
  delete fa_net;

  gradient = makeGradientVec(partial_q, partial_w0);

  return gradient;
}

void EiKEV::standby(){
  fprintf(stderr, "performing kernel pca...\n");
  kernelPCA();
/*
  precompute();
  term_a_zero->saveBinary("term_a_zero.cache");
  term_b_zero->saveBinary("term_b_zero.cache");
  term_a_gau->saveBinary("term_a_gau.cache");
  term_b_gau->saveBinary("term_b_gau.cache");
  exit(0);
*/  
  fprintf(stderr, "loading cache...\n");
  term_a_zero->loadBinary("term_a_zero.cache");
  term_b_zero->loadBinary("term_b_zero.cache");
  term_a_gau->loadBinary("term_a_gau.cache");
  term_b_gau->loadBinary("term_b_gau.cache");
 
}

void EiKEV::init(){
  fprintf(stderr, "performing kernel pca...\n");
  kernelPCA();

  precompute();
  /*
  term_a_zero->saveBinary("term_a_zero.cache");
  term_b_zero->saveBinary("term_b_zero.cache");
  term_a_gau->saveBinary("term_a_gau.cache");
  term_b_gau->saveBinary("term_b_gau.cache");
  */
}

void EiKEV::initAndSave(){
  fprintf(stderr, "performing kernel pca...\n");
  kernelPCA();

  precompute();
  
  term_a_zero->saveBinary("term_a_zero.cache");
  term_b_zero->saveBinary("term_b_zero.cache");
  term_a_gau->saveBinary("term_a_gau.cache");
  term_b_gau->saveBinary("term_b_gau.cache");
}

void EiKEV::adapt(float** x, int t_max, vector<string>& adapt_trans){
  Matrix si_weight, best_weight, partial_q, tmp, best_weight_zero, partial_w0, weight_zero_backup;
  Matrix para_vec, direction, gradient, hess_inv(num_reg_groups*num_eigen_vectors+num_reg_groups, num_reg_groups*num_eigen_vectors+num_reg_groups);
  Matrix prev_vec, prev_grad(num_reg_groups*num_eigen_vectors+num_reg_groups, 1);
  float val, cur, old_min, min, best_rate, si_q;
  int idx=0;
  Network* fa_net;

  //prepare(x, t_max, adapt_trans);
  //exit(0);

  restoreModel();

  fprintf(stderr, "computing SI weights...\n");
  initWeight();
  si_weight = best_weight = weight;
  weight.print();

  fprintf(stderr, "processing...");
  fflush(stdout);
  old_min=0.0;
  si_q=0.0;
  weight_zero = Matrix(1, num_reg_groups);
  best_weight_zero = Matrix(1, num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    weight_zero[0][r] = 0.5;
    best_weight_zero[0][r] = 0.5;
  }
  learning_rate = init_rate;

  precomputeSim();
  computeTransformation();

  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);
  old_min += computeQFunc(fa_net, x, t_max);
  si_q += computeSIQFunc(fa_net, x, t_max);
  delete fa_net;

  if(si_q==0.0){
    fprintf(stderr, "Error in forced alignment\n");
    restoreModel();
    return;
  }
  
  min = old_min;

  fprintf(stderr, "si qfunc: %f, init qfunc: %f\n", si_q, old_min);
  fflush(stdout);

  hess_inv.identity();
  for(int iter=0;iter<GRAD_ITER;iter++){
    fprintf(stderr, "iter: %d\n", iter);
    best_rate = 0.0;

    fprintf(stderr, "computing gradient...\n");
    gradient = computeGradient(x, t_max, adapt_trans);
    fprintf(stderr, "grad norm: %e\n", (gradient.transpose()*gradient)[0][0]);
    if(iter==0){
      direction = gradient;
    }
    else{
      fprintf(stderr, "estimating hessian inverse...\n");
      para_vec = makeParameterVec();
      Matrix y_vec = gradient-prev_grad;
      Matrix s_vec = para_vec-prev_vec;
      Matrix old_h = hess_inv;
      float factor = 1.0/(y_vec.transpose()*s_vec)[0][0];

      hess_inv += s_vec*s_vec.transpose()*factor * (1.0 + factor*(y_vec.transpose()*old_h*y_vec)[0][0]);
      hess_inv += (s_vec*y_vec.transpose()*old_h + old_h*y_vec*s_vec.transpose())*(-1.0*factor);

      direction = hess_inv * gradient;
    }

    fa_net = new Network(mgr);
    recognizer->concatHMM(adapt_trans, *fa_net);
    for(int j=0;j<LINSRH_ITER;j++){
      para_vec = makeParameterVec();
      para_vec += (direction)*(-learning_rate);
      updateParameters(para_vec);

      precomputeSim();
      computeTransformation();

      cur = computeQFunc(fa_net, x, t_max);

      if(cur<min){
        min = cur;
        best_rate = learning_rate;
        if(iter==0)
          learning_rate*=1.20;
        else
          learning_rate*=1.50;
      }
      else{
        if(iter==0)
          learning_rate*=0.5;
        else
          learning_rate*=0.25;
      }
      weight = best_weight;
      weight_zero = best_weight_zero;

      fprintf(stderr, "min: %f, cur: %f, learn: %e, best: %e\n", min, cur, learning_rate, best_rate);
    }
    delete fa_net;

    fprintf(stderr, "Improve: %f\n", (old_min-min)/old_min);

    if(best_rate==0.0){
      weight = best_weight;
      weight_zero = best_weight_zero;
      precomputeSim();
      computeTransformation();
      break;
    }

    para_vec = makeParameterVec();
    prev_vec = para_vec;
    prev_grad = gradient;
    para_vec += (direction)*(-best_rate);
    updateParameters(para_vec);
    
    for(int r=0;r<num_reg_groups;r++){
      for(int m=0;m<num_eigen_vectors;m++){
        best_weight[r][m] = weight[r][m];
      }
    }
    fprintf(stderr, "best weight_zero:\n");
    weight_zero.print();

    best_weight_zero = weight_zero;
    precomputeSim();
    computeTransformation();

    if(best_rate==0.0 || (old_min-min)/old_min<threshold){
      break;
    }

    learning_rate = best_rate;
    old_min = min;

  }

  if(min<si_q)
    updateModel();
}

float EiKEV::computeSIQFunc(Network* fa_net, float** x, int t_max){
  float result=0.0;
  vector<State*> state_seq;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  CacheTable* mix_occ;

  recognizer->setNetwork(fa_net);
  recognizer->process(x, t_max);
  recognizer->decode(state_seq);
  mix_occ = new CacheTable(2, state_seq.size(), num_mix);
  recognizer->computeVitMixOcc(state_seq, x, t_max, mix_occ);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      if(mix_occ->get(j, m)<0.001)
        continue;
      Gaussian* g = mm->getGaussian(m);
      Matrix mean_vec = g->getMeanVec().transpose();

      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
      }
      float sum, tmp;
      sum= 0.0;
      for(int d=0;d<dim;d++){
        tmp = (x[t][d]-mean_vec[d][0]);
        sum += tmp*cov_inv[d][d]*tmp;
      }
      result += sum*mix_occ->get(j, m);
    }
    t++;
  }

  delete mix_occ;

  return result;
}


float EiKEV::computeQFunc(Network* fa_net, float** x, int t_max){
  float result=0.0;
  vector<State*> state_seq;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  CacheTable* mix_occ;

  recognizer->setNetwork(fa_net);
  recognizer->process(x, t_max);
  recognizer->decode(state_seq);

  mix_occ = new CacheTable(2, state_seq.size(), num_mix);
  recognizer->computeVitMixOcc(state_seq, x, t_max, mix_occ);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      if(mix_occ->get(j, m)<0.001)
        continue;
      Gaussian* g = mm->getGaussian(m);
      Matrix mean_vec = g->getMeanVec().transpose();
      int reg_group = label[g->getID()];
      int g_idx = idx2gau_idx[g->getID()];

      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
      }
      Matrix trans_mean_vec = mean_vec*weight_zero[0][reg_group] + mean_trans[g_idx]*(1.0-weight_zero[0][reg_group]);
      float sum=0.0, tmp;
      for(int d=0;d<dim;d++){
        tmp = x[t][d]- trans_mean_vec[d][0];
        sum += tmp*cov_inv[d][d]*tmp;
      }
      result += sum*mix_occ->get(j, m);
//      fprintf(stderr, "t_max: %d, t: %d, j: %d, m: %d, occ: %f\n", t_max, t, j, m, mix_occ->get(j, m));
    }
    t++;
  }

  delete mix_occ;

  return result;
}

Matrix EiKEV::computePartialQ(Network* fa_net, float** x, int t_max){
  Matrix partial_q;
  Matrix p_invf(dim, num_eigen_vectors);
  Matrix obs(dim, 1), cov_inv(dim, dim);
  vector<State*> state_seq;
  CacheTable* mix_occ;
  int g_idx;

  partial_q = Matrix(num_reg_groups, num_eigen_vectors+1);

  recognizer->setNetwork(fa_net);
  recognizer->process(x, t_max);
  recognizer->decode(state_seq);
  mix_occ = new CacheTable(2, state_seq.size(), num_mix);
  recognizer->computeVitMixOcc(x, t_max, mix_occ);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      if(mix_occ->get(j, m) < 0.001)
        continue;
      Gaussian* g = mm->getGaussian(m);
      int reg_group = label[g->getID()];
      Matrix mean_vec = g->getMeanVec().transpose();
      g_idx = idx2gau_idx[g->getID()];

      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
         
        Matrix ptmp = computePartialInvF(reg_group, d, g_idx);
        for(int i=0;i<num_eigen_vectors;i++){
          p_invf[d][i] = ptmp[reg_group][i];
        }
      }
      Matrix comp_mean = mean_vec*weight_zero[0][reg_group] + mean_trans[g_idx]*(1.0-weight_zero[0][reg_group]);
      Matrix tmp_vec(1, dim);
      Matrix diff = mean_vec - mean_trans[g_idx];
      float occ = mix_occ->get(j, m);
      for(int d=0;d<dim;d++){
        tmp_vec[0][d] = (x[t][d] - comp_mean[d][0])*cov_inv[d][d];
        for(int i=0;i<num_eigen_vectors;i++){
          partial_q[reg_group][i] += -2.0*occ*tmp_vec[0][d]*p_invf[d][i];
        }
      }
      partial_q[reg_group][num_eigen_vectors] += -2.0*occ*(tmp_vec*diff)[0][0];
    }
    t++;
  }

  delete mix_occ;
  
  return partial_q;
}

float EiKEV::computeTermA(int reg_group, int row, Matrix& vec){
  float term_a=0.0;
  
  for(int i=0;i<num_spkrs;i++){
    term_a += base_kernel[reg_group*dim+row]->eval(const_norm[reg_group][i][row], vec);
  }
  term_a *= 1.0/num_spkrs;

  return term_a;
}

float EiKEV::computeTermA(int reg_group, int row, int g_idx, CacheTable* cache){
  float term_a=0.0;
  
  for(int i=0;i<num_spkrs;i++){
    //term_a += base_kernel[reg_group*dim+row]->eval(const_norm[reg_group][i][row], vec);
    term_a += cache->get(row, i, g_idx);
  }
  term_a *= 1.0/num_spkrs;

  return term_a;
}

float EiKEV::computeTermB(int reg_group, int row, Matrix& vec, int eigen_idx, const float& term_a){
  float term_b=0.0, tmp;

  for(int i=0;i<num_spkrs;i++){
    tmp = eigen_vector[reg_group][i][eigen_idx];
    tmp*= base_kernel[reg_group*dim+row]->eval(const_norm[reg_group][i][row], vec) - term_a;
    term_b += tmp;
  }

  return term_b;
}

float EiKEV::computeTermB(int reg_group, int row, int g_idx, int eigen_idx, const float& term_a, CacheTable* cache){
  float term_b=0.0, tmp;

  for(int i=0;i<num_spkrs;i++){
    tmp = eigen_vector[reg_group][i][eigen_idx];
    tmp*= cache->get(row, i, g_idx) - term_a;
    term_b += tmp;
  }

  return term_b;
}

void EiKEV::precompute(){
  Matrix zero(dim+1, 1);
  float val;
  CacheTable* cache = new CacheTable(3, dim, num_spkrs, gaussian.size());
  CacheTable* cache_zero = new CacheTable(3, num_reg_groups, dim, num_spkrs);

  term_a_gau->clear();
  term_b_gau->clear();
  term_a_zero->clear();
  term_b_zero->clear();
  
  term_a_gau->makeAvailable();
  term_b_gau->makeAvailable();
  term_a_zero->makeAvailable();
  term_b_zero->makeAvailable();

  fprintf(stderr, "Computing kernel values... %d\n", gaussian.size());
  for(int r=0;r<gaussian.size();r++){
//    printf("r: %d\n", r);
    Matrix mean_off = makeMeanOff(gaussian[r]->getMeanVec());
    int reg_group = label[gaussian[r]->getID()];
    for(int d=0;d<dim;d++){
      for(int i=0;i<num_spkrs;i++){
        float val = base_kernel[reg_group*dim+d]->eval(const_norm[reg_group][i][d], mean_off);

        cache->set(val, d, i, r);
      }
    }
  }
  for(int k=0;k<num_reg_groups;k++){
    for(int d=0;d<dim;d++){
      for(int i=0;i<num_spkrs;i++){
        float val = base_kernel[k*dim+d]->eval(const_norm[k][i][d], zero);
        cache_zero->set(val, k, d, i);
      }
    }
  }

  for(int k=0;k<num_reg_groups;k++){
    for(int d=0;d<dim;d++){
      val = computeTermA(k, d, zero);
      term_a_zero->set(val, k, d);
      for(int v=0;v<num_eigen_vectors;v++){
        val = computeTermB(k, d, zero, v, term_a_zero->get(k, d));
        term_b_zero->set(val, k, d, v);
      }
    }
  }

  fprintf(stderr, "Computing term A & B...\n");
  for(int i=0;i<gaussian.size();i++){
//    printf("i: %d\n", i);
    int reg_group = label[gaussian[i]->getID()];
    for(int d=0;d<dim;d++){
      val = computeTermA(reg_group, d, i, cache);
      term_a_gau->set(val, d, i);
      for(int v=0;v<num_eigen_vectors;v++){
        val = computeTermB(reg_group, d, i, v, term_a_gau->get(d, i), cache);
        term_b_gau->set(val, d, i, v);
      }
    }
  }

  delete cache;
  delete cache_zero;
}

void EiKEV::precomputeSim(){
  Matrix zero(dim+1, 1);
  float val;
  Matrix vec;

  sim_gau->clear();
  sim_zero->clear();
  partial_sim_zero->clear();
  
  sim_gau->makeAvailable();
  sim_zero->makeAvailable();
  partial_sim_zero->makeAvailable();

  for(int i=0;i<gaussian.size();i++){
//    printf("i: %d\n", i);
    int reg_group = label[gaussian[i]->getID()];
    for(int d=0;d<dim;d++){
      val = computeSimilarity(reg_group, d, -1, weight[reg_group]);
      sim_zero->set(val, reg_group, d);
      val = computeSimilarity(reg_group, d, i, weight[reg_group]);
      sim_gau->set(val, d, i);
    }
  }
  for(int d=0;d<dim;d++){
    vec = computePartialSimilarity(0, d, -1);
    for(int v=0;v<num_eigen_vectors;v++)
      partial_sim_zero->set(vec[0][v], 0, d, v);
  }
}

Matrix EiKEV::computePartialSimilarity(int reg_group, int row, int g_idx){
  Matrix result(num_reg_groups, num_eigen_vectors);
  float term_b;

  if(g_idx==-1){
    for(int m=0;m<num_eigen_vectors;m++){
      term_b = term_b_zero->get(reg_group, row, m);
      //result[reg_group][m] = term_b / sqrt(eigen_value[reg_group][0][m]);
      result[reg_group][m] = term_b * inv_sqrt_eigen_value[reg_group][0][m];
    }
  }
  else{
    for(int m=0;m<num_eigen_vectors;m++){
      term_b = term_b_gau->get(row, g_idx, m);
      result[reg_group][m] = term_b * inv_sqrt_eigen_value[reg_group][0][m];
    }
  }

  return result;
}

Matrix EiKEV::computePartialInvF(int reg_group, int row, int g_idx){
  Matrix result(num_reg_groups, num_eigen_vectors), psim(num_reg_groups, num_eigen_vectors), psim_zero(num_reg_groups, num_eigen_vectors);
  float sim, sim_z;

  sim = sim_gau->get(row, g_idx);
  sim_z = sim_zero->get(reg_group, row);
  for(int m=0;m<num_eigen_vectors;m++){
//    psim[reg_group][m] = partial_sim_gau->get(reg_group, row, g_idx, m);
    psim = computePartialSimilarity(reg_group, row, g_idx);
    psim_zero[reg_group][m] = partial_sim_zero->get(reg_group, row, m);
  }

  for(int m=0;m<num_eigen_vectors;m++){
    result[reg_group][m] = -1.0/(2.0*beta);

    result[reg_group][m] *= psim_zero[reg_group][m]/sim_z - psim[reg_group][m]/sim;
  }

  return result;
}

float EiKEV::computeSimilarity(int reg_group, int row, int g_idx, const float* weight){
  float result=0.0, term_a=0.0, term_b=0.0;
  double tmp=0.0;

  if(g_idx==-1){
    term_a = term_a_zero->get(reg_group, row);
    for(int m=0;m<num_eigen_vectors;m++){
      term_b = term_b_zero->get(reg_group, row, m);
      tmp += weight[m]*inv_sqrt_eigen_value[reg_group][0][m]*term_b;
    }
  }
  else{
    term_a = term_a_gau->get(row, g_idx);
    for(int m=0;m<num_eigen_vectors;m++){
      term_b = term_b_gau->get(row, g_idx, m);
      tmp += weight[m]*inv_sqrt_eigen_value[reg_group][0][m]*term_b;
    }
  }
  
  result = term_a + tmp;

  return result;
}

Matrix EiKEV::transformMean(Gaussian* g){
  Matrix mean_off;
  Matrix result(dim, 1);
  int reg_group = label[g->getID()];

  mean_off = makeMeanOff(g->getMeanVec());
  for(int r=0;r<dim;r++){
    float kval = computeSimilarity(reg_group, r, idx2gau_idx[g->getID()], weight[reg_group]);
    result[r][0] = (-1.0/beta)*log(kval)+(1.0/beta)*log(computeSimilarity(reg_group, r, -1, weight[reg_group]))-(mean_off.transpose()*mean_off)[0][0];
    result[r][0] *= -0.5;
  }

  return result;
}

Matrix EiKEV::makeMeanOff(const Matrix& mean){
  Matrix result(dim+1, 1);
/*
  for(int i=0;i<dim;i++){
    result[i][0] = mean.getValue(0, i);
  }
  result[dim][0] = 1.0;
*/

  result[0][0] = 1.0;
  for(int i=1;i<=dim;i++){
    result[i][0] = mean.getValue(0, i-1);
  }

  return result;
}

void EiKEV::getAllGaussians(){
  vector<State*> state_set;
  map<Gaussian*, bool> flag;

  num_mix = 0;
  gaussian.clear();
  label.clear();
  old_mean.clear();
  idx2gau_idx.clear();
  flag.clear();

  hmmset->getStateSet(state_set);
  for(int i=0;i<state_set.size();i++){
    if(state_set[i]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_set[i]->getPDF();
    if(num_mix<mm->numMixture())
      num_mix = mm->numMixture();
    for(int j=0;j<mm->numMixture();j++){
      Gaussian* g = mm->getGaussian(j);
      if(flag[g])
        continue;
       
      gaussian.push_back(g);
      old_mean.push_back(g->getMeanVec());
      idx2gau_idx[g->getID()]=gaussian.size()-1;
      flag[g] = true;
    }
  }
  for(int i=0;i<gaussian.size();i++){
    label[gaussian[i]->getID()] = 0;
  }
  printf("ckpt %d %d\n", state_set.size(), gaussian.size());
}

void EiKEV::updateModel(){
  int r;
  
  for(int i=0;i<gaussian.size();i++){
    r = label[gaussian[i]->getID()];
    Matrix mean_vec = gaussian[i]->getMeanVec().transpose();

    gaussian[i]->setMeanVec(mean_vec*weight_zero[0][r] + mean_trans[i]*(1.0-weight_zero[0][r]));
  }
}

void EiKEV::restoreModel(){
  for(int i=0;i<gaussian.size();i++){
    gaussian[i]->setMeanVec(old_mean[i]);
  }
}
