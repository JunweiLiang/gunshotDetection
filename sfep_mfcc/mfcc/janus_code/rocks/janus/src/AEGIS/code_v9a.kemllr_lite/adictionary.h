#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include "transcription.h"
#include "str_util.h"

using namespace std;

class AegisDictionary{
  public:
    AegisDictionary();
    AegisDictionary(const string& sent_bound);

    void clear();
    void parse(char* dict_file);

    int getNumWords();
    void addWord(const string& word, const vector<string>& phone_seq);
    void getSymSeq(const string& word, vector<string>& sym_seq);
    void getTriphoneSeq(const vector<string>& sentence, const vector<string>& bound_sym, vector<string>& triph_seq);
    Transcription convertToPhoneLevel(Transcription& word_trans);
  private:
    map<string, vector<string> > collection;
    string sent_bound;
};

#endif
