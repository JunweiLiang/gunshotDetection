#include "g-mmie.h"

GMMIE::GMMIE(SenoneSet* sns, CodebookSet* cbs, CodebookSet* base_cbs, DistribSet* dss, FeatureSet* fs, SVMap* svmap, float scale){
  this->sns = sns;
  this->cbs = cbs;
  this->base_cbs = base_cbs;
  this->dss = dss;
  this->fs = fs;
  this->svmap = svmap;
  this->scale = scale;
  this->d_factor = 100.0;

  clearAccu();
}

void GMMIE::clearAccu(){
}

void GMMIE::saveAccu(char* header){
}

void GMMIE::loadAccu(char* header){
}

void GMMIE::loadAddAccu(char* header){
}

float GMMIE::getScale(){
  return scale;
}

void GMMIE::setScale(float scale){
  this->scale = scale;
}

void GMMIE::computeScore(GNode* nodeP, int parentX, int childX, float& cur_score, float& base_score){
  cur_score = base_score = 0.0;
  
  Path* path = nodeP->addon.MPECDStats[parentX][childX]->path;
  if(path==NULL)
    return;

  for(int t=0;t<path->pitemListN;t++){
    PathItemList& plist = path->pitemListA[t];
    int frameX = path->firstFrame + t;
    //fprintf(stderr, "t: %d, frX: %d\n", t, frameX);

    for(int j=0;j<plist.itemN;j++){
      PathItem& item = plist.itemA[j];
      float gamma = item.gamma;
      int snX = item.senoneX;

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
      Codebook* base_cb = base_cbs->list.itemA + ds->cbX;
      Feature* feature = &fs->featA[cb->featX];
      float** obs = feature->data.fmat->matPA;

      vector<float> mix_occ, base_occ;

      computePosterior(cb, obs[frameX], mix_occ);
//      computePosterior(base_cb, obs[frameX], base_occ);

      for(int m=0;m<cb->refN;m++){
        if(mix_occ[m] >= 0.1/cb->refN){
          for(int d=0;d<cb->dimN;d++){
            base_score += gamma*mix_occ[m]*cb->cv[m]->m.d[d]*(obs[frameX][d] - cb->rv->matPA[m][d])*(obs[frameX][d] - cb->rv->matPA[m][d]);
          }
        }
        //if(base_occ[m] >= 0.1/cb->refN){
        if(mix_occ[m] >= 0.1/cb->refN){
          for(int d=0;d<cb->dimN;d++){
            //base_score += gamma*base_occ[m]*base_cb->cv[m]->m.d[d]*(obs[frameX][d] - base_cb->rv->matPA[m][d])*(obs[frameX][d] - base_cb->rv->matPA[m][d]);
            cur_score += gamma*mix_occ[m]*base_cb->cv[m]->m.d[d]*(obs[frameX][d] - base_cb->rv->matPA[m][d])*(obs[frameX][d] - base_cb->rv->matPA[m][d]);
          }
        }
      }
    }
  }
}

