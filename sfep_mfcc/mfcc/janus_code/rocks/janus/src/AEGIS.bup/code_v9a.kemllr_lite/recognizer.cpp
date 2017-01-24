#include <fstream>
#include "recognizer.h"

Recognizer::Recognizer(MemoryManager* mgr, RelationInfo* config){
  char* tmp;
  
  this->mgr=mgr;
  this->info=mgr->getRelationInfo();
  this->network = NULL;

  if(config->hasKey("Config", "dim")){
    config->getAssociateValue("Config", "dim", (Ptr)&dim);
  }
  else{
    printf("dimension undefined\n");
    exit(-1);
  }
     
  if(config->hasKey("Config", "InsertionPenalty")){
    config->getAssociateValue("Config", "InsertionPenalty", (Ptr)&insert_penalty);
  }
  else
    insert_penalty = 0.0;

  if(config->hasKey("Config", "Skip"))
    config->getAssociateValue("Config", "Skip", (Ptr)&skip);
  else
    skip=0;
  
  if(config->hasKey("Config", "Beam")){
    config->getAssociateValue("Config", "Beam", (Ptr)&beam);
    pruning = true;
  }
  else{
    beam=0.0;
    pruning = false;
  }
    
  if(config->hasKey("Config", "Debug"))
    config->getAssociateValue("Config", "Debug", (Ptr)&debug);
  else
    debug=false;

  history_pool.clear();
  plain_cache = NULL;
}

Recognizer::~Recognizer(){
  removeAllHistory();
}

void Recognizer::buildIndices(){
  null_set.clear();
  null_idx.clear();
  real_set.clear();
  real_idx.clear();

  network->topSort(null_set, null_idx);

  for(int i=0;i<state_set.size();i++){
    if(!state_set[i]->isNullNode()){
      real_set.push_back(state_set[i]);
      real_idx.push_back(i);
    }
  }

  int idx;
  vector<PDF*> pdf;

  pdf.resize(state_set.size());
  pdf_idx.resize(state_set.size());

  for(int i=0;i<state_set.size();i++){
    pdf[i] = state_set[i]->getPDF();
    if(state_set[i]->isNullNode())
      pdf_idx[i]=-1;
    else
      pdf_idx[i] = i;
  }
  
  for(int i=0;i<pdf.size();i++){
    for(int j=0;j<i;j++){
      if(pdf[i]==pdf[j] && pdf[j]!=NULL){
        pdf_idx[i] = pdf_idx[j];
        break;
      }
    }
  }
}

void Recognizer::swap(Token** cur, Token** next){
  Token* tmp = *cur;

  *cur = *next;
  *next = tmp;
}

void Recognizer::swap(bool** cur, bool** next){
  bool* tmp = *cur;

  *cur = *next;
  *next = tmp;
}

void Recognizer::forceAlign(const vector<string>& label, float** x, int t_max, Transcription& transcription, Network& tmp_net){
  transcription.reset(t_max);

  concatHMM(label, tmp_net);
  network = &tmp_net;
  setNetwork(network);
 
  process(x, t_max);
  decode(transcription, t_max);
}

