#ifndef PDF_H
#define PDF_H

#include <string>
using namespace std;

class PDF{
  public:
    PDF();
    PDF(const string& id, int dim);
    virtual ~PDF();
   
    virtual string getID();

    virtual float logBj(const float*)=0;
  protected:
    string id;
    int dim;
};

#endif
