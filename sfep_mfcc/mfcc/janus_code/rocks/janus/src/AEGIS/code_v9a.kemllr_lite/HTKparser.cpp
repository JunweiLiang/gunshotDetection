#include "HTKparser.h"

HTKParser::HTKParser(MemoryManager* mgr){
  this->mgr = mgr;
  this->info = mgr->getRelationInfo();
  this->vec_size=0;

  macro.addTable("StateMacroTable");
  macro.addTable("TransitionMacroTable");
}

HTKParser::~HTKParser(){
  macro.removeAllRelations("StateMacroTable");
  macro.removeTable("StateMacroTable");

  macro.destroy();
}

HMMSet HTKParser::makeHMMSet(){
  HMMSet hmmset(vec_size);
  vector<string> key_list;

  info->getKeyList("HMMNameTable", key_list);
  for(int i=0;i<key_list.size();i++){
    AegisHMM* hmm = (AegisHMM*) info->getFirstAnswer("HMMNameTable", key_list[i]);

    hmmset.addHMM(hmm);
  }
  hmmset.lockHMMSet();

  return hmmset;
}

void HTKParser::parse(char* filename){
  FILE* fp;
  char line[STRMAX];
  char* ptr;
  char ch;
  string tmp;

  fp = fopen(filename, "r");

  while(1){
    fgets(line, STRMAX, fp);
    if((ptr=strstr(line, "<VECSIZE>"))!=NULL){
      sscanf(ptr+strlen("<VECSIZE>"), "%d", &vec_size);
      break;
    }
  }
  //printf("Vec Size: %d\n", vec_size);

  while(1){
    ch = fgetc(fp);
    if(feof(fp))
      break;

    //printf("ch: %c\n", ch);

    switch(ch){
      case 'h' : tmp=readHMM(fp); break;
      case 's' : readStateMacro(fp); break;
      case 't' : readTransitionMacro(fp); break;
      case 'r' : readRegressionTree(fp); break;
      case 'v' : readVarFloor(fp); break;
    }
  }
  fclose(fp);
}

void HTKParser::readVarFloor(FILE* fp){
  char buf[STRMAX];
  
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
}

void HTKParser::parseXForm(char* filename){
  FILE* fp;
  char line[STRMAX];
  char* ptr;
  char ch;
  string tmp;

  fp = fopen(filename, "r");

  while(1){
    fgets(line, STRMAX, fp);
    if((ptr=strstr(line, "<VECSIZE>"))!=NULL){
      sscanf(ptr+strlen("<VECSIZE>"), "%d", &vec_size);
      break;
    }
  }
  //printf("Vec Size: %d\n", vec_size);

  while(1){
    ch = fgetc(fp);
    if(feof(fp))
      break;

    switch(ch){
      case 'h' : tmp=readXFormHMM(fp); break;
      case 's' : readXFormStateMacro(fp); break;
      case 't' : readXFormTransitionMacro(fp); break;
    }
  }
  fclose(fp);
}

int HTKParser::getVecSize(){
  return vec_size;
}

RelationInfo* HTKParser::retrieveInfo(){
  return info;
}

AegisHMM* HTKParser::retrieveHMM(string name){
  vector<Ptr> ans;

  info->answer("HMMNameTable", name, ans);

  if(ans.size()==0)
    return NULL;
  
  return (AegisHMM*) ans[0];
}

string HTKParser::readRegressionTree(FILE* fp){
  vector<RegTree*> rtree_list;
  RegTree* rtree;
  vector<pair<int, int> > child_list;
  pair<int, int> tuple;
  string tree_name="";
  char buf[STRMAX];
  int num_child, f1, f2, f3;
  char ch;
  
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
    tree_name+=ch;
  }

  fscanf(fp, "%s %d", buf, &num_child);
  tree_name = buf;

  while(num_child>0){
    fscanf(fp, "%s", buf);
    if(strcmp(buf, "<NODE>")==0){
      fscanf(fp, "%d %d %d", &f1, &f2, &f3);
      //printf("<NODE> %d %d %d\n", f1, f2, f3);
      tuple.first = f2;
      tuple.second = f3;

      rtree = new RegTree(f1-1);
      rtree_list.push_back(rtree);
      child_list.push_back(tuple);
    }
    else if(strcmp(buf, "<TNODE>")==0){
      fscanf(fp, "%d %d", &f1, &f2);
      //printf("<TNODE> %d %d\n", f1, f2);

      rtree = new RegTree(f1-1);
      rtree->setOcc(f2);
      rtree_list.push_back(rtree);
      num_child--;
    }
  }

  info->addTable("RegTree", "AssociateValue");
  info->addTable("Gau2RClass", "AssociateValue");
  info->setAssociateValue("RegTree", tree_name, (Ptr)rtree_list[0], sizeof(RegTree*));

  return tree_name;
}

