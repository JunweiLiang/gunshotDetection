#include "math_engine.h"
/*
double MathEngine::differentiate(double (*f)(double), double x, double h, double* err){
  double result;

  result = ddfridr(f, x, h, err);

  return result;
}

float MathEngine::differentiate(float (*f)(float), float x, float h, float* err){
  float result;

  result = dfridr(f, x, h, err);

  return result;
}

float MathEngine::linearProgramming(const Matrix& obj, const Matrix& smaller, const Matrix& bigger, const Matrix& equal, Matrix& answer){
  int n, m, m1, m2, m3, i, j, k;
  int icase=0, *iposv, *izrov;
  float** a;
  float** map;
  float result;

  n = obj.getNumCol();
  m1 = smaller.getNumRow();
  m2 = bigger.getNumRow();
  m3 = equal.getNumRow();
  m = m1 + m2 + m3;

  a = matrix(1, m+2, 1, n+1);
  for(i=1;i<=m+2;i++){
    for(j=1;j<=n+1;j++){
      a[i][j]=0.0;
    }
  }

  iposv = ivector(1, m);
  izrov = ivector(1, n);

  i=1;
  a[i][1] = 0.0;
  for(j=0;j<n;j++){
    a[i][j+2] = obj.getMap()[0][j];
  }
  i++;
  for(j=0;j<m1;j++){
    for(k=0;k<n+1;k++){
      a[i][k+1] = smaller.getMap()[j][k];
      if(k>0 && a[i][k+1]!=0.0)
        a[i][k+1] *= -1.0;
    }
    i++;
  }
  for(j=0;j<m2;j++){
    for(k=0;k<n+1;k++){
      a[i][k+1] = bigger.getMap()[j][k];
      if(k>0 && a[i][k+1]!=0.0)
        a[i][k+1] *= -1.0;
    }
    i++;
  }
  for(j=0;j<m3;j++){
    for(k=0;k<n+1;k++){
      a[i][k+1] = equal.getMap()[j][k];
      if(k>0 && a[i][k+1]!=0.0)
        a[i][k+1] *= -1.0;
    }
    i++;
  }

  simplx(a, m, n, m1, m2, m3, &icase, izrov, iposv);

  result = a[1][1];
  answer = Matrix(1, n);

  for(j=1;j<=m;j++){
    if(iposv[j]<=n)
      answer[0][iposv[j]-1] = a[j+1][1];
  }

  free_ivector(iposv, 1, m);
  free_ivector(izrov, 1, n);
  free_matrix(a, 1, m+2, 1, n+1);

  return result;
}
*/
/*
void MathEngine::extEigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors){
  FILE* fp;
  float tmp;
  char buf[STRMAX];
  Matrix tmp_mat;
  int n=m.getNumRow();

  eigen_vectors = Matrix(n, n);
  eigen_values = Matrix(1, n);
  tmp_mat = Matrix(n, n);

  fp = fopen("./tmp.mat", "w");

  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      fprintf(fp, "%.9e ", m.getValue(i, j));
//      printf("%.9e ", m.getValue(i, j));
    }
    fprintf(fp, "\n");
//    printf("\n");
  }
  fclose(fp);

  system("octave eigen_decomp.sh > ./tmp.log");
  
  fp = fopen("./tmp_v.mat", "r");
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      fscanf(fp, "%f ", &tmp);
//      eigen_vectors.setValue(i, n-1-j, tmp);
      eigen_vectors.setValue(i, j, tmp);
    }
  }
  fclose(fp);

  fp = fopen("./tmp_d.mat", "r");
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      fscanf(fp, "%f ", &tmp);
      tmp_mat.setValue(i, j, tmp);
    }
  }
  fclose(fp);

  for(int i=0;i<n;i++)
    eigen_values.setValue(0, i, tmp_mat[i][i]);

  system("rm -f ./tmp.log");
  system("rm -f ./tmp.mat");
  system("rm -f ./tmp_v.mat");
  system("rm -f ./tmp_d.mat");
}
*/
/*
void MathEngine::extSVD(const Matrix& a, Matrix& u, Matrix& s, Matrix& v){
  FILE* fp;
  float tmp;
  char buf[STRMAX];
  int m=a.getNumRow();
  int n=a.getNumCol();
  int min = (m>n)?n:m;
  int max = (m>n)?m:n;

  u = Matrix(m, min);
  s = Matrix(1, min);
  v = Matrix(min, n);

  fp = fopen("./tmp.mat", "w");

  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      fprintf(fp, "%.9e ", a.getValue(i, j));
//      printf("%.9e ", m.getValue(i, j));
    }
    fprintf(fp, "\n");
//    printf("\n");
  }
  fclose(fp);

  system("octave svd.sh > ./tmp.log");
 
  fp = fopen("./tmp_u.mat", "r");
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  for(int i=0;i<m;i++){
    for(int j=0;j<min;j++){
      fscanf(fp, "%f ", &tmp);
      u.setValue(i, j, tmp);
    }
  }
  fclose(fp);

  fp = fopen("./tmp_s.mat", "r");
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  for(int i=0;i<min;i++){
    for(int j=0;j<min;j++){
      fscanf(fp, "%f ", &tmp);
      if(i==j)
        s.setValue(0, i, tmp);
    }
  }
  fclose(fp);

  fp = fopen("./tmp_v.mat", "r");
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  fgets(buf, STRMAX, fp);
  for(int i=0;i<min;i++){
    for(int j=0;j<n;j++){
      fscanf(fp, "%f ", &tmp);
      v.setValue(i, j, tmp);
    }
  }
  fclose(fp);

  system("rm -f ./tmp.log");
  system("rm -f ./tmp.mat");
  system("rm -f ./tmp_u.mat");
  system("rm -f ./tmp_s.mat");
  system("rm -f ./tmp_v.mat");
}
*/
void MathEngine::svd(const Matrix& a, Matrix& u, Matrix& s, Matrix& v){
  char jobu='A', jobvt='A';
  int m = a.getNumRow();
  int n = a.getNumCol();
  int min = (m>n)?n:m;
  int max = (m>n)?m:n;
//  float* array_a = new float[m*n];
  float* array_a = (float*) malloc(sizeof(float)*m*n);
  int lda=m;
//  float* array_s = new float[min];
  float* array_s = (float*) malloc(sizeof(float)*min);
//  float* array_u = new float[m*m];
  float* array_u = (float*) malloc(sizeof(float)*m*m);
  int ldu=m;
  //float* array_vt = new float[n*n];
  float* array_vt = (float*) malloc(sizeof(float)*n*n);
  int ldvt=n;
  int lwork=1024*ldvt;
  //float* work = new float[lwork];
  float* work = (float*) malloc(sizeof(float)*lwork);
  int info=0;

  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      array_a[j*m+i] = a.getValue(i, j);
    }
  }

  sgesvd_(&jobu, &jobvt, (integer*)&m, (integer*)&n, array_a, (integer*)&lda, array_s, array_u, (integer*)&ldu, array_vt, (integer*)&ldvt, work, (integer*)&lwork, (integer*)&info);

  if(info!=0){
    printf("Error in computing svd: %d\n", info);
    exit(0);
  }
   
  u = Matrix(m, m);
  s = Matrix(1, min);
  v = Matrix(n, n);

  for(int i=0;i<m;i++){
    for(int j=0;j<m;j++){
      u[j][i] = array_u[i*m + j];
    }
  }
  for(int i=0;i<min;i++){
    s[0][i] = array_s[i];
  }
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      v[i][j] = array_vt[i*n + j];
    }
  }

  free(array_a);
  free(array_u);
  free(array_s);
  free(array_vt);
  free(work);
}

