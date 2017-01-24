#include "gdfsa.h"
#include <omp.h>
#include <errno.h>

GDFSA::GDFSA(SenoneSet* sns, MLAdapt* mladapt, CodebookSet* cbs, CodebookSet* base_cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, bool use_fsa, float scale, int ref_lock, float E, int mmi_init){
  this->sns = sns;
  this->mladapt = mladapt;
  this->cbs = cbs;
  this->base_cbs = base_cbs;
  this->dss = dss;
  this->fs = fs;
  this->svmap = svmap;
  this->scale = scale;
  this->use_fsa = use_fsa;
  this->is_enabled = true;
  this->ref_lock = ref_lock;
  this->frame2class = NULL;
  this->log_deter = NULL;
  this->E = E;
  this->mmi_init = mmi_init;

  dim = 0;
  for(int i=0;i<cbs->list.itemN;i++){
    Codebook* cb = cbs->list.itemA + i;
    if(cb->dimN>0){
      dim = cb->dimN;
      break;
    }
  }

  traverseCTree(mladapt->nodeP);
  this->num_classes = cluster.size();
  cluster_mean_accu = Matrix(num_classes, dim);
  cluster_cov_accu = Matrix(num_classes, dim);
  cluster_count = Matrix(1, num_classes);
  cluster_weight = Matrix(1, num_classes);
  for(int r=0;r<num_classes;r++){
    cluster_weight[0][r] = log(1.0/num_classes);
  }
  fprintf(stderr, "GDFT: no. of transforms = %d\n", num_classes);

  if(log_deter!=NULL)
    free(log_deter);
  log_deter = (float*) malloc(sizeof(float)*num_classes);
  for(int r=0;r<num_classes;r++){
    if(train_trans.size()>0){
//      log_deter[r] = train_trans[r].copy(0, 0, dim-1, dim-1).logDeterminant()*2;
      double tmp  = train_trans[r].copy(0, 0, dim-1, dim-1).determinant();
      log_deter[r] = 2.0*log(fabs(tmp));
    }
    else
      log_deter[r] = 0.0;
  }
}

void GDFSA::traverseCTree(MLAdaptNode* root){
  if(root->leftP == NULL && root->rightP == NULL){
    Gaussian* g = new DiagGaussian("", dim);
    cluster.push_back(g);
    train_trans.push_back(Matrix(dim, dim+1));
    for(int i=0;i<dim;i++){
      train_trans[train_trans.size()-1][i][i] = 1.0;
      ((DiagGaussian*)g)->getMean()[i] = 0.0;
      ((DiagGaussian*)g)->getCov()[i] = 1.0;
    }

    double sum=0.0;
    for(int i=root->refF;i<=root->refL;i++){
      Codebook* cb = base_cbs->list.itemA + mladapt->itemA[i].cbX;
      Distrib* ds = dss->list.itemA + mladapt->itemA[i].cbX;
      int m = mladapt->itemA[i].refX;
      if(cb->dimN == dim && ds->val!=NULL){
        int idx = cluster.size()-1;
        float ds_val = exp(-ds->val[m]);

        if(!isnormal(ds_val))
          continue; 

        for(int d=0;d<cb->dimN;d++){
          ((DiagGaussian*)g)->getMean()[d] += ds_val*cb->rv->matPA[m][d];
        }
        sum += ds_val;
      }
    }
    if(sum > 0.0){
      for(int d=0;d<dim;d++){
        ((DiagGaussian*)g)->getMean()[d] /= sum;
      }
    }
    g->calculateGConst();
//    fprintf(stderr, "g: %d, const: %e\n", cluster.size()-1, g->calculateGConst());
//    g->getMeanVec().printFP(stderr);
//    g->getCovMat().printFP(stderr);
  }
  if(root->leftP != NULL)
    traverseCTree(root->leftP);
  if(root->rightP != NULL)
    traverseCTree(root->rightP);
}

void GDFSA::clearAccu(){
  g_mat.clear();
  z_mat.clear();
  g_mat.resize(num_classes);
  z_mat.resize(num_classes);
  for(int r=0;r<num_classes;r++){
    z_mat[r] = Matrix(dim, dim+1);
    g_mat[r].resize(dim);
    for(int i=0;i<dim;i++){
      g_mat[r][i] = Matrix(dim+1, dim+1);
    }
  }
  gamma_sum = Matrix(1, num_classes);
}

