// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IStatik_h_
#define _IStatik_h_

#include "ISError.h"

#include "exstatik/Compiler.h"

#include <curses.h>

#include <string>
#include <vector>

namespace istatik {

class IStatik {
public:
  IStatik(const std::string& compiler_name);
  ~IStatik();
  void run();

private:
  static void finish(int sig);

  void init_screen();

  std::string m_compiler_name;
  exstatik::Compiler m_compiler;
  std::vector<WINDOW*> m_windows;
  std::vector<int> m_windowSizes;
};

}

#endif // _IStatik_h_
