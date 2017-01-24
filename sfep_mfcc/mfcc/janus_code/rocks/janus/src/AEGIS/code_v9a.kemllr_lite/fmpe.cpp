#include "fmpe.h"
#include "omp.h"

#define WINDOW_SIZE 3

FMPE::FMPE(SenoneSet* sns, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float E, float scale, int dim, int num_classes){
  this->sns = sns;
  this->cbs = cbs;
  this->dss = dss;
  this->fs = fs;
  this->svmap = svmap;
  this->E = E;
  this->scale = scale;
  this->dim = dim;
  this->turn = 1;
  this->num_classes = 0;
  this->is_enabled = false;
  this->num_classes = num_classes;
  this->num_idx = 1;
  this->den_idx = 2;

  getClusterCBS();
}

void FMPE::setCBAIdx(int num_idx, int den_idx){
  this->num_idx = num_idx;
  this->den_idx = den_idx;
}

void FMPE::setStatus(bool is_enabled){
  this->is_enabled = is_enabled;
}

void FMPE::getClusterCBS(){
  int num_clusters = 0;

  if(cluster.size()!=0){
    for(int i=0;i<cluster.size();i++){
      if(cluster[i]!=NULL)
        delete cluster[i];
    }
    cluster.clear();
  }

  cluster.resize(num_classes);
  for(int r=0;r<num_classes;r++){
    Gaussian* g = new DiagGaussian("", dim);
    Matrix mean(1, dim), cov(1, dim);
    mean.zero();
    cov.zero();
    int count = 0;
    for(int i=cbs->list.itemN/num_classes*r;i<cbs->list.itemN/num_classes*(r+1);i++){
      Codebook* cb = cbs->list.itemA + i;
      for(int m=0;m<cb->refN;m++){
        for(int d=0;d<dim;d++){
          mean[0][d] += cb->rv->matPA[m][d];
          count++;
        }
      }
    }
    for(int d=0;d<dim;d++){
      mean[0][d] /= (float) count;
      cov[0][d] = 1.0;
    }
    g->setMeanVec(mean);
    g->setCovMat(cov);
    cluster[r] = g;
  }

  fprintf(stderr, "no clusters: %d\n", cluster.size());

  sqrt_cov_inv = Matrix(cluster.size(), dim);
  for(int i=0;i<cluster.size();i++){
    for(int d=0;d<dim;d++){
      sqrt_cov_inv[i][d] = sqrt(cluster[i]->getCovAt(d, d));
    }
  }

  num_classes = cluster.size();

  initTransformation();
  clearAccu();
}
/*
Matrix FMPE::computePosterior(float** x, int t_max){
  Matrix post_mat(t_max, cluster.size());
  int offset = 6;
  float beam = 60;
  float imp_thresh=5.0;
  int num_classes = cluster.size();
  Matrix likelihood1(1, num_classes), likelihood2(1, num_classes);
  Matrix& prev_like = likelihood1;
  Matrix& cur_like = likelihood2;
  Matrix a_vec(1, dim), b_vec(1, dim);
  int max_idx = -1;
  float max_score = 0.0;
  float risk_factor = 0.1;

  for(int r=0;r<num_classes;r++){
    prev_like[0][r] = LZERO;
  }
  for(int t=0;t<t_max;t++){
    if(t%offset==0){
      max_idx = -1;
      max_score = 0.0;
      float norm = LZERO;
      for(int r=0;r<num_classes;r++){
        cur_like[0][r] = cluster[r]->logBj(x[t]) = cluster_weight[0][r];
        norm = LogAdd(norm, cur_like[0][r]);
        if(max_idx==-1 || max_score < cur_like[0][r]){
          max_idx = r;
          max_score = cur_like[0][r];
        }
      }
      for(int r=0;r<num_classes;r++){
        post_mat[t][r] = exp(cur_like[0][r] - norm);
      }
    } else {
      float upper_bound = 0.0;
      float u_d=0.0;
      vector<int> non_p;
      vector<int> selected_gau;
      for(int d=0;d<dim;d++){
        a_vec[0][d] = 0.5*(x[t-1][d]*x[t-1][d] - x[t][d]*x[t][d]);
        b_vec[0][d] = x[t][d] - x[t-1][d];
        if(b_vec[0][d] >= 0.0){
          u_d = (a_vec[0][d] + b_vec[0][d]*cluster_mean_max)*cluster_cov_max;
        } else {
          u_d = (a_vec[0][d] + b_vec[0][d]*cluster_mean_min)*cluster_cov_max;
        }
        if(u_d <= imp_thresh){
          upper_bound += u_d;
        } else {
          non_p.push_back(d);
        }
      }
      float norm = LZERO;
      for(int r=0;r<num_classes;r++){
        float p_bound = 0.0;
        if(prev_like[0][r]<LSMALL){
          cur_like[0][r] = LZERO;
          continue;
        }
        for(int i=0;i<non_p.size();i++){
          int d = non_p[i];
          float pr_change = (a_vec[0][d] + b_vec[0][d]*cluster_mean[r][d])*cluster_cov[r][d];
          p_bound += pr_change;
        }
        if(prev_like[0][r] + upper_bound*(((float)dim-non_p.size())/dim)*risk_factor + p_bound < prev_like[0][max_idx] - beam){
          cur_like[0][r] = LZERO;
          continue;
        }
        cur_like[0][r] = cluster[r]->logBj(x[t]);
        norm = LogAdd(norm, cur_like[0][r]);
        selected_gau.push_back(r);
      }
      max_idx = -1;
      max_score = 0.0;
      for(int i=0;i<selected_gau.size();i++){
        int r = selected_gau[i];
        post_mat[t][r] = exp(cur_like[0][r] - norm);
        if(max_idx==-1 || max_score < cur_like[0][r]){
          max_idx = r;
          max_score = cur_like[0][r];
        }
      }
    }
    swap(prev_like, cur_like);
  }
  return post_mat;
}
*/

