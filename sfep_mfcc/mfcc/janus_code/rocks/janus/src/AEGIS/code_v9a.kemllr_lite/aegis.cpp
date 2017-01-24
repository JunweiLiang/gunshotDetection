#include "aegis.h"

Aegis::Aegis(int argc, char* argv[]){
  info = new RelationInfo();
  config = new RelationInfo();
  mgr = new MemoryManager(info);
  parser = new HTKParser(mgr);
  reader = new MFCCReader();
  hmm_set = NULL;
  ei_kev = NULL;
  version = "9.0a";

  defineGarbagePhone();

  info->addTable("HMMNameTable");

  this->num_threads = 1;

  setNumThreads(num_threads);

  getOpt(argc, argv, config);
}

Aegis::~Aegis(){
  delete reader;
  delete recognizer;
  delete dict;
  delete parser;
  delete network;
  delete mgr;
  delete config;
  delete info;
}

int Aegis::setNumThreads(int num_threads){
  this->num_threads = num_threads;
  omp_set_num_threads(num_threads);
  return 0;
}

void Aegis::setKEMLLR(EiKEV* ei_kev){
  this->ei_kev = ei_kev;
}

EiKEV* Aegis::getKEMLLR(){
  return ei_kev;
}

void Aegis::setMPE(MPE* mpe){
  this->mpe = mpe;
}

MPE* Aegis::getMPE(){
  return mpe;
}

void Aegis::setFMPE(FMPE* fmpe){
  this->fmpe = fmpe;
}

FMPE* Aegis::getFMPE(){
  return fmpe;
}

void Aegis::setGMMIE(GMMIE* gmmie){
  this->gmmie = gmmie;
}

GMMIE* Aegis::getGMMIE(){
  return gmmie;
}

void Aegis::setGDFSA(GDFSA* gdfsa){
  this->gdfsa = gdfsa;
}

GDFSA* Aegis::getGDFSA(){
  return gdfsa;
}

void Aegis::setGDLR(GDLR* gdlr){
  this->gdlr = gdlr;
}

GDLR* Aegis::getGDLR(){
  return gdlr;
}

void Aegis::setRSW(EV* ev){
  this->ev = ev;
}

EV* Aegis::getRSW(){
  return ev;
}

void Aegis::setRSWExt(RSWExt* rsw_ext){
  this->rsw_ext = rsw_ext;
}

RSWExt* Aegis::getRSWExt(){
  return rsw_ext;
}

void Aegis::exportPDFSet(CodebookSet* codebook_set, DistribSet* distrib_set){
  fprintf(stderr, "Aegis: exporting cbs %s and dss %s...\n", codebook_set->name, distrib_set->name);

  CodebookList cb_list = codebook_set->list;
  DistribList ds_list = distrib_set->list;
  dim = cb_list.itemA[0].dimN;

  for(int i=0;i<ds_list.itemN;i++){
    Distrib* ds = &ds_list.itemA[i];
    Codebook* cb = &cb_list.itemA[ds->cbX];
    //MultiMix* mm = (MultiMix*) pdf_set[ds_name2pdf_idx[ds->name]];
    MultiMix* mm = (MultiMix*) pdf_set[ds_idx2pdf_idx[i]];
    for(int m=0;m<ds->valN;m++){
      Gaussian* g = mm->getGaussian(m);
      for(int d=0;d<dim;d++){
        cb->rv->matPA[m][d] = g->getMeanAt(d);
        cb->cv[m]->m.d[d] = g->getCovAt(d, d);
      }
    }
  }
  fprintf(stderr, "Aegis: exported %d codebooks\n", pdf_set.size());
}

void Aegis::updatePDFSet(CodebookSet* codebook_set, DistribSet* distrib_set){
  fprintf(stderr, "Aegis: updating Aegis' internal cbs %s and dss %s...\n", codebook_set->name, distrib_set->name);

  CodebookList cb_list = codebook_set->list;
  DistribList ds_list = distrib_set->list;
  dim = cb_list.itemA[0].dimN;

  for(int i=0;i<ds_list.itemN;i++){
    Distrib* ds = &ds_list.itemA[i];
    Codebook* cb = &cb_list.itemA[ds->cbX];
    MultiMix* mm = (MultiMix*) pdf_set[ds_name2pdf_idx[ds->name]];
    for(int m=0;m<ds->valN;m++){
      DiagGaussian* g = (DiagGaussian*) mm->getGaussian(m);
      for(int d=0;d<dim;d++){
        g->getMean()[d] = cb->rv->matPA[m][d];
        g->getCov()[d] = cb->cv[m]->m.d[d];
      }
    }
  }
  fprintf(stderr, "Aegis: Done\n");
  fflush(stdout);
}