void HTKParser::readTransitionMacro(FILE* fp){
  string macro_name="";
  char ch;
  char buffer[STRMAX], tmp[STRMAX];
  int num_fields, i, j;
  float prob;
  Matrix* tran;

  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
    macro_name+=ch;
  }
  //printf("TransitionMacro: %s\n", macro_name.c_str());

  fscanf(fp, "%s %d", buffer, &num_fields);
  tran = new Matrix(num_fields, num_fields);

  for(i=0;i<num_fields;i++){
    for(j=0;j<num_fields;j++){
      fscanf(fp, "%f", &prob);
      (*tran)[i][j] = prob;
    }
  }

  macro.addRelation("TransitionMacroTable", macro_name, (Ptr)tran);
}

void HTKParser::readXFormTransitionMacro(FILE* fp){
  string macro_name="";
  char ch;
  char buffer[STRMAX], tmp[STRMAX];
  int num_fields, i, j;
  float prob;
  Matrix* tran;

  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
    macro_name+=ch;
  }
  //printf("TransitionMacro: %s\n", macro_name.c_str());

  fscanf(fp, "%s %d", buffer, &num_fields);
  tran = new Matrix(num_fields, num_fields);

  for(i=0;i<num_fields;i++){
    for(j=0;j<num_fields;j++){
      fscanf(fp, "%f", &prob);
      (*tran)[i][j] = prob;
    }
  }

  macro.addRelation("TransitionMacroTable", macro_name, (Ptr)tran);
}

void HTKParser::readStateMacro(FILE* fp){
  char ch;
  string macro_name="";
  int num_mix;
  float weight;
  float* mean_vec;
  float* cov_vec;
  float g_const;
  char symbol[STRMAX];
  int num, rclass;
  MultiMix* mm;
  Gaussian* g;
  bool is_diag=true;
  Matrix inv_full_cov;

  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
    macro_name+=ch;
  }
  //printf("Name: %s\n", macro_name.c_str());

  mean_vec = new float[vec_size];
  cov_vec = new float[vec_size];
  inv_full_cov = Matrix(vec_size, vec_size);

  fscanf(fp, "%s", symbol);
  if(strcmp(symbol, "<NUMMIXES>")==0){
    fscanf(fp, "%d", &num_mix);
  }
  else
    num_mix = 1;
  
  //printf("Nummix: %d\n", num_mix);
  
  mm = mgr->createMultiMix(vec_size);
  while(1){
    while((ch=fgetc(fp))=='\n'){}
    if(ch=='~')
      break;

    rclass = -1;

    if(num_mix>1){
    
      fscanf(fp, "%s %d %f", symbol, &num, &weight);    
      fscanf(fp, "%s %d", symbol, &num);
      if(strcmp(symbol, "<RCLASS>")==0){
        rclass = num - 1;
        fscanf(fp, "%s %d", symbol, &num);
      }
      //printf("%s %d\n", symbol, num);
      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &mean_vec[i]);
        //printf("%f ", mean_vec[i]);
      }
      //printf("\n");
      fscanf(fp, "%s %d", symbol, &num); //VARIANCE

      if(strcmp(symbol, "<VARIANCE>")==0){
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &cov_vec[i]);
          cov_vec[i] = 1.0/cov_vec[i];
        }
        is_diag=true;
      }
      else if(strcmp(symbol, "<INVCOVAR>")==0){
        for(int i=0;i<vec_size;i++){
          for(int j=i;j<vec_size;j++){
            fscanf(fp, "%f", &inv_full_cov[i][j]);
            inv_full_cov[j][i] = inv_full_cov[i][j];
          }
        }
        is_diag=false;
      }
      fscanf(fp, "%s %f", symbol, &g_const);
    }
    else{
      weight = 1.0;
      fscanf(fp, "%d", &num);
      if(strcmp(symbol, "<RCLASS>")==0){
        rclass = num - 1;
        fscanf(fp, "%s %d", symbol, &num);
      }
      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &mean_vec[i]);
      }
      fscanf(fp, "%s %d", symbol, &num); //VARIANCE

      if(strcmp(symbol, "<VARIANCE>")==0){
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &cov_vec[i]);
          cov_vec[i] = 1.0/cov_vec[i];
        }
        is_diag=true;
      }
      else if(strcmp(symbol, "<INVCOVAR>")==0){
        for(int i=0;i<vec_size;i++){
          for(int j=i;j<vec_size;j++){
            fscanf(fp, "%f", &inv_full_cov[i][j]);
            inv_full_cov[j][i] = inv_full_cov[i][j];
          }
        }
        is_diag=false;
      }
      fscanf(fp, "%s %f", symbol, &g_const);
    }

    if(is_diag){
      g = mgr->createDiagGaussian(vec_size);
      ((DiagGaussian*)g)->setMean(mean_vec);
      ((DiagGaussian*)g)->setCov(cov_vec);
      g->calculateGConst();
    }
    else{
      g = mgr->createFullGaussian(vec_size);
      g->setMeanVec(Matrix::makeVector(mean_vec, vec_size));
      g->setCovMat(inv_full_cov);
      g->calculateGConst();
    }

    if(rclass!=-1)
      info->setAssociateValue("Gau2RClass", g->getID(), (Ptr)&rclass, sizeof(int));

    mm->addGaussian(g, weight);
  }

  macro.addRelation("StateMacroTable", macro_name, (Ptr)mm);

  delete [] mean_vec;
  delete [] cov_vec;
}