Matrix FMPE::computePosterior(float** x, int t_max){
  Matrix post_mat(t_max, cluster.size());
  vector<float> likelihood;

  likelihood.resize(cluster.size());

  for(int t=0;t<t_max;t++){
    float norm = LZERO;
    for(int i=0;i<cluster.size();i++){
      float score = cluster[i]->logBj(x[t]) + cluster_weight[0][i];
      if(!isnormal(score) || score <= LSMALL){
        likelihood[i] = LZERO;
        continue;
      }
      likelihood[i] = score;
      norm = LogAdd(norm, score);
    }

    for(int i=0;i<cluster.size();i++){
      if(likelihood[i] <= LSMALL){
        post_mat[t][i] = 0.0;
        continue;
      }
      post_mat[t][i] = exp(likelihood[i] - norm);
    }
  }

  likelihood.clear();

  return post_mat;
}

void FMPE::loadAccu(char* header){
  char filename[STRMAX];
  FILE* fp;
  Matrix m1_tmp, m2_tmp;

  sprintf(filename, "%s.accu.mat", header);
  if(m1_direct_accu.size()==0){
    clearAccu();
  }
  fp = fopen(filename, "rb");

  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_tmp.bload(fp);
    m1_direct_accu[i] += m1_tmp;
  }

  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_tmp.bload(fp);
    m1_indirect_accu[i] += m1_tmp;
  }

  fclose(fp);

}

void FMPE::saveAccu(char* header){
  char filename[STRMAX];
  FILE* fp;

  sprintf(filename, "%s.accu.mat", header);
  fp = fopen(filename, "wb");

  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_direct_accu[i].bsave(fp);
  }
  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_direct_accu[i].bsave(fp);
  }

  fclose(fp);
}

void FMPE::loadTransforms(char* header){
  char filename[STRMAX];
  FILE* fp;

  fprintf(stderr, "Loading transformations...\n");
  sprintf(filename, "%s.fmpe.mat", header);
  fp = fopen(filename, "rb");
  for(int i=0;i<2*WINDOW_SIZE+1;i++)
    m1_transform[i].bload(fp);

  fclose(fp);
}