void GDFSA::saveAccu(char* header){
  char filename[STRMAX];
  FILE* fp;

  sprintf(filename, "%s.accu.mat", header);
  fp = fopen(filename, "wb");

  fprintf(stderr, "saving accus...\n");
  for(int r=0;r<num_classes;r++){
    z_mat[r].bsave(fp);
    for(int d=0;d<dim;d++){
      g_mat[r][d].bsave(fp);
    }
  }
  gamma_sum.bsave(fp);

  fclose(fp);
}

void GDFSA::loadAccu(char* header){
  char filename[STRMAX];
  FILE* fp;

  sprintf(filename, "%s.accu.mat", header);
  fp = fopen(filename, "rb");

  if(z_mat.size()==0){
    clearAccu();
  }
  fprintf(stderr, "loading accus...\n");
  for(int r=0;r<num_classes;r++){
    z_mat[r].bload(fp);
    for(int d=0;d<dim;d++){
      g_mat[r][d].bload(fp);
    }
  }
  gamma_sum.bload(fp);
  fclose(fp);
}

void GDFSA::loadAddAccu(char* header){
  char filename[STRMAX];
  Matrix tmp;
  FILE* fp;

  sprintf(filename, "%s.accu.mat", header);
  fp = fopen(filename, "rb");

  if(z_mat.size()==0){
    clearAccu();
  }
  fprintf(stderr, "loading accus...\n");
  for(int r=0;r<num_classes;r++){
    tmp.bload(fp);
    z_mat[r] += tmp;
    for(int d=0;d<dim;d++){
      tmp.bload(fp);
      g_mat[r][d]+=tmp;
    }
  }
  tmp.bload(fp);
  gamma_sum += tmp;
  tmp.bload(fp);
  fclose(fp);
}

float GDFSA::getScale(){
  return scale;
}

void GDFSA::setScale(float scale){
  this->scale = scale;
}

void GDFSA::computeScore(GNode* nodeP, int parentX, int childX, float& cur_score, float& base_score){
  cur_score = base_score = 0.0;

  Path* path = nodeP->addon.MPECDStats[parentX][childX]->path;
  if(path==NULL)
    return;

  Matrix new_obs = transformFeaturesForTest();
  for(int t=0;t<path->pitemListN;t++){
    PathItemList& plist = path->pitemListA[t];
    int frameX = path->firstFrame + t;
    int r = frame2class[frameX];

    float* data = NULL;
    float* new_data = NULL;

    for(int j=0;j<plist.itemN;j++){
      PathItem& item = plist.itemA[j];
      float gamma = item.gamma;
      int snX = item.senoneX;

      Senone* snP = sns->list.itemA + snX;
      if(snP->streamN!=1){
        fprintf(stderr, "GDFSA: not supporting multiple streams, quitting...\n");
        exit(-1);
      }
      int idx = snP->classXA[0];
      if(idx<0)
        continue;
      Distrib* ds = dss->list.itemA + idx;
      Codebook* cb = cbs->list.itemA + ds->cbX;
      Codebook* base_cb = base_cbs->list.itemA + ds->cbX;

      float mix_occ[MAX_MIX];
      if(!use_fsa)
        data = orig_obs[frameX];
      else
        data = fsa_obs[frameX];
      computePosterior(cb, ds, data, (float*)mix_occ);

      for(int m=0;m<cb->refN;m++){
        if(mix_occ[m] >= 0.1/cb->refN){
        
          if(!use_fsa)
            data = orig_obs[frameX];
          else
            data = fsa_obs[frameX];
          new_data = new_obs[frameX];

          float base_sum = 0.0;
          float cur_sum = 0.0;
          float* cov_ptr = cb->cv[m]->m.d;
          float* mean_ptr = cb->rv->matPA[m];
          float tmp1, tmp2;
          int d = 0;
          for(d=0;d<cb->dimN/6;d++){
            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;

            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;

            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;

            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;

            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;

            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;
          }
          for(d=6*(cb->dimN/6);d<cb->dimN;d++){
            tmp1 = *data - *mean_ptr;
            tmp2 = *new_data - *mean_ptr;
            base_sum += (tmp1*tmp1) * (*cov_ptr);
            cur_sum += (tmp2*tmp2) * (*cov_ptr);
            data++;
            new_data++;
            mean_ptr++;
            cov_ptr++;
          }
          base_score += gamma*mix_occ[m]*base_sum;
          cur_score += gamma*mix_occ[m]*cur_sum;
        }
      }
    }
  }
}