void HTKParser::readXFormStateMacro(FILE* fp){
  char ch;
  string macro_name="";
  int num_mix;
  float weight;
  float* mean_vec;
  float* cov_vec;
  float g_const;
  char symbol[STRMAX];
  int num;
  MultiMix* mm;
  DiagGaussian* g;

  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return;
    }
    if(ch=='"')
      break;
    macro_name+=ch;
  }
  //printf("Name: %s\n", macro_name.c_str());

  mean_vec = new float[vec_size];
  cov_vec = new float[vec_size];

  fscanf(fp, "%s %d", symbol, &num);
  num_mix=num;
  
  //printf("Nummix: %d\n", num_mix);
  
  mm = mgr->createMultiMix(vec_size);
  while(1){
    while((ch=fgetc(fp))=='\n'){}
    if(ch=='~')
      break;
    //printf("Char: %c\n", ch);

    //printf("ckpt2\n");
    fscanf(fp, "%s %d %f", symbol, &num, &weight);
    //printf("%s\n", symbol);
    fscanf(fp, "%s %d", symbol, &num);
    //printf("%s\n", symbol);
    for(int i=0;i<vec_size;i++){
      fscanf(fp, "%f", &mean_vec[i]);
    }
    fscanf(fp, "%s %d", symbol, &num);
    //printf("%s\n", symbol);
    for(int i=0;i<vec_size;i++){
      fscanf(fp, "%f", &cov_vec[i]);
      cov_vec[i] = cov_vec[i];
    }
    fscanf(fp, "%s %f", symbol, &g_const);
    //printf("%s\n", symbol);

    g = mgr->createDiagGaussian(vec_size);
    g->setMean(mean_vec);
    g->setCov(cov_vec, false);

    mm->addGaussian(g, weight, false);
    
  }

  macro.addRelation("StateMacroTable", macro_name, (Ptr)mm);

  delete [] mean_vec;
  delete [] cov_vec;
}

