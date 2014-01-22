// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* SymbolTable
 *
 * The SymbolTable is a list of Symbols that it owns, and has commit/revert
 * logic on new additions.  Scope and Symbol each have an SymbolTable
 * internally backing their members.
 *
 * newSymbol and delSymbol operations are the only "necessary" ones, for our
 * purpose of allowing "hey that object doesn't exist yet!" error checking
 * before evaluation-time.  But since the SymbolTable owns the memory of its
 * objects, it's also where we choose to invoke object constructors and
 * destructors.  That's why we also have replaceObject -- it's so the
 * SymbolTable can be responsible for calling the destructor of the replaced
 * object (also at evaluation-time aka commit-time, of course).
 */

#include "Common.h"
#include "EvalError.h"
#include "Log.h"
#include "Symbol.h"

#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace eval {

class Object;
class Symbol;

class SymbolTable {
public:
  typedef std::map<std::string,Symbol*> symbol_map;
  typedef std::pair<std::string,Symbol*> symbol_pair;
  typedef symbol_map::const_iterator symbol_iter;
  typedef symbol_map::iterator symbol_mod_iter;

  SymbolTable(Log& log)
    : m_log(log) {}
  ~SymbolTable();

  void reset();
  void commitFirst();
  void commitAll();
  void revertLast();
  void revertAll();

  // Lookup a symbol, returning NULL if it is not here
  Symbol* getSymbol(const std::string& varname) const;
  // Construct a new symbol, as "pending" until it's either commit or revert
  Symbol& newSymbol(const std::string& varname, std::auto_ptr<Type> type);
  // Delete a symbol.  Calls the object's destructor when commit.
  void delSymbol(const std::string& varname);
  // Initialize a new'd symbol with its first value.  (evaluation-time)
  void initSymbol(const std::string& varname, std::auto_ptr<Object> newObject);
  // Assign a new value to an object.  (evaluation-time)
  void replaceObject(const std::string& varname,
                     std::auto_ptr<Object> newObject);

  std::auto_ptr<SymbolTable> duplicate() const;

  const symbol_map& getSymbolMap() const { return m_symbols; }
  size_t size() const { return m_symbols.size(); }

private:
  typedef std::pair<std::string,bool> ordering_pair;
  typedef std::deque<ordering_pair> order_vec;
  typedef order_vec::const_iterator ordering_iter;
  typedef order_vec::iterator ordering_mod_iter;
  typedef order_vec::const_reverse_iterator ordering_rev_iter;
  typedef order_vec::reverse_iterator ordering_rev_mod_iter;

  struct SymbolChange {
    SymbolChange(const std::string& varname)
      : varname(varname) {}
    virtual ~SymbolChange() {}
    std::string varname;
  };
  struct AddSymbol : public SymbolChange {
    AddSymbol(const std::string& varname)
      : SymbolChange(varname) {}
    virtual ~AddSymbol() {}
  };
  struct DelSymbol : public SymbolChange {
    DelSymbol(const std::string& varname,
              std::auto_ptr<Type> oldType,
              std::auto_ptr<Object> oldObject)
      : SymbolChange(varname), oldType(oldType), oldObject(oldObject) {
    }
    virtual ~DelSymbol() {}
    std::auto_ptr<Type> oldType;
    std::auto_ptr<Object> oldObject;
  };

  typedef std::deque<SymbolChange*> changeset_vec;
  typedef changeset_vec::const_iterator changeset_iter;
  typedef changeset_vec::const_reverse_iterator changeset_rev_iter;

  Log& m_log;
  // The store of symbols (typed objects).
  // Eager: includes the latest uncommit changeset
  symbol_map m_symbols;
  // The object names in the order in which they are constructed (hence the
  // reverse order in which they should be deleted).  The flag indicates if the
  // object is pending deletion; used to help commit/revert deletions.
  order_vec m_ordering;
  // Changeset pending commit; changes can be rolled back
  changeset_vec m_changeset;
};

}

#endif // _SymbolTable_h_