float GDFSA::findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, float& alpha_ref, float& beta_ref, float& alpha_com, float& beta_com, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda){
  char buf[STRMAX];
  bool* is_ref;
  int ref_start=-1;
  vector<unsigned short> ref_idx;
  int ref_phone_count = 0;

  is_ref = (bool*) malloc(sizeof(bool)*glatP->nodeN);
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    for(int i=0;i<refNodeN;i++){
      if(refNodeLst[i]==nodeX){
        is_ref[nodeX] = true;
        ref_phone_count += nodeP->addon.MPEComp->monoPhoneN;
        if(ref_start==-1)
          ref_start = nodeX;
        break;
      }
    }
  }

  sprintf(buf, "%s.dat", in_lambda);
  FILE* fp = fopen(buf, "rb");
  
  if(fp==NULL){
    ref_sil_alpha = 1.0;
    ref_sil_beta = 0.0;
    alpha_ref = 1.0;
    beta_ref = 0.0;
    if(!mmi_init){
      alpha_com = 0.5;
      beta_com = 0.5;
    } else {
      alpha_com = 0.0;
      beta_com = 1.0;
    }
  }
  else{
    float val;

    fread(&val, sizeof(float), 1, fp);
    alpha_ref = val;
    beta_ref = 1.0-alpha_ref;
    fread(&val, sizeof(float), 1, fp);
    alpha_com = val;
    beta_com = 1.0-alpha_com;
    fclose(fp);
  }

  float total_score = 0.0;
  float ref_alpha_sum = 0.0, ref_beta_sum = 0.0;
  float com_alpha_sum = 0.0, com_beta_sum = 0.0;
  int ref_count = 0;
  int com_count = 0;
  float alpha_ref_grad=0.0, alpha_com_grad=0.0;
/*
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    GLatMPEComp* MPEComp = nodeP->addon.MPEComp;
    GLatMPECDStats ***MPECDStats=nodeP->addon.MPECDStats;

    for(unsigned short parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(unsigned short childX=0;childX<nodeP->addon.childCtxN;childX++){
        Path* path = MPECDStats[parentX][childX]->path;
        if(path==NULL)
          continue;

        float node_cd_occ = exp(-nodeP->addon.gamma[parentX][childX]);
        float cur_score, base_score;
        computeScore(nodeP, parentX, childX, cur_score, base_score);
        float scaled_score = 0.0;
        float a=0.0;
        float b=0.0;

        float accuracy = MPECDStats[parentX][childX]->approxCq[MPEComp->monoPhoneN-1]/ref_phone_count;

        if(is_ref[nodeX]){
          a = alpha_ref;
          b = beta_ref;
        } else {
          a = alpha_com;
          b = beta_com;
        }

        if(is_ref[nodeX]){
          scaled_score = base_score - scale*base_score;
          total_score += node_cd_occ*fabs(cur_score - scaled_score);
          ref_alpha_sum += a;
          ref_beta_sum += b;
          ref_count++;
        }
        else{
          scaled_score = base_score + scale*(1.0-accuracy)*base_score;
          total_score += node_cd_occ*fabs(cur_score - scaled_score);
          com_alpha_sum += a;
          com_beta_sum += b;
          com_count++;
        }

        float epsilon = fabs(cur_score - scaled_score);
        float diff = cur_score - scaled_score;

        if(is_ref[nodeX] && svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
          a = ref_sil_alpha;
          b = ref_sil_beta;
        }
        if(is_ref[nodeX]){
          alpha_ref_grad += diff;
        } else {
          alpha_com_grad += diff*node_cd_occ;
        }
      }
    }
  }
*/
  float diff = 0.0;

  if(!ref_lock){
    diff = cur_score - (base_score - base_score * scale);
    alpha_ref = alpha_ref + learning_rate*diff;
  } else {
    alpha_ref = 1.0;
  }
  diff = cur_score - (base_score + base_score * scale);
  alpha_com = alpha_com + learning_rate*diff;

  if(alpha_ref<0.0)
    alpha_ref=0.0;
  else if(alpha_ref>1.0)
    alpha_ref=1.0;
  beta_ref = 1.0 - alpha_ref;

  if(alpha_com<0.0)
    alpha_com=0.0;
  else if(alpha_com>1.0)
    alpha_com=1.0;
  beta_com = 1.0 - alpha_com;

  sprintf(buf, "%s.dat", out_lambda);
  fp = fopen(buf, "wb");
  if(fp==NULL){
    fprintf(stderr, "Error in writing lambdas - errno: %d\n", errno);
    sleep(1);
    fp = fopen(buf, "wb");
    if(fp==NULL){
      fprintf(stderr, "INFO    ref_alpha: %f, ref_beta: %f\n", alpha_ref, beta_ref);
      fprintf(stderr, "INFO    com_alpha: %f, com_beta: %f\n", alpha_com, beta_com);
      free(is_ref);
      return total_score;
    }
    fprintf(stderr, "retry is successful\n");
  }

  int code = 0;
  float tmp_array[2];
  tmp_array[0] = alpha_ref;
  tmp_array[1] = alpha_com;

  code = fwrite(tmp_array, sizeof(float), 2, fp);
  if(code != 2){
    fprintf(stderr, "Error in writing alpha_ref - errno: %d\n", errno);
    sleep(1);
    code = fwrite(tmp_array, sizeof(float), 2, fp);
    if(code!=2){
      fprintf(stderr, "retry failed - errno: %d\n", errno);
    }
  }

  code = fflush(fp);
  if(code!=0){
    fprintf(stderr, "Error in flushing lambdas - errno: %d\n", errno);
    sleep(1);
    code = fflush(fp);
    if(code!=0){
      fprintf(stderr, "retry failed - errno: %d\n", errno);
    }
  }

  code = fclose(fp);
  if(code!=0){
    fprintf(stderr, "Error in closing lambdas - errno: %d\n", errno);
    sleep(1);
    code = fclose(fp);
    if(code!=0){
      fprintf(stderr, "retry failed - errno: %d\n", errno);
    }
  }
  fprintf(stderr, "INFO    ref_alpha: %f, ref_beta: %f\n", alpha_ref, beta_ref);
  fprintf(stderr, "INFO    com_alpha: %f, com_beta: %f\n", alpha_com, beta_com);
  free(is_ref);

  return total_score;
}

