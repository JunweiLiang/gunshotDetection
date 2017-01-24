#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <math.h>

using namespace std;

class CacheTable{
  public:
    CacheTable();
    CacheTable(int num_dimensions, int num_elements);
    CacheTable(const vector<int>& dimension);
    CacheTable(int num_dimensions, ...);
    ~CacheTable();

    void calculateDSum();

    void set(float value, ...);
    void setAll(float value);
    void filterAll(float (*func)(float));
    void filterAll(double (*func)(double));
    float get(int d0, ...);
    void add(float value, ...);
    bool isAvailable(int d0, ...);
    int width(int d);

    void save(char* filename);
    void load(char* filename);
    void saveBinary(char* filename);
    void loadBinary(char* filename);

    void clear();
    void makeAvailable();
  private:
    float* map;
    bool* available;
    vector<int> dimension;
    vector<int> d_sum;
    int total;
};

#endif
