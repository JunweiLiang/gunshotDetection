#include <fstream>
#include "rsw_ext.h"

RSWExt::RSWExt(MemoryManager* mgr, Recognizer* recognizer, HMMSet* hmmset, int dim, int num_spkrs, int num_eigen_vectors){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->recognizer = recognizer;
  this->network = NULL;
  this->hmmset = hmmset;
  this->reader = NULL;
  this->num_spkrs = num_spkrs;
  this->dim = dim;
  this->num_eigen_vectors = num_eigen_vectors;
  this->learning_rate = 0.0;
  this->threshold = 0.0;

  getAllGaussians();

  weight_zero = 0.5;

  vector<AegisHMM*> hmm;
  hmmset->getAllHMM(hmm);

  for(int i=0;i<hmm.size();i++){
    all_classes.push_back(hmm[i]->getName());
  }
}


RSWExt::~RSWExt(){
}

void RSWExt::load(char* mllr_file){
  FILE* fp;
  char buf[STRMAX];
  vector<Matrix> empty;
  Matrix trans(dim, dim+1);

  mllr_transformation.clear();
  mllr_transformation.push_back(empty);
  
  fp = fopen(mllr_file, "r");

  for(int k=0;k<num_spkrs;k++){
    for(int i=0;i<dim;i++){
      for(int j=0;j<dim+1;j++){
        float val;
        fscanf(fp, "%f", &val);

        trans[i][j] = val;
      }
    }
    mllr_transformation[0].push_back(trans);
  }
  
  fclose(fp);
}

void RSWExt::loadHTKMLLR(char* mllr_list){
  FILE *fp1, *fp2;
  char buf[STRMAX], tag[STRMAX], field[STRMAX];
  float val;
  int reg_group=0, spkr=0, num_reg_groups=0;
  vector<bool> tnode;
  map<int, int> tnode2reg_group;
  bool ignore=false;

  printf("Loading MLLR transformations...\n");

  for(int r=0;r<gaussian.size();r++){
    int rclass;
    info->getAssociateValue("Gau2RClass", gaussian[r]->getID(), (Ptr)&rclass);

    if(rclass>num_reg_groups)
      num_reg_groups = rclass;
  }
  num_reg_groups = (num_reg_groups+2)/2;
 
  label.clear(); 
  tnode.clear();
  tnode2reg_group.clear();
  tnode.resize(2*num_reg_groups-1);
  for(int r=0;r<gaussian.size();r++){
    int rclass;
    info->getAssociateValue("Gau2RClass", gaussian[r]->getID(), (Ptr)&rclass);
    tnode[rclass]=true;
  }
  int counter=0;
  for(int i=0;i<2*num_reg_groups-1;i++){
    if(tnode[i]){
      tnode2reg_group[i] = counter;
      counter++;
    }
  }
  for(int r=0;r<gaussian.size();r++){
    int rclass;
    info->getAssociateValue("Gau2RClass", gaussian[r]->getID(), (Ptr)&rclass);
    label[gaussian[r]->getID()] = tnode2reg_group[rclass];
  }
  mllr_transformation.clear();
  mllr_transformation.resize(num_reg_groups);
  for(int r=0;r<num_reg_groups;r++){
    mllr_transformation[r].resize(num_spkrs);
    for(int i=0;i<num_spkrs;i++){
      mllr_transformation[r][i] = Matrix(dim, dim+1);
    }
  }

  fp1 = fopen(mllr_list, "r");
  for(int i=0;i<num_spkrs;i++){
    fscanf(fp1, "%s", buf);
    fp2 = fopen(buf, "r");
    while(true){
      fgets(buf, STRMAX, fp2);

      if(feof(fp2))
        break;
      if(strlen(buf)==0)
        continue;
      if(buf[strlen(buf)-1]=='\n')
        buf[strlen(buf)-1] = 0;
      if(strcmp(buf, "")==0)
        continue;

      extractField(buf, 0, " \t\n", tag);

      if(strcmp(tag, "<TRANSFORM>")==0){
        extractField(buf, 1, " \t\n", field);
        reg_group = atoi(field);
        if(num_reg_groups==1){
          reg_group--;
        }
        else{
          reg_group--;
          if(reg_group==0){
            ignore = true;
            continue;
          }
          reg_group = tnode2reg_group[reg_group];
          ignore = false;
        }
      }
      else if(strcmp(tag, "<BLOCK>")==0){
        if(ignore)
          continue;
        for(int j=0;j<dim;j++){
          for(int k=0;k<dim;k++){
            fscanf(fp2, "%f", &val);
            mllr_transformation[reg_group][i][j][k] = val;
          }
        }
      }
      else if(strcmp(tag, "<BIASOFFSET>")==0){
        if(ignore)
          continue;
        for(int j=0;j<dim;j++){
          fscanf(fp2, "%f", &val);
          mllr_transformation[reg_group][i][j][dim] = val;
        }
      }
    }
    fclose(fp2);
  }
  fclose(fp1);
}