string HTKParser::readHMM(FILE* fp){
  string hmm_name="", macro_name="";
  char ch;
  char symbol[STRMAX];
  int num, num_state, rclass;
  AegisHMM* hmm;
  Gaussian* g;
  MultiMix* mm;
  Edge* e;
  State* state;
  vector<State*> storage;
  float weight;
  float* mean_vec;
  float* cov_vec;
  float g_const;
  Matrix inv_full_cov;
  bool is_diag=true;

  //printf("READHMM\n");
  mean_vec = new float[vec_size];
  cov_vec = new float[vec_size];
  inv_full_cov = Matrix(vec_size, vec_size);
  
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
    hmm_name+=ch;
  }

  //printf("~h \"%s\"\n", hmm_name.c_str());

  hmm = mgr->createHMM(hmm_name);

  fscanf(fp, "%s", symbol); //<BEGINHMM>
  //printf("%s\n", symbol);

  fscanf(fp, "%s %d", symbol, &num); //<NUMSTATES> 5
  //printf("%s %d\n", symbol, num);
  num_state = num;

  storage.clear();
  
  state = mgr->createState(NULL); //create NULL state
  storage.push_back(state);

  info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
  //printf("%s\n", state->getID().c_str());
  hmm->insertStartState(state);

  fscanf(fp, "%s", symbol); //<STATE>
  
  for(int i=0;i<num_state-2;i++){
    fscanf(fp, "%d", &num); //2
    //printf("%s %d\n", symbol, num);
    //

    rclass = -1;

    fscanf(fp, "%s", symbol); //<NUMMIXES> 16

    //printf("%s\n", symbol);
    
    if(strcmp(symbol, "~s")==0){ //state macro
      //printf("%s ", symbol);
      while((ch=fgetc(fp))!='"') {/*do nth*/}
      fscanf(fp, "%s", symbol);
      symbol[strlen(symbol)-1]=0;

      //printf("%s\n", symbol);

      macro_name = string(symbol);
      mm = (MultiMix*) *(macro.retrieve("StateMacroTable", macro_name))->begin();
      state = mgr->createState(mm);
      storage.push_back(state);
      
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      
      hmm->insertState(state);
      fscanf(fp, "%s", symbol);

      continue;
    }
    else if(strcmp(symbol, "<RCLASS>")==0){ //single mix RegClass
      fscanf(fp, "%d", &rclass);
      rclass--;
      fscanf(fp, "%s", symbol);

      mm = mgr->createMultiMix(vec_size);
      fscanf(fp, "%d", &num);
      //printf("%s %d\n", symbol, num);

      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &mean_vec[i]);
        //printf("%f ", mean_vec[i]);
      }
      fscanf(fp, "%s %d", symbol, &num);//<VARIANCE> 39
      if(strcmp(symbol, "<VARIANCE>")==0){
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &cov_vec[i]);
          cov_vec[i] = 1.0/cov_vec[i];
        }
        is_diag=true;
      }
      else if(strcmp(symbol, "<INVCOVAR>")==0){
        for(int i=0;i<vec_size;i++){
          for(int j=i;j<vec_size;j++){
            fscanf(fp, "%f", &inv_full_cov[i][j]);
            inv_full_cov[j][i] = inv_full_cov[i][j];
          }
        }
        is_diag=false;
      }
      //printf("\n%s %d\n", symbol, num);
      fscanf(fp, "%s %f", symbol, &g_const);//<GCONST> 5.21400e-02
      //printf("\n%s %f\n", symbol, g_const);

      if(is_diag){
        g = mgr->createDiagGaussian(vec_size);
        ((DiagGaussian*)g)->setMean(mean_vec);
        ((DiagGaussian*)g)->setCov(cov_vec);
      }
      else{
        g = mgr->createFullGaussian(vec_size);
        g->setMeanVec(Matrix::makeVector(mean_vec, vec_size));
        g->setCovMat(inv_full_cov);
      }

      mm->addGaussian(g, 1.0);

      if(rclass!=-1){
        info->setAssociateValue("Gau2RClass", g->getID(), (Ptr)&rclass, sizeof(int));
      }

      fscanf(fp, "%s", symbol);
    }
    else if(strcmp(symbol, "<MEAN>")==0){ //single mixture
      mm = mgr->createMultiMix(vec_size);
      fscanf(fp, "%d", &num);
      //printf("%s %d\n", symbol, num);

      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &mean_vec[i]);
        //printf("%f ", mean_vec[i]);
      }
      fscanf(fp, "%s %d", symbol, &num);//<VARIANCE> 39
      //printf("\n%s %d\n", symbol, num);
      if(strcmp(symbol, "<VARIANCE>")==0){
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &cov_vec[i]);
          cov_vec[i] = 1.0/cov_vec[i];
        }
        is_diag=true;
      }
      else if(strcmp(symbol, "<INVCOVAR>")==0){
        for(int i=0;i<vec_size;i++){
          for(int j=i;j<vec_size;j++){
            fscanf(fp, "%f", &inv_full_cov[i][j]);
            inv_full_cov[j][i] = inv_full_cov[i][j];
          }
        }
        is_diag=false;
      }
      fscanf(fp, "%s %f", symbol, &g_const);//<GCONST> 5.21400e-02
      //printf("\n%s %f\n", symbol, g_const);

      if(is_diag){
        g = mgr->createDiagGaussian(vec_size);
        ((DiagGaussian*)g)->setMean(mean_vec);
        ((DiagGaussian*)g)->setCov(cov_vec);
      }
      else{
        g = mgr->createFullGaussian(vec_size);
        g->setMeanVec(Matrix::makeVector(mean_vec, vec_size));
        g->setCovMat(inv_full_cov);
      }

      mm->addGaussian(g, 1.0);

      fscanf(fp, "%s", symbol);
    }
    else{
      fscanf(fp, "%d", &num);
      //printf("%s %d\n", symbol, num);
      
      mm = mgr->createMultiMix(vec_size);
    
      while(1){
        rclass = -1;
        
        fscanf(fp, "%s", symbol);
        //printf("%s ", symbol); //<MIXTURE>
        if(strcmp(symbol, "<MIXTURE>")!=0){
          //printf("quitting\n");
          break;
        }
        fscanf(fp, "%d %f", &num, &weight);//1 2.3018e-02
        //printf("%d %f\n", num, weight);
        fscanf(fp, "%s %d", symbol, &num);//<MEAN> 39

        if(strcmp(symbol, "<RCLASS>")==0){
          rclass = num-1;
          fscanf(fp, "%s %d", symbol, &num);//<MEAN> 39
        }
        //printf("%s %d\n", symbol, num);
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &mean_vec[i]);
          //printf("%f ", mean_vec[i]);
        }
        fscanf(fp, "%s %d", symbol, &num);//<VARIANCE> 39
        //printf("\n%s %d\n", symbol, num);
        if(strcmp(symbol, "<VARIANCE>")==0){
          for(int i=0;i<vec_size;i++){
            fscanf(fp, "%f", &cov_vec[i]);
            cov_vec[i] = 1.0/cov_vec[i];
          }
          is_diag=true;
        }
        else if(strcmp(symbol, "<INVCOVAR>")==0){
          for(int i=0;i<vec_size;i++){
            for(int j=i;j<vec_size;j++){
              fscanf(fp, "%f", &inv_full_cov[i][j]);
              inv_full_cov[j][i] = inv_full_cov[i][j];
            }
          }
          is_diag=false;
        }
        fscanf(fp, "%s %f", symbol, &g_const);//<GCONST> 5.21400e-02
        //printf("\nck: %s %f\n", symbol, g_const);

        if(is_diag){
          g = mgr->createDiagGaussian(vec_size);
          ((DiagGaussian*)g)->setMean(mean_vec);
          ((DiagGaussian*)g)->setCov(cov_vec);
        }
        else{
          g = mgr->createFullGaussian(vec_size);
          g->setMeanVec(Matrix::makeVector(mean_vec, vec_size));
          g->setCovMat(inv_full_cov);
        }

        if(rclass!=-1){
          info->setAssociateValue("Gau2RClass", g->getID(), (Ptr)&rclass, sizeof(int));
        }

        mm->addGaussian(g, weight);
      }
    }
    state = mgr->createState(mm);    
    info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
    hmm->insertState(state);

    storage.push_back(state);
  }

  state = mgr->createState(NULL); //create NULL state
  storage.push_back(state);
  info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
  hmm->insertEndState(state);
  
  if(strcmp(symbol, "~t")==0){
    Matrix* tran;

    //printf("%s ", symbol);
    while((ch=fgetc(fp))!='"') {/*do nth*/}
    fscanf(fp, "%s", symbol);
    symbol[strlen(symbol)-1]=0;

    //printf("%s\n", symbol);

    tran = (Matrix*) macro.getFirstAnswer("TransitionMacroTable", symbol);

    //printf("Storage: %d\n", storage.size());

    for(int i=0;i<tran->getNumRow();i++){
      for(int j=0;j<tran->getNumCol();j++){
        if((*tran)[i][j]>0.0){
          //printf("%f ", (*tran)[i][j]);
          e = mgr->createEdge(storage[i], storage[j], (*tran)[i][j]);
          hmm->insertEdge(e);
        }
      }
    }
  }
  else{
    
    fscanf(fp, "%d", &num);
    //printf("%s %d\n", symbol, num);
    //printf("Storage: %d\n", storage.size());

    for(int i=0;i<num;i++){
      for(int j=0;j<num;j++){
        fscanf(fp, "%f", &weight);
        //printf("%f ", weight);
        if(weight>0.0){
          e = mgr->createEdge(storage[i], storage[j], weight);
          hmm->insertEdge(e);
        }
      }
      //printf("\n");
    }

  }

  fscanf(fp, "%s", symbol);//<ENDHMM>

  //printf("Name: %s Null: %d\n", hmm->getName().c_str(), hmm->getNumNullState());
  delete [] mean_vec;
  delete [] cov_vec;

  return hmm_name;
}