void Recognizer::estimate(const vector<Transcription>& transcription, MFCCReader* reader, HMMSet* hmmset){
  Accumulator* acc;
  vector<State*> state_set;
  vector<Edge*> edge_set;
  Matrix mean_vec, cov, tmp;
  
  acc = new Accumulator(hmmset);
  for(int u=0;u<reader->getTotalUtterance();u++){
    printf("u: %d\n", u);
    reader->readData(u);
    
    accumulate(transcription[u].getSeq(), reader->getX(), reader->getTotalFrames(), acc);
  }

  hmmset->getStateSet(state_set);
  hmmset->getEdgeSet(edge_set);

  for(int j=0;j<state_set.size();j++){
    if(state_set[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*)state_set[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      if(acc->getMixOccSum(j, m) == 0.0)
        continue;

      mean_vec = acc->getMeanNumer(j, m)*(1.0/acc->getMixOccSum(j, m));
      cov = acc->getCovNumer(j, m)*(1.0/acc->getMixOccSum(j, m));

      for(int d=0;d<dim;d++){
        cov[d][0] = cov[d][0] - mean_vec[d][0]*mean_vec[d][0];
        cov[d][0] = 1.0/cov[d][0];
      }

      DiagGaussian* g = (DiagGaussian*) mm->getGaussian(m);
      g->setMeanVec(mean_vec.transpose());
      g->setCovMat(cov.transpose());

      mm->setWeight(m, acc->getMixOccSum(j, m)/acc->getGammaSum(j));
    }
    hmmset->getEmittingEdgeSet(state_set[j], edge_set);
    for(int i=0;i<edge_set.size();i++){
      edge_set[i]->setProb(acc->getEpsilonSum(edge_set[i])/acc->getGammaSumMinus1(j));
    }
  }
  
  hmmset->print("tmp.mmf");

  delete acc;
}

void Recognizer::accumulate(const vector<string>& label, float** x, int t_max, Accumulator* accumulator){
  Network* fa_net;
  Matrix alpha, beta, gamma;
  CacheTable* mix_occ;
  vector<map<int, map<int, float> > > log_epsilon;
  int num_mix = accumulator->getHMMSet()->getNumMix();
  float* bak_cache=this->plain_cache;
  bool* bak_avail=this->available;
  CacheTable* bak = cache;

  fa_net = new Network(mgr);
  concatHMM(label, *fa_net);
  setNetwork(fa_net);

  initPlainCache(pdf_idx.size());
  computeForwardScore(x, t_max, alpha, plain_cache, available);
  computeBackwardScore(x, t_max, beta, plain_cache, available);
  computeLogGamma(x, t_max, gamma);

  mix_occ = new CacheTable(3, t_max, fa_net->getNumStates(), num_mix);
  computeLogMixOcc(x, t_max, mix_occ);
  computeLogEpsilon(x, t_max, log_epsilon);

  accumulator->accumulate(fa_net, x, t_max, cache, alpha, beta, gamma, mix_occ, log_epsilon);
 
  freePlainCache();
  delete mix_occ;
  delete fa_net;

  this->plain_cache = bak_cache;
  this->available = bak_avail;
}

void Recognizer::process(float** x, int t_max){
  Token *token_list, *next_token_list, *init_token_list, *tmp_list;
  int start, end;

  initPlainCache(pdf_idx.size());

  removeAllHistory();
  active_set.clear();
  next_active_set.clear();

  is_active = new bool[state_set.size()];
  next_is_active = new bool[state_set.size()];
  memset(is_active, 0, sizeof(bool)*state_set.size());
  memset(next_is_active, 0, sizeof(bool)*state_set.size());

  token_list = new Token[state_set.size()];
  next_token_list = new Token[state_set.size()];
  init_token_list = new Token[state_set.size()];
//  init_token_list = (Token*) malloc(sizeof(Token)*state_set.size());
  memcpy(init_token_list, next_token_list, sizeof(Token)*state_set.size());

  token_list[start_idx].setScore(0.0);
  token_list[start_idx].setState(start_idx);
  token_list[start_idx].setHistory(makeHistory(NULL, state_set[start_idx], start_idx));
  active_set.push_back(start_idx);
  history_pool.push_back(token_list[start_idx].getHistory());

  for(start=0;start<null_set.size();start++){
    if(null_idx[start]==start_idx)
      break;
  }
  for(end=null_set.size()-1;end>=0;end--){
    if(null_idx[end]==end_idx)
      break;
  }

  updateNullNode(token_list, 0, 0);
  
  real2pdf_idx.clear();
  real2pdf_idx.resize(state_set.size());
  for(int i=0;i<state_set.size();i++){
    real2pdf_idx[i]=-1;
  }
  for(int i=0;i<real_idx.size();i++){
    real2pdf_idx[real_idx[i]]=pdf_idx[real_idx[i]];
  }
  
  for(int t=0;t<t_max;t++){
//    printf("t: %d, %d %f\n", t, active_set.size(), max_score);
    max_score = LZERO;
    active_set.reserve(RESERV_MEM);
    next_active_set.reserve(RESERV_MEM);
    memset(available, 0, sizeof(bool)*pdf_idx.size());
    updateRealNode(token_list, next_token_list, x, t);

    swap(&token_list, &next_token_list);
    swap(&is_active, &next_is_active);
    active_set.swap(next_active_set);

    if(pruning)
      pruneRealNode(token_list, beam);

    next_active_set.clear();
    memcpy(next_token_list, init_token_list, sizeof(Token)*state_set.size());
    memset(next_is_active, 0, sizeof(bool)*state_set.size());

    updateNullNode(token_list, t, 0);
  }

  result = token_list[end_idx];

  delete [] is_active;
  delete [] next_is_active;
  freePlainCache();
  delete [] token_list;
  delete [] next_token_list;
  delete [] init_token_list;

  return;
}

int Recognizer::pruneRealNode(Token* token_list, float beam){
  int num_active=0;
  int cur_idx;
  float thres = max_score - beam;
  vector<short> new_active_set;

  for(int i=0;i<active_set.size();i++){
    cur_idx = active_set[i];
    if(!state_set[cur_idx]->isNullNode() && token_list[cur_idx].getScore()<thres){
      token_list[cur_idx].clear();
      is_active[cur_idx] = false;
    }
    else{
      new_active_set.push_back(cur_idx);
      num_active++;
    }
  }
  active_set.swap(new_active_set);
  return num_active;
}

float Recognizer::getTotalProb(float** x, int t_max){
  float* bak_cache = this->plain_cache;
  bool* bak_avail = this->available;
  Matrix alpha;

  initPlainCache(pdf_idx.size());
  computeForwardScore(x, t_max, alpha, plain_cache, available);
  freePlainCache();

  this->plain_cache = bak_cache;
  this->available = bak_avail;

  return getLogLikelihood();
}

void Recognizer::computeLogGamma(float** x, int t_max, Matrix& log_gamma){
  Matrix alpha, beta;
  
  log_gamma = Matrix(t_max, network->getNumStates());

  initPlainCache(pdf_idx.size());
  computeForwardScore(x, t_max, alpha, plain_cache, available);
  computeBackwardScore(x, t_max, beta, plain_cache, available);
  //beta.print();
  freePlainCache();

  for(int t=0;t<t_max;t++){
    float tmp=LZERO;

    for(int j=0;j<network->getNumStates();j++){
      if(state_set[j]->isNullNode())
        continue;
      tmp = LogAdd(tmp, alpha[t][j]+beta[t][j]);
    }
    for(int j=0;j<network->getNumStates();j++){
      if(!state_set[j]->isNullNode())
        log_gamma[t][j] = alpha[t][j]+beta[t][j]-tmp;
      else
        log_gamma[t][j] = LZERO;
      if(log_gamma[t][j]>0.0){
        log_gamma[t][j] = 0.0;
      }
    }
  }
}

void Recognizer::transform(vector<Matrix>& transformation){
/*
  ClusterEngine* engine;

  engine = new ClusterEngine(dim);
  for(int i=0;i<gaussian.size();i++){
    Matrix tmp = gaussian[i]->getMeanVec().transpose();
    engine->addDataPoint(gaussian[i]->getID(), &tmp);
  }

  engine->buildIndices();
  engine->clustering(num_groups, 10);

  for(int i=0;i<gaussian.size();i++){
    gau2group[gaussian[i]->getID()] = engine->getClusterNo(gaussian[i]->getID());
    group2gau[gau2group[gaussian[i]->getID()]].push_back(gaussian[i]);
    old_mean.push_back(gaussian[i]->getMeanVec());
  }

  delete engine;
*/
}

void Recognizer::computeLogEpsilon(float** x, int t_max, vector<map<int, map<int, float> > >& log_epsilon){
/*
  Matrix alpha, beta;
  map<int, map<int, float> > log_tran_prob;
  vector<vector<int> > adj_list;
  vector<float> frac;
  float value;
  CacheTable* bak = cache;
  map<int, float> empty;

  log_epsilon.clear();
  log_epsilon.resize(t_max-1);

  network->buildAdjList(adj_list);
  network->buildTranTable(log_tran_prob);

  cache = new CacheTable(1, state_set.size());
  computeForwardScore(x, t_max, alpha, cache);
  computeBackwardScore(x, t_max, beta, cache);

  frac.resize(t_max-1);
  for(int t=0;t<t_max-1;t++){
    frac[t] = LZERO;
    for(int i=0;i<state_set.size();i++){
      for(int k=0;k<adj_list[i].size();k++){
        int j=adj_list[i][k];

        if(state_set[i]->isNullNode() || state_set[j]->isNullNode())
          continue;

        frac[t] = LogAdd(frac[t], alpha[t][i] + log_tran_prob[i][j] + cache->get(pdf_idx[j]) + beta[t+1][j]);
      }
    }
//    frac[t] = 1.0/exp(frac[t]);
  }

  for(int t=0;t<t_max-1;t++){
    for(int i=0;i<state_set.size();i++){
      log_epsilon[t][i] = empty;
      for(int k=0;k<adj_list[i].size();k++){
        int j=adj_list[i][k];

        if(state_set[j]->isNullNode())
          continue;

        value = alpha[t][i] + log_tran_prob[i][j] + cache->get(pdf_idx[j]) + beta[t+1][j];

        log_epsilon[t][i][j] = value - frac[t];
      }
    }
  }
  
  delete cache;
  cache = bak;
*/
}

void Recognizer::computeMixOcc(float** x, int t_max, CacheTable* mix_occ){
  computeLogMixOcc(x, t_max, mix_occ);

  mix_occ->filterAll(exp);
}

void Recognizer::computeVitMixOcc(float** x, int t_max, CacheTable* mix_occ){
  computeVitLogMixOcc(x, t_max, mix_occ);

  mix_occ->filterAll(exp);
}

void Recognizer::computeVitMixOcc(vector<State*>& state_seq, float** x, int t_max, CacheTable* mix_occ){
  computeVitLogMixOcc(state_seq, x, t_max, mix_occ);

  mix_occ->filterAll(exp);
}

void Recognizer::computeLogMixOcc(float** x, int t_max, CacheTable* mix_occ){
  Matrix alpha, beta;
  float value;
  Matrix log_gamma;

  computeLogGamma(x, t_max, log_gamma);

  mix_occ->makeAvailable();
  mix_occ->setAll(LZERO);

  for(int t=0;t<t_max;t++){
    for(int j=0;j<network->getNumStates();j++){
      if(state_set[j]->isNullNode())
        continue;
      MultiMix* mm = (MultiMix*) state_set[j]->getPDF();
      for(int m=0;m<mm->numMixture();m++){
        Gaussian* g = mm->getGaussian(m);
       
        value = log_gamma[t][j] + log(mm->getWeight(m)) + g->logBj(x[t]) - mm->logBj(x[t]);
        if(value < LSMALL)
          continue;
        mix_occ->set(value, t, j, m);
      }
    }
  }
}

void Recognizer::computeVitLogMixOcc(float** x, int t_max, CacheTable* mix_occ){
  float value;
  vector<State*> state_seq;

  process(x, t_max);
  decode(state_seq);
//  fprintf(stderr, "ckpt %d\n", state_seq.size());

  mix_occ->makeAvailable();
  mix_occ->setAll(LZERO);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      Gaussian* g = mm->getGaussian(m);
      value = log(mm->getWeight(m)) + g->logBj(x[t]) - mm->logBj(x[t]);
      if(value < LSMALL)
        continue;
      mix_occ->set(value, j, m);
    }
    t++;
  }
}

