#include "cache.h"

CacheTable::CacheTable(){
  map = NULL;
  available=NULL;
  dimension.clear();
}

CacheTable::CacheTable(int num_dimensions, int num_elements){
  this->map = new float[num_dimensions*num_elements];
  this->available = new bool[num_dimensions*num_elements];
  for(int d=0;d<num_dimensions;d++)
    this->dimension.push_back(num_elements);

  total = num_dimensions*num_elements;
  memset(map, 0, total*sizeof(float));
  memset(available, 0, total*sizeof(bool));

  calculateDSum();
}

CacheTable::CacheTable(const vector<int>& dimension){
  int sum=0;

  for(int d=0;d<dimension.size();d++){
    sum+=dimension[d];
  }
  this->map = new float[sum];
  this->available = new bool[sum];

  this->dimension = dimension;

  total = sum;
  memset(map, 0, total*sizeof(float));
  memset(available, 0, total*sizeof(bool));

  calculateDSum();
}

CacheTable::CacheTable(int num_dimensions, ...){
  va_list args;
  int total=1, code;
 
  if(num_dimensions==0)
    total = 0;

  va_start(args, num_dimensions);
  for(int d=0;d<num_dimensions;d++){
    code = va_arg(args, int);
    total = code*total;
    dimension.push_back(code);
  }

//  printf("total: %d\n", total);

  map = new float[total];
  available = new bool[total];
  memset(map, 0, total*sizeof(float));
  memset(available, 0, total*sizeof(bool));

  this->total = total;

  calculateDSum();

  va_end(args);
}

CacheTable::~CacheTable(){
  if(map!=NULL){
    delete [] map;
    delete [] available;
    dimension.clear();
  }
}

void CacheTable::save(char* filename){
  FILE* fp = fopen(filename, "w");
  int tmp;

  fprintf(fp, "%d %d\n", total, dimension.size());
  for(int i=0;i<dimension.size();i++){
    fprintf(fp, "%d ", dimension[i]);
  }
  fprintf(fp, "\n");

  for(int i=0;i<total;i++){
    tmp = available[i];
    fprintf(fp, "%e %d\n", map[i], tmp);
  }
  fclose(fp);
}

void CacheTable::saveBinary(char* filename){
  FILE* fp = fopen(filename, "wb");
  int tmp = dimension.size();

  fwrite((void*)&total, sizeof(total), 1, fp);
  fwrite((void*)&tmp, sizeof(tmp), 1, fp);
  for(int i=0;i<dimension.size();i++){
    fwrite((void*)&dimension[i], sizeof(dimension[i]), 1, fp);
  }

  fwrite((void*)map, sizeof(float), total, fp);
  fwrite((void*)available, sizeof(bool), total, fp);
  fclose(fp);
}

void CacheTable::load(char* filename){
  FILE* fp = fopen(filename, "r");
  int num_dim, tmp;

  clear();

  fscanf(fp, "%d %d\n", &total, &num_dim);
  dimension.resize(num_dim);
  for(int i=0;i<num_dim;i++){
    fscanf(fp, "%d", &dimension[i]);
  }

  map = new float[total];
  available = new bool[total];
  
  for(int i=0;i<total;i++){
    fscanf(fp, "%f %d", &map[i], &tmp);
    if(tmp==0)
      available[i] = false;
    else
      available[i] = true;
  }

  calculateDSum();

  fclose(fp);
}

void CacheTable::loadBinary(char* filename){
  FILE* fp = fopen(filename, "rb");
  int num_dim, tmp;

  clear();

  fread((void*)&total, sizeof(total), 1, fp);
//  fprintf(stderr, "total: %d\n", total);
  fread((void*)&num_dim, sizeof(num_dim), 1, fp);
//  fprintf(stderr, "num_dim: %d\n", num_dim);
  dimension.resize(num_dim);
  for(int i=0;i<num_dim;i++){
    fread((void*)&dimension[i], sizeof(dimension[i]), 1, fp);
//    fprintf(stderr, "dim %d: %d\n", i, dimension[i]);
  }
  
  if(map!=NULL)
    delete [] map;
  if(available!=NULL)
    delete [] available;

  map = new float[total];
  available = new bool[total];
 
  fread((void*)map, sizeof(float), total, fp); 
  fread((void*)available, sizeof(bool), total, fp);

  calculateDSum();

  fclose(fp);
}

void CacheTable::makeAvailable(){
  memset(available, 1, total);
}

void CacheTable::setAll(float value){
  for(int i=0;i<total;i++){
    map[i] = value;
  }
}

void CacheTable::set(float value, ...){
  va_list args;
  int loc=0, code;

  va_start(args, value);
  
  for(int d=0;d<dimension.size();d++){
    code = va_arg(args, int);
    loc += code*d_sum[d];
  }

  map[loc] = value;
  available[loc] = true;

  //printf("result: %d = %f\n", loc, value);

  va_end(args);
}

void CacheTable::filterAll(float (*func)(float)){
  for(int i=0;i<total;i++){
    if(available[i]){
      map[i] = func(map[i]);
    }
  }
}

void CacheTable::filterAll(double (*func)(double)){
  for(int i=0;i<total;i++){
    if(available[i]){
      map[i] = func(map[i]);
    }
  }
}

void CacheTable::add(float value, ...){
  va_list args;
  int loc=0, code;

  va_start(args, value);
  
  for(int d=0;d<dimension.size();d++){
    code = va_arg(args, int);
    loc += code*d_sum[d];
  }

  if(available[loc]){
    map[loc] += value;
  }
  else{
    map[loc] = value;
    available[loc] = true;
  }

  //printf("result: %d = %f\n", loc, value);

  va_end(args);
}

float CacheTable::get(int d0, ...){
  va_list args;
  int loc, code;

  va_start(args, d0);
  
  loc = d0*d_sum[0];
  for(int d=1;d<dimension.size();d++){
    code = va_arg(args, int);
    loc += code*d_sum[d];
  }

  va_end(args);

  if(!available[loc]){
    printf("not cached, error...\n");
    exit(-1);
  }
  //printf("result: %d = %f\n", loc, map[loc]);

  return map[loc];
}

void CacheTable::clear(){
  if(map!=NULL)
    memset(map, 0, sizeof(float)*total);
  if(available!=NULL)
    memset(available, 0, sizeof(bool)*total);
}

int CacheTable::width(int d){
  return dimension[d];
}

bool CacheTable::isAvailable(int d0, ...){
  va_list args;
  int loc, code;

  va_start(args, d0);
  
  loc = d0*d_sum[0];
  for(int d=1;d<dimension.size();d++){
    code = va_arg(args, int);
    loc += code*d_sum[d];
  }

  va_end(args);

  return available[loc];
}

void CacheTable::calculateDSum(){
  d_sum.clear();
  d_sum.resize(dimension.size());

  d_sum[d_sum.size()-1] = 1;
  for(int d=dimension.size()-2;d>=0;d--){
    d_sum[d] = dimension[d+1]*d_sum[d+1];
  }
}
