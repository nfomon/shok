// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Util.h"

#include <sstream>
#include <string>
#include <utility>

using namespace Util;

using std::pair;
using std::string;
using std::stringstream;
using std::vector;

pair<string,string> Util::break_word(string s) {
  size_t ws_pos = s.find(' ');
  return make_pair(string(s, 0, ws_pos), string(s, ws_pos+1, string::npos));
}

vector<string> Util::split(const string& s, char delim) {
  vector<string> v;
  stringstream ss(s);
  string item;
  while (std::getline(ss, item, delim)) {
    v.push_back(item);
  }
  return v;
}

string Util::ltrim_space(const string& _s) {
  string s(_s);
  size_t startpos = s.find_first_not_of(' ');
  if (startpos != string::npos) {
    return s.substr(startpos);
  }
  return "";
}

string Util::rtrim_space(const string& _s) {
  string s(_s);
  size_t endpos = s.find_last_not_of(' ');
  if (endpos != string::npos) {
    return s.substr(0, endpos+1);
  }
  return "";
}

string Util::pad_str(const string& _s, char c, size_t len) {
  string s(_s);
  while (s.size() < len) {
    s = c + s;
  }
  return s;
}
