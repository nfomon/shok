// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OutputFunc_h_
#define _OutputFunc_h_

#include "Hotlist.h"
#include "IList.h"

#include <memory>
#include <set>
#include <string>

namespace fw {

class FWTree;
class OutputFunc;

std::auto_ptr<OutputFunc> MakeOutputFunc_Silent();
std::auto_ptr<OutputFunc> MakeOutputFunc_Basic(const std::string& name, const std::string& value = "");
std::auto_ptr<OutputFunc> MakeOutputFunc_IValues(const std::string& name);
std::auto_ptr<OutputFunc> MakeOutputFunc_Winner();
std::auto_ptr<OutputFunc> MakeOutputFunc_Sequence();
std::auto_ptr<OutputFunc> MakeOutputFunc_Cap(std::auto_ptr<OutputFunc> outputFunc, const std::string& cap);

class OutputFunc {
public:
  typedef std::set<const IList*> emitting_set;
  typedef emitting_set::const_iterator emitting_iter;

  OutputFunc();
  virtual ~OutputFunc() {}

  virtual void Init(const FWTree& x) { m_node = &x; }

  IList* OStart() { return m_ostart; }
  IList* OEnd() { return m_oend; }
  const emitting_set& Emitting() const { return m_emitting; }
  const emitting_set& WasEmitting() const { return m_wasEmitting; }
  const Hotlist::hotlist_vec& GetHotlist() const { return m_hotlist.GetHotlist(); }

  virtual void operator() () = 0;
  virtual std::auto_ptr<OutputFunc> Clone() = 0;

  //std::string DrawEmitting(const std::string& context) const;
  std::string PrintHotlist() const { return m_hotlist.Print(); }

protected:
  // Convenience methods that can be called by operator() to use child OLists
  // to produce this node's OList.
  void ApproveChild(const FWTree& child);
  void InsertChild(const FWTree& child);
  void DeleteChild(const FWTree& child);

  const FWTree* m_node;
  IList* m_ostart;    // first emittable olist node that is spanned
  IList* m_oend;      // last emittable olist node that is spanned
  emitting_set m_emitting;    // ONodes that we are currently happy to emit
  emitting_set m_wasEmitting; // ONodes that we emit last tree-cycle
  Hotlist m_hotlist;  // active olist updates
};

class OutputFunc_Silent : public OutputFunc {
public:
  OutputFunc_Silent()
    : OutputFunc() {}
  virtual ~OutputFunc_Silent() {}
  virtual void operator() () {}
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Silent());
  }
};

// Emits a single output list node, that contains the provided name and value.
class OutputFunc_Basic : public OutputFunc {
public:
  OutputFunc_Basic(const std::string& name, const std::string& value = "");
  virtual ~OutputFunc_Basic() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Basic(m_onode.name, m_onode.value));
  }

protected:
  IList m_onode;   // Single output list node
  bool m_isFirstTime;
};

// Single output node with the provided name.  The value is the concatenation
// of all of the input node values.
class OutputFunc_IValues : public OutputFunc {
public:
  OutputFunc_IValues(const std::string& name);
  virtual ~OutputFunc_IValues() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_IValues(m_onode.name));
  }

protected:
  IList m_onode;    // Single output list node
  bool m_isFirstTime;
};

// Outputs all the nodes from the single child that is a "winner".  Corresponds
// with the Or rule.
class OutputFunc_Winner : public OutputFunc {
public:
  OutputFunc_Winner();
  virtual ~OutputFunc_Winner() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Winner());
  }

private:
  const FWTree* m_winner;
};

// Outputs all the nodes from the children that are emitting, in child order.
class OutputFunc_Sequence : public OutputFunc {
public:
  OutputFunc_Sequence()
    : OutputFunc() {}
  virtual ~OutputFunc_Sequence() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Sequence());
  }

private:
  typedef std::set<const FWTree*> emitchildren_set;
  typedef emitchildren_set::const_iterator emitchildren_iter;
  typedef emitchildren_set::iterator emitchildren_mod_iter;
  emitchildren_set m_emitChildren;
};

// Wraps an OutputFunc with an extra output node at the start and end.
class OutputFunc_Cap : public OutputFunc {
public:
  OutputFunc_Cap(std::auto_ptr<OutputFunc> outputFunc, const std::string& cap);
  virtual ~OutputFunc_Cap() {}

  virtual void Init(const FWTree& x);
  virtual void operator() ();

  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Cap(m_outputFunc->Clone(), m_cap));
  }

protected:
  std::auto_ptr<OutputFunc> m_outputFunc;
  std::string m_cap;
  IList m_capStart;
  IList m_capEnd;
  bool m_isFirstTime;
};

}

#endif // _OutputFunc_h_