float GMMIE::findAlphaBeta(GLat* glatP, int* refNodeLst, int refNodeN, vector<map<int, map<int, float> > >& alpha, vector<map<int, map<int, float> > >& beta, float learning_rate, char* in_lambda, char* out_lambda){
  char buf[STRMAX];
  vector<bool> is_ref;
  int ref_start=-1;
  vector<unsigned short> ref_idx;
  float ref_sil_alpha=0.0, ref_sil_beta=0.0;

  is_ref.resize(glatP->nodeN);
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    for(int i=0;i<refNodeN;i++){
      if(refNodeLst[i]==nodeX){
        is_ref[nodeX] = true;
        if(ref_start==-1)
          ref_start = nodeX;
        break;
      }
    }
  }
  unsigned short cur_idx = 0;
  ref_idx.resize(glatP->nodeN);
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    ref_idx[nodeX]=-1;
    if(is_ref[nodeX]){
      GNode* nodeP = glatP->mapping.x2n[nodeX];
      //fprintf(stderr, "ref: nodeX: %d, svx: %d, filler: %d\n", nodeX, nodeP->svX, svmap->svocabP->list.itemA[nodeP->svX].fillerTag);
      if(!svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
        ref_idx[nodeX]=cur_idx;
        cur_idx++;
      }
    }
  }

  sprintf(buf, "%s.dat", in_lambda);
  FILE* fp = fopen(buf, "rb");
  
  alpha.clear();
  alpha.resize(glatP->nodeN);
  beta.clear();
  beta.resize(glatP->nodeN);
  if(fp==NULL){
    ref_sil_alpha = 1.0;
    ref_sil_beta = 0.0;
    for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
      GNode* nodeP = glatP->mapping.x2n[nodeX];
      GLatMPECDStats ***MPECDStats=nodeP->addon.MPECDStats;
      
      for(unsigned short parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
        for(unsigned short childX=0;childX<nodeP->addon.childCtxN;childX++){
          if(is_ref[nodeX]){
            alpha[nodeX][parentX][childX] = 1.0;
            beta[nodeX][parentX][childX] = 0.0;
          }
          else{
            alpha[nodeX][parentX][childX] = 0.5;
            beta[nodeX][parentX][childX] = 0.5;
          }
        }
      }
    }
  }
  else{
    while(true){
      unsigned short nodeX;
      unsigned short parentX, childX;
      float val;
      unsigned char flag;

      fread(&flag, sizeof(unsigned char), 1, fp);
      if(feof(fp))
        break;
      if(flag==0){
        fread(&nodeX, sizeof(unsigned short), 1, fp);
        fread(&parentX, sizeof(unsigned short), 1, fp);
        fread(&childX, sizeof(unsigned short), 1, fp);
        fread(&val, sizeof(float), 1, fp);
        alpha[nodeX][parentX][childX] = val;
        fread(&val, sizeof(float), 1, fp);
        beta[nodeX][parentX][childX] = val;
      }
      else if(flag==1){
        unsigned short idx;
        fread(&idx, sizeof(unsigned short), 1, fp);
        int tmp = 0;
        for(int i=0;i<refNodeN;i++){
          GNode* nodeP = glatP->mapping.x2n[refNodeLst[i]];
          if(!svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
            if(tmp==idx){
              idx = i;
              break;
            }
            tmp++;
          }
        }
        nodeX = ref_start + idx;
        fread(&val, sizeof(float), 1, fp);
        alpha[nodeX][0][0] = val;
        fread(&val, sizeof(float), 1, fp);
        beta[nodeX][0][0] = val;
      }
      else{
        fread(&ref_sil_alpha, sizeof(float), 1, fp);
        fread(&ref_sil_beta, sizeof(float), 1, fp);

        for(nodeX=ref_start;nodeX<glatP->nodeN;nodeX++){
          GNode* nodeP = glatP->mapping.x2n[nodeX];
          if(is_ref[nodeX] && svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
            alpha[nodeX][0][0] = ref_sil_alpha;
            beta[nodeX][0][0] = ref_sil_beta;
          }
        }
      }
    }
    fclose(fp);
  }

  sprintf(buf, "%s.dat", out_lambda);
  fp = fopen(buf, "wb");

  float total_score = 0.0;
  float ref_alpha_sum = 0.0, ref_beta_sum = 0.0;
  float com_alpha_sum = 0.0, com_beta_sum = 0.0;
  int ref_count = 0;
  int com_count = 0;
  vector<map<int, map<int, float> > > alpha_grad, beta_grad;
  vector<map<int, map<int, float> > > q_score, c_score;

  alpha_grad.resize(glatP->nodeN);
  beta_grad.resize(glatP->nodeN);
  q_score.resize(glatP->nodeN);
  c_score.resize(glatP->nodeN);
  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
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
        float a = alpha[nodeX][parentX][childX];
        float b = beta[nodeX][parentX][childX];
        q_score[nodeX][parentX][childX] = cur_score;
        c_score[nodeX][parentX][childX] = base_score;
        
        if(is_ref[nodeX]){
          scaled_score = base_score - scale*base_score;
          total_score += fabs(cur_score - scaled_score);
          ref_alpha_sum += a;
          ref_beta_sum += b;
          ref_count++;
        //fprintf(stderr, "ckpt 2 %d %d %d %f %f %f %0.2f %0.2f\n", nodeX, parentX, childX, cur_score, scaled_score, base_score, alpha[nodeX][parentX][childX], beta[nodeX][parentX][childX]);
        }
        else{
          scaled_score = base_score + scale*base_score;
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
        
        vector<Matrix> partial_q_phi, partial_phi_alpha, partial_phi_beta;
        partial_q_phi.resize(cbs->list.itemN);
        partial_phi_alpha.resize(cbs->list.itemN);
        
        for(int t=0;t<path->pitemListN;t++){
          PathItemList& plist = path->pitemListA[t];
          int frameX = path->firstFrame + t;

          for(int j=0;j<plist.itemN;j++){
            PathItem& item = plist.itemA[j];
            float gamma = item.gamma;
            int snX = item.senoneX;

            if(gamma==0.0)
              continue;

            if(!is_ref[nodeX])
              gamma *= node_cd_occ;

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
            Codebook* base_cb = base_cbs->list.itemA + ds->cbX;
            Feature* feature = &fs->featA[cb->featX];
            float** obs = feature->data.fmat->matPA;

            vector<float> mix_occ;

            computePosterior(cb, obs[frameX], mix_occ);

            if(partial_q_phi[ds->cbX].isEmpty()){
              partial_q_phi[ds->cbX] = Matrix(cb->refN, cb->dimN);
              partial_phi_alpha[ds->cbX] = Matrix(cb->dimN, cb->refN);
            }
          
            for(int m=0;m<cb->refN;m++){
              if(mix_occ[m] < 0.1/cb->refN)
                continue;
              for(int d=0;d<cb->dimN;d++){
                partial_q_phi[ds->cbX][m][d] += gamma*mix_occ[m]*cb->cv[m]->m.d[d]*(obs[frameX][d]-cb->rv->matPA[m][d])*(-1.0);
                partial_phi_alpha[ds->cbX][d][m] += gamma*mix_occ[m]*(obs[frameX][d]);
              }
            }
          }
        }
        for(int i=0;i<cbs->list.itemN;i++){
          Codebook* cb = cbs->list.itemA + i;
          Codebook* base_cb = base_cbs->list.itemA + i;
          if(partial_q_phi[i].isEmpty())
            continue;
          if(fabs(a-b) > 0.001){
            for(int m=0;m<cb->refN;m++){
              float norm = base_cb->accu->count[0][m] - base_cb->accu->count[1][m];
              if(fabs(norm)<10.0)
                continue;
              float val = 0.0;
              for(int d=0;d<cb->dimN;d++){
                val += partial_q_phi[i][m][d]*partial_phi_alpha[i][d][m];
              }
              alpha_grad[nodeX][parentX][childX] += (a - b)*val / norm;
              beta_grad[nodeX][parentX][childX] -= (a - b)*val / norm;
            }
          }
        }
        partial_q_phi.clear();
        partial_phi_alpha.clear();
        partial_phi_beta.clear();

        alpha_grad[nodeX][parentX][childX] += diff - epsilon/d_factor;
        beta_grad[nodeX][parentX][childX] += -diff - epsilon/d_factor;
      }
    }
  }

  for(unsigned short nodeX=0;nodeX<glatP->nodeN;nodeX++){
    GNode* nodeP = glatP->mapping.x2n[nodeX];
    GLatMPECDStats ***MPECDStats=nodeP->addon.MPECDStats;

    for(unsigned short parentX=0;parentX<nodeP->addon.parentCtxN;parentX++){
      for(unsigned short childX=0;childX<nodeP->addon.childCtxN;childX++){
        Path* path = MPECDStats[parentX][childX]->path;
        if(path==NULL)
          continue;

//        fprintf(stderr, "ckpt 1: %d %f %f %f %f %f %f\n", is_ref, alpha_grad, beta_grad, diff, epsilon, a, b);
        float cur_score = q_score[nodeX][parentX][childX];
        float base_score = c_score[nodeX][parentX][childX];
        //computeScore(nodeP, parentX, childX, cur_score, base_score);
        float scaled_score = 0.0;
        float a = alpha[nodeX][parentX][childX];
        float b = beta[nodeX][parentX][childX];
        if(is_ref[nodeX]){
          scaled_score = base_score - scale*base_score;
        }
        else{
          scaled_score = base_score + scale*base_score;
        }

        a = a + learning_rate*alpha_grad[nodeX][parentX][childX]; //0.0001
        //b = b + learning_rate*beta_grad[nodeX][parentX][childX];

        if(a<0.0)
          a=0.0;
        else if(a>1.0)
          a=1.0;
        b = 1.0 - a;

        if(fabs(cur_score - scaled_score) > 0.015*base_score){
          alpha[nodeX][parentX][childX] = a;
          beta[nodeX][parentX][childX] = b;
          if(is_ref[nodeX] && svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
            ref_sil_alpha = a;
            ref_sil_beta = b;
          }
        }
 //        fprintf(stderr, "ckpt 1a: %f %f\n", alpha[nodeX][parentX][childX], beta[nodeX][parentX][childX]);
        unsigned char flag=0;

        if(!is_ref[nodeX]){
          flag = 0;
          fwrite(&flag, sizeof(unsigned char), 1, fp);
          fwrite(&nodeX, sizeof(unsigned short), 1, fp);
          fwrite(&parentX, sizeof(unsigned short), 1, fp);
          fwrite(&childX, sizeof(unsigned short), 1, fp);
          fwrite(&alpha[nodeX][parentX][childX], sizeof(float), 1, fp);
          fwrite(&beta[nodeX][parentX][childX], sizeof(float), 1, fp);
        }
        else if(!svmap->svocabP->list.itemA[nodeP->svX].fillerTag){
          flag = 1;
          fwrite(&flag, sizeof(unsigned char), 1, fp);
          fwrite(&ref_idx[nodeX], sizeof(unsigned short), 1, fp);
          fwrite(&alpha[nodeX][0][0], sizeof(float), 1, fp);
          fwrite(&beta[nodeX][0][0], sizeof(float), 1, fp);
//          fprintf(stderr, "ckpt a %d %d(%d %d) %.2f(%f) %.2f(%f)\n", flag, ref_idx[nodeX], svmap->svocabP->list.itemA[nodeP->svX].fillerTag, nodeX, alpha[nodeX][0][0], alpha_grad, beta[nodeX][0][0], beta_grad);
        }
//        bool tmp = is_ref[nodeX];
//        fprintf(stderr, "ckpt 2 %d(%d) %d %d %.4f %.4f, c: %.4f s: %.4f\n", nodeX, tmp, parentX, childX, alpha[nodeX][parentX][childX], beta[nodeX][parentX][childX], cur_score, scaled_score);
      }
    }
  }
  unsigned char flag=2;
  fwrite(&flag, sizeof(unsigned char), 1, fp);
  fwrite(&ref_sil_alpha, sizeof(float), 1, fp);
  fwrite(&ref_sil_beta, sizeof(float), 1, fp);

  fclose(fp);
  INFO("ref_alpha_avg: %f, ref_beta_avg: %f\n", ref_alpha_sum/ref_count, ref_beta_sum/ref_count);
  INFO("com_alpha_avg: %f, com_beta_avg: %f\n", com_alpha_sum/com_count, com_beta_sum/com_count);
  INFO("ref_fil_alpha: %f, ref_fil_beta: %f\n", ref_sil_alpha, ref_sil_beta);

  return total_score;
}
    
