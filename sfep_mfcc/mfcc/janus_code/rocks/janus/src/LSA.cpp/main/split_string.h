#ifndef _split_string_h
#define _split_string_h
#include <string>
#include <iostream>
#include <vector>

using namespace std;

extern const string RXwhite;
// splits A_STRING into PARTS; the split is based using on a separator_list_LIST;
// returns the number of fields resulting from the split
unsigned int 
split(const string&		a_string, 
      vector<string>&     parts, 
      string		separator_list=RXwhite);

// reads a line from stream and removes head/trailing whitespace
istream&
get_clean_line(istream& s, string& line);
#endif
