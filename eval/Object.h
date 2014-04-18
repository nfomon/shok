// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object
 *
 * This is our internal Object representation.  It should be a fairly dumb,
 * collapsed namespace of member objects, with no concern for types.
 *
 * Object is not a Node; it's a thing created by a SymbolTable.
 */

#include "Common.h"
#include "Method.h"
#include "Type.h"

#include "util/Log.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace compiler {

class Expression;
class Type;

class Object {
public:
  Object(Log& log, const std::string& name);
  ~Object();

  std::string getName() const { return m_name; }
  std::string print() const { return m_name; }

  // Retrieve a member
  Object* getMember(const std::string& name) const;
  void newMember(const std::string& name, std::auto_ptr<Object> object);
  // TODO replaceMember, delMember
  void newMethod(const arg_vec* args,
                 std::auto_ptr<Type> returnType,
                 std::auto_ptr<Block> body);

  // Function
  bool isFunction() const { return !m_methods.empty(); }
  /*
  bool takesArgs(const paramtype_vec& params) const;
  std::auto_ptr<Type> getPossibleReturnTypes(const paramtype_vec& params) const;
  std::auto_ptr<Object> call(const param_vec& params) const;
  */

  // Constructor/destructor functions.
  void construct();
  void destruct();
  std::auto_ptr<Object> clone(const std::string& newName) const;

private:
  typedef std::map<std::string,Object*> member_map;
  typedef std::pair<std::string,Object*> member_pair;
  typedef member_map::const_iterator member_iter;
  typedef std::vector<Method*> method_vec;
  typedef method_vec::const_iterator method_iter;

  Log& m_log;
  std::string m_name;
  // An abstract is an object with any non-Function Signatures, and/or any
  // OrType members that do not also have an initial value.
  bool m_isAbstract;
  bool m_isConst;
  // Track whether the object has yet been constructed or destructed
  bool m_isConstructed;
  bool m_isDestructed;
  member_map m_members;
  method_vec m_methods;
};

}

#endif // _Object_h_
