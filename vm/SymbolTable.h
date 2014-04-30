// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* A runtime symbol table */

#include <boost/ptr_container/ptr_map.hpp>

#include <string>

namespace vm {

class Object;

typedef boost::ptr_map<std::string,Object> symbol_map;
typedef symbol_map::const_iterator symbol_iter;

}

#endif // _SymbolTable_h_