string HTKParser::readXFormHMM(FILE* fp){
  string hmm_name="", macro_name="";
  char ch;
  char symbol[STRMAX];
  int num, num_state;
  AegisHMM* hmm;
  DiagGaussian* g;
  MultiMix* mm;
  Edge* e;
  State* state;
  vector<State*> storage;
  float weight;
  float* mean_vec;
  float* cov_vec;
  float g_const;

  //printf("READHMM\n");
  mean_vec = new float[vec_size];
  cov_vec = new float[vec_size];
  
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
  }
  while(1){
    ch = fgetc(fp);
    if(feof(fp)){
      return "";
    }
    if(ch=='"')
      break;
    hmm_name+=ch;
  }

  //printf("~h \"%s\"\n", hmm_name.c_str());

  hmm = mgr->createHMM(hmm_name);

  fscanf(fp, "%s", symbol); //<BEGINHMM>
  //printf("%s\n", symbol);

  fscanf(fp, "%s %d", symbol, &num); //<NUMSTATES> 5
  //printf("%s %d\n", symbol, num);
  num_state = num;

  storage.clear();
  
  state = mgr->createState(NULL); //create NULL state
  storage.push_back(state);

  info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
  //printf("%s\n", state->getID().c_str());
  hmm->insertStartState(state);

  fscanf(fp, "%s", symbol); //<STATE>
  
  for(int i=0;i<num_state-2;i++){
    fscanf(fp, "%d", &num); //2
    //printf("%s %d\n", symbol, num);

    fscanf(fp, "%s", symbol); //<NUMMIXES> 16
    
    if(strcmp(symbol, "~s")==0){ //state macro
      //printf("%s ", symbol);
      while((ch=fgetc(fp))!='"') {/*do nth*/}
      fscanf(fp, "%s", symbol);
      symbol[strlen(symbol)-1]=0;

      //printf("%s\n", symbol);

      macro_name = string(symbol);
      mm = (MultiMix*) *(macro.retrieve("StateMacroTable", macro_name))->begin();
      state = mgr->createState(mm);
      storage.push_back(state);
      
      info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
      
      hmm->insertState(state);
      fscanf(fp, "%s", symbol);

      continue;
    }
    else if(strcmp(symbol, "<MEAN>")==0){ //single mixture
      mm = mgr->createMultiMix(vec_size);
      fscanf(fp, "%d", &num);
      //printf("%s %d\n", symbol, num);

      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &mean_vec[i]);
        //printf("%f ", mean_vec[i]);
      }
      fscanf(fp, "%s %d", symbol, &num);//<VARIANCE> 39
      //printf("\n%s %d\n", symbol, num);
      for(int i=0;i<vec_size;i++){
        fscanf(fp, "%f", &cov_vec[i]);
	cov_vec[i] = cov_vec[i];
        //printf("%f ", cov_vec[i]);
      }
      fscanf(fp, "%s %f", symbol, &g_const);//<GCONST> 5.21400e-02
      //printf("\n%s %f\n", symbol, g_const);

      g = mgr->createDiagGaussian(vec_size);
      g->setMean(mean_vec);
      g->setCov(cov_vec, false);

      mm->addGaussian(g, 1.0);

      fscanf(fp, "%s", symbol);
    }
    else{
      fscanf(fp, "%d", &num);
      //printf("%s %d\n", symbol, num);
      
      mm = mgr->createMultiMix(vec_size);
    
      while(1){
        fscanf(fp, "%s", symbol);
        //printf("%s ", symbol); //<MIXTURE>
        if(strcmp(symbol, "<MIXTURE>")!=0){
          //printf("quitting\n");
          break;
        }
        fscanf(fp, "%d %f", &num, &weight);//1 2.3018e-02
        //printf("%d %f\n", num, weight);
        fscanf(fp, "%s %d", symbol, &num);//<MEAN> 39
        //printf("%s %d\n", symbol, num);
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &mean_vec[i]);
          //printf("%f ", mean_vec[i]);
        }
        fscanf(fp, "%s %d", symbol, &num);//<VARIANCE> 39
        //printf("\n%s %d\n", symbol, num);
        for(int i=0;i<vec_size;i++){
          fscanf(fp, "%f", &cov_vec[i]);
	  cov_vec[i] = cov_vec[i];
          //printf("%f ", cov_vec[i]);
        }
        fscanf(fp, "%s %f", symbol, &g_const);//<GCONST> 5.21400e-02
        //printf("\nck: %s %f\n", symbol, g_const);

        g = mgr->createDiagGaussian(vec_size);
        g->setMean(mean_vec);
        g->setCov(cov_vec, false);

	//printf("weight: %f %f\n", weight, exp(weight));

        mm->addGaussian(g, weight, false);
      }
    }
    state = mgr->createState(mm);    
    info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
    hmm->insertState(state);

    storage.push_back(state);
  }

  state = mgr->createState(NULL); //create NULL state
  storage.push_back(state);
  info->addRelation("StateInHMM", state->getID(), (Ptr)hmm);
  hmm->insertEndState(state);
  
  if(strcmp(symbol, "~t")==0){
    Matrix* tran;

    //printf("%s ", symbol);
    while((ch=fgetc(fp))!='"') {/*do nth*/}
    fscanf(fp, "%s", symbol);
    symbol[strlen(symbol)-1]=0;

    //printf("%s\n", symbol);

    tran = (Matrix*) macro.getFirstAnswer("TransitionMacroTable", symbol);

    //printf("Storage: %d\n", storage.size());

    for(int i=0;i<tran->getNumRow();i++){
      for(int j=0;j<tran->getNumCol();j++){
        if(exp((*tran)[i][j])>0.0){
          //printf("%f ", (*tran)[i][j]);
          e = mgr->createEdge(storage[i], storage[j], (*tran)[i][j]);
          hmm->insertEdge(e);
        }
      }
    }
  }
  else{
    
    fscanf(fp, "%d", &num);
    //printf("%s %d\n", symbol, num);
    //printf("Storage: %d\n", storage.size());

    for(int i=0;i<num;i++){
      for(int j=0;j<num;j++){
        fscanf(fp, "%f", &weight);
        //printf("%f ", weight);
        if(exp(weight)>0.0){
          e = mgr->createEdge(storage[i], storage[j], weight);
          hmm->insertEdge(e);
        }
      }
      //printf("\n");
    }

  }

  fscanf(fp, "%s", symbol);//<ENDHMM>

  //printf("Name: %s Null: %d\n", hmm->getName().c_str(), hmm->getNumNullState());
  delete [] mean_vec;
  delete [] cov_vec;

  return hmm_name;
}