void FMPE::saveTransforms(char* header){
  char filename[STRMAX];
  FILE* fp;

  fprintf(stderr, "Saving transformations...\n");
  sprintf(filename, "%s.fmpe.mat", header);
  fp = fopen(filename, "wb");
  for(int i=0;i<2*WINDOW_SIZE+1;i++)
    m1_transform[i].bsave(fp);

  fclose(fp);
}

void FMPE::saveCluster(char* filename){
  FILE* fp = fopen(filename, "wb");
  for(int r=0;r<num_classes;r++){
    cluster[r]->getMeanVec().bsave(fp);
    cluster[r]->getCovMat().bsave(fp);
  }
  cluster_weight.bsave(fp);
  fclose(fp);
}

void FMPE::loadCluster(char* filename){
  FILE* fp = fopen(filename, "rb");
  Matrix tmp;
  bool flag = false;

  for(int r=0;r<num_classes;r++){
    tmp.bload(fp);
    if(feof(fp)){
      flag = true;
      break;
    }
    cluster[r]->setMeanVec(tmp);
    tmp.bload(fp);
    cluster[r]->setCovMat(tmp);
    cluster[r]->calculateGConst();
  }

  if(flag){
    fprintf(stderr, "WARNING: %s does not have %d clusters\n", filename, num_classes);
  }

  cluster_weight.bload(fp);
  sqrt_cov_inv = Matrix(cluster.size(), dim);

  for(int i=0;i<cluster.size();i++){
    for(int d=0;d<dim;d++){
      sqrt_cov_inv[i][d] = sqrt(cluster[i]->getCovAt(d, d));
    }
  }

  fclose(fp);
}

Matrix FMPE::fmpeMul(Matrix& m1_transform, Matrix& h_raw, int t){
  Matrix result;

  return result;
}

void FMPE::initTransformation(){
  initM1();
}

void FMPE::clearAccu(){
  fprintf(stderr, "FMPE: clearing accus...\n");
  m1_direct_accu.clear();
  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_direct_accu.push_back(Matrix(dim, (dim+1)*cluster.size()));
  }
  m1_indirect_accu.clear();
  for(int i=0;i<2*WINDOW_SIZE+1;i++){
    m1_indirect_accu.push_back(Matrix(dim, (dim+1)*cluster.size()));
  }
}

void FMPE::initM1(){
  //m1_transform = Matrix(9*dim, (dim+1)*cluster.size());
  m1_transform.clear();
  for(int i=0;i<2*WINDOW_SIZE+1;i++)
    m1_transform.push_back(Matrix(dim, (dim+1)*cluster.size()));
}

void FMPE::initM2(){
  int idx = 0;
  
//  m2_transform = Matrix(dim, (2*WINDOW_SIZE+1)*9*dim);
/*
  for(int i=0;i<dim;i++){
    m2_transform[idx][0] = 1.0/3.0;
    m2_transform[idx][1] = 1.0/3.0;
    m2_transform[idx++][2] = 1.0/3.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][3] = 0.5;
    m2_transform[idx++][4] = 0.5;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][5] = 0.5;
    m2_transform[idx++][6] = 0.5;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx++][7] = 1.0;
  }

  for(int i=0;i<dim;i++){
    m2_transform[idx++][WINDOW_SIZE] = 1.0;
  }

  for(int i=0;i<dim;i++){
    m2_transform[idx++][9] = 1.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][10] = 0.5;
    m2_transform[idx++][11] = 0.5;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][12] = 0.5;
    m2_transform[idx++][13] = 0.5;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][14] = 1.0/3.0;
    m2_transform[idx][15] = 1.0/3.0;
    m2_transform[idx++][16] = 1.0/3.0;
  }
*/
/*
  for(int i=0;i<dim;i++){
    m2_transform[idx][0] = 0.0/3.0;
    m2_transform[idx][1] = 0.0/3.0;
    m2_transform[idx++][2] = 0.0/3.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][3] = 0.0;
    m2_transform[idx++][4] = 0.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][5] = 0.0;
    m2_transform[idx++][6] = 0.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx++][7] = 0.0;
  }

  for(int i=0;i<dim;i++){
    m2_transform[idx++][WINDOW_SIZE] = 1.0;
  }

  for(int i=0;i<dim;i++){
    m2_transform[idx++][9] = 0.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][10] = 0.0;
    m2_transform[idx++][11] = 0.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][12] = 0.0;
    m2_transform[idx++][13] = 0.0;
  }
  for(int i=0;i<dim;i++){
    m2_transform[idx][14] = 0.0/3.0;
    m2_transform[idx][15] = 0.0/3.0;
    m2_transform[idx++][16] = 0.0/3.0;
  }
*/
}