void MathEngine::dsvd(const Matrix& a, Matrix& u, Matrix& s, Matrix& v){
  char jobu='A', jobvt='A';
  int m = a.getNumRow();
  int n = a.getNumCol();
  int min = (m>n)?n:m;
  int max = (m>n)?m:n;
  double* array_a = new double[m*n];
  int lda=m;
  double* array_s = new double[min];
  double* array_u = new double[m*m];
  int ldu=m;
  double* array_vt = new double[n*n];
  int ldvt=n;
  int lwork=1024*ldvt;
  double* work = new double[lwork];
  int info=0;

  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      array_a[j*m+i] = a.getValue(i, j);
    }
  }

  dgesvd_(&jobu, &jobvt, (integer*)&m, (integer*)&n, array_a, (integer*)&lda, array_s, array_u, (integer*)&ldu, array_vt, (integer*)&ldvt, work, (integer*)&lwork, (integer*)&info);

  if(info!=0){
    printf("Error in computing svd: %d\n", info);
    exit(0);
  }
   
  u = Matrix(m, m);
  s = Matrix(1, min);
  v = Matrix(n, n);

  for(int i=0;i<m;i++){
    for(int j=0;j<m;j++){
      u[j][i] = array_u[i*m + j];
    }
  }
  for(int i=0;i<min;i++){
    s[0][i] = array_s[i];
  }
  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      v[i][j] = array_vt[i*n + j];
    }
  }

  delete [] array_a;
  delete [] array_u;
  delete [] array_s;
  delete [] array_vt;
  delete [] work;
}