Matrix RSWExt::makeMeanOff(const Matrix& mean){
  Matrix result(dim+1, 1);

  result[0][0] = 1.0;
  for(int i=1;i<=dim;i++){
    result[i][0] = mean.getValue(0, i-1);
  }

/*
  result[dim][0] = 1.0;
  for(int i=0;i<dim;i++){
    result[i][0] = mean.getValue(0, i);
  }
*/
  return result;
}

void RSWExt::robustAdapt(float** x, int t_max){
  Network* fa_net;
  CacheTable* mix_occ;
  vector<State*> state_seq;
  vector<string> adapt_trans;
  deque<Matrix> collection;
  vector<Matrix> working_set;
  Matrix id(dim, dim+1);
  float si_q;
  int count=0;
  vector<Gaussian*> all_gaussians;
  vector<Matrix> all_means;
  vector<float> exp_mix_occ;
  vector<int> time;
  
  for(int i=0;i<dim;i++){
    id[i][i+1] = 1.0;
  }

  adapt_trans.push_back("ahmm");
  
  fprintf(stderr, "Aegis: start adaptation using RSW Extension\n");
  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);
  recognizer->setNetwork(fa_net);
  recognizer->process(x, t_max);
  recognizer->decode(state_seq);
  mix_occ = new CacheTable(2, state_seq.size(), t_max);
  recognizer->computeVitMixOcc(state_seq, x, t_max, mix_occ);

  for(int i=0;i<mllr_transformation[0].size();i++){
    collection.push_back(mllr_transformation[0][i]);
  }

  si_q = computeSIQFunc(fa_net, x, t_max);
  fprintf(stderr, "Aegis (RSW): SI Qfunc: %f\n", si_q);
  if(si_q==0.0){
    fprintf(stderr, "Aegis (RSW): Error in computing Q, falling back...\n", si_q);
    working_set.clear();
    working_set.push_back(id);
    Matrix w(1, 1);
    w[0][0] = 1.0;
    computeTransformation(working_set, w);
    delete fa_net;
    delete mix_occ;
    return;
  }
 
  time.clear();
  all_gaussians.clear();
  all_means.clear();
  exp_mix_occ.clear();
  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      if(mix_occ->get(j, m)<0.001)
        continue;

      Gaussian* g = mm->getGaussian(m);
      time.push_back(t);
      all_gaussians.push_back(g);
      all_means.push_back(makeMeanOff(g->getMeanVec()));
      exp_mix_occ.push_back(mix_occ->get(j, m));
    }
    t++;
  }
  fprintf(stderr, "Aegis (RSW): #Gaussians = %d\n", all_gaussians.size());


  while(collection.size()>1){
    working_set.clear();
    int working_size = num_eigen_vectors;
    float q;

    if(working_size > collection.size())
      working_size = collection.size();

    for(int i=0;i<working_size;i++){
      working_set.push_back(collection[0]);
      collection.pop_front();
    }
    working_set.push_back(id);
    working_size++;

    Matrix result = optimize(all_gaussians, all_means, exp_mix_occ, time, x, t_max, working_set);
    q = computeRobustQFunc(fa_net, state_seq, mix_occ, x, t_max);
    if(q <= si_q){
      collection.push_back(result);
    }
    else if(q > si_q && collection.size()==0){
      collection.push_back(id);
      working_set.clear();
      working_set.push_back(id);
      Matrix w(1, 1);
      w[0][0] = 1.0;
      computeTransformation(working_set, w);
    }
    fprintf(stderr, "Aegis (RSW): Set %d (size %d), Qfunc: %f\n", count, working_size, q);
    count++;
  }

  fprintf(stderr, "Aegis (RSW): Opt Qfunc: %f\n", computeRobustQFunc(fa_net, x, t_max));
 
  delete fa_net;
  delete mix_occ;
}

