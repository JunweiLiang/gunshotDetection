#include "linear_algebra.h"

Matrix::Matrix(){
  this->num_row=0;
  this->num_col=0;

  val = NULL;
}

Matrix::Matrix(int num_row, int num_col){
  this->num_row=num_row;
  this->num_col=num_col;

  val = matrix(0, num_row-1, 0, num_col-1);
  //printf("constructor! %x\n", val);

  zero();
}

Matrix::Matrix(const Matrix& m){
  int row, col;
  float** map;
/*  
  if(val!=NULL){
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  }
*/
  this->num_row=m.getNumRow();
  this->num_col=m.getNumCol();

  if(num_row==0 || num_col==0){
    val = NULL;
    return;
  }

  val = matrix(0, m.getNumRow()-1, 0, m.getNumCol()-1);
  //printf("copy constructor! %x\n", val);
  map = m.getMap();
  
  for(row=0;row<num_row;row++){
    for(col=0;col<num_col;col++){
      val[row][col]=map[row][col];
    }
  }
}

Matrix::~Matrix(){
  //printf("destructor! %x\n", val);
  if(val!=NULL)
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  num_row=num_col=0;
  val=NULL;
}

Matrix Matrix::makeVector(float* v, int d){
  Matrix vec(1, d);

  for(int i=0;i<d;i++){
    vec[0][i] = v[i];
  }

  return vec;
}

Matrix Matrix::makeMatrix(float** m, int r, int c){
  Matrix mat(r, c);

  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      mat[i][j] = m[i][j];
    }
  }

  return mat;
}

Matrix Matrix::makeMatrix(float* m, int r, int c){
  Matrix mat(r, c);

  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      mat[i][j] = m[i*c+j];
    }
  }

  return mat;
}

Matrix Matrix::makeMatrix(double** m, int r, int c){
  Matrix mat(r, c);

  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      mat[i][j] = m[i][j];
    }
  }

  return mat;
}

Matrix Matrix::makeMatrix(double* m, int r, int c){
  Matrix mat(r, c);

  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      mat[i][j] = m[i*c+j];
    }
  }

  return mat;
}

void Matrix::random(){
  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      if(rand()%2)
        val[i][j] = ((float)rand())/RAND_MAX;
      else
        val[i][j] = -((float)rand())/RAND_MAX;
    }
  }
}

void Matrix::destroy(){
  if(val!=NULL)
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  num_row=num_col=0;
  val=NULL;
}

void Matrix::zero(){
  int row, col;

  for(row=0;row<num_row;row++){
    for(col=0;col<num_col;col++){
      val[row][col]=0.0;
    }
  }
}

void Matrix::identity(){
  int i;
/*
  if(num_row!=num_col){
    printf("Error in identity\n");
    exit(-1);
  }
*/  
  zero();
  for(i=0;i<num_row;i++){
    val[i][i]=1.0;
  }
}

Matrix Matrix::vectorize() const{
  Matrix vec(1, num_row*num_col);

  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      vec[0][i*num_col+j] = val[i][j];
    }
  }

  return vec;
}

Matrix Matrix::kroneckerProduct(const Matrix& mat) const{
  Matrix result(num_row*mat.getNumRow(), num_col*mat.getNumCol());

  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      result.paste(mat*val[i][j], i*mat.getNumRow(), j*mat.getNumCol());
    }
  }

  return result;
}

float Matrix::trace() const{
  float result=0.0;
  
  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }

  for(int i=0;i<num_col;i++){
    result += val[i][i];
  }

  return result;
}

Matrix Matrix::copy(int row_start, int col_start, int row_end, int col_end) const{
  Matrix result(row_end-row_start+1, col_end-col_start+1);

  for(int r=0;r<result.getNumRow();r++){
    for(int c=0;c<result.getNumCol();c++){
      result[r][c] = val[r+row_start][c+col_start];
    }
  }

  return result;
}

void Matrix::paste(const Matrix& m, int row, int col){
  if(row+m.getNumRow()>getNumRow() || col+m.getNumCol()>getNumCol()){
    printf("ERROR: error in pasting a matrix\n");
    exit(-1);
  }
  for(int r=0;r<m.getNumRow();r++){
    for(int c=0;c<m.getNumCol();c++){
      val[r+row][c+col] = m.getValue(r, c);
    }
  }
}

