#include "gdlr.h"
#include <errno.h>

GDLR::GDLR(SenoneSet* sns, MLAdapt* mladapt, CodebookSet* cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float scale, float E){
  this->sns = sns;
  this->mladapt = mladapt;
  this->cbs = cbs;
  this->dss = dss;
  this->fs = fs;
  this->svmap = svmap;
  this->scale = scale;
  this->d_factor = 100.0;
  this->num_classes = 0;
  this->E = E;

  dim = 0;
  for(int i=0;i<cbs->list.itemN;i++){
    Codebook* cb = cbs->list.itemA + i;
    if(cb->dimN>0){
      dim = cb->dimN;
      break;
    }
  }
  cb2w_idx.resize(cbs->list.itemN);
  for(int i=0;i<cbs->list.itemN;i++){
    cb2w_idx[i].resize(MAX_MIX);
    for(int j=0;j<MAX_MIX;j++){
      cb2w_idx[i][j] = -1;
    }
  }

  traverseCTree(mladapt->nodeP);
  num_classes = transformation.size();

  clearAccu();
}

void GDLR::traverseCTree(MLAdaptNode* root){
  if(root->leftP == NULL && root->rightP == NULL){
    transformation.push_back(Matrix(dim, dim+1));
    for(int i=0;i<dim;i++){
      transformation[transformation.size()-1][i][i] = 1.0;
    }
    for(int i=root->refF;i<=root->refL;i++){
      Codebook* cb = cbs->list.itemA + mladapt->itemA[i].cbX;
      cb2w_idx[mladapt->itemA[i].cbX][mladapt->itemA[i].refX] = transformation.size()-1;
    }
  }
  if(root->leftP != NULL)
    traverseCTree(root->leftP);
  if(root->rightP != NULL)
    traverseCTree(root->rightP);
}

void GDLR::linkMLAdapt(){
}

void GDLR::clearAccu(){
  z_mat.clear();
  g_mat.clear();
  z_mat.resize(num_classes);
  g_mat.resize(num_classes);
  gamma_sum = Matrix(1, num_classes);
  for(int r=0;r<num_classes;r++){
    z_mat[r] = Matrix(dim, dim+1);
    g_mat[r].clear();
    g_mat[r].resize(dim);
    for(int i=0;i<dim;i++){
      g_mat[r][i] = Matrix(dim+1, dim+1);
    }
  }
}

void GDLR::saveTransformation(char* filename){
  FILE* fp;

  fp = fopen(filename, "wb");
  for(int r=0;r<num_classes;r++){
    transformation[r].bsave(fp);
  }
  fclose(fp);
}

void GDLR::loadTransformation(char* filename){
  FILE* fp;

  fp = fopen(filename, "rb");
  for(int r=0;r<num_classes;r++){
    transformation[r].bload(fp);
  }
  fclose(fp);
}

void GDLR::saveAccu(char* filename){
  FILE* fp;

  fp = fopen(filename, "wb");
  fprintf(stderr, "saving accus...\n");
  for(int r=0;r<num_classes;r++){
    z_mat[r].bsave(fp);
  }
  for(int r=0;r<num_classes;r++){
    for(int i=0;i<dim;i++){
      g_mat[r][i].bsave(fp);
    }
  }
  gamma_sum.bsave(fp);

  fclose(fp);
}

void GDLR::loadAccu(char* filename){
  FILE* fp;
  Matrix tmp;

  fp = fopen(filename, "rb");
  fprintf(stderr, "loading accus...\n");
  for(int r=0;r<num_classes;r++){
    tmp.bload(fp);
    z_mat[r] += tmp;
  }
  for(int r=0;r<num_classes;r++){
    for(int i=0;i<dim;i++){
      tmp.bload(fp);
      g_mat[r][i] += tmp;
    }
  }
  tmp.bload(fp);
  gamma_sum += tmp;

  fclose(fp);
}

float GDLR::getScale(){
  return scale;
}

void GDLR::setScale(float scale){
  this->scale = scale;
}

