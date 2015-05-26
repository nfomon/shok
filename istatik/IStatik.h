// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _istatik_IStatik_h_
#define _istatik_IStatik_h_

#include "ISError.h"
#include "WindowResponse.h"

#include "exstatik/Compiler.h"

#include <curses.h>

#include <string>
#include <vector>

namespace istatik {

class IStatik {
public:
  IStatik(const std::string& compiler_name, const std::string& graphdir);
  ~IStatik();
  void run();

private:
  static void finish(int sig);

  void InitScreen(size_t numParsers);
  void UpdateWindow(int window_index,
                    const WindowResponse::action_vec& actions);

  std::string m_compiler_name;
  std::string m_graphdir;
  std::vector<WINDOW*> m_windows;
  std::vector<int> m_windowSizes;
};

}

#endif // _istatik_IStatik_h_