Matrix RSWExt::optimize(vector<Gaussian*>& all_gaussians, vector<Matrix>& all_means, vector<float>& exp_mix_occ, vector<int>& time, float** x, int t_max, vector<Matrix>& mllr_transformation){
  Matrix A, b;
  int num_transform = mllr_transformation.size();
  Matrix result(dim, dim+1), obs(1, dim);
  Matrix v_mat(num_transform, dim);
  Matrix p_mat(dim, num_transform);
  vector<vector<Matrix> > mean_trans;

  A = Matrix(num_transform, num_transform);
  b = Matrix(num_transform, 1);

  mean_trans.resize(num_transform);
  for(int k=0;k<num_transform;k++){
    for(int r=0;r<all_gaussians.size();r++){
      mean_trans[k].push_back(mllr_transformation[k]*all_means[r]);
    }
  }

  for(int r=0;r<all_gaussians.size();r++){
    Gaussian* g = all_gaussians[r];
    int t = time[r];

    for(int d=0;d<dim;d++){
      obs[0][d] = x[t][d];
    }

    for(int k=0;k<num_transform;k++){
      for(int d=0;d<dim;d++){
        v_mat[k][d] = mean_trans[k][r][d][0];
        p_mat[d][k] = v_mat[k][d]*g->getCovAt(d, d);
      }
    }

    b += (obs*p_mat*exp_mix_occ[r]).transpose();
    A += (v_mat*p_mat*exp_mix_occ[r]).transpose();
  }      

  //Matrix tmp = A.dPseudoInverse()*b;
  Matrix tmp = A.dInverse()*b;
  weight = Matrix(num_transform+1, 1);
  for(int k=0;k<num_transform;k++){
    weight[k][0] = tmp[k][0];
  }
  weight[num_transform][0] = 0.0;
  computeTransformation(mllr_transformation, weight);

  for(int k=0;k<num_transform;k++){
    result += mllr_transformation[k]*weight[k][0];
  }

  return result;
}

void RSWExt::performPCA(Matrix& data){
  float factor = 1.0/data.getNumRow();
  Matrix tmp;
  
  MathEngine::deconSVD(data, tmp, super_eigen_value, super_eigen_vector);
//  MathEngine::extSVD(data, tmp, super_eigen_value, super_eigen_vector);
  super_eigen_value.print();
  super_eigen_vector = super_eigen_vector.copy(0, 0, num_eigen_vectors-1, sdim-1);
  super_eigen_vector = super_eigen_vector.transpose();
}

void RSWExt::formSuperVec(const vector<int>& neigh_idx, Matrix& data){
  vector<State*> state_set;
  map<Gaussian*, bool> flag;
  int count;

  data = Matrix(neigh_idx.size(), sdim);
  
  for(int i=0;i<neigh_idx.size();i++){
    count=0;
    for(int r=0;r<gaussian.size();r++){
      Gaussian* g = gaussian[r];
      Matrix mean_vec = g->getMeanVec();
      int reg_group = label[g->getID()];

      mean_vec = mllr_transformation[reg_group][neigh_idx[i]]*makeMeanOff(mean_vec);
      for(int d=0;d<dim;d++){
        data[i][count] = mean_vec[d][0];
        count++;
      }
    }
  }
}