float GDLR::findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, float& alpha_ref, float& beta_ref, float& alpha_com, float& beta_com, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda){
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
    alpha_com = 0.5;
    beta_com = 0.5;
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
  float diff = 0.0;

  diff = cur_score - (base_score - base_score * scale);
  alpha_ref = alpha_ref + learning_rate*diff;
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
    
float GDLR::accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, float c_avg, float base_score, float cur_score, char* in_lambda, char* out_lambda){
  float result = 0.0;
  float alpha_ref, beta_ref, alpha_com, beta_com;
  vector<map<int, Matrix> > d_mat;
  int max_iter = 5;

  Codebook* cb = cbs->list.itemA + 0;
  Feature* feature = &fs->featA[cb->featX];
  float** obs = feature->data.fmat->matPA;
  int t_max = feature->data.fmat->m;
  Matrix utt_gamma_sum(1, num_classes);
  Matrix factor_sum(cbs->list.itemN, MAX_MIX);
  map<pair<int, int>, bool> active_gau;
  map<pair<int, int>, bool>::iterator it;
  pair<int, int> tuple;
  map<int, Matrix> z_factor_sum;
  
  result+=findAlphaBeta(glatP, refNodeLst, refNodeN, alpha_ref, beta_ref, alpha_com, beta_com, learning_rate, c_avg, base_score, cur_score, in_lambda, out_lambda);

  for(int nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
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
        float a, b;

        if(is_ref){
          a = alpha_ref;
          b = beta_ref;
        } else {
          a = alpha_com;
          b = beta_com;
        }

        float factor = a - b;
        if(!is_ref)
          factor*=node_cd_occ;
        
        if(fabs(factor)<0.00001){
          continue;
        }

        for(int t=0;t<path->pitemListN;t++){
          PathItemList& plist = path->pitemListA[t];
          int frameX = path->firstFrame + t;

          for(int j=0;j<plist.itemN;j++){
            PathItem& item = plist.itemA[j];
            float gamma;
            int snX = item.senoneX;
  
            if(is_ref)
              gamma = item.gamma;
            else
              gamma = item.gamma*factor;

            if(fabs(gamma)==0.0)
              continue;

            Senone* snP = sns->list.itemA + snX;
            if(snP->streamN!=1){
              fprintf(stderr, "GDLR: not supporting multiple streams, quitting...\n");
              exit(-1);
            }
            int idx = snP->classXA[0];
            if(idx<0)
              continue;
            Distrib* ds = dss->list.itemA + idx;
            Codebook* cb = cbs->list.itemA + ds->cbX;
            Feature* feature = &fs->featA[cb->featX];
            float** obs = feature->data.fmat->matPA;
            float mix_occ[MAX_MIX];

            computePosterior(cb, ds, obs[frameX], mix_occ);

            for(int m=0;m<cb->refN;m++){
              if(mix_occ[m] < 0.1/cb->refN)
                continue;

              int r = cb2w_idx[ds->cbX][m];
              if(r==-1)
                continue;

              tuple.first = ds->cbX;
              tuple.second = m;
              active_gau[tuple] = true;

              if(z_factor_sum[ds->cbX].isEmpty()){
                z_factor_sum[ds->cbX] = Matrix(cb->refN, dim);
              }
              Matrix& z_factor = z_factor_sum[ds->cbX];

              for(int i=0;i<cb->dimN;i++){
                z_factor[m][i] += gamma*mix_occ[m]*cb->cv[m]->m.d[i]*obs[frameX][i];
              }

              factor_sum[ds->cbX][m] += gamma*mix_occ[m];
              utt_gamma_sum[0][r] += gamma*mix_occ[m];
            }
          }
        }  
      }
    }
  }

  for(int r=0;r<num_classes;r++){
    gamma_sum[0][r] += utt_gamma_sum[0][r];
  }

  for(it=active_gau.begin();it!=active_gau.end();it++){
    tuple = it->first;
    int j = tuple.first;
    int m = tuple.second;
    int r = cb2w_idx[j][m];
    Codebook* cb = cbs->list.itemA + j;
    if(r==-1)
      continue;
    if(utt_gamma_sum[0][r]==0.0)
      continue;
    if(factor_sum[j][m] == 0.0)
      continue;
    Matrix& z_factor = z_factor_sum[j];
    if(z_factor.isEmpty())
      continue;
    Matrix d_mat(dim+1, dim+1);
    for(int i=0;i<dim+1;i++){
      float mean_i = 0.0;
      if(i<dim)
        mean_i = cb->rv->matPA[m][i];
      else
        mean_i = 1.0;
      for(int k=i;k<dim+1;k++){
        float mean_k = 0.0;
        if(k<dim)
          mean_k = cb->rv->matPA[m][k];
        else
          mean_k = 1.0;

        d_mat[i][k] = mean_i * mean_k;
        if(i!=k)
          d_mat[k][i] = d_mat[i][k];
      }
    }
    for(int i=0;i<dim;i++){
      g_mat[r][i] += d_mat*(factor_sum[j][m]*cb->cv[m]->m.d[i]);
      for(int k=0;k<dim;k++){
        z_mat[r][i][k] += z_factor[m][i]*cb->rv->matPA[m][k];
      }
      z_mat[r][i][dim] += z_factor[m][i]*1.0;
    }
  }

  return result;
}