void Aegis::importPDFSet(CodebookSet* codebook_set, DistribSet* distrib_set){
  fprintf(stderr, "Aegis: importing cbs %s and dss %s...\n", codebook_set->name, distrib_set->name);

  pdf_set.clear();
  ds_name2pdf_idx.clear();
  ds_idx2pdf_idx.clear();

  CodebookList cb_list = codebook_set->list;
  DistribList ds_list = distrib_set->list;
  dim = cb_list.itemA[0].dimN;
  ds_idx2pdf_idx.resize(ds_list.itemN);

  for(int i=0;i<ds_list.itemN;i++){
    Distrib* ds = &ds_list.itemA[i];
    //printf("%s, valN: %d, count: %f, cbX: %d, dim: %d\n", ds->name, ds->valN, ds->count, ds->cbX, dim);
    //fflush(stdout);

    //if(ds->count==0.0)
    //  continue;
    MultiMix* multi_mix = mgr->createMultiMix(dim);
    Codebook* cb = &cb_list.itemA[ds->cbX];
    for(int m=0;m<ds->valN;m++){
      Gaussian* g = mgr->createDiagGaussian(dim);
      Matrix mean(1, dim), cov(1, dim);
      for(int d=0;d<dim;d++){
        mean[0][d] = cb->rv->matPA[m][d];
        cov[0][d] = cb->cv[m]->m.d[d];
      }
      g->setMeanVec(mean);
      g->setCovMat(cov);
      g->calculateGConst();
      multi_mix->addGaussian(g, exp(-ds->val[m]));
    }
    pdf_set.push_back(multi_mix);
    ds_idx2pdf_idx[i] = pdf_set.size()-1;
    ds_name2pdf_idx[ds->name] = pdf_set.size()-1;
  }
  gaussian.clear();
  for(int i=0;i<pdf_set.size();i++){
    MultiMix* mm = (MultiMix*) pdf_set[i];
    for(int m=0;m<mm->numMixture();m++){
      Gaussian* g = mm->getGaussian(m);

      gaussian.push_back(g);
    }
  }
  fprintf(stderr, "Aegis: imported %d codebooks\n", pdf_set.size());
}

void Aegis::importPhonesSet(PhonesSet* phones_set, bool tri_phone){
  PhonesList phs_list = phones_set->list;
  vector<string> monophone;

  fprintf(stderr, "Aegis: importing phones set %s...\n", phones_set->name);

  all_classes.clear();

  for(int i=0;i<phs_list.itemN;i++){
    PhoneList ph_list = phs_list.itemA[i].list;
    for(int j=0;j<ph_list.itemN;j++){
      bool flag=false;
      if(strcmp(ph_list.itemA[j].name, "@")==0)
        continue;
      for(int k=0;k<monophone.size();k++){
        if(monophone[k]==ph_list.itemA[j].name){
          flag=true;
          break;
        }
      }
      if(flag)
        continue;
      monophone.push_back(ph_list.itemA[j].name);
    }
  }

  if(!tri_phone){
    all_classes = monophone;
  }
  else{
    for(int c=0;c<monophone.size();c++){
      if(isGarbage(monophone[c]))
        continue;
      for(int l=0;l<monophone.size();l++){
        for(int r=0;r<monophone.size();r++){
          string ph = monophone[l]+string("-");
          ph += monophone[c]+string("-");
          ph += monophone[r];
          all_classes.push_back(ph);
        }
      }
    }
    for(int i=0;i<garbage.size();i++){
      all_classes.push_back(garbage[i]);
    }
  }
/*
  for(int i=0;i<all_classes.size();i++){
    printf("ckpt: %s\n", all_classes[i].c_str());
  }
*/
  fprintf(stderr, "Aegis: imported %d phones\n", all_classes.size());
  return;
}