float GMMIE::accu(GLat* glatP, int* refNodeLst, int refNodeN, float learning_rate, char* in_lambda, char* out_lambda){
  float result = 0.0;
  int pos_active, neg_active, in_active, pruned;
  vector<map<int, map<int, float> > > alpha, beta;
  int max_iter = 5;

  pos_active = neg_active = in_active = pruned = 0;

  result+=findAlphaBeta(glatP, refNodeLst, refNodeN, alpha, beta, learning_rate, in_lambda, out_lambda);
  //findAlphaBeta(glatP, refNodeLst, refNodeN, alpha, beta, learning_rate, in_lambda, out_lambda);

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

        int accu_idx = -1;
        float factor = alpha[nodeX][parentX][childX] - beta[nodeX][parentX][childX];
//        fprintf(stderr, "ckpt 4 %d %d %d %f\n", nodeX, parentX, childX, factor);
        if(factor>0.0001){
          pos_active++;
          accu_idx = 0;
        }
        else if(factor<0.0001){
          neg_active++;
          accu_idx = 1;
        }
        else{
          in_active++;
          accu_idx = -1;
        }

        if(accu_idx==-1)
          continue;

        factor = fabs(factor);
        if(!is_ref)
          factor*=node_cd_occ;
        
        if(factor<0.00001){
          pruned++;
          if(accu_idx==0)
            pos_active--;
          else if(accu_idx==1)
            neg_active--;
          continue;
        }

        for(int t=0;t<path->pitemListN;t++){
          PathItemList& plist = path->pitemListA[t];
          int frameX = path->firstFrame + t;

          for(int j=0;j<plist.itemN;j++){
            PathItem& item = plist.itemA[j];
            float gamma = item.gamma*factor;
            int snX = item.senoneX;

            if(gamma==0.0)
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
            Codebook* base_cb = base_cbs->list.itemA + ds->cbX;
            Feature* feature = &fs->featA[cb->featX];
            float** obs = feature->data.fmat->matPA;

            vector<float> mix_occ;

            computePosterior(cb, obs[frameX], mix_occ);
          
            for(int m=0;m<cb->refN;m++){
              if(mix_occ[m] < 0.1/cb->refN)
                continue;
//              if(is_ref)
//                result+=mix_occ[m]*node_cd_occ;
              for(int d=0;d<cb->dimN;d++){
                cb->accu->rv[accu_idx]->matPA[m][d] += gamma*mix_occ[m]*(obs[frameX][d]);
                cb->accu->sumOsq[accu_idx][m].m.d[d] += gamma*mix_occ[m]*(obs[frameX][d])*(obs[frameX][d]);
              }
              cb->accu->count[accu_idx][m] += gamma*mix_occ[m];
            }
          }
        }  
      }
    }
  }

  INFO("pos: %d, neg: %d, in_act: %d, pruned: %d\n", pos_active, neg_active, in_active, pruned);

  return result;
}

