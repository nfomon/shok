// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_OutputFunc_h_
#define _statik_OutputFunc_h_

#include "List.h"

#include <list>
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

struct OutputItem {
  OutputItem(const List& onode)
    : onode(&onode),
      child(NULL) {}
  OutputItem(const STree& child)
    : onode(NULL),
      child(&child) {}
  bool operator==(const OutputItem& rhs) const {
    return onode == rhs.onode && child == rhs.child;
  }
  bool operator!=(const OutputItem& rhs) const {
    return !(*this==rhs);
  }
  bool operator<(const OutputItem& rhs) const {
    if (onode && rhs.onode) {
      return onode < rhs.onode;
    }
    if (child && rhs.child) {
      return child < rhs.child;
    }
    return !!child;
  }
  const List* onode;
  const STree* child;
  const List* ostart;     // set by IncParser
  const List* oend;       // set by IncParser
  std::string prev_value; // set/used by IncParser, not OutputFuncs
};

/*
class OutputList {
public:
  typedef output_list::const_iterator const_iterator;
  typedef output_list::iterator iterator;
  const_iterator begin() const { return m_output.begin(); }
  const_iterator end() const { return m_output.end(); }
  size_t size() const { return m_output.size(); }
  List* OStart() const {
    if (m_output.empty()) {
      return NULL;
    }
    OutputItem& start = m_output.front();
    if (start.onode) {
      return start.onode;
    }
    return start.child->GetOutputList().OStart();
  }
private:
  typedef std::list<OutputItem> output_list;
  output_list m_output;
};
*/
typedef std::list<OutputItem> OutputList;

class OutputFunc {
public:
  OutputFunc();
  virtual ~OutputFunc() {}

  virtual void Init(const STree& x) { m_node = &x; }

  OutputList& GetOutput() { return m_output; }
  const List* OStart() { return m_ostart; }
  const List* OEnd() { return m_oend; }

  // Compute new output state.  Called before any children.
  virtual void operator() () = 0;
  void Sync();    // Determine start/end.  Called after children are computed.
  virtual std::auto_ptr<OutputFunc> Clone() = 0;

protected:

  const STree* m_node;
  OutputList m_output;

private:
  const List* m_ostart; // first emittable olist node that is spanned
  const List* m_oend;   // last emittable olist node that is spanned
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
  List m_onode;    // Single output list node
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
  List m_onode;    // Single output list node
};

// Outputs all the nodes from the single child that is a "winner".  Corresponds
// with the Or rule.
class OutputFunc_Winner : public OutputFunc {
public:
  OutputFunc_Winner();
  virtual ~OutputFunc_Winner() {}

  virtual void operator() ();
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
  virtual std::auto_ptr<OutputFunc> Clone();
};

// Wraps an OutputFunc with an extra output node at the start and end.
class OutputFunc_Cap : public OutputFunc {
public:
  OutputFunc_Cap(std::auto_ptr<OutputFunc> outputFunc, const std::string& cap);
  virtual ~OutputFunc_Cap() {}

  virtual void Init(const STree& x);
  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone();

protected:
  std::auto_ptr<OutputFunc> m_outputFunc;
  std::string m_cap;
  List m_capStart;
  List m_capEnd;
};

}

#endif // _statik_OutputFunc_h_