void Aegis::importHMMSet(Tree* distrib_tree){

  TreeNodeList dtree_list = distrib_tree->list;
  Word word;
  int cd=0, argc=1;
  char *argv;
  char keys[STRMAX];

  fprintf(stderr, "Aegis: importing distrib tree %s...\n", distrib_tree->name);
  fprintf(stderr, "Aegis: constructing HMM representations...\n");

  hmm_set = new HMMSet(dim);

  argv = new char[STRMAX];
  sprintf(argv, "");
  /*
  sprintf(argv[0], "");
  sprintf(argv[1], "-1");
  sprintf(argv[2], "1");
  sprintf(argv[3], "-node");
  sprintf(argv[4], "1");
  */
  sprintf(keys, "<tagged phones>");

  wordInit(&word, distrib_tree->questions.phones, distrib_tree->questions.tags);

  for(int i=0;i<all_classes.size();i++){
    int context = numField((char*)all_classes[i].c_str(), "-");
    argc = 1;
    if(context==3){
      char left_ph[STRMAX], center_ph[STRMAX], right_ph[STRMAX], ph_pos[STRMAX], buf[STRMAX];
      int nodeX=-1, leafX=-1;
      State* state=NULL;
      vector<State*> state_set;
      
      AegisHMM* hmm = mgr->createHMM(all_classes[i]);
      info->addRelation("HMMNameTable", hmm->getName(), (Ptr)hmm);

      sprintf(buf, "%s", all_classes[i].c_str());
      extractField(buf, 0, "-", left_ph);
      extractField(buf, 1, "-", center_ph);
      extractField(buf, 2, "-", right_ph);
      
      sprintf(&argv[0], "{%s WB} %s {%s WB}", left_ph, center_ph, right_ph);
    //  sprintf(argv[1], "-1");
     // sprintf(argv[2], "1");
      getWord((void*)&cd, keys, (void*)&word, &argc, (char**) &argv);

      state_set.clear();
      state = mgr->createState(NULL);
      hmm->insertStartState(state);
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      state_set.push_back(state);

      sprintf(ph_pos, "%s(|)-s1", center_ph);
      nodeX = getNodeX((char*)ph_pos, &dtree_list);
      leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
      state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      state_set.push_back(state);
      hmm->insertState(state);
    
      sprintf(ph_pos, "%s(|)-s2", center_ph);
      nodeX = getNodeX((char*)ph_pos, &dtree_list);
      leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
      state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      state_set.push_back(state);
      hmm->insertState(state);
    
      sprintf(ph_pos, "%s(|)-s3", center_ph);
      nodeX = getNodeX((char*)ph_pos, &dtree_list);
      leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
      state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      state_set.push_back(state);
      hmm->insertState(state);

      state = mgr->createState(NULL);
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      state_set.push_back(state);
      hmm->insertEndState(state);

      for(int j=0;j<state_set.size()-1;j++){
        Edge* edge;
        if(j!=0){
          edge = mgr->createEdge(state_set[j], state_set[j], 0.5);
          hmm->insertEdge(edge);
        }
        if(j!=0)
          edge = mgr->createEdge(state_set[j], state_set[j+1], 0.5);
        else
          edge = mgr->createEdge(state_set[j], state_set[j+1], 1.0);
        hmm->insertEdge(edge);
      }
      
      hmm_set->addHMM(hmm);
    }
    else if(context==1){
    /*
      char ph_pos[STRMAX];
      int nodeX=-1, leafX=-1;
      State* state=NULL;
      vector<State*> state_set;

      AegisHMM* hmm = mgr->createHMM(all_classes[i]);

      sprintf(&argv[0], "%s", all_classes[i].c_str());
//      sprintf(argv[1], "0");
//      sprintf(argv[2], "0");
     
      argc = 1;
      getWord((void*)&cd, keys, (void*)&word, &argc, (char**)&argv);
      
      state_set.clear();
      if(all_classes[i]=="SIL"){
        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertStartState(state);
        state_set.push_back(state);
        
        sprintf(ph_pos, "SIL(|)-s2");
        nodeX = getNodeX((char*)ph_pos, &dtree_list);
        leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
        for(int i=0;i<4;i++){
          state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
          info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
          //state = mgr->createState(pdf_set[leafX]);
          state_set.push_back(state);
          hmm->insertState(state);
        }

        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertEndState(state);
        state_set.push_back(state);
      }
      else if(all_classes[i]=="sp"){
        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertStartState(state);
        state_set.push_back(state);
        
        sprintf(ph_pos, "SIL(|)-s2");
        nodeX = getNodeX((char*)ph_pos, &dtree_list);
        leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
        state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        //state = mgr->createState(pdf_set[leafX]);
        state_set.push_back(state);
        hmm->insertState(state);

        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertEndState(state);
        state_set.push_back(state);
      }
      else if(all_classes[i]=="+GRB+"){
        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertStartState(state);
        state_set.push_back(state);
        
        sprintf(ph_pos, "+GRB+(|)-m");
        nodeX = getNodeX((char*)ph_pos, &dtree_list);
        leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
        state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        //state = mgr->createState(pdf_set[leafX]);
        state_set.push_back(state);
        hmm->insertState(state);

        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertEndState(state);
        state_set.push_back(state);
      }
      else if(all_classes[i]=="+UH+"){
        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertStartState(state);
        state_set.push_back(state);
        
        sprintf(ph_pos, "+UH+(|)-m");
        nodeX = getNodeX((char*)ph_pos, &dtree_list);
        leafX = treeGetNode(distrib_tree, nodeX, &word, 1, 1);
        state = mgr->createState(pdf_set[ds_name2pdf_idx[dtree_list.itemA[leafX].name]]);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        //state = mgr->createState(pdf_set[leafX]);
        state_set.push_back(state);
        hmm->insertState(state);

        state = mgr->createState(NULL);
        info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
        hmm->insertEndState(state);
        state_set.push_back(state);
      }
      for(int j=0;j<state_set.size()-1;j++){
        Edge* edge;
        if(j!=0){
          edge = mgr->createEdge(state_set[j], state_set[j], 0.5);
          hmm->insertEdge(edge);
        }
        if(j!=0)
          edge = mgr->createEdge(state_set[j], state_set[j+1], 0.5);
        else
          edge = mgr->createEdge(state_set[j], state_set[j+1], 1.0);
        hmm->insertEdge(edge);
      }

      hmm_set->addHMM(hmm);
      */
    }
  }

/*
  getWord((void*)&cd, keys, (void*)&word, &argc, (char**)argv);
  fflush(stdout);

  int leafX = treeGetNode(distrib_tree, 15, &word, 1, 1);
  printf("leafX: %d\n", leafX);
  fflush(stdout);
  printf("model: %d\n", dtree_list.itemA[leafX].model);
  fflush(stdout);
  printf("ptree: %d\n", dtree_list.itemA[leafX].ptree);
  fflush(stdout);
  printf("leaf: %s\n", dtree_list.itemA[leafX].name);
  fflush(stdout);
*/

  wordDeinit(&word);

  fprintf(stderr, "Aegis: finished loading HMM\n");
}

