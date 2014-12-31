// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OutputFunc_h_
#define _OutputFunc_h_

#include "Hotlist.h"
#include "IList.h"

#include "util/Log.h"

#include <memory>
#include <set>
#include <string>

namespace fw {

class FWTree;

enum OutputFuncType {
  OS_SILENT,
  OS_SINGLE,
  OS_VALUE,
  OS_WINNER,
  OS_SEQUENCE
}; 

class OutputFunc {
public:
  typedef std::set<const IList*> emitting_set;
  typedef emitting_set::const_iterator emitting_iter;

  OutputFunc(Log& log, const FWTree& x);
  virtual ~OutputFunc() {}

  IList* OStart() { return m_ostart; }
  IList* OEnd() { return m_oend; }
  const emitting_set& Emitting() const { return m_emitting; }
  const emitting_set& WasEmitting() const { return m_wasEmitting; }
  const Hotlist::hotlist_vec& GetHotlist() const { return m_hotlist.GetHotlist(); }

  // Wipe out state
  virtual void Clear() = 0;

  // Clears the hotlist and flips the Emitting set as appropriate.
  // Called by the Connector between full-tree updates.
  virtual void Reset() {
    m_wasEmitting = m_emitting;
    m_hotlist.Clear();
  }

  virtual void Update() = 0;

  //std::string DrawEmitting(const std::string& context) const;
  std::string PrintHotlist() const { return m_hotlist.Print(); }

protected:
  // Convenience methods that can be called by Update() to use child OLists to
  // produce this node's OList.
  void ApproveChild(const FWTree& child);
  void InsertChild(const FWTree& child);
  void DeleteChild(const FWTree& child);

  Log& m_log;
  const FWTree& m_node;
  IList* m_ostart;    // first emittable olist node that is spanned
  IList* m_oend;      // last emittable olist node that is spanned
  emitting_set m_emitting;    // ONodes that we are currently happy to emit
  emitting_set m_wasEmitting; // ONodes that we emit last tree-cycle
  Hotlist m_hotlist;  // active olist updates
};

class OutputFuncSingle : public OutputFunc {
public:
  OutputFuncSingle(Log& log, const FWTree& x);
  virtual ~OutputFuncSingle() {}

  virtual void Clear() {}
  virtual void Update();

protected:
  IList m_onode;   // Single output list node
};

class OutputFuncValue : public OutputFuncSingle {
public:
  OutputFuncValue(Log& log, const FWTree& x)
    : OutputFuncSingle(log, x) {}
  virtual ~OutputFuncValue() {}

  virtual void Update();
};

class OutputFuncWinner : public OutputFunc {
public:
  OutputFuncWinner(Log& log, const FWTree& x)
    : OutputFunc(log, x),
      m_winner(NULL) {}
  virtual ~OutputFuncWinner() {}

  virtual void Clear();
  virtual void Reset();
  virtual void Update();

private:
  const FWTree* m_winner;
};

class OutputFuncSequence : public OutputFunc {
public:
  OutputFuncSequence(Log& log, const FWTree& x)
    : OutputFunc(log, x) {}
  virtual ~OutputFuncSequence() {}

  virtual void Clear();
  virtual void Reset();
  virtual void Update();

private:
  typedef std::set<const FWTree*> emitchildren_set;
  typedef emitchildren_set::const_iterator emitchildren_iter;
  typedef emitchildren_set::iterator emitchildren_mod_iter;
  emitchildren_set m_emitChildren;
};

}

#endif // _OutputFunc_h_
