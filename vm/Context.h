// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Context_h_
#define _Context_h_

/* Execution context: call stack and global variables */

#include "Symbol.h"

#include <boost/lexical_cast.hpp>

#include <ostream>
#include <vector>

namespace vm {

class Context {
public:
  const symbol_map& globals() const;
  symbol_map& globals();
  const symbol_map& locals() const;
  symbol_map& locals();

  const Object* find(const std::string& name) const;
  Object* find(const std::string& name);

  const Object* findGlobal(const std::string& name) const;
  Object* findGlobal(const std::string& name);

  void addFrame();
  void removeFrame();

  size_t depth() const { return m_callstack.size(); }

private:
  typedef symbol_map stackframe_map;
  typedef std::vector<stackframe_map> callstack_vec;
  typedef callstack_vec::const_iterator callstack_iter;

  symbol_map m_globals;
  callstack_vec m_callstack;
};

inline std::ostream& operator<<(std::ostream& out, const Context& context) {
  out << "Context with " << boost::lexical_cast<std::string>(context.globals().size()) << " globals, stack depth=" << boost::lexical_cast<std::string>(context.depth()) << "\n";
  out << "Globals: " << context.globals();
  return out;
}

}

#endif // _Context_h_