void FMPE::storeFeatures(){
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;

  orig_obs = Matrix(t_max, dim);
  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      orig_obs[t][d] = obs[t][d];
    }
  }
}

void FMPE::transformFeatures(){
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;

  if(!is_enabled)
    return;
  fprintf(stderr, "computing fMPE features...\n");

  Matrix fmpe_obs = computeFeatures(orig_obs, t_max);

  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      obs[t][d] = fmpe_obs[t][d];
    }
  }
}

void FMPE::update(float learning_rate){
  for(int i=0;i<dim;i++){
    float sigma = 0.0;
    //float learning_rate = 0.000000001;
    for(int r=0;r<cluster.size();r++){
      sigma += sqrt(1.0/cluster[r]->getCovAt(i, i));
    }
    sigma = sigma/cluster.size();
    for(int f=0;f<2*WINDOW_SIZE+1;f++){
      for(int k=0;k<(dim+1)*cluster.size();k++){
        m1_transform[f][i][k] += learning_rate * (m1_direct_accu[f][i][k] + m1_indirect_accu[f][i][k]);
        //m1_transform[f][i][k] += learning_rate * (m1_direct_accu[f][i][k]);
      }
    }
  }
}


void FMPE::accu(GLat* glatP, int* refNodeLst, int refNodeN){
  int frameS = glatP->startP->frameS;
  int frameE = glatP->end.frameX;
  int t_max = frameE - frameS + 1;

  float** obs = orig_obs.getMap();
/*
  Matrix gamma = computePosterior(&obs[frameS], t_max);
  Matrix h_raw = spliceRaw(&obs[frameS], t_max, gamma);
  Matrix v_mat = h_raw*m1_transform;
  Matrix adjustment = spliceContext(v_mat);
  */

//  fprintf(stderr, "ckpt 1 %d %d %d\n", frameS, frameE, t_max);
  Matrix fmpe_obs(t_max, dim);
  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      fmpe_obs[t][d] = obs[t][d] + adjustment[t][d];
    }
  }


  vector<bool> is_ref;
  int ref_phone_count=0;
  is_ref.resize(glatP->nodeN);
  for(int nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    for(int i=0;i<refNodeN;i++){
      if(refNodeLst[i]==nodeX){
        is_ref[nodeX] = true;
        ref_phone_count += MPECompP->monoPhoneN;
        break;
      }
    }
  }

  Matrix direct(t_max, dim), indirect(t_max, dim);
  Matrix norm(cbs->list.itemN, MAX_MIX);
  direct.zero();
  indirect.zero();
  #pragma omp parallel if(glatP->nodeN > 1000)
  {
  #pragma omp for
  for(int nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    GLatMPEComp* MPECompP = nodeP->addon.MPEComp;
    GLatMPECDStats*** MPECDStats = nodeP->addon.MPECDStats;

    for(int parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(int childX=0;childX<nodeP->addon.childCtxN;childX++){
        Path* path = MPECDStats[parentX][childX]->path;
        if(path==NULL)
          continue;

        float node_occ = exp(-nodeP->addon.gamma[parentX][childX]);
        float mpe_scale = 0.0;
        if(is_ref[nodeX]){
          mpe_scale = (1.0 - node_occ);
        } else {
          mpe_scale = -node_occ;
        }
        for(int t=0;t<path->pitemListN;t++){
          PathItemList& plist = path->pitemListA[t];
          int frameX = path->firstFrame + t;

          for(int j=0;j<plist.itemN;j++){
            PathItem& item = plist.itemA[j];
            int snX = item.senoneX;
            float factor = item.gamma*mpe_scale;

            if(fabs(factor)<0.0001)
              continue;

            Senone* snP = sns->list.itemA + snX;
            if(snP->streamN!=1){
              fprintf(stderr, "GMMIE: not supporting multiple streams, quitting...\n");
              exit(-1);
            }
            int idx = snP->classXA[0];
            if(idx<0)
              continue;
            Distrib* ds = dss->list.itemA + idx;
            Codebook* cb = cbs->list.itemA + ds->cbX;
            float mix_occ[1024];

            computePosterior(cb, ds, fmpe_obs[frameX],(float*) mix_occ);
            for(int m=0;m<cb->refN;m++){
              float num_occ = cb->accu->count[num_idx][m];
              float den_occ = cb->accu->count[den_idx][m];
              float norm = 1.0/cb->accu->count[0][m];
              if(num_occ<10.0 || den_occ<10.0 || !isnormal(norm))
                continue;
              if(mix_occ[m] < 0.1/cb->refN)
                continue;
              for(int i=0;i<dim;i++){
                float num = cb->accu->rv[num_idx]->matPA[m][i];
                float den = cb->accu->rv[den_idx]->matPA[m][i];
                float s_num = (cb->accu->sumOsq[num_idx][m].m.d[i] - 2*num*cb->rv->matPA[m][i] + num_occ*cb->rv->matPA[m][i]*cb->rv->matPA[m][i])/num_occ;
                float s_den = (cb->accu->sumOsq[den_idx][m].m.d[i] - 2*den*cb->rv->matPA[m][i] + den_occ*cb->rv->matPA[m][i]*cb->rv->matPA[m][i])/den_occ;
                float partial_f_mean = cb->cv[m]->m.d[i]*(num - den - cb->rv->matPA[m][i]*(num_occ - den_occ));
                float partial_f_cov = num_occ*0.5*(s_num*cb->cv[m]->m.d[i]*cb->cv[m]->m.d[i] - cb->cv[m]->m.d[i]) - den_occ*0.5*(s_den*cb->cv[m]->m.d[i]*cb->cv[m]->m.d[i] - cb->cv[m]->m.d[i]);
                #pragma omp atomic
                direct[frameX][i] += scale*factor*mix_occ[m]*(cb->rv->matPA[m][i] - fmpe_obs[frameX][i])*cb->cv[m]->m.d[i];
                #pragma omp atomic
                indirect[frameX][i] += scale*node_occ*mix_occ[m]*norm*(partial_f_mean + 2*partial_f_cov*(fmpe_obs[frameX][i] - cb->rv->matPA[m][i]));
              }
            }
          }
        }
      }
    }
  }

  #pragma omp for nowait
  for(int t=0;t<t_max;t++){
    for(int f=-WINDOW_SIZE;f<=WINDOW_SIZE;f++){
      if(t+f < 0 || t+f >= t_max)
        continue;
      float factor = powf(2.0, - abs(f));
      for(int k=0;k<h_raw_active[t+f].size();k++){
        int idx = h_raw_active[t+f][k];
        for(int i=0;i<dim;i++){
//          float partial_f_yti = (direct[t][i] + indirect[t][i])*factor;
          float partial_f_yti = direct[t][i] * factor;
          if(fabs(partial_f_yti)<0.0001)
            continue;
          for(int d=0;d<dim+1;d++){
            #pragma omp atomic
            m1_direct_accu[f+WINDOW_SIZE][i][idx+d] += (partial_f_yti)*h_raw[t+f][idx+d];
            #pragma omp atomic
            m1_indirect_accu[f+WINDOW_SIZE][i][idx+d] += indirect[t][i]*factor*h_raw[t+f][idx+d];
          }
        }
      }
    }
  }
  }
  turn++;
  if(turn > 9)
    turn = 0;
}