double Matrix::cofactor(int i, int j) const{
  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }
  
  if(num_row<=1)
    return 0.0;
    
  Matrix term1, term2, term3, term4;
  Matrix tmp(num_row-1, num_col-1);

  if(i>0 && j>0){
    term1 = copy(0, 0, i-1, j-1);
  }
  if(i>0 && j<num_col-1){
    term2 = copy(0, j+1, i-1, num_col-1);
  }
  if(i<num_row-1 && j>0){
    term3 = copy(i+1, 0, num_row-1, j-1);
  }
  if(i<num_row-1 && j<num_col-1){
    term4 = copy(i+1, j+1, num_row-1, num_col-1);
  }

  tmp.paste(term1, 0, 0);
  tmp.paste(term2, 0, j);
  tmp.paste(term3, i, 0);
  tmp.paste(term4, i, j);

  double sign = 1.0;
  double det = tmp.determinant();

  if((i+j)%2!=0)
    sign = -1;
  
  return sign*det;
}

double Matrix::determinant() const{
  Matrix result(num_row, num_col);
  int *ipiv, code;
  int dim = num_row;
  double* tmp;

  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }

  ipiv = new int[num_row];
  tmp = new double[num_row*num_col];
  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      tmp[i*num_col+j] = val[i][j];
    }
  }

  code = clapack_dgetrf(CblasRowMajor, num_row, num_col, tmp, num_col, ipiv);
//  result = makeMatrix(tmp.getMap()[0], num_row, num_col);

  delete [] ipiv;

  double det = 1.0;
  for(int i=0;i<num_row;i++){
    det *= tmp[i*num_col+i];
  }

  delete [] tmp;

  if(code>0){
    //printf("singular matrix\n");
    return 0.0;
  }
  if(code<0){
    printf("wrong argument(s) in calling sgetrf\n");
    exit(-1);
  }

  return det;
}

double Matrix::logDeterminant() const{
  Matrix result(num_row, num_col);
  int *ipiv, code;
  int dim = num_row;
  double* tmp;

  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }

  ipiv = new int[num_row];
  tmp = new double[num_row*num_col];
  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      tmp[i*num_col+j] = val[i][j];
    }
  }

  code = clapack_dgetrf(CblasRowMajor, num_row, num_col, tmp, num_col, ipiv);
//  result = makeMatrix(tmp.getMap()[0], num_row, num_col);

  delete [] ipiv;

  double log_det = 0.0;
  for(int i=0;i<num_row;i++){
    if(tmp[i*num_col+i] <= 0.0){
      log_det = LZERO;
      break;
    }
    log_det += log(tmp[i*num_col+i]);
  }

  delete [] tmp;

  if(code>0){
    //printf("singular matrix\n");
    return 0.0;
  }
  if(code<0){
    printf("wrong argument(s) in calling sgetrf\n");
    exit(-1);
  }

  return log_det;
}

/*
float Matrix::determinant() const{
  float** a;
  float d;
  int n, code;
  int* indx;

  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }
  n = num_col;
  indx = new int[n+1];

  a = matrix(1, n, 1, n);
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      a[i+1][j+1]=val[i][j];
    }
    indx[i]=0;
  }

  ludcmp(a, n, indx, &d);

  for(int i=1;i<=n;i++){
    d *= a[i][i];
  }

  free_matrix(a, 1, n, 1, n);
  delete [] indx;

  return d;
}
*/
/*
float Matrix::logDeterminant() const{
  float result=0.0, d;
  float** a;
  int n;
  float sign=1;
  int* indx;

  if(num_row != num_col){
    printf("ERROR: not a square matrix\n");
    exit(-1);
  }
  n = num_col;
  indx = new int[n+1];

  a = matrix(1, n, 1, n);
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      a[i+1][j+1]=val[i][j];
    }
  }

  ludcmp(a, n, indx, &d);
  sign=d;

  for(int i=1;i<=n;i++){
    if(a[i][i]>0)
      result += log(a[i][i]);
    else{
      result += log(-a[i][i]);
      sign*=-1;
    }
  }

  free_matrix(a, 1, n, 1, n);
  delete [] indx;

  if(sign!=1){
    printf("ERROR: invalid log determinant %f\n", determinant());
    exit(-1);
  }

  return result;
}
*/
Matrix Matrix::transpose() const{
  Matrix result(num_col, num_row);
  int i, j;

  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      result.setValue(j, i, val[i][j]);
    }
  }

  return result;
}

Matrix Matrix::inverse() const{
  Matrix result(num_row, num_col);
  Matrix tmp;
  int *ipiv, code;
  int dim = num_row;

  ipiv = new int[num_row];
  tmp = vectorize();

  code = clapack_sgetrf(CblasRowMajor, num_row, num_col, tmp.getMap()[0], num_col, ipiv);
  clapack_sgetri(CblasRowMajor, num_row, tmp.getMap()[0], num_col, ipiv);
  result = makeMatrix(tmp.getMap()[0], num_row, num_col);

  delete [] ipiv;

  if(code>0){
    //printf("singular matrix\n");
    result.zero();
    return result;
  }
  if(code<0){
    printf("wrong argument(s) in calling sgetri\n");
    exit(-1);
  }

  return result;
}

