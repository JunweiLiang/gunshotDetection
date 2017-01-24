#include "adictionary.h"

AegisDictionary::AegisDictionary(){
  collection.clear();
  sent_bound = "SIL";
}

AegisDictionary::AegisDictionary(const string& sent_bound){
  collection.clear();
  this->sent_bound = sent_bound;
}

void AegisDictionary::clear(){
  collection.clear();
}

void AegisDictionary::addWord(const string& word, const vector<string>& phone_seq){
  collection[word] = phone_seq;
}

void AegisDictionary::parse(char* dict_file){
  FILE* fp = fopen(dict_file, "r");
  char buf[STRMAX], word[STRMAX], sym[STRMAX];
  vector<string> sym_seq;

  clear();

  while(true){
    fgets(buf, STRMAX, fp);
    if(feof(fp))
      break;
    if(buf[0]=='#')
      continue;
    if(strcmp(buf, "")==0 || strcmp(buf, "\n")==0)
      continue;
    sym_seq.clear();
    extractField(buf, 0, " \t\n", word);
    for(int i=1;i<numField(buf, " \t\n");i++){
      extractField(buf, i, " \t\n", sym);
      sym_seq.push_back(sym);
    }
    collection[word] = sym_seq;
  }

  fclose(fp);
}

int AegisDictionary::getNumWords(){
  return collection.size();
}

void AegisDictionary::getSymSeq(const string& word, vector<string>& sym_seq){
  sym_seq.clear();

  sym_seq = collection[word];
}

void AegisDictionary::getTriphoneSeq(const vector<string>& sentence, const vector<string>& bound_sym, vector<string>& triph_seq){
  vector<string> mono_seq, tmp_seq;
  map<string, bool> is_bound;
  string tri_ph;
  
  triph_seq.clear();

  mono_seq.push_back("SIL");
  for(int i=0;i<sentence.size();i++){
    getSymSeq(sentence[i], tmp_seq);
    for(int j=0;j<tmp_seq.size();j++){
      mono_seq.push_back(tmp_seq[j]);
    }
  }
  mono_seq.push_back("SIL");

  for(int i=0;i<bound_sym.size();i++){
    is_bound[bound_sym[i]] = true;
  }

  for(int i=0;i<mono_seq.size();i++){
    if(is_bound[mono_seq[i]]){
      triph_seq.push_back(mono_seq[i]);
      continue;
    }
    if(i==0 || i==mono_seq.size()-1)
      continue;
    tri_ph=mono_seq[i-1]+"-"+mono_seq[i]+"-"+mono_seq[i+1];
    triph_seq.push_back(tri_ph);
  }
}

Transcription AegisDictionary::convertToPhoneLevel(Transcription& word_trans){
  Transcription phone_trans;
  vector<string> phone_seq;

  for(int i=0;i<word_trans.getMax();i++){
    getSymSeq(word_trans[i], phone_seq);
    for(int j=0;j<phone_seq.size();j++){
      phone_trans.append(phone_seq[j]);
    }
  }

  return phone_trans;
}
