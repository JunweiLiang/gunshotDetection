#ifndef MLF_H
#define MLF_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <deque>
#include "transcription.h"
#include "str_util.h"

using namespace std;

class MasterLabel{
  public:
    MasterLabel();

    void readMLF(char filename[]);
    void createTranscription(const vector<string>& filelist, vector<Transcription>& transcription);
    void createAlignedTranscription(const vector<string>& filelist, const vector<int>& real_length, vector<Transcription>& transcription, int window_size, int target_rate);
    bool isAligned();

    void print();

    string removeQuote(const string& str);
    string toRegularExpr(const string& header);

  private:
    string filename;
    vector<string> reg_header;
    vector<string> header;
    vector<vector<string> > content;
};

#endif