vector<PDF*> HTKParser::getAllPDFs(){
  vector<PDF*> pdf;
  vector<string> hmm_name;
  AegisHMM* hmm;
  State* s;
  MultiMix* mm;
  Gaussian* g;

  info->getKeyList("HMMNameTable", hmm_name);
  for(int i=0;i<hmm_name.size();i++){
    hmm = retrieveHMM(hmm_name[i]);
    for(int j=0;j<hmm->getNumState();j++){
      s = hmm->getState(j);
      if(s->isNullNode())
        continue;
      
      mm = (MultiMix*) s->getPDF();
      for(int k=0;k<mm->numMixture();k++){
        g = mm->getGaussian(k);
        pdf.push_back(g);
      }
    }
  }

  return pdf;
}

void HTKParser::parseLattice(const string& lattice_file, AegisDictionary* dict, Network* network){
  FILE* fp;
  char buf[STRMAX], tmp[STRMAX], field[STRMAX];
  int num_nodes, num_arcs, s_index, e_index, cur_start, cur_end;
  float cur_penalty;
  string name;
  vector<string> hmm_name;
  vector<int> start, end;
  vector<float> penalty;

  fp = fopen(lattice_file.c_str(), "r");
  if(fp==NULL){
    printf("cannot open %s\n", lattice_file.c_str());
    exit(-1);
  }

  while(1){
    fgets(buf, STRMAX, fp);
    if(feof(fp))
      break;

    if(buf[0]=='#') //if it is comment
      continue;

    name = "";
    s_index = -1;
    e_index = -1;
    cur_start = -1;
    cur_end = -1;

    if(numField(buf, " \t\n")==1){
      continue;
    }
    else if(numField(buf, " \t\n")==2){
      for(int f=0;f<2;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);
      
        if(strcmp(field, "N")==0){
          extractField(tmp, 1, "=", field);
          num_nodes = atoi(field);
          hmm_name.resize(num_nodes);
        }
        else if(strcmp(field, "L")==0){
          extractField(tmp, 1, "=", field);
          num_arcs = atoi(field);
          start.resize(num_arcs);
          end.resize(num_arcs);
          penalty.resize(num_arcs);
        }
        else if(strcmp(field, "I")==0){
          extractField(tmp, 1, "=", field);
          s_index = atoi(field);
        }
        else if(strcmp(field, "W")==0){
          extractField(tmp, 1, "=", field);
          name = string(field);
        }

        if(s_index!=-1 && name!=""){
          hmm_name[s_index] = name;
          s_index=-1;
          name="";
        }
      }
    }
    else if(numField(buf, " \t\n")==3){
      for(int f=0;f<3;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);

        if(strcmp(field, "J")==0){
          extractField(tmp, 1, "=", field);
          e_index = atoi(field);
        }
        else if(strcmp(field, "S")==0){
          extractField(tmp, 1, "=", field);
          cur_start = atoi(field);
        }
        else if(strcmp(field, "E")==0){
          extractField(tmp, 1, "=", field);
          cur_end = atoi(field);
        }
      }

      if(e_index!=-1 && cur_start!=-1 && cur_end!=-1){
        start[e_index] = cur_start;
        end[e_index] = cur_end;

        e_index = -1;
        cur_start = -1;
        cur_end = -1;
      }
    }
    else if(numField(buf, " \t\n")==4){
      for(int f=0;f<4;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);

        if(strcmp(field, "J")==0){
          extractField(tmp, 1, "=", field);
          e_index = atoi(field);
        }
        else if(strcmp(field, "S")==0){
          extractField(tmp, 1, "=", field);
          cur_start = atoi(field);
        }
        else if(strcmp(field, "E")==0){
          extractField(tmp, 1, "=", field);
          cur_end = atoi(field);
        }
        else if(strcmp(field, "l")==0){
          extractField(tmp, 1, "=", field);
          cur_penalty = atof(field);
        }
      }

      if(e_index!=-1 && cur_start!=-1 && cur_end!=-1 && cur_penalty!=-1.0){
        start[e_index] = cur_start;
        end[e_index] = cur_end;
        penalty[e_index] = cur_penalty;

        e_index = -1;
        cur_start = -1;
        cur_end = -1;
        cur_penalty = -1.0;
      }
    }
  }

  fclose(fp);

  for(int i=0;i<num_nodes;i++){
    if(hmm_name[i]=="!NULL"){
      network->addNull();
    }
    else{
      vector<AegisHMM*> hmm_list;
      vector<string> symbol;
      dict->getSymSeq(hmm_name[i], symbol);

      for(int j=0;j<symbol.size();j++){
        AegisHMM* hmm = (AegisHMM*) info->getFirstAnswer("HMMNameTable", symbol[j]);
        hmm_list.push_back(hmm);
      }
      network->addHMM(hmm_name[i], hmm_list);
    }
  }

  for(int i=0;i<num_arcs;i++){
    //printf("%d %d %f\n", start[i], end[i], penalty[i]);
    network->linkNetNode(start[i], end[i], penalty[i]);
  }
}