Matrix Matrix::dInverse() const{
  Matrix result(num_row, num_col);

  if(num_row!=num_col){
    fprintf(stderr, "dInverse: not a square matrix\n");
    exit(0);
  }
  
  Matrix tmp;
  double* array = new double[num_row*num_col];
  int *ipiv, code;
  int dim = num_row;

  ipiv = new int[num_row];
  tmp = vectorize();
  for(int i=0;i<tmp.getNumCol();i++){
    array[i] = tmp[0][i];
  }

  code = clapack_dgetrf(CblasRowMajor, num_row, num_col, array, num_col, ipiv);
  clapack_dgetri(CblasRowMajor, num_row, array, num_col, ipiv);
  result = makeMatrix(array, num_row, num_col);

  delete [] ipiv;
  delete [] array;

  if(code>0){
    //printf("singular matrix\n");
    //exit(-1);
    result.zero();
    return result;
  }
  if(code<0){
    fprintf(stderr, "wrong argument(s) in calling dgetri\n");
    exit(-1);
  }
  return result;
}

Matrix Matrix::dPseudoInverse() const{
  Matrix result(num_row, num_col);
  Matrix u, s, v;
  Matrix tmp(num_row, num_col);
  int dim = num_row;

  if(num_row!=num_col){
    fprintf(stderr, "dInverse: not a square matrix\n");
    exit(0);
  }
  
  MathEngine::dsvd(*this, u, s, v);

//  s.printFP(stderr);
  double sum=0.0;
  for(int i=0;i<dim;i++){
    sum += s[0][i];
  }
  for(int i=0;i<dim;i++){
    double acc=0.0;
    for(int j=0;j<dim;j++){
      acc += s[0][j];
      if((acc/sum)<=0.9999 && s[0][j]>0.0)
        tmp[i][j] = v[i][j] / s[0][j];
    }
  }
 /* 
  s.printFP(stderr);
  double acc=0.0;
  for(int i=0;i<dim;i++){
    acc += s[0][i];
    s[0][i]=acc/sum;
  }
  s.printFP(stderr);
  */
  result = tmp*u.transpose();
  
  return result;
}

bool Matrix::operator==(const Matrix& m) const{
  int i, j;

  if(this->num_row!=m.getNumRow() || this->num_col!=m.getNumCol())
    return false;
  
  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      if(val[i][j] != m.getValue(i, j))
        return false;
    }
  }
  return true;
}

bool Matrix::operator!=(const Matrix& m) const{
  return !(*this==m);
}


Matrix& Matrix::operator=(const Matrix& m){
  int i, j;

  //printf("operator= enter %x\n", val);
  if(val!=NULL){
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  }
  this->num_row=m.getNumRow();
  this->num_col=m.getNumCol();
  val = matrix(0, num_row-1, 0, num_col-1);
  
  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      val[i][j] = m.getValue(i, j);
    }
  }
  //printf("operator= leave  %x\n", val);
  return *this;
}

float* Matrix::operator[](const int& row_index){
  return val[row_index];
}

Matrix Matrix::operator+(const Matrix& m) const{
  return this->add(m);
}

Matrix Matrix::operator-(const Matrix& m) const{
  return this->sub(m);
}

Matrix Matrix::operator*(const Matrix& m) const{
  return this->mul(m);
}

Matrix Matrix::operator*(const float& constant) const{
  return this->mul(constant);
}

void Matrix::operator+=(const Matrix& m) const{
  if(!hasSameDimension(m)){
    printf("error in matrix addition\n");
    exit(-1);
  }

  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      val[i][j] += m.getValue(i, j);
    }
  }

//  return (*this);
}

Matrix Matrix::add(const Matrix& m) const{
  Matrix result(num_row, num_col);
  int i, j;
  
  if(!hasSameDimension(m)){
    printf("Error in matrix addition\n");
    exit(-1);
  }

  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      result.setValue(i, j, val[i][j] + m.getValue(i, j));
    }
  }

  return result;
}

Matrix Matrix::sub(const Matrix& m) const{
  Matrix result(num_row, num_col);
  int i, j;
  
  if(!hasSameDimension(m)){
    printf("Error in matrix addition\n");
    exit(-1);
  }

  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      result.setValue(i, j, val[i][j] - m.getValue(i, j));
    }
  }
  
  return result;
}

Matrix Matrix::mul(const float& constant) const{
  Matrix result(num_row, num_col);
  int i, j;
  float tmp;

  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      result.setValue(i, j, constant*val[i][j]);
    }
  }

  return result;
}

