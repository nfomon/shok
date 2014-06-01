// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object */

#include "Context.h"
#include "Instruction.h"
#include "Symbol.h"

#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/variant.hpp>

#include <memory>
#include <string>
#include <vector>

namespace vm {

typedef std::vector<Instruction> function_vec;
typedef function_vec::const_iterator function_iter;

typedef boost::ptr_vector<Object> args_vec;
typedef args_vec::const_iterator args_iter;

typedef boost::variant<
  int,
  std::string
> BuiltinData;

class Object {
public:
  Object();
  Object(const Object&);
  Object& operator= (const Object&);
  ~Object();

  const Object* find(const std::string& name) const;
  Object* find(const std::string& name);

  void insert(const std::string& name, std::auto_ptr<Object> value);
  void assign(const std::string& name, std::auto_ptr<Object> value);

  void insertFunction(std::auto_ptr<function_vec> function);
  std::auto_ptr<Object> callFunction(Context& context, args_vec& args) const;

  void insertBuiltin(const BuiltinData& builtin) {
    m_builtin = builtin;
  }
  boost::optional<BuiltinData> builtinData() const { return m_builtin; }

protected:
  symbol_map m_members;
  std::auto_ptr<function_vec> m_function;
  boost::optional<BuiltinData> m_builtin;
};

}

#endif // _Object_h_