void Recognizer::computeVitLogMixOcc(vector<State*>& state_seq, float** x, int t_max, CacheTable* mix_occ){
  float value;

  mix_occ->makeAvailable();
  mix_occ->setAll(LZERO);

  int t=0;
  for(int j=0;j<state_seq.size();j++){
    if(state_seq[j]->isNullNode())
      continue;
    MultiMix* mm = (MultiMix*) state_seq[j]->getPDF();
    for(int m=0;m<mm->numMixture();m++){
      Gaussian* g = mm->getGaussian(m);
      value = log(mm->getWeight(m)) + g->logBj(x[t]) - mm->logBj(x[t]);
      if(value < LSMALL)
        continue;
      mix_occ->set(value, j, m);
    }
    t++;
  }
}


void Recognizer::computeForwardScore(float** x, int t_max, Matrix& alpha, float* plain_cache, bool* available){
  Token *token_list, *next_token_list, *tmp_list;
  float* bak_cache = this->plain_cache;
  bool* bak_avail = this->available;
  int start, end;

  this->plain_cache = plain_cache;
  this->available = available;

  alpha = Matrix(t_max, network->getNumStates());

  active_set.clear();
  next_active_set.clear();

  is_active = new bool[state_set.size()];
  next_is_active = new bool[state_set.size()];
  memset(is_active, 0, sizeof(bool)*state_set.size());
  memset(next_is_active, 0, sizeof(bool)*state_set.size());

  removeAllHistory();

  token_list = new Token[state_set.size()];
  next_token_list = new Token[state_set.size()];

  token_list[start_idx].setScore(0.0);
  active_set.push_back(start_idx);

  for(start=0;start<null_set.size();start++){
    if(null_idx[start]==start_idx)
      break;
  }
  for(end=null_set.size()-1;end>=0;end--){
    if(null_idx[end]==end_idx)
      break;
  }
  
  updateFwdNullNode(token_list, 0, 0);

  for(int t=0;t<t_max;t++){
//    printf("t: %d %d\n", t, active_set.size());
    active_set.reserve(RESERV_MEM);
    next_active_set.reserve(RESERV_MEM);
    memset(available, 0, sizeof(bool)*pdf_idx.size());
    updateFwdRealNode(token_list, next_token_list, x, t);

    swap(&token_list, &next_token_list);
    swap(&is_active, &next_is_active);
    active_set.swap(next_active_set);

    if(pruning)
      pruneRealNode(token_list, beam);

    updateFwdNullNode(token_list, t, 0);

    for(int i=0;i<state_set.size();i++){
      if(!state_set[i]->isNullNode())
        alpha[t][i] = token_list[i].getScore();
      else
        alpha[t][i] = LZERO;
      next_token_list[i].clear();
    }
    next_active_set.clear();
    memset(next_is_active, 0, sizeof(bool)*state_set.size());
  }

  result = token_list[end_idx];

  delete [] is_active;
  delete [] next_is_active;
  delete [] token_list;
  delete [] next_token_list;
  this->plain_cache = bak_cache;
  this->available = bak_avail;
}