void GDFSA::accuCluster(){
  Matrix& obs = orig_obs;
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  int t_max = feature->data.fmat->m;
  Matrix posterior;

  if(use_fsa)
    obs = fsa_obs;
  posterior = computePosterior(obs.getMap(), t_max);
  for(int t=0;t<t_max;t++){
    for(int r=0;r<num_classes;r++){
      if(!isnormal(posterior[t][r]) || posterior[t][r]<0.1/num_classes)
        continue;
      for(int d=0;d<dim;d++){
        cluster_mean_accu[r][d] += posterior[t][r] * obs[t][d];
        cluster_cov_accu[r][d] += posterior[t][r] * obs[t][d] * obs[t][d];
      }
      cluster_count[0][r] += posterior[t][r];
    }
  }
}

void GDFSA::clearAccuCluster(){
  cluster_mean_accu.zero();
  cluster_cov_accu.zero();
  cluster_count.zero();
}

void GDFSA::updateCluster(){
  double acc = 0.0;

  fprintf(stderr, "updating the GMM...\n");
  cluster_count.printFP(stderr);
  for(int r=0;r<num_classes;r++){
    acc += cluster_count[0][r];
    if(cluster_count[0][r]<1.0)
      continue;
    Matrix mean = cluster_mean_accu.getRowVec(r) * (1.0/cluster_count[0][r]);
    Matrix cov = cluster_cov_accu.getRowVec(r) ;
    for(int d=0;d<dim;d++){
      cov[0][d] *= (1.0/cluster_count[0][r]);
      cov[0][d] -= mean[0][d]*mean[0][d];
      cov[0][d] = 1.0/cov[0][d];
    }
    
    if(r==0){
      cluster[0]->getMeanVec().printFP(stderr);
      cluster[0]->getCovMat().printFP(stderr);
      mean.printFP(stderr);
      cov.printFP(stderr);
    }
    
    cluster[r]->setMeanVec(mean);
    cluster[r]->setCovMat(cov);
    cluster[r]->calculateGConst();
  }
  for(int r=0;r<num_classes;r++){
    if(cluster_count[0][r] == 0.0){
      cluster_weight[0][r] = LZERO;
      continue;
    }
    cluster_weight[0][r] = log(cluster_count[0][r] / acc);
  }
  cluster_weight.printFP(stderr);
}

void GDFSA::saveAccuCluster(char* filename){
  FILE* fp = fopen(filename, "w");
  cluster_mean_accu.bsave(fp);
  cluster_cov_accu.bsave(fp);
  cluster_count.bsave(fp);
  fclose(fp);
}

void GDFSA::loadAccuCluster(char* filename){
  Matrix tmp;
  FILE* fp = fopen(filename, "r");
  tmp.bload(fp);
  cluster_mean_accu += tmp;
  tmp.bload(fp);
  cluster_cov_accu += tmp;
  tmp.bload(fp);
  cluster_count += tmp;
  fclose(fp);
}

