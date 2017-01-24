#ifndef TOKEN_H
#define TOKEN_H

#include "state.h"
#include "util_new.h"

struct History{
  History* past;
  //State* state;
  unsigned short time;
  unsigned short state_id;
  float score;
};

class Token{
  public:
    Token();
    Token(int state_id);

    void clear();
    Token& operator=(const Token& token);
    bool operator>(const Token& token) const;
    bool operator<(const Token& token) const;

    void setScore(const float& score);
    float getScore() const;
    void setTime(int time);
    int getTime() const;
    void setState(int state_id);
    int getState();

    void setAll(History* his, const float& score, const int& time);
    
    void setHistory(History* history);
    History* getHistory() const;
    void extendHistory(State* state, int state_id, int time, float score);
  private:
    float score;
    int state_id;
    History* history;
};

#endif