void HTKParser::parseLattice(const string& lattice_file, Network* network){
  FILE* fp;
  char buf[STRMAX], tmp[STRMAX], field[STRMAX];
  int num_nodes, num_arcs, s_index, e_index, cur_start, cur_end;
  float cur_penalty;
  string name;
  vector<string> hmm_name;
  vector<int> start, end;
  vector<float> penalty;

  fp = fopen(lattice_file.c_str(), "r");
  if(fp==NULL){
    printf("cannot open %s\n", lattice_file.c_str());
    exit(-1);
  }

  while(1){
    fgets(buf, STRMAX, fp);
    if(feof(fp))
      break;

    if(buf[0]=='#') //if it is comment
      continue;

    name = "";
    s_index = -1;
    e_index = -1;
    cur_start = -1;
    cur_end = -1;

    if(numField(buf, " \t\n")==1){
      continue;
    }
    else if(numField(buf, " \t\n")==2){
      for(int f=0;f<2;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);
      
        if(strcmp(field, "N")==0){
          extractField(tmp, 1, "=", field);
          num_nodes = atoi(field);
          hmm_name.resize(num_nodes);
        }
        else if(strcmp(field, "L")==0){
          extractField(tmp, 1, "=", field);
          num_arcs = atoi(field);
          start.resize(num_arcs);
          end.resize(num_arcs);
          penalty.resize(num_arcs);
        }
        else if(strcmp(field, "I")==0){
          extractField(tmp, 1, "=", field);
          s_index = atoi(field);
        }
        else if(strcmp(field, "W")==0){
          extractField(tmp, 1, "=", field);
          name = string(field);
        }

        if(s_index!=-1 && name!=""){
          hmm_name[s_index] = name;
          s_index=-1;
          name="";
        }
      }
    }
    else if(numField(buf, " \t\n")==3){
      for(int f=0;f<3;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);

        if(strcmp(field, "J")==0){
          extractField(tmp, 1, "=", field);
          e_index = atoi(field);
        }
        else if(strcmp(field, "S")==0){
          extractField(tmp, 1, "=", field);
          cur_start = atoi(field);
        }
        else if(strcmp(field, "E")==0){
          extractField(tmp, 1, "=", field);
          cur_end = atoi(field);
        }
      }

      if(e_index!=-1 && cur_start!=-1 && cur_end!=-1){
        start[e_index] = cur_start;
        end[e_index] = cur_end;

        e_index = -1;
        cur_start = -1;
        cur_end = -1;
      }
    }
    else if(numField(buf, " \t\n")==4){
      for(int f=0;f<4;f++){
        extractField(buf, f, " \t\n", tmp);
        extractField(tmp, 0, "=", field);

        if(strcmp(field, "J")==0){
          extractField(tmp, 1, "=", field);
          e_index = atoi(field);
        }
        else if(strcmp(field, "S")==0){
          extractField(tmp, 1, "=", field);
          cur_start = atoi(field);
        }
        else if(strcmp(field, "E")==0){
          extractField(tmp, 1, "=", field);
          cur_end = atoi(field);
        }
        else if(strcmp(field, "l")==0){
          extractField(tmp, 1, "=", field);
          cur_penalty = atof(field);
        }
      }

      if(e_index!=-1 && cur_start!=-1 && cur_end!=-1 && cur_penalty!=-1.0){
        start[e_index] = cur_start;
        end[e_index] = cur_end;
        penalty[e_index] = cur_penalty;

        e_index = -1;
        cur_start = -1;
        cur_end = -1;
        cur_penalty = -1.0;
      }
    }
  }

  fclose(fp);

  for(int i=0;i<num_nodes;i++){
    if(hmm_name[i]=="!NULL"){
      network->addNull();
    }
    else{
      AegisHMM* hmm = (AegisHMM*) info->getFirstAnswer("HMMNameTable", hmm_name[i]);
      network->addHMM(hmm);
    }
  }

  for(int i=0;i<num_arcs;i++){
    network->linkNetNode(start[i], end[i], penalty[i]);
  }
}
