// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Context_h_
#define _Context_h_

/* Execution context: call stack and global variables */

#include "Object.h"

#include <vector>

namespace vm {

class Context {
public:
  symbol_map& globals();
  symbol_map& locals();

  symbol_mod_iter find(const std::string& name);
  symbol_iter find(const std::string& name) const;
  symbol_iter end() const;

  void addFrame();
  void removeFrame();

private:
  typedef symbol_map stackframe_map;
  typedef std::vector<stackframe_map> callstack_vec;
  typedef callstack_vec::const_iterator callstack_iter;

  symbol_map m_globals;
  callstack_vec m_callstack;
};

}

#endif // _Context_h_