void FMPE::computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ){
  float norm = LZERO;
  for(int m=0;m<cb->refN;m++){
    float g_const = cb->pi + cb->cv[m]->det;
    float dist = g_const;
    float tmp;
    float* mean_ptr = cb->rv->matPA[m];
    float* cov_ptr = cb->cv[m]->m.d;
    float* data = obs;
    int d;
    for(d=0;d<cb->dimN/6;d++){
      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;

      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;

      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;

      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;

      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;

      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;
//      dist += (obs[d] - cb->rv->matPA[m][d])*(obs[d] - cb->rv->matPA[m][d])*cb->cv[m]->m.d[d];
    }
    for(d=6*(cb->dimN/6);d<cb->dimN;d++){
      tmp = *data - *mean_ptr;
      dist += (tmp*tmp) * (*cov_ptr);
      data++;
      mean_ptr++;
      cov_ptr++;
    }
    mix_occ[m] = -0.5*dist - ds->val[m];
    norm = LogAdd(norm, -0.5*dist - ds->val[m]);
  }
  for(int m=0;m<cb->refN;m++){
    if(mix_occ[m] - norm <LSMALL)
      mix_occ[m] = 0.0;
    else
      mix_occ[m] = exp(mix_occ[m] - norm);
  }
}


Matrix FMPE::computeFeatures(Matrix& x, int t_max){

  gamma = computePosterior(x.getMap(), t_max);

//  gamma.getRowVec(0).printFP(stderr);

  spliceRaw(x.getMap(), t_max, gamma);
  adjustment = spliceContext(v_mat);

  Matrix fmpe_obs = x + adjustment;

  return fmpe_obs;
}