void GDFSA::saveCluster(char* filename){
  FILE* fp = fopen(filename, "w");
  for(int r=0;r<num_classes;r++){
    cluster[r]->getMeanVec().bsave(fp);
    cluster[r]->getCovMat().bsave(fp);
  }
  cluster_weight.bsave(fp);
  fclose(fp);
}

void GDFSA::loadCluster(char* filename){
  FILE* fp = fopen(filename, "r");
  Matrix tmp;
  for(int r=0;r<num_classes;r++){
    tmp.bload(fp);
    cluster[r]->setMeanVec(tmp);
    tmp.bload(fp);
    cluster[r]->setCovMat(tmp);
    cluster[r]->calculateGConst();
  }
  cluster_weight.bload(fp);
  fclose(fp);
}

    
float GDFSA::accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda){
  float result = 0.0;
  float alpha_ref, beta_ref, alpha_com, beta_com;
  vector<map<int, Matrix> > d_mat;
  int max_iter = 5;
  Matrix g_factor_sum;
  Matrix z_factor_sum;
  int t_max;
  Matrix utt_gamma_sum(1, num_classes);

  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  t_max = feature->data.fmat->m;
  if(frame2class!=NULL){
    free(frame2class);
  }
  frame2class = (int*) malloc(sizeof(int)*t_max);
  memset(frame2class, 0, sizeof(int)*t_max);

  if(!use_fsa)
    assignClasses(orig_obs.getMap(), t_max, frame2class, posterior);
  else
    assignClasses(fsa_obs.getMap(), t_max, frame2class, posterior);

  result+=findAlphaBeta(glatP, refNodeLst, refNodeN, alpha_ref, beta_ref, alpha_com, beta_com, learning_rate, c_avg, base_score, cur_score, in_lambda, out_lambda);

  Matrix obs_aug(t_max, dim+1);
  Matrix& obs = orig_obs;
  if(use_fsa)
    obs = fsa_obs;
  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      obs_aug[t][d] = obs[t][d];
    }
    obs_aug[t][dim] = 1.0;
  }
  g_factor_sum = Matrix(t_max, dim);
  z_factor_sum = Matrix(t_max, dim);
/*
  int ref_phone_count = 0;
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    for(int i=0;i<refNodeN;i++){
      if(refNodeLst[i]==nodeX){
        ref_phone_count += nodeP->addon.MPEComp->monoPhoneN;
      }
    }
  }
*/
  #pragma omp parallel if(glatP->nodeN > 1000)
  {
  #pragma omp for nowait
  for(int nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    GLatMPEComp *MPECompP=nodeP->addon.MPEComp;
    GLatMPECDStats ***MPECDStats=nodeP->addon.MPECDStats;
    bool is_ref = 0;

    for(int i=0;i<refNodeN;i++){
      if(refNodeLst[i]==nodeX){
        is_ref = 1;
        break;
      }
    }

    for(int parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(int childX=0;childX<nodeP->addon.childCtxN;childX++){
        Path* path = MPECDStats[parentX][childX]->path;
        if(path==NULL)
          continue;
        float node_cd_occ = exp(-nodeP->addon.gamma[parentX][childX]);

        int accu_idx = -1;
        float a = 0.0;
        float b = 0.0;

        if(is_ref){
          a = alpha_ref;
          b = beta_ref;
        } else {
          a = alpha_com;
          b = beta_com;
        }
        float factor = a - b;
        if(factor>0.0001)
          accu_idx = 0;
        else if(factor<-0.0001)
          accu_idx = 1;
        else
          continue;

        if(!is_ref)
          factor*=node_cd_occ;
        
        if(fabs(factor)<0.00001)
          continue;

        for(int t=0;t<path->pitemListN;t++){
          PathItemList& plist = path->pitemListA[t];
          int frameX = path->firstFrame + t;

          for(int j=0;j<plist.itemN;j++){
            PathItem& item = plist.itemA[j];
            float gamma;
            int snX = item.senoneX;
  
            gamma = item.gamma*factor;

            if(fabs(gamma)==0.0)
              continue;

            Senone* snP = sns->list.itemA + snX;
            if(snP->streamN!=1){
              fprintf(stderr, "GDFSA: not supporting multiple streams, quitting...\n");
              exit(-1);
            }
            int idx = snP->classXA[0];
            if(idx<0)
              continue;
            Distrib* ds = dss->list.itemA + idx;
            Codebook* cb = cbs->list.itemA + ds->cbX;
            Feature* feature = &fs->featA[cb->featX];
            float mix_occ[MAX_MIX];

            computePosterior(cb, ds, obs[frameX],(float*) mix_occ);
            int r = frame2class[frameX];

            for(int m=0;m<cb->refN;m++){
              if(mix_occ[m] < 0.1/cb->refN)
                continue;

              #pragma omp atomic
              utt_gamma_sum[0][r] += gamma * mix_occ[m];
              for(int i=0;i<cb->dimN;i++){
                #pragma omp atomic
                g_factor_sum[frameX][i] += gamma * mix_occ[m] * cb->cv[m]->m.d[i];
                #pragma omp atomic
                z_factor_sum[frameX][i] += cb->rv->matPA[m][i]*gamma*mix_occ[m]*cb->cv[m]->m.d[i]; 
              }
            }
          }
        }  
      }
    }
  }
  }

  for(int r=0;r<num_classes;r++){
    if(fabs(utt_gamma_sum[0][r])<1.0)
      continue;
    gamma_sum[0][r] += utt_gamma_sum[0][r];
  }
  for(int t=0;t<t_max;t++){
    int r = frame2class[t];
    if(fabs(utt_gamma_sum[0][r])<1.0)
      continue;
    Matrix x_mat = obs_aug.getRowVec(t).transpose()*obs_aug.getRowVec(t);
    for(int i=0;i<dim;i++){
      if(g_factor_sum[t][i] == 0.0 || z_factor_sum[t][i] == 0.0)
        continue;
      g_mat[r][i] += x_mat*g_factor_sum[t][i];
      for(int j=0;j<=dim;j++){
        z_mat[r][i][j] += obs_aug[t][j]*z_factor_sum[t][i];;
      }
    }
  }

  return result;
}