void GMMIE::computePosterior(Codebook* cb, float* obs, vector<float>& mix_occ){
  mix_occ.clear();
  mix_occ.resize(cb->refN);

  float norm = LZERO;
  for(int m=0;m<cb->refN;m++){
    float g_const = - cb->pi - cb->cv[m]->det;
    float dist = 0.0;
    for(int d=0;d<cb->dimN;d++){
      dist += (obs[d] - cb->rv->matPA[m][d])*(obs[d] - cb->rv->matPA[m][d])*cb->cv[m]->m.d[d];
    }
    dist *= -0.5;
    mix_occ[m] = g_const + dist;
    norm = LogAdd(norm, g_const + dist);
  }
  for(int m=0;m<cb->refN;m++){
    if(mix_occ[m] - norm <LSMALL)
      mix_occ[m] = 0.0;
    else
      mix_occ[m] = exp(mix_occ[m] - norm);
  }
}

void GMMIE::update(){
 
  for(int i=0;i<cbs->list.itemN;i++){
    Codebook* cb = cbs->list.itemA + i;
    Codebook* base_cb = base_cbs->list.itemA + i;

    for(int m=0;m<cb->refN;m++){
      if(fabs(cb->accu->count[0][m] - cb->accu->count[1][m]) < 50.0)
        continue;
      float val = 0.0;
      base_cb->cv[m]->det = 0.0;
      for(int d=0;d<cb->dimN;d++){
//        fprintf(stderr, "m: %d, d: %d, old_mean: %e, ", m, d, cb->rv->matPA[m][d]);
        base_cb->rv->matPA[m][d] = (cb->accu->rv[0]->matPA[m][d] - cb->accu->rv[1]->matPA[m][d])/(cb->accu->count[0][m] - cb->accu->count[1][m]);
//        fprintf(stderr, "new_mean: %e %0.2f\n", cb->rv->matPA[m][d], cb->accu->count[0][m] - cb->accu->count[1][m]);
        float val = (cb->accu->sumOsq[0][m].m.d[d] - cb->accu->sumOsq[1][m].m.d[d])/(cb->accu->count[0][m] - cb->accu->count[1][m]) - base_cb->rv->matPA[m][d]*base_cb->rv->matPA[m][d];
        if(val<0.0){
          base_cb->cv[m]->det += log(1.0/base_cb->cv[m]->m.d[d]);
          continue;
        }
        if(val>COV_FLOOR){
          base_cb->cv[m]->m.d[d] = 1.0/val;
          base_cb->cv[m]->det += log(val);
        }
        else{
          base_cb->cv[m]->det += log(1.0/base_cb->cv[m]->m.d[d]);
        }
      }
    }
  }
  clearAccu();
}