void Recognizer::computeBackwardScore(float** x, int t_max, Matrix& beta, float* plain_cache, bool* available){
  Token *token_list, *next_token_list, *tmp_list;
  float* bak_cache = this->plain_cache;
  bool* bak_avail = this->available;
  int start, end;

  this->plain_cache = plain_cache;
  this->available = available;
  
  beta = Matrix(t_max, network->getNumStates());

  active_set.clear();
  next_active_set.clear();

  is_active = new bool[state_set.size()];
  next_is_active = new bool[state_set.size()];
  memset(is_active, 0, sizeof(bool)*state_set.size());
  memset(next_is_active, 0, sizeof(bool)*state_set.size());

  removeAllHistory();

  token_list = new Token[state_set.size()];
  next_token_list = new Token[state_set.size()];

  token_list[end_idx].setScore(0.0);
  active_set.push_back(end_idx);

  for(start=0;start<null_set.size();start++){
    if(null_idx[start]==start_idx)
      break;
  }
  for(end=null_set.size()-1;end>=0;end--){
    if(null_idx[end]==end_idx)
      break;
  }

  updateBwdNullNode(token_list, x, t_max-1, 0);
  
  for(int t=t_max-1;t>=0;t--){
    active_set.reserve(RESERV_MEM);
    next_active_set.reserve(RESERV_MEM);
    memset(available, 0, sizeof(bool)*pdf_idx.size());
    updateBwdRealNode(token_list, next_token_list, x, t);

    swap(&token_list, &next_token_list);
    swap(&is_active, &next_is_active);
    active_set.swap(next_active_set);

    if(pruning)
      pruneRealNode(token_list, beam);

    updateBwdNullNode(token_list, x, t, 0);

    for(int i=0;i<state_set.size();i++){
      beta[t][i] = token_list[i].getScore();
      if(!state_set[i]->isNullNode())
        beta[t][i] -= state_set[i]->logBj(x[t]);
      else
        beta[t][i] = LZERO;
      next_token_list[i].clear();
    }

    next_active_set.clear();
    memset(next_is_active, 0, sizeof(bool)*state_set.size());
  }
  result = token_list[start_idx];

  delete [] is_active;
  delete [] next_is_active;
  delete [] token_list;
  delete [] next_token_list;
  
  this->plain_cache = bak_cache;
  this->available = bak_avail;
}

