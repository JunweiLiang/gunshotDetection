#include "mllr.h"

MLLR::MLLR(MemoryManager* memmgr, Recognizer* recognizer, int dim, int num_mix, bool is_diag){
  this->mgr=memmgr;
  this->recognizer=recognizer;
  this->dim = dim;
  this->num_mix = num_mix;
  this->is_diag = is_diag;

  this->info = mgr->getRelationInfo();

  num_groups=0;
  mix_occ=NULL;

  getAllGaussians();
}

MLLR::~MLLR(){
//  mllr_info.destroy();
}

void MLLR::adapt(MFCCReader* reader, const vector<Transcription>& transcription, const char* log_gamma_list){
  adaptFull(reader, transcription, log_gamma_list);
}

void MLLR::adaptFull(MFCCReader* reader, const vector<Transcription>& transcription, const char* log_gamma_list){
  Network* fa_net;
  bool compute_log_gamma = (log_gamma_list==NULL);
  vector<State*> state_set;
  Matrix occ, obs, obs_t, occ_t, all_one;
  Matrix mean_off, w_i, a_r, v;
  Matrix i_d(dim, dim);
  vector<vector<Matrix> > term1, term2;
  float tmp, total_prob;

  i_d.identity();
  transformation.clear();
  term1.resize(num_groups);
  term2.resize(num_groups);
  for(int i=0;i<num_groups;i++){
    transformation.push_back(Matrix(dim, dim+1));
    if(is_diag){
      for(int j=0;j<dim;j++){
        term1[i].push_back(Matrix(dim+1, dim+1));
        term2[i].push_back(Matrix(1, dim+1));
      }
    }
    else{
      term1[i].push_back(Matrix(dim*(dim+1), dim*(dim+1)));
      term2[i].push_back(Matrix(dim*(dim+1), 1));
    }
  }
  for(int u=0;u<reader->getTotalUtterance();u++){
//  for(int u=0;u<1;u++){
    printf("adapting utterance %d...\n", u);
    reader->readData(u);
    fa_net = new Network(mgr);
    if(compute_log_gamma){
      recognizer->concatHMM(transcription[u].getSeq(), *fa_net);
      recognizer->setNetwork(fa_net);

      mix_occ = new CacheTable(3, reader->getTotalFrames(), fa_net->getNumStates(), num_mix);
      total_prob = recognizer->getTotalProb(reader->getX(), reader->getTotalFrames());
      recognizer->computeMixOcc(reader->getX(), reader->getTotalFrames(), mix_occ);

      recognizer->process(reader->getX(), reader->getTotalFrames());
      printf("Score: %f\n", total_prob);
    }
    else{
      printf("Not implemented\n");
      exit(0);
    }

    obs = Matrix::makeMatrix(reader->getX(), reader->getTotalFrames(), dim);
    obs_t = obs.transpose();
    occ = Matrix(1, reader->getTotalFrames());
    all_one = Matrix(1, reader->getTotalFrames());
    for(int t=0;t<reader->getTotalFrames();t++)
      all_one[0][t] = 1.0;

    if(is_diag){ //if it is diag cov
      fa_net->getStateSet(state_set);
      for(int j=0;j<state_set.size();j++){
        if(state_set[j]->isNullNode())
          continue;
        MultiMix* mm = (MultiMix*) state_set[j]->getPDF();

        for(int m=0;m<mm->numMixture();m++){
          Gaussian* g = mm->getGaussian(m);
          int group = gau2group[g->getID()];
          mean_off = makeMeanOff(g->getMeanVec());

          for(int t=0;t<reader->getTotalFrames();t++){
            occ[0][t] = mix_occ->get(t, j, m);
          }
          float factor = (all_one*occ.transpose())[0][0];
          Matrix tmp_mat = (mean_off.transpose()*mean_off);
          Matrix tmp_mat2 = obs_t*occ.transpose();
          for(int i=0;i<dim;i++){
            tmp = factor * g->getCovAt(i, i);

            term1[group][i] += tmp_mat*tmp;
            term2[group][i] += (mean_off.transpose()*(tmp_mat2*g->getCovAt(i, i))[i][0]).transpose();
          }
        }
      }
    }
    else{ //if it is full cov
      fa_net->getStateSet(state_set);
      for(int j=0;j<state_set.size();j++){
        if(state_set[j]->isNullNode())
          continue;
        MultiMix* mm = (MultiMix*) state_set[j]->getPDF();

        for(int m=0;m<mm->numMixture();m++){
          Gaussian* g = mm->getGaussian(m);
          int group = gau2group[g->getID()];

          for(int t=0;t<reader->getTotalFrames();t++){
            occ[0][t] = mix_occ->get(t, j, m);
          }
          tmp = (all_one*occ.transpose())[0][0];
          mean_off = makeMeanOff(g->getMeanVec());
          a_r = mean_off.kroneckerProduct(i_d);
          Matrix tmp_mat = a_r.transpose()*g->getCovMat().inverse();

          term1[group][0] += tmp_mat*a_r*tmp;
          term2[group][0] += tmp_mat*(obs_t*occ.transpose());
        }
      }
    }

    delete fa_net;
    delete mix_occ;
  }
  printf("updating the transformation...\n");
  if(is_diag){
    for(int i=0;i<dim;i++){
      for(int g=0;g<num_groups;g++){
        w_i = term1[g][i].inverse()*term2[g][i].transpose();
        for(int l=0;l<dim+1;l++){
          transformation[g][i][l] += w_i[l][0];
        }
      }
    }
  }
  else{
    for(int g=0;g<num_groups;g++){
      v = term1[g][0].dInverse()*term2[g][0];
      //v.transpose().print();
      for(int i=0;i<dim+1;i++){
        for(int j=0;j<dim;j++){
          transformation[g][j][i] += v[i*dim+j][0];
        }
      }
    }
  }

//  transformation[0].print();

  printf("transforming the mean vectors...\n");
  for(int r=0;r<gaussian.size();r++){
    Gaussian* g = gaussian[r];
    int group = gau2group[gaussian[r]->getID()];
    Matrix new_mean(1, dim);

    new_mean = transformation[group]*makeMeanOff(g->getMeanVec()).transpose();

    g->setMeanVec(new_mean.transpose());
  }
/*  
  for(int u=0;u<reader->getTotalUtterance();u++){
    printf("processing utterance %d...\n", u);
    reader->readData(u);
    fa_net = new Network(mgr);
    
    recognizer->concatHMM(transcription[u].getSeq(), *fa_net);
    recognizer->setNetwork(fa_net);

    printf("Score: %f\n", recognizer->getTotalProb(reader->getX(), reader->getTotalFrames()));

    delete fa_net;
  }
*/  
}

