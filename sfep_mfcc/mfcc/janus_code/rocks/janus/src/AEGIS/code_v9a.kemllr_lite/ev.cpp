#include <fstream>
#include "ev.h"

EV::EV(vector<string>& spkr_filelist, vector<vector<Transcription> >& spkr_trans, MemoryManager* mgr, Recognizer* recognizer, Network* network, HMMSet* hmmset, MFCCReader* reader, int dim, int num_eigen_vectors, const float& learning_rate, const float& threshold){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->recognizer = recognizer;
  this->network = network;
  this->hmmset = hmmset;
  this->reader = reader;
  this->num_spkrs = spkr_filelist.size();
  this->spkr_filelist = spkr_filelist;
  this->spkr_trans = spkr_trans;
  this->dim = dim;
  this->num_eigen_vectors = num_eigen_vectors;
  this->learning_rate = learning_rate;
  this->threshold = threshold;

  getAllGaussians();

  weight_zero = 0.5;

  vector<AegisHMM*> hmm;
  hmmset->getAllHMM(hmm);

  for(int i=0;i<hmm.size();i++){
    all_classes.push_back(hmm[i]->getName());
  }
}

EV::EV(MemoryManager* mgr, Recognizer* recognizer, HMMSet* hmmset, int dim, int num_eigen_vectors){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->recognizer = recognizer;
  this->network = NULL;
  this->hmmset = hmmset;
  this->reader = NULL;
  this->num_spkrs = num_eigen_vectors;
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


EV::~EV(){
}

void EV::load(char* mllr_file){
  FILE* fp;
  char buf[STRMAX];
  vector<Matrix> empty;
  Matrix trans(dim, dim+1);

  mllr_transformation.clear();
  mllr_transformation.push_back(empty);
  
  fp = fopen(mllr_file, "r");

  for(int k=0;k<num_eigen_vectors;k++){
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

void EV::loadHTKMLLR(char* mllr_list){
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

Matrix EV::makeMeanOff(const Matrix& mean){
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


void EV::findAllNeighbours(MFCCReader* reader, vector<string>& adaptlist, vector<vector<Transcription> >& adapt_trans, int num_neighbours, vector<vector<int> >& neigh_idx){
  Network* fa_net;
  float val, max;
  int max_idx;
  vector<vector<float> > score;
  Transcription result;

  neigh_idx.clear();
  neigh_idx.resize(adaptlist.size());
  score.resize(num_spkrs);
  
  for(int i=0;i<num_spkrs;i++){ //for each trn spkr
    for(int r=0;r<gaussian.size();r++){
      Gaussian* g = gaussian[r];
      int reg_group = label[g->getID()];
      Matrix mean_vec = g->getMeanVec();

      mean_vec = mllr_transformation[reg_group][i]*makeMeanOff(mean_vec);
      g->setMeanVec(mean_vec.transpose());
    }
    score[i].resize(adaptlist.size());
    for(int j=0;j<adaptlist.size();j++){
      reader->readFileList(adaptlist[j]); 
      for(int u=0;u<reader->getTotalUtterance();u++){
        fa_net = new Network(mgr);
        reader->readData(u);
        recognizer->forceAlign(adapt_trans[j][u].getSeq(), reader->getX(), reader->getTotalFrames(), result, *fa_net);
        score[i][j]+=recognizer->getLogLikelihood();
        delete fa_net;
      }
    }
    for(int r=0;r<gaussian.size();r++){
      Gaussian* g = gaussian[r];
      g->setMeanVec(old_mean[r]);
    }
  }

  for(int i=0;i<adaptlist.size();i++){
    for(int k=0;k<num_neighbours;k++){
      max_idx = -1;
      max = LZERO;
      for(int j=0;j<num_spkrs;j++){
        if(max_idx==-1 || score[j][i]>max){
          max = score[j][i];
          max_idx = j;
        }
      }

      neigh_idx[i].push_back(max_idx);
      score[max_idx][i] = LZERO;
    }
  }
}

void EV::prepare(vector<int>& neigh_idx){
  Matrix data;
  float factor;
  map<Gaussian*, bool> flag;

  printf("constructing supervec...\n");
  formSuperVec(neigh_idx, data);
  factor = 1.0/num_spkrs;
  super_mean = Matrix(sdim, 1);
  super_std = Matrix(sdim, 1);

  for(int i=0;i<neigh_idx.size();i++){
    super_mean += data.getRowVec(i).transpose();
  }
  super_mean = super_mean*factor;

  for(int i=0;i<neigh_idx.size();i++){
    for(int d=0;d<sdim;d++){
      super_std[d][0] += (data[i][d]-super_mean[d][0])*(data[i][d]-super_mean[d][0]);
    }
  }
  for(int d=0;d<sdim;d++){
    super_std[d][0] = sqrt(super_std[d][0]*factor);
  }
/*
  for(int i=0;i<sd_model_list.size();i++){
    for(int d=0;d<sdim;d++){
      data[i][d] = (data[i][d]-super_mean[d][0])/super_std[d][0];
      //data[i][d] = (data[i][d]-super_mean[d][0]);
    }
  }
*/
//  printf("performing PCA...\n");
//  performPCA(data);

  super_eigen_value = Matrix(1, num_eigen_vectors);
  super_eigen_vector = Matrix(sdim, num_eigen_vectors);
  for(int i=0;i<num_eigen_vectors;i++){
    for(int d=0;d<sdim;d++){
      super_eigen_vector[d][i] = data[i][d];
    }
  }
}

void EV::robustAdapt(float** x, int t_max){
  Matrix A, b;
  Network* fa_net;
  CacheTable* mix_occ;
  vector<State*> state_seq;
  string hmm_name;
  vector<vector<int> > neigh_idx;
  vector<string> adapt_trans;
  adapt_trans.push_back("ahmm");
  Matrix obs(1, dim);

  fprintf(stderr, "Aegis: start adaptation using RSW...\n");

  A = Matrix(num_eigen_vectors, num_eigen_vectors);
  b = Matrix(num_eigen_vectors, 1);

  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);

  recognizer->setNetwork(fa_net);
  recognizer->process(x, t_max);
  recognizer->decode(state_seq);
  mix_occ = new CacheTable(2, state_seq.size(), num_mix);
  recognizer->computeVitMixOcc(state_seq, x, t_max, mix_occ);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    //printf("j: %d\n", j);
    if(state_seq[j]->isNullNode())
      continue;
    for(int d=0;d<dim;d++){
      obs[0][d] = x[t][d];
    }
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      float exp_mix_occ = mix_occ->get(j, m);
      if(exp_mix_occ<0.001)
        continue;

      Gaussian* g = mm->getGaussian(m);
      Matrix mean_off = makeMeanOff(g->getMeanVec());
      //vector<Matrix> v;
      Matrix v_mat(num_eigen_vectors, dim);
      Matrix p_mat(dim, num_eigen_vectors);

/*
      v.clear();
      for(int k=0;k<num_eigen_vectors;k++){
        v.push_back(mllr_transformation[0][k]*mean_off);
      }
*/
      for(int k=0;k<num_eigen_vectors;k++){
        Matrix tmp = mllr_transformation[0][k]*mean_off;
        for(int d=0;d<dim;d++){
          v_mat[k][d] = tmp[d][0];
          p_mat[d][k] = tmp[d][0]*g->getCovAt(d, d);
        }
      }
      /*
      for(int k=0;k<num_eigen_vectors;k++){
        for(int d=0;d<dim;d++){
          b[k][0] += x[t][d]*g->getCovAt(d, d)*v[k][d][0]*exp_mix_occ;
        }              
      }
      */
      b += (obs*p_mat*exp_mix_occ).transpose();
      A += (v_mat*p_mat*exp_mix_occ).transpose();
      /*
      for(int k=0;k<num_eigen_vectors;k++){
        for(int l=k;l<num_eigen_vectors;l++){
          float dot_product = 0.0;
          for(int d=0;d<dim;d++){
            dot_product += v[k][d][0] * v[l][d][0] * g->getCovAt(d, d);
          }
          float val = dot_product*exp_mix_occ;
          A[k][l] += val;
          if(l!=k)
            A[l][k] += val;
        }
      }
      */
    }
    t++;
  }      

  delete mix_occ;
  delete fa_net;

  //Matrix tmp = A.extInverse()*b;
  Matrix tmp = A.dPseudoInverse()*b;
//  Matrix tmp = A.inverse()*b;
  weight = Matrix(num_eigen_vectors+1, 1);
  for(int k=0;k<num_eigen_vectors;k++){
    weight[k][0] = tmp[k][0];
  }
  weight[num_eigen_vectors][0] = 0.0;
  computeTransformation();

  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);

  fprintf(stderr, "before RSW: %f\n", computeSIQFunc(fa_net, x, t_max));
  fprintf(stderr, "after RSW: %f\n", computeRobustQFunc(fa_net, x, t_max));

  delete fa_net;

/*
  new_mean.clear();
  for(int r=0;r<gaussian.size();r++){
    new_mean.push_back(gaussian[r]->getMeanVec().transpose());
  }

  float acc_num = 0.0, acc_den = 0.0;
  fprintf(stderr, "computing w0...\n");

  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);
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
      if(mix_occ->get(j, m)<0.001)
        continue;

      Gaussian* g = mm->getGaussian(m);
      Matrix vec, obs(dim, 1);
      int g_idx = gau2gau_idx[g->getID()];
      float exp_mix_occ = mix_occ->get(j, m);

      vec = old_mean[g_idx].transpose() - new_mean[g_idx];
      for(int d=0;d<dim;d++){
        vec[d][0] *= g->getCovAt(d, d);
        obs[d][0] = x[t][d];
      }
      acc_num += (vec.transpose()*(obs - new_mean[g_idx]))[0][0]*exp_mix_occ;
      acc_den += (vec.transpose()*(old_mean[g_idx].transpose() - new_mean[g_idx]))[0][0]*exp_mix_occ;
    }
    t++;
  }      

  delete mix_occ;
  delete fa_net;


  Matrix tmp(num_eigen_vectors+1, 1);
  for(int k=0;k<num_eigen_vectors;k++)
    tmp[k][0] = weight[k][0];
  tmp[num_eigen_vectors][0] = acc_num/acc_den;
  if(tmp[num_eigen_vectors][0]>1.0)
    tmp[num_eigen_vectors][0] = 1.0;
  else if(tmp[num_eigen_vectors][0]<0.0)
    tmp[num_eigen_vectors][0] = 0.0;
  float factor = tmp[num_eigen_vectors][0];
  weight = tmp;
  weight.transpose().printFP(stderr);

  for(int r=0;r<gaussian.size();r++){
    gaussian[r]->setMeanVec(old_mean[r].transpose()*(factor)+ new_mean[r]*(1.0 - factor));
  }
*/
/*
  fprintf(stderr, "evaluating...\n");

  fa_net = new Network(mgr);
  recognizer->concatHMM(adapt_trans, *fa_net);

  fprintf(stderr, "QFunc: %lf\n", computeSIQFunc(fa_net, x, t_max));

  delete fa_net;
*/
}

void EV::performPCA(Matrix& data){
  float factor = 1.0/data.getNumRow();
  Matrix tmp;
  
  MathEngine::deconSVD(data, tmp, super_eigen_value, super_eigen_vector);
//  MathEngine::extSVD(data, tmp, super_eigen_value, super_eigen_vector);
  super_eigen_value.print();
  super_eigen_vector = super_eigen_vector.copy(0, 0, num_eigen_vectors-1, sdim-1);
  super_eigen_vector = super_eigen_vector.transpose();
}

void EV::formSuperVec(const vector<int>& neigh_idx, Matrix& data){
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

void EV::getAllGaussians(){
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

float EV::computeQFunc(Network* fa_net, float** x, int t_max){
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

float EV::computeRobustQFunc(Network* fa_net, float** x, int t_max){
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

//  weight[num_eigen_vectors][0] = 1.0;
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
      Matrix trans_mean_vec = g->getMeanVec().transpose()*weight[num_eigen_vectors][0]+mean_trans[g_idx]*(1.0-weight[num_eigen_vectors][0]);
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

float EV::computeSIQFunc(Network* fa_net, float** x, int t_max){
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


void EV::computeTransformation(){
  Matrix new_mean(dim, 1);
  vector<State*> state_set;
  
  mean_trans.clear();
  mean_trans.resize(gaussian.size());

  for(int r=0;r<gaussian.size();r++){
    Gaussian* g = gaussian[r];
    Matrix mean_off = makeMeanOff(g->getMeanVec());
    int g_idx = gau2gau_idx[g->getID()];
    for(int k=0;k<num_eigen_vectors;k++){
      Matrix ek = mllr_transformation[0][k]*mean_off;
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

void EV::updateModel(){
  int j, m;
  
  for(int i=0;i<gaussian.size();i++){
    if(mean_trans[i].getNumRow()==0 && mean_trans[i].getNumCol()==0)
      continue;
    gaussian[i]->setMeanVec(mean_trans[i]);
  }
}

void EV::updateRobustModel(vector<Gaussian*>& gaussian){
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

void EV::updateRobustModel(){
  int j, m;
  Matrix new_mean(dim, 1);

  for(int i=0;i<gaussian.size();i++){
    if(mean_trans[i].getNumRow()==0 && mean_trans[i].getNumCol()==0)
      continue;
    gaussian[i]->setMeanVec(gaussian[i]->getMeanVec().transpose()*weight[num_eigen_vectors][0] + mean_trans[i]*(1.0-weight[num_eigen_vectors][0]));
  }
}

void EV::restoreModel(){
  for(int i=0;i<gaussian.size();i++){
    gaussian[i]->setMeanVec(old_mean[i]);
  }
}