void Recognizer::updateNullNode(Token* token_list, int t, int start){
  float cur_score, tmp_score;
  int parent_idx, cur_idx, max_idx, k;
 
  for(int i=start;i<null_idx.size();i++){
    cur_idx = null_idx[i];
    cur_score = LZERO;
    for(int j=0;j<parent_list[cur_idx].size();j++){
      parent_idx = parent_list[cur_idx][j];
      tmp_score = token_list[parent_idx].getScore() + log_tran_prob[parent_idx][cur_idx];
      if(network->isWordEnd(parent_idx))
        tmp_score += insert_penalty;
      if(tmp_score > cur_score){
        cur_score = tmp_score;
        max_idx = parent_idx;
      }
    }
    
    if(cur_score < LSMALL){
      continue;
    }
    
    token_list[cur_idx].setHistory(token_list[max_idx].getHistory());
    token_list[cur_idx].setScore(cur_score);
    if(!is_active[cur_idx]){
      active_set.push_back(cur_idx);
    }
  }
}

void Recognizer::updateFwdNullNode(Token* token_list, int t, int start){
  float cur_score, tmp_score;
  int parent_idx, cur_idx, max_idx, k;
 
  for(int i=start;i<null_idx.size();i++){
    cur_idx = null_idx[i];
    cur_score = LZERO;
    for(int j=0;j<parent_list[cur_idx].size();j++){
      parent_idx = parent_list[cur_idx][j];
      tmp_score = token_list[parent_idx].getScore() + log_tran_prob[parent_idx][cur_idx];
      if(tmp_score<LSMALL)
        continue;
      cur_score = LogAdd(cur_score, tmp_score);
    }
    if(cur_score < LSMALL){
      continue;
    }
    token_list[cur_idx].setScore(cur_score);
    if(!is_active[cur_idx]){
      active_set.push_back(cur_idx);
    }
  }
}

void Recognizer::updateBwdNullNode(Token* token_list, float** x, int t, int start){
  float cur_score, tmp_score, obs;
  int adj_idx, cur_idx, parent_idx, max_idx, k;
 
  for(int i=null_idx.size()-1;i>=0;i--){
    cur_idx = null_idx[i];
    cur_score = LZERO;
    for(int j=0;j<adj_list[cur_idx].size();j++){
      adj_idx = adj_list[cur_idx][j];

      tmp_score = token_list[adj_idx].getScore() + log_tran_prob[cur_idx][adj_idx];
      if(tmp_score < LSMALL)
        continue;
      cur_score = LogAdd(cur_score, tmp_score);
    }

    if(cur_score < LSMALL){
      continue;
    }

    token_list[cur_idx].setScore(cur_score);
    if(!is_active[cur_idx]){
      active_set.push_back(cur_idx);
    }
  }
}

string Recognizer::getHMM(int index){ //debug purpose
  AegisHMM* h = (AegisHMM*) info->getFirstAnswer("StateInHMM", state_set[index]->getID());

  if(h==NULL)
    return "NULL: "+state_set[index]->getID();

  return h->getName();
}