void GDFSA::assignClasses(float** obs, int t_max, int* frame2class, Matrix& posterior){
  Matrix cur_like(1, num_classes);
  int max_idx = -1;
  float max_score = 0.0;

  for(int t=0;t<t_max;t++){
    max_idx = -1;
    max_score = 0.0;
    for(int r=0;r<num_classes;r++){
      cur_like[0][r] = cluster[r]->logBj(obs[t]) + cluster_weight[0][r];
      if(max_idx==-1 || max_score < cur_like[0][r]){
        max_idx = r;
        max_score = cur_like[0][r];
      }
    }
    frame2class[t]=max_idx;
  }
}

void GDFSA::computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ){
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


Matrix GDFSA::computePosterior(float** x, int t_max){
  Matrix post_mat(t_max, cluster.size());
  vector<float> likelihood;

  float best_score=0.0;
  int max_idx = -1;
  likelihood.resize(cluster.size());
  for(int t=0;t<t_max;t++){
    float norm = LZERO;
    for(int i=0;i<cluster.size();i++){
      likelihood[i] =  cluster[i]->logBj(x[t]) + cluster_weight[0][i];
      if(!isnormal(likelihood[i]) || likelihood[i]<LSMALL)
        continue;
      norm = LogAdd(norm, likelihood[i]);
    }
    for(int i=0;i<cluster.size();i++){
      if(!isnormal(likelihood[i]) || likelihood[i]<LSMALL)
        post_mat[t][i] = 0.0;
      else
        post_mat[t][i] = exp(likelihood[i] - norm);
    }
  }

  return post_mat;
}

void GDFSA::storeFeatures(){
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;

  if(t_max!=orig_obs.getNumRow())
    orig_obs = Matrix(t_max, dim);

  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      orig_obs[t][d] = obs[t][d];
    }
  }
}

void GDFSA::storeFSAFeatures(){
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;
  
  if(t_max!=fsa_obs.getNumRow())
    fsa_obs = Matrix(t_max, dim);
  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      fsa_obs[t][d] = obs[t][d];
    }
  }
}

void GDFSA::restoreFeatures(){
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;

  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      obs[t][d] = orig_obs[t][d];
    }
  }
}

void GDFSA::clearTransformation(){
  transformation.clear();
  if(log_deter!=NULL)
    free(log_deter);
  log_deter = NULL;
}

void GDFSA::loadTransformation(char* header){
  char filename[STRMAX];
  FILE* fp;
  Matrix identity(dim, dim+1);

  for(int i=0;i<dim;i++)
    identity[i][i] = 1.0;

  sprintf(filename, "%s.trans.mat", header);
  fp = fopen(filename, "rb");

  fprintf(stderr, "loading %d transformations...\n", num_classes);
  transformation.clear();
  transformation.resize(num_classes);
  for(int r=0;r<num_classes;r++){
    transformation[r].bload(fp);
  }
  train_trans = transformation;

  if(log_deter!=NULL)
    free(log_deter);
  log_deter = (float*) malloc(sizeof(float)*num_classes);
  for(int r=0;r<num_classes;r++){
//    log_deter[r] = transformation[r].copy(0, 0, dim-1, dim-1).logDeterminant()*2.0;
    double tmp  = train_trans[r].copy(0, 0, dim-1, dim-1).determinant();
    log_deter[r] = 2.0*log(fabs(tmp));
    if(log_deter[r] < LSMALL){
      transformation[r] = identity;
      log_deter[r] = 0.0;
    }
  }
  fclose(fp);
}

