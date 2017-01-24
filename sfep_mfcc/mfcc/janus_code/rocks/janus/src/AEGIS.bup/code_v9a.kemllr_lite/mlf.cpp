#include "mlf.h"

MasterLabel::MasterLabel(){
  filename = "";
}

void MasterLabel::readMLF(char filename[]){
  FILE* fp;
  char line[STRMAX], token[STRMAX];
  int count=0, num_fields;
  vector<string> empty;

  this->filename = filename;
  header.clear();
  content.clear();

  fp = fopen(filename, "r");
  
  fgets(line, STRMAX, fp); // #!MLF!#
  while(1){
    fgets(line, STRMAX, fp);
    if(feof(fp))
      break;

    if(line[strlen(line)-1]=='\n')
      line[strlen(line)-1] = '\0';

    header.push_back(line);
    reg_header.push_back(toRegularExpr(removeQuote(line)));
    content.push_back(empty);
    while(1){
      fgets(line, STRMAX, fp);

      if(line[strlen(line)-1]=='\n')
        line[strlen(line)-1] = '\0';

      if(line[0]=='.')
        break;
      content[count].push_back(line);
    }

    count++;
  }

  fclose(fp);
}

void MasterLabel::createTranscription(const vector<string>& filelist, vector<Transcription>& transcription){
  int num_fields;
  int i, j, idx, counter, loc;
  char token[STRMAX];
  deque<int> queue;

  for(int i=0;i<reg_header.size();i++)
    queue.push_back(i);

  transcription.clear();
  transcription.resize(filelist.size());

  printf("filelist: %d, mlf: %d\n", filelist.size(), reg_header.size());

  loc=0;
  for(i=0;i<filelist.size();i++){
    idx = -1;
/*
    for(j=0;j<queue.size();j++){
      if(match((char*)filelist[i].c_str(), (char*)reg_header[queue[j]].c_str())){
        idx = j;
        break;
      }
    }
*/
    counter=0;
    while(true){
      if(match((char*)filelist[i].c_str(), (char*)reg_header[queue[loc]].c_str())){
        idx = loc;
        break;
      }
      loc = (loc+1)%queue.size();
      counter++;
      if(counter==queue.size()+1)
        break;
    }
 
    if(idx==-1){
      printf("MLF: %s not found\n", filelist[i].c_str());
      continue;
    }

    transcription[i].reset(content[queue[idx]].size());
    num_fields = numField((char*)content[idx][0].c_str(), " \t\n");
    for(j=0;j<content[queue[idx]].size();j++){
      extractField(content[queue[idx]][j].c_str(), num_fields-1, " \t\n", token);
      transcription[i].set(token, j);
    }

    //swap(queue[idx], queue[0]);
    //queue.pop_front();
  }
}

void MasterLabel::createAlignedTranscription(const vector<string>& filelist, const vector<int>& real_length, vector<Transcription>& transcription, int window_size, int target_rate){
  int num_fields;
  int i, j, k, t, frame, idx;
  char token[STRMAX], buf[STRMAX];
  int start, end, tmp, counter, loc;
  bool ignore;
  deque<int> queue;

  if(!isAligned())
    return;

  for(int i=0;i<reg_header.size();i++)
    queue.push_back(i);

  transcription.clear();
  transcription.resize(filelist.size());
  for(i=0;i<transcription.size();i++){
    transcription[i].reset(real_length[i]);
  }

  loc=0;
  for(i=0;i<filelist.size();i++){
    idx = -1;
/*    
    for(j=0;j<queue.size();j++){
      if(match((char*)filelist[i].c_str(), (char*)reg_header[queue[j]].c_str())){
        idx = j;
        break;
      }
    }
*/
    counter=0;
    while(true){
      if(match((char*)filelist[i].c_str(), (char*)reg_header[queue[loc]].c_str())){
        idx = loc;
        break;
      }
      loc = (loc+1)%queue.size();
      counter++;
      if(counter==queue.size()+1)
        break;
    }
    
    if(idx==-1){
      printf("MLF: %s not found\n", filelist[i].c_str());
      continue;
    }

    frame = 0;
    t = 0;
    
    ignore = false;
    num_fields = numField((char*)content[queue[idx]][0].c_str(), " \t\n");
    for(j=0;j<content[queue[idx]].size();j++){
      extractField(content[queue[idx]][j].c_str(), 0, " \t\n", buf);
      start = atoi(buf);
      extractField(content[queue[idx]][j].c_str(), 1, " \t\n", buf);
      end = atoi(buf);
      extractField(content[queue[idx]][j].c_str(), 2, " \t\n", token);

      if(j==content[queue[idx]].size()-1 || end/target_rate >= real_length[i]){
        tmp = end;
        while(end+window_size>tmp)
          end-=target_rate;
        if(end/target_rate >= real_length[i])
          ignore = true;
      }
//      printf("%d %d %s %d\n", start, end, token, transcription[i].getMax());
      start /= target_rate;
      end /= target_rate;

      if(j!=0 && transcription[i].get(start-1)!="")
        start++;

      for(k=start;k<=end;k++)
        transcription[i].set(token, k);
      extractField(content[queue[idx]][j].c_str(), 0, " \t\n", buf);
      start = atoi(buf);
      extractField(content[queue[idx]][j].c_str(), 1, " \t\n", buf);
      end = atoi(buf);
      start /= target_rate;
      end /= target_rate;
      transcription[i].addWord(token, start, end);

      if(ignore){
        break;
      }
    }

    //swap(queue[idx], queue[0]);
    //queue.pop_front();
  }
}

bool MasterLabel::isAligned(){
  if(numField((char*)content[0][0].c_str(), " \t\n")==3)
    return true;
  return false;
}

void MasterLabel::print(){
  printf("#!MLF!#\n");
  for(int i=0;i<header.size();i++){
    printf("%s\n", header[i].c_str());
    for(int j=0;j<content[i].size();j++){
      printf("%s\n", content[i][j].c_str());
    }
    printf(".\n");
  }
}

string MasterLabel::removeQuote(const string& str){
  if(str[0]=='"' && str[str.size()-1]=='"')
    return str.substr(1, str.size()-2);
  return str;
}

string MasterLabel::toRegularExpr(const string& str){
  string result = "";
  int loc=0;

  for(int i=0;i<str.size();i++){
    if(str[i]=='*'){
      result += '.';
      result += '*';
    }
    else if(str[i]=='.'){
      result += '\\';
      result += '.';
    }
    else{
      result += str[i];
    }
  }

  if(result.substr(result.size()-3, result.size()-1)=="lab" || result.substr(result.size()-3, result.size()-1)=="rec"){
    result = result.substr(0, result.size()-3);
    result+='.';
    result+='*';
  }

  return result;
}
