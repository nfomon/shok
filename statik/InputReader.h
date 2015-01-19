// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _InputReader_h_
#define _InputReader_h_

#include "StatikError.h"

namespace statik {

class InputReader {
  InputReader() {}
  virtual ~InputReader() {}

  virtual void Insert(int line, int col, char c) = 0;
  virtual void Delete(int line, int col) = 0;
};

}

#endif // _InputReader_h_
