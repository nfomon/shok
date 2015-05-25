// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _exstatik_Compiler_h_
#define _exstatik_Compiler_h_

#include "statik/Rule.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <string>

namespace exstatik {

typedef boost::ptr_vector<statik::Rule> Compiler;
typedef Compiler::const_iterator Compiler_iter;
typedef Compiler::iterator Compiler_mod_iter;
std::auto_ptr<Compiler> MakeCompiler(const std::string& name);

}

#endif // _exstatik_Compiler_h_