void GDLR::computePosterior(Codebook* cb, Distrib* ds, float* obs, float* mix_occ){
  float norm = LZERO;
  for(int m=0;m<cb->refN;m++){
    float g_const = cb->pi + cb->cv[m]->det;
    float dist = g_const;
    for(int d=0;d<cb->dimN;d++){
      dist += (obs[d] - cb->rv->matPA[m][d])*(obs[d] - cb->rv->matPA[m][d])*cb->cv[m]->m.d[d];
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

void GDLR::update(){
  float ratio = 0.0;
  vector<bool> disabled;

  gamma_sum.printFP(stderr);
  disabled.resize(num_classes);

  for(int r=0;r<num_classes;r++){
    float d = E * gamma_sum[0][r];
    for(int i=0;i<dim;i++){
      z_mat[r][i][i] += d;
      for(int j=0;j<=dim;j++){
        g_mat[r][i][j][j] += d;
      }
    }
  }
  for(int r=0;r<num_classes;r++){
    ratio = 0.0;
    disabled[r] = false;
    transformation[r] = Matrix(dim, dim+1);
    for(int i=0;i<dim;i++){
      float tmp = (g_mat[r][i]*g_mat[r][i].dInverse()).trace();
      if(!isfinite(tmp) || fabs(tmp - (dim+1)) > 1.0){
        transformation[r][i][i] = 1.0;
        continue;
      }
      ratio += 1.0;
      Matrix w_mat = z_mat[r].getRowVec(i) * g_mat[r][i].dInverse();
      for(int j=0;j<dim+1;j++){
        transformation[r][i][j] = w_mat[0][j];
      }
    }
    ratio /= (float)dim;
    if(ratio<0.90 || gamma_sum[0][r] < 1000){
      disabled[r] = true;
      transformation[r].zero();
      for(int i=0;i<dim;i++){
        transformation[r][i][i] = 1.0;
      }
    }
  }

}

void GDLR::transformModel(){
  for(int i=0;i<cbs->list.itemN;i++){
    Codebook* cb = cbs->list.itemA + i;
    Matrix mean_off(dim+1, 1);
    for(int m=0;m<cb->refN;m++){
      int r = cb2w_idx[i][m];
      if(r == -1)
        continue;
      for(int d=0;d<cb->dimN;d++){
        mean_off[d][0] = cb->rv->matPA[m][d];
      }
      mean_off[dim][0] = 1.0;
      Matrix adapted_mean = transformation[r] * mean_off;

      for(int d=0;d<cb->dimN;d++){
        cb->rv->matPA[m][d] = adapted_mean[d][0];
      }
    }
  }
}

