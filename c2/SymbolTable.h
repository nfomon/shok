// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* SymbolTable */

#include "CompileError.h"

#include <boost/ptr_container/ptr_map.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Type;

// TODO We need to be able to lookup symbols by name, but we also need to keep
// track of the ordering of their lifetimes, so that they can be deleted in the
// correct order in Scope::bytecode().

typedef boost::ptr_map<std::string,Type> symbol_map;
typedef symbol_map::const_iterator symbol_iter;
typedef symbol_map::const_reverse_iterator symbol_rev_iter;

class SymbolTable {
public:
  typedef std::vector<std::pair<std::string,Type*> > lifetime_vec;
  typedef lifetime_vec::const_iterator lifetime_iter;
  typedef lifetime_vec::const_reverse_iterator lifetime_rev_iter;

private:
  symbol_map m_symbols;
  lifetime_vec m_lifetimes;
};

}

#endif // _SymbolTable_h_
