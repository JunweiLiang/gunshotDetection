#ifndef TRANSCRIPTION_H
#define TRANSCRIPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Transcription{
  public:
    Transcription();
    Transcription(int max);

    void reset(int max);
    void set(const string& lab, int idx);
    void append(const string& lab);
    string get(int idx) const;
    int getMax() const;
    vector<string> getSeq() const;

    void addWord(const string& word, int start, int end);
    string getWord(int idx);
    int getNumWords();
    pair<int, int> getDuration(int idx);

    string& operator[](int idx);
  private:
    vector<string> label;
    vector<string> word_seq;
    vector<pair<int, int> > word_duration;
    int max;
};

#endif
