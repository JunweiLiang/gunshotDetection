#include "transcription.h"

Transcription::Transcription(){
  label.clear();
  word_seq.clear();
  word_duration.clear();
  max=0;
}

Transcription::Transcription(int max){
  label.resize(max);
  word_seq.clear();
  word_duration.clear();
  this->max = max;
}

void Transcription::reset(int max){
  label.clear();
  label.resize(max);
  word_seq.clear();
  word_duration.clear();
  this->max = max;
}

void Transcription::set(const string& lab, int idx){
  label[idx] = lab;
}

void Transcription::append(const string& lab){
  label.push_back(lab);
  max++;
}

void Transcription::addWord(const string& word, int start, int end){
  pair<int, int> tuple;
  
  word_seq.push_back(word);
  tuple.first = start;
  tuple.second = end;
  word_duration.push_back(tuple);
}

string Transcription::getWord(int idx){
  return word_seq[idx];
}

int Transcription::getNumWords(){
  return word_seq.size();
}

pair<int, int> Transcription::getDuration(int idx){
  return word_duration[idx];
}

string Transcription::get(int idx) const{
  return label[idx];
}

vector<string> Transcription::getSeq() const{
  return label;
}

int Transcription::getMax() const{
  return max;
}

string& Transcription::operator[](int idx){
  return label[idx];
}