void Recognizer::updateRealNode(Token* token_list, Token* next_token_list, float** x, int t){
  float cur_score, tmp_score, obs;
  int parent_idx, cur_idx, adj_idx, max_idx;
  vector<int> prev_idx;

  prev_idx.resize(state_set.size());
  max_score = LZERO;

  for(int i=0;i<active_set.size();i++){
    cur_idx = active_set[i];
    cur_score = token_list[cur_idx].getScore();

    for(int j=0;j<adj_list[cur_idx].size();j++){
      adj_idx = adj_list[cur_idx][j];
      if(state_set[adj_idx]->isNullNode())
        continue;
      if(available[real2pdf_idx[adj_idx]]){
        obs = plain_cache[real2pdf_idx[adj_idx]];
      }
      else{
        obs = state_set[adj_idx]->logBj(x[t]);
        plain_cache[real2pdf_idx[adj_idx]] = obs;
        available[real2pdf_idx[adj_idx]]=true;
      }
      tmp_score =  cur_score + obs + log_tran_prob[cur_idx][adj_idx];
//      printf("obs: %f %f %f %f\n", tmp_score, cur_score, obs, log_tran_prob[cur_idx][adj_idx]);

      if(tmp_score > next_token_list[adj_idx].getScore()){
        next_token_list[adj_idx].setScore(tmp_score);
        prev_idx[adj_idx] = cur_idx;
        if(!next_is_active[adj_idx]){
          next_active_set.push_back(adj_idx);
          next_is_active[adj_idx] = true;
        }
      }
    }
  }
  for(int i=0;i<next_active_set.size();i++){
    cur_idx = next_active_set[i];
    History* h = token_list[prev_idx[cur_idx]].getHistory();
    next_token_list[cur_idx].setHistory(h);
    
    if(next_token_list[cur_idx].getScore() > max_score)
      max_score = next_token_list[cur_idx].getScore();

//    if(state_set[prev_idx[cur_idx]]->isNullNode() && !state_set[cur_idx]->isNullNode()){
    if(!state_set[cur_idx]->isNullNode()){
      next_token_list[cur_idx].extendHistory(state_set[cur_idx], cur_idx, t, h->score);
      history_pool.push_back(next_token_list[cur_idx].getHistory());
    }
  }
/*
  for(int i=0;i<real_idx.size();i++){
    cur_idx = real_idx[i];
    cur_score = LZERO;
    
    if(available[real2pdf_idx[cur_idx]]){
      obs = plain_cache[real2pdf_idx[cur_idx]];
    }
    else{
      obs = state_set[cur_idx]->logBj(x[t]);
      plain_cache[real2pdf_idx[cur_idx]] = obs;
      available[real2pdf_idx[cur_idx]]=true;
    }
    for(int j=0;j<parent_list[cur_idx].size();j++){
      parent_idx = parent_list[cur_idx][j];
      tmp_score = token_list[parent_idx].getScore() + log_tran_prob[parent_idx][cur_idx];
      if(tmp_score > cur_score){
        cur_score = tmp_score;
        max_idx = parent_idx;
      }
    }
    cur_score += obs;
    if(cur_score < LSMALL)
      continue;
    if(cur_score > max_score)
      max_score = cur_score;

    next_token_list[cur_idx].setHistory(token_list[max_idx].getHistory());
    next_token_list[cur_idx].setScore(cur_score);
    //next_active_set.push_back(cur_idx);
    //next_is_active[cur_idx] = true;

    if(state_set[max_idx]->isNullNode()){
      next_token_list[cur_idx].extendHistory(state_set[cur_idx], cur_idx, t, cur_score);

      history_pool.push_back(next_token_list[cur_idx].getHistory());
    }
  }
*/
}

void Recognizer::updateFwdRealNode(Token* token_list, Token* next_token_list, float** x, int t){
  /*
  float cur_score, tmp_score, obs;
  int parent_idx, cur_idx, max_idx;
 
  for(int i=0;i<real_idx.size();i++){
    cur_idx = real_idx[i];
    cur_score = LZERO;
   
    if(available[pdf_idx[cur_idx]]){
      obs = plain_cache[pdf_idx[cur_idx]];
    }
    else{
      obs = state_set[cur_idx]->logBj(x[t]);
      plain_cache[pdf_idx[cur_idx]] = obs;
      available[pdf_idx[cur_idx]] = true;
    }

    for(int j=0;j<parent_list[cur_idx].size();j++){
      parent_idx = parent_list[cur_idx][j];
      tmp_score = token_list[parent_idx].getScore() + log_tran_prob[parent_idx][cur_idx] + obs;
      if(tmp_score < LSMALL)
        continue;
      cur_score = LogAdd(cur_score, tmp_score);
    }
    if(cur_score < LSMALL)
      continue;
    next_token_list[cur_idx].setScore(cur_score);
  }
  */
  float cur_score, tmp_score, next_score, obs;
  int parent_idx, cur_idx, adj_idx, max_idx;
  vector<int> prev_idx;

  prev_idx.resize(state_set.size());
  max_score = LZERO;

  for(int i=0;i<active_set.size();i++){
    cur_idx = active_set[i];
    cur_score = token_list[cur_idx].getScore();

    for(int j=0;j<adj_list[cur_idx].size();j++){
      adj_idx = adj_list[cur_idx][j];
      if(state_set[adj_idx]->isNullNode())
        continue;
      if(available[pdf_idx[adj_idx]]){
        obs = plain_cache[pdf_idx[adj_idx]];
      }
      else{
        obs = state_set[adj_idx]->logBj(x[t]);
        plain_cache[pdf_idx[adj_idx]] = obs;
        available[pdf_idx[adj_idx]]=true;
      }
      tmp_score =  cur_score + obs + log_tran_prob[cur_idx][adj_idx];
      next_score = next_token_list[adj_idx].getScore();

      next_score = LogAdd(next_score, tmp_score);

      next_token_list[adj_idx].setScore(next_score);
      prev_idx[adj_idx] = cur_idx;
      if(!next_is_active[adj_idx]){
        next_active_set.push_back(adj_idx);
        next_is_active[adj_idx] = true;
      }
    }
  }
  for(int i=0;i<next_active_set.size();i++){
    cur_idx = next_active_set[i];
    if(next_token_list[cur_idx].getScore() > max_score)
      max_score = next_token_list[cur_idx].getScore();
  }
}