void Aegis::importDictionary(Dictionary* jdict){
  DictWordList dlist = jdict->list;
  vector<string> phone_seq;

  fprintf(stderr, "Aegis: importing dictionary %s...\n", jdict->name);

  adict = new AegisDictionary();

  for(int i=0;i<dlist.itemN;i++){
//    printf("%s\n", dlist.itemA[i].name);
    
    Word w = dlist.itemA[i].word;
    for(int j=0;j<w.itemN;j++){
      //printf("%s\n", phonesName(w.phonesP, w.phoneA[j]));
      phone_seq.push_back(phonesName(w.phonesP, w.phoneA[j]));
    }
    adict->addWord(dlist.itemA[i].name, phone_seq);
    phone_seq.clear();
  }

  fprintf(stderr, "Aegis: imported %d words\n", adict->getNumWords());
}

void Aegis::initRecognizer(){
  float beam = 500.0;
  fprintf(stderr, "Aegis: starting up recognition module (version %s)...\n", version.c_str());

  config->clear();
  config->addTable("Config", "AssociateValue");
  config->setAssociateValue("Config", "dim", (Ptr)&dim, sizeof(int));
  config->setAssociateValue("Config", "Beam", (Ptr)&beam, sizeof(float));
  
  recognizer = new Recognizer(mgr, config);
  fprintf(stderr, "Aegis: successful\n");
  fprintf(stderr, "Aegis: ready to launch\n");
}

