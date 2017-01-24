#include <cassert>
#include "split_string.h"

const string RXwhite = " \t\n";
//using namespace std;
// splits A_STRING into PARTS; the split is based on a separator_list;
// returns the number of fields resulting from the split
unsigned int 
split(const string&		a_string, 
      vector<string>&         parts, 
      string		separator_list){
   
   assert(parts.empty() == true);
   unsigned int _ret = 0;
   
   unsigned int first = 0;
   unsigned int last  = 0;
   while(true){
      first = a_string.find_first_not_of(separator_list, last);
      if(first >= a_string.length()){
         break;
      }else if((first > 0) && (last == 0)){// it starts with a separator
         parts.push_back("");
         _ret++;			
      }
      last  = a_string.find_first_of(separator_list, first);
      assert(first < last);
      if(last > a_string.length()){
         last = a_string.length();
      }
      string element = a_string.substr(first,last-first);
      parts.push_back(element);
      _ret++;
   }
   assert(_ret == parts.size());
   return _ret;
}

// reads a line from stream and removes head/trailing whitespace
istream&
get_clean_line(istream& s, string& line){
   getline(s, line);    assert(s.good());
   
   // remove begining/trailing white spaces
   unsigned int first_non_white = line.find_first_not_of(RXwhite);
   if( first_non_white > 0)
      line.erase(0,first_non_white<line.length()?first_non_white:line.length());
   if(line.find_last_not_of(RXwhite) < line.length()-1)
      line.erase(line.find_last_not_of(RXwhite)+1, line.length());
   
   return s;
}