void RSWExt::getAllGaussians(){
  vector<State*> state_set;
  map<Gaussian*, bool> flag;

  num_mix = 0;
  gaussian.clear();
  old_mean.clear();
  sdim = 0;

  for(int k=0;k<hmmset->getNumHMM();k++){
    AegisHMM* h = hmmset->getHMM(k);
    h->getStateSet(state_set);
    for(int i=0;i<state_set.size();i++){
      if(state_set[i]->isNullNode())
        continue;
      MultiMix* mm = (MultiMix*) state_set[i]->getPDF();
      if(mm->numMixture()>num_mix)
        num_mix = mm->numMixture();
      for(int j=0;j<mm->numMixture();j++){
        Gaussian* g = mm->getGaussian(j);

        if(flag[g])
          continue;
          
        sdim += dim;

        gaussian.push_back(g);
        old_mean.push_back(g->getMeanVec());
        gau2gau_idx[g->getID()] = gaussian.size()-1;
        flag[g] = true;
      }
    }
  }
}

float RSWExt::computeQFunc(Network* fa_net, float** x, int t_max){
  float result=0.0;
  CacheTable* mix_occ;
  vector<State*> state_set;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  int state_idx, mix_idx, g_idx;

  fa_net->getStateSet(state_set);
  mix_occ = new CacheTable(3, t_max, state_set.size(), num_mix);

  recognizer->setNetwork(fa_net);
  recognizer->computeMixOcc(x, t_max, mix_occ);

  for(int j=0;j<state_set.size();j++){
    if(state_set[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_set[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      Gaussian* g = mm->getGaussian(m);
      Matrix mean_vec = g->getMeanVec().transpose();

      g_idx = gau2gau_idx[g->getID()];
      
      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
      }
      Matrix trans_mean_vec = mean_trans[g_idx];
      for(int t=0;t<t_max;t++){
        for(int d=0;d<dim;d++){
          obs[d][0] = x[t][d];
        }
        Matrix vec = obs - trans_mean_vec;
        Matrix tmp_vec(1, dim);
        for(int d=0;d<dim;d++)
          tmp_vec[0][d] = vec[d][0]*cov_inv[d][d];
        result += (tmp_vec*vec)[0][0]*mix_occ->get(t, j, m);
      }
    }
  }

  delete mix_occ;

  return result;
}

float RSWExt::computeRobustQFunc(Network* fa_net, float** x, int t_max){
  float result=0.0;
  CacheTable* mix_occ;
  vector<State*> state_seq;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  Matrix tmp_vec(1, dim);

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
      int g_idx = gau2gau_idx[g->getID()];
      
      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
        obs[d][0] = x[t][d];
      }
      Matrix trans_mean_vec = mean_trans[g_idx];
      Matrix vec = obs - trans_mean_vec;
      for(int d=0;d<dim;d++)
        tmp_vec[0][d] = vec[d][0]*cov_inv[d][d];
      result += (tmp_vec*vec)[0][0]*mix_occ->get(j, m);
    }
    t++;
  }

  delete mix_occ;

  return result;
}

float RSWExt::computeRobustQFunc(Network* fa_net, vector<State*>& state_seq, CacheTable* mix_occ, float** x, int t_max){
  float result=0.0;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  Matrix tmp_vec(1, dim);

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
      int g_idx = gau2gau_idx[g->getID()];
      
      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
        obs[d][0] = x[t][d];
      }
      Matrix trans_mean_vec = mean_trans[g_idx];
      Matrix vec = obs - trans_mean_vec;
      for(int d=0;d<dim;d++)
        tmp_vec[0][d] = vec[d][0]*cov_inv[d][d];
      result += (tmp_vec*vec)[0][0]*mix_occ->get(j, m);
    }
    t++;
  }

  return result;
}