void MathEngine::deconSVD(const Matrix& a, Matrix& u, Matrix& s, Matrix& v){
  char jobz='S';
  int m = a.getNumRow();
  int n = a.getNumCol();
  int min = (m>n)?n:m;
  int max = (m>n)?m:n;
  double* array_a = new double[m*n];
  int lda=m;
  double* array_u = new double[m*min];
  double* array_s = new double[min];
  int ldu=m;
  double* array_vt = new double[min*n];
  int ldvt=min;
  int lwork=3*min*min+max+4*min*min+4*min;
  double* work = new double[lwork];
  int* iwork = new int[8*min];
  int info=0;

  for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
      array_a[j*m+i] = a.getValue(i, j);
    }
  }

  dgesdd_(&jobz, (integer*)&m, (integer*)&n, array_a, (integer*)&lda, array_s, array_u, (integer*)&ldu, array_vt, (integer*)&ldvt, work, (integer*)&lwork, (integer*)iwork, (integer*)&info);

  if(info!=0){
    printf("Error in computing svd: %d\n", info);
    exit(0);
  }
   
  u = Matrix(m, min);
  s = Matrix(1, min);
  v = Matrix(min, n);

  for(int i=0;i<m;i++){
    for(int j=0;j<min;j++){
      u[j][i] = array_u[i*min + j];
    }
  }
  for(int i=0;i<min;i++){
    s[0][i] = array_s[i];
  }
  for(int i=0;i<min;i++){
    for(int j=0;j<n;j++){
      v[i][j] = array_vt[i*n + j];
    }
  }

  delete [] array_a;
  delete [] array_u;
  delete [] array_s;
  delete [] array_vt;
  delete [] work;
  delete [] iwork;
}


void MathEngine::eigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors){
  char jobvl='N', jobvr='V';
  int n = m.getNumRow();
  float* a = (float*) malloc(sizeof(float)*n*n);
  float* wr = (float*) malloc(sizeof(float)*n);
  float* wi = (float*) malloc(sizeof(float)*n);
  int ldvl = 1;
  float* vr = (float*) malloc(sizeof(float)*n*n);
  float* work = (float*) malloc(sizeof(float)*8*n);
  int lwork=16*n;
  int code;

  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      a[i*n+j] = m.getValue(j, i);
    }
  }
  
  sgeev_(&jobvl, &jobvr, (integer*)&n, a, (integer*)&n, wr, wi, NULL, (integer*)&ldvl, vr, (integer*)&n, work, (integer*)&lwork, (integer*)&code);

  eigen_values = Matrix(1, n);
  eigen_vectors = Matrix(n, n);

  for(int i=0;i<n;i++){
    eigen_values[0][i] = wr[i];
    for(int j=0;j<n;j++){
      eigen_vectors[j][i] = vr[i*n+j];
    }
  }
  
  free(a);
  free(wr);
  free(wi);
  free(vr);
  free(work);
}

void MathEngine::dEigenDecomp(const Matrix& m, Matrix& eigen_values, Matrix& eigen_vectors){
  char jobvl='N', jobvr='V';
  int n = m.getNumRow();
  double* a = (double*) malloc(sizeof(double)*n*n);
  double* wr = (double*) malloc(sizeof(double)*n);
  double* wi = (double*) malloc(sizeof(double)*n);
  int ldvl = 1;
  double* vr = (double*) malloc(sizeof(double)*n*n);
  double* work = (double*) malloc(sizeof(double)*8*n);
  int lwork=16*n;
  int code;

  for(int i=0;i<n;i++){
    for(int j=0;j<n;j++){
      a[i*n+j] = m.getValue(j, i);
    }
  }
  
  dgeev_(&jobvl, &jobvr, (integer*)&n, a, (integer*)&n, wr, wi, NULL, (integer*)&ldvl, vr, (integer*)&n, work, (integer*)&lwork, (integer*)&code);

  eigen_values = Matrix(1, n);
  eigen_vectors = Matrix(n, n);

  for(int i=0;i<n;i++){
    eigen_values[0][i] = wr[i];
    for(int j=0;j<n;j++){
      eigen_vectors[j][i] = vr[i*n+j];
    }
  }
  
  free(a);
  free(wr);
  free(wi);
  free(vr);
  free(work);
}


float MathEngine::sigmoid(const float& x, const float& slope, const float& offset){
  return 1.0/(1.0+exp(-slope*x+offset));
}

int MathEngine::factorial(int n){
  if(n<0){
    printf("Error in calling factorial: %d\n", n);
    exit(-1);
  }
  if(n==0)
    return 1;
  for(int i=n-1;i>1;i--){
    n*=i;
  }

  return n;
}

int MathEngine::nCr(int n, int r){
  return factorial(n)/(factorial(r)*factorial(n-r));
}
