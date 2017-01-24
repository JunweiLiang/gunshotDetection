#include "whitener.h"

Whitener::Whitener(MFCCReader* reader, int dim){
  this->reader=reader;
  this->dim = dim;
}

void Whitener::prepare(){
  Matrix obs;
  int count=0;
  
  mean = Matrix(dim, 1);
  cov = Matrix(dim, dim);

  printf("calculating mean and cov\n");
  
  for(int i=0;i<reader->getTotalUtterance();i++){
    reader->readData(i);
    for(int t=0;t<reader->getTotalFrames();t++){
      obs = convert(reader->getX()[t]);
      mean = mean+obs;
      count++;
    }
  }
  mean = mean*(1.0/count);

  for(int i=0;i<reader->getTotalUtterance();i++){
    reader->readData(i);
    for(int t=0;t<reader->getTotalFrames();t++){
      obs = convert(reader->getX()[t]);
      cov = cov + (obs-mean)*((obs-mean).transpose());
    }
  }


  cov = cov*(1.0/count);

//  mean.print();
//  cov.print();
//  printf("consistency checking for cov: %lf\n", cov.determinant());
}

void Whitener::findTransformation(){
  Matrix eigen_values, eigen_vectors;
  Matrix lambda(dim, dim);

  printf("finding whitening transformation\n");

  MathEngine::eigenDecomp(cov, eigen_values, eigen_vectors);
//  eigen_values.print();
//  eigen_vectors.print();

  for(int i=0;i<dim;i++){
    lambda[i][i] = 1.0/sqrt(eigen_values[0][i]);
  }
  transformation = (eigen_vectors*lambda).transpose();
}

void Whitener::whiten(){
  Matrix x, v, u, utterance;
  Matrix obs;
  int count=0;

  printf("performing whitening\n");
  utterance = Matrix(reader->getMaxT(), reader->getMaxDim());
  reader->setShareX(utterance.getMap(), reader->getMaxT(), reader->getMaxDim());

  for(int i=0;i<reader->getTotalUtterance();i++){
//    printf("i: %d\n", i);
    reader->readData(i);
    for(int t=0;t<reader->getTotalFrames();t++){
      x = convert(reader->getX()[t]);
      v = x - mean;
      u = transformation*v;

      for(int d=0;d<dim;d++){
        utterance[t][d] = u[0][d];
      }
    }
    reader->writeData(i);
  }

  mean = Matrix(dim, 1);
  cov = Matrix(dim, dim);

  for(int i=0;i<reader->getTotalUtterance();i++){
    reader->readData(i);
    for(int t=0;t<reader->getTotalFrames();t++){
      obs = convert(reader->getX()[t]);
      mean = mean+obs;
      count++;
    }
  }
  mean = mean*(1.0/count);

  for(int i=0;i<reader->getTotalUtterance();i++){
    reader->readData(i);
    for(int t=0;t<reader->getTotalFrames();t++){
      obs = convert(reader->getX()[t]);
      cov = cov + (obs-mean)*((obs-mean).transpose());
    }
  }

  cov = cov*(1.0/count);

//  mean.print();
//  cov.print();
  printf("consistency checking for cov: %lf\n", cov.determinant());
}

Matrix Whitener::convert(float* x){
  Matrix obs(dim, 1);

  for(int i=0;i<dim;i++){
    obs[i][0] = x[i];
  }

  return obs;
}