void Aegis::defineGarbagePhone(){
  garbage.clear();
  garbage.push_back("SIL");
  garbage.push_back("sp");
  garbage.push_back("+GRB+");
  garbage.push_back("+UH+");
}

bool Aegis::isGarbage(const string& phone){
  for(int i=0;i<garbage.size();i++){
    if(phone==garbage[i])
      return true;
  }
  return false;
}

void Aegis::getGarbagePhone(vector<string>& garbage){
  garbage = this->garbage;
}

AegisDictionary* Aegis::getADictionary(){
  return adict;
}

HMMSet* Aegis::getHMMSet(){
  return hmm_set;
}

void Aegis::setHMMSet(HMMSet* hmm_set){
  this->hmm_set = hmm_set;
}

Recognizer* Aegis::getRecognizer(){
  return recognizer;
}

MemoryManager* Aegis::getMemoryManager(){
  return mgr;
}

PDF* Aegis::getPDF(const string& ds_name){
  return pdf_set[ds_name2pdf_idx[ds_name]];
}

void Aegis::getPDFSet(vector<PDF*>& pdf_set){
  pdf_set = this->pdf_set;
}

void Aegis::getAllGaussians(vector<Gaussian*>& gaussian){
  gaussian = this->gaussian;
}

int Aegis::getNodeX(char* ph_pos, TreeNodeList* list){
  for(int i=0;i<list->itemN;i++){
    TreeNode* node = &list->itemA[i];
    if(strcmp(ph_pos, node->name)==0)
      return i;
  }
  return -1;
}

int Aegis::getDim(){
  return dim;
}

void Aegis::backupMeans(){
  mean_backup.clear();

  for(int i=0;i<gaussian.size();i++){
    mean_backup.push_back(gaussian[i]->getMeanVec());
  }
}

void Aegis::reloadBackup(){
  for(int i=0;i<gaussian.size();i++){
    gaussian[i]->setMeanVec(mean_backup[i]);
  }
}

void Aegis::readHTKConfig(RelationInfo* config){
  FILE* fp;
  char line[STRMAX], buf[STRMAX];
  int tmp;

  fp = fopen(getVal(config, "HTKConfig"), "r");

  while(1){
    fgets(line, STRMAX, fp);
    if(feof(fp))
      break;

    if(line[strlen(line)-1]=='\n')
      line[strlen(line)-1] = '\0';

    if(numField(line, " \t\n=")<2)
      continue;

    extractField(line, 0, " \t\n", buf);
    if(strcmp(buf, "WINDOWSIZE")==0){
      extractField(line, 2, " \t\n", buf);
      tmp = atoi(buf);

      config->setAssociateValue("Config", "WindowSize", (Ptr)&tmp, sizeof(tmp));
    }
    else if(strcmp(buf, "TARGETRATE")==0){
      extractField(line, 2, " \t\n", buf);
      tmp = atoi(buf);

      config->setAssociateValue("Config", "TargetRate", (Ptr)&tmp, sizeof(tmp));
    }
  }

  fclose(fp);
}

void Aegis::readFilelist(RelationInfo* config, const char* listname, vector<string>& filelist){
  FILE* fp;
  char buf[STRMAX];
  
  filelist.clear();
  fp = fopen(getVal(config, (char*)listname), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    filelist.push_back(buf);
  }

  fclose(fp);
}

void Aegis::readFilelist(RelationInfo* config, vector<string>& filelist){
  FILE* fp;
  char buf[STRMAX];
  
  filelist.clear();
  fp = fopen(getVal(config, "Filelist"), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    filelist.push_back(buf);
  }

  fclose(fp);
}

void Aegis::readHMMList(RelationInfo* config, vector<string>& all_classes){
  FILE* fp;
  char buf[STRMAX];
  
  all_classes.clear();
  fp = fopen(getVal(config, "Phone"), "r");

  while(1){
    fscanf(fp, "%s", buf);
    if(feof(fp))
      break;

    all_classes.push_back(buf);
  }

  fclose(fp);
}

