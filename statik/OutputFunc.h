// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_OutputFunc_h_
#define _statik_OutputFunc_h_

#include "IList.h"

#include <map>
#include <memory>
#include <set>
#include <string>

namespace statik {

class STree;
class OutputFunc;

std::auto_ptr<OutputFunc> MakeOutputFunc_Silent();
std::auto_ptr<OutputFunc> MakeOutputFunc_Pass();
std::auto_ptr<OutputFunc> MakeOutputFunc_Basic(const std::string& name, const std::string& value = "");
std::auto_ptr<OutputFunc> MakeOutputFunc_IValues(const std::string& name);
std::auto_ptr<OutputFunc> MakeOutputFunc_Winner();
std::auto_ptr<OutputFunc> MakeOutputFunc_Sequence();
std::auto_ptr<OutputFunc> MakeOutputFunc_Cap(std::auto_ptr<OutputFunc> outputFunc, const std::string& cap);

struct OutputState {
  typedef std::set<const IList*> onode_set;
  typedef onode_set::const_iterator onode_iter;
  typedef std::set<const STree*> child_set;
  typedef child_set::const_iterator child_iter;
  onode_set onodes;
  child_set children;
  std::string value;
};

class OutputFunc {
public:
  OutputFunc();
  virtual ~OutputFunc() {}

  virtual void Init(const STree& x) { m_node = &x; }

  const OutputState& GetState() { return m_state; }
  IList* OStart() { return m_ostart; }
  IList* OEnd() { return m_oend; }

  virtual void operator() () = 0;
  virtual void ConnectONodes() {}
  virtual std::auto_ptr<OutputFunc> Clone() = 0;

protected:
  const STree* m_node;
  OutputState m_state;
  IList* m_ostart;    // first emittable olist node that is spanned
  IList* m_oend;      // last emittable olist node that is spanned
};

class OutputFunc_Silent : public OutputFunc {
public:
  OutputFunc_Silent()
    : OutputFunc() {}
  virtual ~OutputFunc_Silent() {}
  virtual void operator() () {}
  virtual std::auto_ptr<OutputFunc> Clone();
};

// Simple pass through the child node's output, no changes
class OutputFunc_Pass : public OutputFunc {
public:
  OutputFunc_Pass()
    : OutputFunc() {}
  virtual ~OutputFunc_Pass() {}
  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone();
};

// Emits a single output list node, that contains the provided name and value.
class OutputFunc_Basic : public OutputFunc {
public:
  OutputFunc_Basic(const std::string& name, const std::string& value = "");
  virtual ~OutputFunc_Basic() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone();

protected:
  IList m_onode;    // Single output list node
};

// Single output node with the provided name.  The value is the concatenation
// of all of the input node values.
class OutputFunc_IValues : public OutputFunc {
public:
  OutputFunc_IValues(const std::string& name);
  virtual ~OutputFunc_IValues() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone();

protected:
  IList m_onode;    // Single output list node
};

// Outputs all the nodes from the single child that is a "winner".  Corresponds
// with the Or rule.
class OutputFunc_Winner : public OutputFunc {
public:
  OutputFunc_Winner();
  virtual ~OutputFunc_Winner() {}

  virtual void operator() ();
  virtual void ConnectONodes();
  virtual std::auto_ptr<OutputFunc> Clone();

private:
  const STree* m_winner;
};

// Outputs all the nodes from the children that are emitting, in child order.
class OutputFunc_Sequence : public OutputFunc {
public:
  OutputFunc_Sequence()
    : OutputFunc() {}
  virtual ~OutputFunc_Sequence() {}

  virtual void operator() ();
  virtual void ConnectONodes();
  virtual std::auto_ptr<OutputFunc> Clone();
};

// Wraps an OutputFunc with an extra output node at the start and end.
class OutputFunc_Cap : public OutputFunc {
public:
  OutputFunc_Cap(std::auto_ptr<OutputFunc> outputFunc, const std::string& cap);
  virtual ~OutputFunc_Cap() {}

  virtual void Init(const STree& x);
  virtual void operator() ();
  virtual void ConnectONodes();
  virtual std::auto_ptr<OutputFunc> Clone();

protected:
  std::auto_ptr<OutputFunc> m_outputFunc;
  std::string m_cap;
  IList m_capStart;
  IList m_capEnd;
};

}

#endif // _statik_OutputFunc_h_