void MLLR::adaptDiag(MFCCReader* reader, const vector<Transcription>& transcription, const char* log_gamma_list){
}

void MLLR::printTransformation(FILE* fp){
  fprintf(fp, "<UID> unknown\n");
  fprintf(fp, "<NAME> Unknown\n");
  fprintf(fp, "<MMFID> models\n");
  if(num_groups==1)
    fprintf(fp, "<RCID> global\n");
  else
    fprintf(fp, "<RCID> rtree_32\n");
  fprintf(fp, "<CHAN> Standard\n");
  fprintf(fp, "<DESC> None\n");
  fprintf(fp, "<NBLOCKS> 1\n");
  fprintf(fp, "<NODETHRESH> 0.0\n");

  for(int i=0;i<num_groups;i++){
    fprintf(fp, "<TRANSFORM> %d\n", i+1);
    fprintf(fp, "<MEAN_TR> %d\n", dim);
    fprintf(fp, "<BLOCK> 1\n");
    for(int r=0;r<transformation[i].getNumRow();r++){
      for(int c=1;c<transformation[i].getNumCol();c++){
        fprintf(fp, "%e ", transformation[i][r][c]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "<BIASOFFSET> %d\n", dim);
    for(int r=0;r<transformation[i].getNumRow();r++)
      fprintf(fp, "%e ", transformation[i][r][0]);
    fprintf(fp, "\n");
  }
}

Matrix MLLR::makeMeanOff(const Matrix& mean){
  Matrix result(1, dim+1);

  for(int i=0;i<dim;i++){
    result[0][i] = mean.getValue(0, i);
  }
  result[0][dim] = 1.0;

  return result;
}

Matrix MLLR::makeMeanOffMat(int group){
  return Matrix();
}

Matrix MLLR::makeArMat(const Matrix& mean){
  Matrix ar(dim, 2*dim);

  for(int i=0;i<dim;i++){
    ar[i][i] = mean.getValue(0, i);
    ar[i][i+dim] = 1.0;
  }

  return ar;
}

void MLLR::readGammaMatrix(const char* log_gamma_file){
  FILE* fp;
  int row, col, i, j;

  fp = fopen(log_gamma_file, "r");
  if(fp==NULL){
    printf("Cannot read %s\n", log_gamma_file);
    exit(-1);
  }
  fscanf(fp, "%d %d", &row, &col);

  log_gamma = Matrix(row, col);

  for(i=0;i<row;i++){
    for(j=0;j<col;j++){
      fscanf(fp, "%f", &log_gamma[i][j]);
    }
  }

  fclose(fp);
}

void MLLR::setGlobalGrouping(){
  num_groups = 1;
  transformation.clear();
  if(is_diag)
    transformation.push_back(Matrix(1, dim));
  else
    transformation.push_back(Matrix(dim, dim+1));

  gau2group.clear();
  group2gau.clear();
  for(int i=0;i<gaussian.size();i++){
    gau2group[gaussian[i]->getID()] = 0;
    group2gau[0].push_back(gaussian[i]);
  }
}

void MLLR::setRegGrouping(int num_groups){
  ClusterEngine* engine;

  engine = new ClusterEngine(dim);

  this->num_groups = num_groups;
  transformation.clear();

  for(int i=0;i<num_groups;i++){
    if(is_diag)
      transformation.push_back(Matrix(1, dim));
    else
      transformation.push_back(Matrix(dim, dim+1));
  }

  gau2group.clear();
  group2gau.clear();
  for(int i=0;i<gaussian.size();i++){
    Matrix tmp = gaussian[i]->getMeanVec().transpose();
    engine->addDataPoint(gaussian[i]->getID(), &tmp);
  }

  engine->buildIndices();
  engine->clustering(num_groups, 10);

  for(int i=0;i<gaussian.size();i++){
    gau2group[gaussian[i]->getID()] = engine->getClusterNo(gaussian[i]->getID());
    group2gau[gau2group[gaussian[i]->getID()]].push_back(gaussian[i]);
  }

  delete engine;
}

void MLLR::setGrouping(){
  gau2group.clear();
}

vector<Matrix> MLLR::getTransformations(){
  return transformation;
}

map<string, int> MLLR::getGroupInfo(){
  return gau2group;
}

void MLLR::getAllGaussians(){
  vector<State*> state_set;
  vector<string> key_list;
  map<Gaussian*, bool> flag;

  gaussian.clear();

  info->getKeyList("HMMNameTable", key_list);
  for(int k=0;k<key_list.size();k++){
    AegisHMM* h = (AegisHMM*) info->getFirstAnswer("HMMNameTable", key_list[k]);
    h->getStateSet(state_set);
    for(int i=0;i<state_set.size();i++){
      if(state_set[i]->isNullNode())
        continue;
      MultiMix* mm = (MultiMix*) state_set[i]->getPDF();
      for(int j=0;j<mm->numMixture();j++){
        Gaussian* g = mm->getGaussian(j);
        if(flag[g])
          continue;
        gaussian.push_back(g);
        flag[g]=true;
      }
    }
  }
}