void GDFSA::saveTransformation(char* header){
  char filename[STRMAX];
  FILE* fp;

  sprintf(filename, "%s.trans.mat", header);
  fp = fopen(filename, "wb");

  fprintf(stderr, "saving %d transformations...\n", transformation.size());
  for(int r=0;r<transformation.size();r++){
    transformation[r].bsave(fp);
  }
  fclose(fp);
}

void GDFSA::update(){
  vector<char> enabled;
  Matrix identity(dim, dim+1);

  gamma_sum.printFP(stderr);
  enabled.resize(num_classes);
  for(int i=0;i<dim;i++){
    identity[i][i] = 1.0;
  }

  if(train_trans.size()==0){
    train_trans.clear();
    train_trans.resize(num_classes);
    for(int r=0;r<num_classes;r++){
      train_trans[r] = Matrix(dim, dim+1);
      for(int i=0;i<dim;i++){
        train_trans[r][i][i] = 1.0;
      }
    }
  }

  for(int r=0;r<num_classes;r++){
    for(int i=0;i<dim;i++){
      z_mat[r][i][i] += E*gamma_sum[0][r];
      for(int j=0;j<=dim;j++){
        g_mat[r][i][j][j] += E*gamma_sum[0][r];
      }
    }
  }

  vector<vector<Matrix> > g_inv;
  g_inv.resize(num_classes);
  for(int r=0;r<num_classes;r++){
    g_inv[r].resize(dim);
    for(int i=0;i<dim;i++){
      g_inv[r][i] = g_mat[r][i].dInverse();
    }
  }


  for(int it=0;it<30;it++){
    fprintf(stderr, "updating transformations: iter = %d\n", it);
    #pragma omp parallel if(num_classes > 200)
    {
    #pragma omp for nowait
    for(int r=0;r<num_classes;r++){
      Matrix new_transform = train_trans[r];
      enabled[r] = true;
      for(int i=0;i<dim;i++){
        float tmp = (g_mat[r][i]*g_inv[r][i]).trace();
        if(!isfinite(tmp) || fabs(tmp - (dim+1)) > 1.0){
          for(int j=0;j<dim+1;j++){
            new_transform[i][j] = 0.0;
          }
          new_transform[i][i] = 1.0;
          continue;
        }

        double* p_vec = new double[dim+1];
        for(int j=0;j<dim;j++){
          p_vec[j] = train_trans[r].copy(0, 0, dim-1, dim-1).cofactor(i, j);
        }
        p_vec[dim] = 0.0;
        double term1 = 0.0; //(p_vec.transpose()*g_inv*p_vec)[0][0];
        for(int j=0;j<=dim;j++){
          for(int k=0;k<=dim;k++){
            term1 += p_vec[j]*g_inv[r][i][j][k]*p_vec[k];
          }
        }
        double term2 = 0.0; //(p_vec.transpose()*g_inv*z_mat[r].getRowVec(i).transpose())[0][0];
        for(int j=0;j<=dim;j++){
          for(int k=0;k<=dim;k++){
            term2 += p_vec[j]*g_inv[r][i][j][k]*z_mat[r][i][k];
          }
        }
        double term3 = -gamma_sum[0][r];
        double delta = term2*term2 - 4.0*term1*term3;
//        fprintf(stderr, "terms: %e %e %e %e\n", term1, term2, term3, delta);

        double alpha1 = (-term2 + sqrt(delta))/(2.0*term1);
        double alpha2 = (-term2 - sqrt(delta))/(2.0*term1);
        double alpha= 0.0;
        double kullback1= gamma_sum[0][r] * log(fabs(alpha1*term1+term2)) - 0.5 * alpha1*alpha1*term1;
        double kullback2= gamma_sum[0][r] * log(fabs(alpha2*term1+term2)) - 0.5 * alpha2*alpha2*term1;
//        fprintf(stderr, "a: %e %e, k:%e %e\n", alpha1, alpha2, kullback1, kullback2);
        if (kullback2 > kullback1) {
          alpha = alpha2;
        } else {
          alpha = alpha1;
        }

        if(!isfinite(alpha)){
          for(int j=0;j<dim+1;j++){
            new_transform[i][j] = 0.0;
          }
          new_transform[i][i] = 1.0;
          continue;
        }
        Matrix alpha_p_vec(1, dim+1);
        for(int j=0;j<dim+1;j++){
          alpha_p_vec[0][j] = p_vec[j]*alpha;
        }
        delete [] p_vec;
        Matrix w_mat = (alpha_p_vec + z_mat[r].getRowVec(i)) * g_inv[r][i];
        for(int j=0;j<dim+1;j++){
          new_transform[i][j] = w_mat[0][j];
        }
      }
      train_trans[r] = new_transform;
//      train_trans[r].printFP(stderr);
    }
    }
  }
  for(int r=0;r<num_classes;r++){
    double tmp  = train_trans[r].copy(0, 0, dim-1, dim-1).determinant();
    float offset = 2.0*log(fabs(tmp));

    if(offset<LSMALL){
      train_trans[r] = identity;
      offset = 0.0;
    }
    fprintf(stderr, "transform %d: %e\n", r, offset);
  }
}