float RSWExt::computeSIQFunc(Network* fa_net, float** x, int t_max){
  float result=0.0;
  CacheTable* mix_occ;
  vector<State*> state_seq;
  Matrix obs(dim, 1), cov_inv(dim, dim);
  Matrix tmp_vec(1, dim);

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
      int g_idx = gau2gau_idx[g->getID()];
      
      for(int d=0;d<dim;d++){
        cov_inv[d][d] = g->getCovAt(d, d);
        obs[d][0] = x[t][d];
      }
      Matrix trans_mean_vec = g->getMeanVec().transpose();
      Matrix vec = obs - trans_mean_vec;
      for(int d=0;d<dim;d++)
        tmp_vec[0][d] = vec[d][0]*cov_inv[d][d];
      result += (tmp_vec*vec)[0][0]*mix_occ->get(j, m);
    }
    t++;
  }

  delete mix_occ;

  return result;

}


void RSWExt::computeTransformation(vector<Matrix>& mllr_transformation, Matrix& weight){
  Matrix new_mean(dim, 1);
  vector<State*> state_set;
  
  mean_trans.clear();
  mean_trans.resize(gaussian.size());

  for(int r=0;r<gaussian.size();r++){
    Gaussian* g = gaussian[r];
    Matrix mean_off = makeMeanOff(g->getMeanVec());
    int g_idx = gau2gau_idx[g->getID()];
    for(int k=0;k<mllr_transformation.size();k++){
      Matrix ek = mllr_transformation[k]*mean_off;
      new_mean += ek*weight[k][0];
    }

    mean_trans[g_idx] = new_mean;
    new_mean = Matrix(dim, 1);
  }
  /*
  fprintf(stderr, "old mean\n");
  gaussian[0]->getMeanVec().printFP(stderr);
  fprintf(stderr, "adapted mean\n");
  mean_trans[0].transpose().printFP(stderr);
  */
}

void RSWExt::updateModel(){
  int j, m;
  
  for(int i=0;i<gaussian.size();i++){
    if(mean_trans[i].getNumRow()==0 && mean_trans[i].getNumCol()==0)
      continue;
    gaussian[i]->setMeanVec(mean_trans[i]);
  }
}

void RSWExt::updateRobustModel(vector<Gaussian*>& gaussian){
  int j, m;
  Matrix new_mean(dim, 1);
  
  for(int r=0;r<gaussian.size();r++){
    Gaussian* g = gaussian[r];
    Matrix mean_off = makeMeanOff(g->getMeanVec());
    for(int k=0;k<num_eigen_vectors;k++){
      Matrix ek = mllr_transformation[0][k]*mean_off;
      new_mean += ek*weight[k][0];
    }

    g->setMeanVec(new_mean.transpose());
    new_mean = Matrix(dim, 1);
  }
/*  
  for(int i=0;i<gaussian.size();i++){
    if(mean_trans[i].getNumRow()==0 && mean_trans[i].getNumCol()==0)
      continue;
    gaussian[i]->setMeanVec(gaussian[i]->getMeanVec().transpose()*weight[num_eigen_vectors][0] + mean_trans[i]*(1.0-weight[num_eigen_vectors][0]));
  }
*/
}

void RSWExt::updateRobustModel(){
  int j, m;
  Matrix new_mean(dim, 1);

  for(int i=0;i<gaussian.size();i++){
    if(mean_trans[i].getNumRow()==0 && mean_trans[i].getNumCol()==0)
      continue;
    gaussian[i]->setMeanVec(mean_trans[i]);
  }
}

void RSWExt::restoreModel(){
  for(int i=0;i<gaussian.size();i++){
    gaussian[i]->setMeanVec(old_mean[i]);
  }
}
