// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Common_h_
#define _Common_h_

/* Some common types used throughout the program.  Here to avoid circular
 * dependencies (no typedef forward-declarations in C++)
 */

#include <vector>

namespace compiler {

class Arg;
class ArgSpec;
class Object;
class Type;

// An ObjectStore changeset commit ID
typedef int change_id;

// List of types that a function caller will provide
typedef std::vector<const Type*> paramtype_vec;
typedef paramtype_vec::const_iterator paramtype_iter;

// List of actual Object* arguments provided by a function caller
typedef std::vector<Object*> param_vec;
typedef param_vec::const_iterator param_iter;
typedef param_vec::iterator param_mod_iter;

// The function's specification of the arguments it accepts
typedef std::vector<ArgSpec*> argspec_vec;
typedef argspec_vec::const_iterator argspec_iter;

// A list of Arg nodes that hold function argument specifications
typedef std::vector<Arg*> arg_vec;
typedef arg_vec::const_iterator arg_iter;

}

#endif // _Common_h_