void Recognizer::updateBwdRealNode(Token* token_list, Token* next_token_list, float** x, int t){
  float tmp_score, next_score, obs;
  int adj_idx, cur_idx, max_idx, parent_idx;
 
  for(int i=0;i<active_set.size();i++){
    cur_idx = active_set[i];
    
    for(int j=0;j<parent_list[cur_idx].size();j++){
      parent_idx = parent_list[cur_idx][j];
      if(state_set[parent_idx]->isNullNode())
        continue;
      if(available[pdf_idx[parent_idx]]){
        obs = plain_cache[pdf_idx[parent_idx]];
      }
      else{
        obs = state_set[parent_idx]->logBj(x[t]);
        plain_cache[pdf_idx[parent_idx]] = obs;
        available[pdf_idx[parent_idx]] = true;
      }

      tmp_score = token_list[cur_idx].getScore() + log_tran_prob[parent_idx][cur_idx] + obs;

      if(tmp_score < LSMALL)
        continue;

      next_score = next_token_list[parent_idx].getScore();
      next_score = LogAdd(next_score, tmp_score);
      next_token_list[parent_idx].setScore(next_score);
      if(!next_is_active[parent_idx]){
        next_active_set.push_back(parent_idx);
        next_is_active[parent_idx] = true;
      }
    }
  }
/*
  float cur_score, tmp_score, obs;
  int adj_idx, cur_idx, max_idx, parent_idx;
 
  for(int i=0;i<real_idx.size();i++){
    cur_idx = real_idx[i];
    cur_score = LZERO;
    
    for(int j=0;j<adj_list[cur_idx].size();j++){
      adj_idx = adj_list[cur_idx][j];
      if(available[pdf_idx[cur_idx]]){
        obs = plain_cache[pdf_idx[cur_idx]];
      }
      else{
        obs = state_set[cur_idx]->logBj(x[t]);
        plain_cache[pdf_idx[cur_idx]] = obs;
        available[pdf_idx[cur_idx]] = true;
      }

      tmp_score = token_list[adj_idx].getScore() + log_tran_prob[cur_idx][adj_idx] + obs;

      if(tmp_score < LSMALL)
        continue;
      cur_score = LogAdd(cur_score, tmp_score);
    }
    if(cur_score < LSMALL)
      continue;
    next_token_list[cur_idx].setScore(cur_score);
  }
*/
}

void Recognizer::decode(vector<State*>& state_seq){
  vector<State*> tmp;
  History* h;
  vector<float> tmp_s;
  vector<int> tmp_seq;

  decode(tmp_seq);
  
  score_list.clear();
  state_seq.clear();
  h = result.getHistory();

  if(h==NULL){
    printf("Null history in decoding\n");
    return;
  }

  for(int i=0;i<tmp_seq.size();i++){
    state_seq.push_back(state_set[tmp_seq[i]]);
  }
}

void Recognizer::decode(vector<int>& state_seq){
  vector<int> tmp;
  History* h;
  vector<float> tmp_s;

  score_list.clear();
  state_seq.clear();
  h = result.getHistory();

  if(h==NULL){
    printf("Null history in decoding\n");
    return;
  }

  do{
    tmp.push_back(h->state_id);
    tmp_s.push_back(h->score);
    h = h->past;
  }while(h!=NULL);
  tmp.pop_back();
  tmp_s.pop_back();

  for(int i=tmp.size()-1;i>=0;i--){
    state_seq.push_back(tmp[i]);
    score_list.push_back(tmp_s[i]);
  }
}

void Recognizer::decode(vector<int>& state_seq, vector<int>& time){
  vector<int> tmp;
  History* h;
  vector<int> tmp_s;

  time.clear();
  score_list.clear();
  state_seq.clear();
  h = result.getHistory();

  if(h==NULL){
    printf("Null history in decoding\n");
    return;
  }

  do{
    tmp.push_back(h->state_id);
    tmp_s.push_back(h->time);
    h = h->past;
  }while(h!=NULL);
  tmp.pop_back();
  tmp_s.pop_back();

  for(int i=tmp.size()-1;i>=0;i--){
    state_seq.push_back(tmp[i]);
    time.push_back(tmp_s[i]);
  }
}

void Recognizer::decode(vector<int>& state_seq, vector<float>& score){
  vector<int> tmp;
  History* h;
  vector<float> tmp_s;

  score.clear();
  state_seq.clear();
  h = result.getHistory();

  if(h==NULL){
    printf("Null history in decoding\n");
    return;
  }

  do{
    tmp.push_back(h->state_id);
    tmp_s.push_back(h->score);
    h = h->past;
  }while(h!=NULL);
  tmp.pop_back();
  tmp_s.pop_back();

  for(int i=tmp.size()-1;i>=0;i--){
    state_seq.push_back(tmp[i]);
    score.push_back(tmp_s[i]);
  }
}

void Recognizer::decode(vector<State*>& state_seq, vector<float>& score){
  vector<State*> tmp;
  History* h;
  vector<float> tmp_s;
  vector<int> tmp_seq;

  decode(tmp_seq, score);
  for(int i=0;i<tmp_seq.size();i++){
    state_seq.push_back(state_set[tmp_seq[i]]);
  }
}


