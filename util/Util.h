// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Util_h_
#define _Util_h_

/* Miscellaneous utilities */

#include <string>

namespace Util {

std::string ltrim_space(std::string s) {
  size_t startpos = s.find_first_not_of(' ');
  if (startpos != std::string::npos) {
    return s.substr(startpos);
  }
  return "";
}

std::string rtrim_space(std::string s) {
  size_t endpos = s.find_last_not_of(' ');
  if (endpos != std::string::npos) {
    return s.substr(0, endpos+1);
  }
  return "";
}

};

#endif // _Util_h_
