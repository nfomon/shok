// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IStatik_h_
#define _IStatik_h_

#include "ISError.h"

#include <string>

namespace istatik {

class IStatik {
public:
  IStatik(const std::string& compiler_name);
  ~IStatik();
  void run();

private:
  static void clear_screen(int sig);

  std::string m_compiler_name;
  // Compiler m_compiler;   // TODO
};

}

#endif // _IStatik_h_