void Recognizer::decode(vector<string>& word_seq){
  vector<State*> state_seq;
  AegisHMM* h;
  bool flag=true;
  History* his;
  vector<int> time;  

  word_seq.clear();
  decode(state_seq);

  his = result.getHistory();
  time.resize(state_seq.size());
  for(int i=state_seq.size()-1;i>=0;i--){
    if(his==NULL)
      continue;
    time[i] = his->time;
    his = his->past;
  }

  for(int i=0;i<state_seq.size();i++){
    h = (AegisHMM*) info->getFirstAnswer("StateInHMM", state_seq[i]->getID().c_str());

    if(h==NULL)
      continue;

    if(h->getEndState()==state_seq[i] && flag){
      word_seq.push_back(h->getName());
      flag = false;
    }
    if(h->getEndState()!=state_seq[i])
      flag = true;
  }
}

void Recognizer::decode(vector<string>& word_seq, vector<float>& score){
  vector<State*> state_seq;
  AegisHMM* h;
  bool flag=true;
  History* his;
  vector<int> time;
  float pre, cur;
  vector<float> tmp;

  word_seq.clear();
  score.clear();
  decode(state_seq);

  his = result.getHistory();
  time.resize(state_seq.size());
  tmp.resize(state_seq.size());
  for(int i=state_seq.size()-1;i>=0;i--){
    if(his==NULL)
      continue;
    time[i] = his->time;
    tmp[i] = his->score;
    his = his->past;
  }

  pre = 0.0;
  for(int i=0;i<state_seq.size();i++){
    h = (AegisHMM*) info->getFirstAnswer("StateInHMM", state_seq[i]->getID().c_str());

    if(h==NULL)
      continue;

    if(h->getEndState()==state_seq[i] && flag){
      word_seq.push_back(h->getName());
      cur = tmp[i];
      score.push_back(cur-pre);
      pre = cur;
      flag = false;
    }
    if(h->getEndState()!=state_seq[i])
      flag = true;
  }
}

void Recognizer::decode(Transcription& transcription, int t_max){
  vector<int> seq, time;
  AegisHMM* h;
  bool flag=true;
  History* his;

  transcription.reset(0);
  decode(seq, time);

  for(int i=0;i<seq.size();i++){
    h = (AegisHMM*) info->getFirstAnswer("StateInHMM", state_set[seq[i]]->getID().c_str());
    if(h==NULL)
      continue;
    if(i!=seq.size()-1){
      transcription.addWord(h->getName(), time[i], time[i+1]);
    }
    else{
      transcription.addWord(h->getName(), time[i], t_max);
    }
  }
}

float Recognizer::getLogLikelihood(){
//  if(result.getHistory()==NULL)
//    return LZERO;
  return result.getScore();
}

History* Recognizer::makeHistory(History* past, State* state, int state_id){
  History* history;

  history = new History();
  history->past = past;
  //history->state = state;
  history->state_id = state_id;
  if(past!=NULL)
    history->time = past->time+1;
  else
    history->time = 0;

  return history;
}

void Recognizer::removeAllHistory(){
  for(int i=0;i<history_pool.size();i++){
    delete history_pool[i];
  }
  history_pool.clear();
}

void Recognizer::concatHMM(const vector<string>& label, Network& network){
  network.clear();

  for(int i=0;i<label.size();i++){
    AegisHMM* h = (AegisHMM*) info->getFirstAnswer("HMMNameTable", label[i]);
    vector<State*> state_set;
    if(h==NULL){
      fprintf(stderr, "HMM %s not found\n", label[i].c_str());
      exit(0);
    }
    h->getStateSet(state_set);
    network.addHMM(h);
    //fprintf(stderr, "concat: %s %s %d %d\n", label[i].c_str(), h->getName().c_str(), state_set.size(), network.getNumStates());
    if(i>0){
      network.linkNetNode(i-1, i);
    }
  }
  network.getStateSet(state_set);
  //printf("concat: %d\n", state_set.size());
  network.determineBeginEnd();
}

void Recognizer::setNetwork(Network* network){
  this->network = network;

  network->getStateSet(state_set);
  network->buildAdjList(adj_list);
//  network->buildAdjRealList(adj_real_list);
  network->buildParentList(parent_list);
  network->buildTranTable(log_tran_prob);

  network->determineBeginEnd();
  start_idx = network->getStart();
  end_idx = network->getEnd();

//  printf("%d %d\n", start_idx, end_idx);

  buildIndices();
}

Network* Recognizer::getNetwork(){
  return network;
}

void Recognizer::initPlainCache(int size){
  if(plain_cache!=NULL){
    delete plain_cache;
    delete available;
  }
  plain_cache = new float[size];
  available = new bool[size];
  memset(available, 0, sizeof(bool)*size);
}

void Recognizer::freePlainCache(){
  if(plain_cache!=NULL){
    delete [] plain_cache;
    delete [] available;
  }
  plain_cache = NULL;
  available = NULL;
}