Matrix Matrix::mul(const Matrix& m) const{
//  Matrix result(num_row, m.getNumCol());
  Matrix result;
  int i, j, k;
  float** map = m.getMap();
  Matrix tmp(1, num_row*m.getNumCol());

  if(!canMultiply(m)){
    printf("Error in matrix multiplication\n");
    exit(-1);
  }
/*
  for(i=0;i<num_row;i++){
    for(j=0;j<m.getNumCol();j++){
      for(k=0;k<num_col;k++){
        result[i][j]+=val[i][k]*m.getValue(k, j);
      }
    }
  }
*/

  if(m.getNumCol()!=1)
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, num_row, m.getNumCol(), num_col, 1.0, vectorize().getMap()[0], num_col, m.vectorize().getMap()[0], m.getNumCol(), 0.0, tmp.getMap()[0], m.getNumCol());
  else
    cblas_sgemv(CblasRowMajor, CblasNoTrans, num_row, num_col, 1.0, vectorize().getMap()[0], num_col, m.vectorize().getMap()[0], 1, 0.0, tmp.getMap()[0], 1);

  result = makeMatrix(tmp.getMap()[0], num_row, m.getNumCol());
  
  return result;
}

void Matrix::setValue(int row, int col, float value){
  val[row][col]=value;
}

float Matrix::getValue(int row, int col) const{
  return val[row][col];
}

float** Matrix::getMap() const{
  return val;
}

Matrix Matrix::getRowVec(int row) const{
  Matrix vec(1, num_col);

  for(int i=0;i<num_col;i++){
    vec[0][i]=val[row][i];
  }

  return vec;
}

Matrix Matrix::getColVec(int col) const{
  Matrix vec(num_row, 1);

  for(int i=0;i<num_row;i++){
    vec[i][0]=val[i][col];
  }

  return vec;
}

int Matrix::getNumRow() const{
  return num_row;
}

int Matrix::getNumCol() const{
  return num_col;
}

  bool Matrix::isEmpty() const{
    if(num_row==0 && num_col==0)
      return true;
    return false;
  }

bool Matrix::hasSameDimension(const Matrix& m) const{
  if(num_row==m.getNumRow() && num_col==m.getNumCol()){
    return true;
  }
  return false;
}

bool Matrix::canMultiply(const Matrix& m) const{
  if(num_col==m.getNumRow()){
    return true;
  }
  return false;
}
void Matrix::print() const{
  int i, j;
  
  printf("row: %d, col: %d\n", num_row, num_col);
  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      printf("%e ", val[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void Matrix::printFP(FILE* fp) const{
  int i, j;
  
  fprintf(fp, "row: %d, col: %d\n", num_row, num_col);
  for(i=0;i<num_row;i++){
    for(j=0;j<num_col;j++){
      fprintf(fp, "%e ", val[i][j]);
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
}

void Matrix::bsave(char* filename) const{
  FILE* fp = fopen(filename, "wb");

  fwrite(&num_row, sizeof(float), 1, fp);
  fwrite(&num_col, sizeof(float), 1, fp);
  for(int i=0;i<num_row;i++){
    fwrite(val[i], sizeof(float), num_col, fp);
  }
  fclose(fp);
}

void Matrix::bload(char* filename){
  FILE* fp = fopen(filename, "rb");

  if(val!=NULL){
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  }

  fread(&num_row, sizeof(float), 1, fp);
  fread(&num_col, sizeof(float), 1, fp);
  val = matrix(0, num_row-1, 0, num_col-1);
  for(int i=0;i<num_row;i++){
    fread(val[i], sizeof(float), num_col, fp);
  }

  fclose(fp);
}

void Matrix::bsave(FILE* fp) const{
  fwrite(&num_row, sizeof(float), 1, fp);
  fwrite(&num_col, sizeof(float), 1, fp);
  for(int i=0;i<num_row;i++){
    fwrite(val[i], sizeof(float), num_col, fp);
  }
}

void Matrix::bload(FILE* fp){
  if(val!=NULL){
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  }

  fread(&num_row, sizeof(float), 1, fp);
  fread(&num_col, sizeof(float), 1, fp);
  val = matrix(0, num_row-1, 0, num_col-1);
  for(int i=0;i<num_row;i++){
    fread(val[i], sizeof(float), num_col, fp);
  }
}


void Matrix::save(char* filename) const{
  FILE* fp = fopen(filename, "w");

  fprintf(fp, "%d %d\n", num_row, num_col);
  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      fprintf(fp, "%.10e ", val[i][j]);
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void Matrix::load(char* filename){
  FILE* fp;

  if(val!=NULL){
    free_matrix(val, 0, num_row-1, 0, num_col-1);
  }

  fp = fopen(filename, "r");

  fscanf(fp, "%d %d", &num_row, &num_col);
  val = matrix(0, num_row-1, 0, num_col-1);
  for(int i=0;i<num_row;i++){
    for(int j=0;j<num_col;j++){
      fscanf(fp, "%f", &val[i][j]);
    }
  }
  fclose(fp);
}

