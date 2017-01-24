#include "token.h"

Token::Token(){
  score = LZERO;
  state_id = -1;
  history = NULL;
}

Token::Token(int state_id){
  score = LZERO;
  this->state_id = state_id;
  history = NULL;
}

void Token::clear(){
  score = LZERO;
  history = NULL;
}

Token& Token::operator=(const Token& token){
  score = token.getScore();
  history = token.getHistory();
  return *this;
}

bool Token::operator>(const Token& token) const{
  return score > token.getScore();
}

bool Token::operator<(const Token& token) const{
  return score < token.getScore();
}

void Token::setScore(const float& score){
  this->score = score;
}

void Token::setAll(History* history, const float& score, const int& time){
  this->history = history;
  this->score = score;
  this->history->time = time;
}

float Token::getScore() const{
  return score;
}

void Token::setState(int state_id){
  this->state_id = state_id;
}

int Token::getState(){
  return state_id;
}

void Token::setHistory(History* history){
  this->history = history;
}

History* Token::getHistory() const{
  return history;
}

void Token::setTime(int time){
  history->time = time;
}

int Token::getTime() const{
  if(history==NULL)
    return -1;
  return history->time;
}

void Token::extendHistory(State* state, int state_id, int time, float score){
  History* history;

  history = new History();
  //history->state = state;
  history->state_id = state_id;
  history->time = time;
  history->score = score;

  history->past = this->history;

  this->history = history;
}