char* Aegis::getVal(RelationInfo* config, char option[]){
  char* val=NULL;

  config->getAssociateValue("Config", option, (Ptr)&val);

  return val;
}

int Aegis::getOpt(int argc, char* argv[], RelationInfo* config){
  int tmp;
  float tmpf;
  float tmpd;

  config->clear();
  config->addTable("Config", "AssociateValue");

  for(int i=1;i<argc;i+=2){
    if(strcmp(argv[i], "-I")==0){
      config->setAssociateValue("Config", "TrainMLF", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-i")==0){
      config->setAssociateValue("Config", "TestMLF", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-H")==0){
      config->setAssociateValue("Config", "Model", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-O")==0){
      config->setAssociateValue("Config", "OutModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-X")==0){
      config->setAssociateValue("Config", "XFormModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Y")==0){
      config->setAssociateValue("Config", "OutXFormModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-A")==0){
      config->setAssociateValue("Config", "OrigModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-B")==0){
      config->setAssociateValue("Config", "XFormOrigModel", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-f")==0){
      config->setAssociateValue("Config", "FEParam", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-of")==0){
      config->setAssociateValue("Config", "OutFEParam", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-it")==0){
      config->setAssociateValue("Config", "InTrans", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ot")==0){
      config->setAssociateValue("Config", "OutTrans", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-S")==0){
      config->setAssociateValue("Config", "Filelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Spkr")==0){
      config->setAssociateValue("Config", "SpkrFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrTrans")==0){
      config->setAssociateValue("Config", "SpkrTransformation", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrAdapt")==0){
      config->setAssociateValue("Config", "SpkrAdaptFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrTest")==0){
      config->setAssociateValue("Config", "SpkrTestFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrModel")==0){
      config->setAssociateValue("Config", "SAModellist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SpkrDeptModel")==0){
      config->setAssociateValue("Config", "SDModellist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-F")==0){
      config->setAssociateValue("Config", "FBFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-M")==0){
      config->setAssociateValue("Config", "MFCCFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ObjF")==0){
      config->setAssociateValue("Config", "ObjFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ConstF")==0){
      config->setAssociateValue("Config", "ConstFilelist", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-DC")==0){
      config->setAssociateValue("Config", "ObjCons", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-C")==0){
      config->setAssociateValue("Config", "HTKConfig", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-D")==0){
      config->setAssociateValue("Config", "Dict", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-P")==0){
      config->setAssociateValue("Config", "Phone", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-p")==0){
      tmpd = atof(argv[i+1]);
      config->setAssociateValue("Config", "InsertionPenalty", (Ptr)&tmpd, sizeof(float));
    }
    else if(strcmp(argv[i], "-t")==0){
      tmpd = atof(argv[i+1]);
      config->setAssociateValue("Config", "Beam", (Ptr)&tmpd, sizeof(float));
    }
    else if(strcmp(argv[i], "-e")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "LearningRate", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-b")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "Beta", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-d")==0){
      tmpf = atof(argv[i+1]);
      config->setAssociateValue("Config", "Delta", (Ptr)&tmpf, sizeof(float));
    }
    else if(strcmp(argv[i], "-L")==0){
      config->setAssociateValue("Config", "Lattice", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-NUMREGC")==0){
      config->setAssociateValue("Config", "NumRegGroup", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-NUMEIGEN")==0){
      config->setAssociateValue("Config", "NumEigenVec", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-ADAPT")==0){
      config->setAssociateValue("Config", "Adapt", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SMOOTH")==0){
      config->setAssociateValue("Config", "Smooth", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-COV")==0){
      config->setAssociateValue("Config", "Covariance", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SigSlope")==0){
      config->setAssociateValue("Config", "SigSlope", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-SigOff")==0){
      config->setAssociateValue("Config", "SigOff", (Ptr)&argv[i+1], sizeof(char*));
    }
    else if(strcmp(argv[i], "-Thres")==0){
      config->setAssociateValue("Config", "Thres", (Ptr)&argv[i+1], sizeof(char*));
    }
    else{
      printf("Unknown option\n");
      return -1;
    }
  }

  return 0;
}