Matrix FMPE::spliceRaw(float** x, int t_max, Matrix& post_mat){
  Matrix result;

  h_raw_active.clear();
  h_raw_active.resize(t_max);

  h_raw = Matrix(t_max, (dim+1)*cluster.size());

  #pragma omp parallel if(t_max > 800)
  {
  #pragma omp for nowait
  for(int t=0;t<t_max;t++){
    int idx = 0;
    for(int i=0;i<cluster.size();i++){
      if(post_mat[t][i] < 1.0 / cluster.size()){
        idx += dim+1;
        continue;
      }
      h_raw[t][idx] = 5.0*post_mat[t][i];
      h_raw_active[t].push_back(idx);
      for(int d=0;d<dim;d++){
        if(isnormal(sqrt_cov_inv[i][d]))
          h_raw[t][idx+d+1] = post_mat[t][i]*(x[t][d] - cluster[i]->getMeanAt(d))* sqrt_cov_inv[i][d];
        if(!isnormal(h_raw[t][idx+d+1]))
          h_raw[t][idx+d+1] = 0.0;
      }
      idx += dim+1;
    }
  }
  }

  return result;
}

   
Matrix FMPE::spliceContext(Matrix& v_mat){
  int t_max = h_raw.getNumRow();
  Matrix h_context(t_max, dim);

  for(int f=-WINDOW_SIZE;f<=WINDOW_SIZE;f++){
    Matrix& m1 = m1_transform[f+WINDOW_SIZE];
    float factor = powf(2.0, - abs(f));
    for(int t=0;t<t_max;t++){
      if(t+f<0 || t+f>=t_max)
        continue;
      for(int k=0;k<h_raw_active[t+f].size();k++){
        int idx = h_raw_active[t+f][k];
        for(int i=0;i<dim;i++){
          for(int d=0;d<dim+1;d++){
            h_context[t][i] += factor * m1[i][idx+d] * h_raw[t+f][idx+d];
          }
        }
      }
    }
  }

  return h_context;
}