void GDFSA::transferTransformation(){
  Matrix identity(dim, dim+1);
  for(int i=0;i<dim;i++)
    identity[i][i] = 1.0;

  transformation = train_trans;

  if(log_deter!=NULL)
    free(log_deter);
  if(transformation.size()==0){
    log_deter = NULL;
    return;
  }

  log_deter = (float*) malloc(sizeof(float)*num_classes);
  for(int r=0;r<num_classes;r++){
//    log_deter[r] = transformation[r].copy(0, 0, dim-1, dim-1).logDeterminant()*2.0;
    double tmp  = train_trans[r].copy(0, 0, dim-1, dim-1).determinant();
    log_deter[r] = 2.0*log(fabs(tmp));
    if(log_deter[r] < LSMALL){
      transformation[r] = identity;
      log_deter[r] = 0.0;
    }
  }
}

void GDFSA::setStatus(bool is_enabled){
  this->is_enabled = is_enabled;
}

Matrix GDFSA::transformFeaturesForTest(){
  Matrix obs_aug(dim+1, 1);
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;
  Matrix new_obs(t_max, dim);

  for(int t=0;t<t_max;t++){
    for(int d=0;d<dim;d++){
      new_obs[t][d] = obs[t][d];
    }
  }

  if(train_trans.size()==0)
    return new_obs;

  for(int t=0;t<t_max;t++){
    int r = frame2class[t];

    if(!use_fsa){
      for(int d=0;d<dim;d++){
        obs_aug[d][0] = orig_obs[t][d];
      }
    } else {
      for(int d=0;d<dim;d++){
        obs_aug[d][0] = fsa_obs[t][d];
      }
    }
    obs_aug[dim][0] = 1.0;
    Matrix obs_adapted = train_trans[r] * obs_aug;
    for(int d=0;d<dim;d++){
      new_obs[t][d] = obs_adapted[d][0];
    }
  }
  return new_obs;
}

void GDFSA::transformFeatures(){
  Matrix obs_aug(dim+1, 1);
  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;

  if(!is_enabled || transformation.size()==0){
    if(cbs->gdft_frame2class!=NULL)
      free(cbs->gdft_frame2class);
    cbs->gdft_frame2class = NULL;
    cbs->gdft_offset = NULL;
    return;
  }
  fprintf(stderr, "GDFT: transforming features (fsa=%d) %d %d\n", use_fsa, is_enabled, transformation.size());

  if(cbs->gdft_frame2class!=NULL)
    free(cbs->gdft_frame2class);
  cbs->gdft_frame2class = (int*) malloc(sizeof(int)*t_max);

  if(!use_fsa)
    assignClasses(orig_obs.getMap(), t_max, cbs->gdft_frame2class, posterior);
  else
    assignClasses(fsa_obs.getMap(), t_max, cbs->gdft_frame2class, posterior);
  cbs->gdft_offset = log_deter;

  for(int t=0;t<t_max;t++){
    int r = cbs->gdft_frame2class[t];

    if(!use_fsa){
      for(int d=0;d<dim;d++){
        obs_aug[d][0] = orig_obs[t][d];
      }
    } else {
      for(int d=0;d<dim;d++){
        obs_aug[d][0] = fsa_obs[t][d];
      }
    }
    obs_aug[dim][0] = 1.0;
    Matrix obs_adapted = transformation[r] * obs_aug;
    for(int d=0;d<dim;d++){
      obs[t][d] = obs_adapted[d][0];
    }
  }
}
