// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OutputStrategy_h_
#define _OutputStrategy_h_

#include "Hotlist.h"
#include "IList.h"

#include "util/Log.h"

#include <memory>
#include <set>
#include <string>

namespace fw {

class FWTree;

enum OutputStrategyType {
  OS_SILENT,
  OS_SINGLE,
  OS_VALUE,
  OS_WINNER,
  OS_SEQUENCE
}; 

class OutputStrategy {
public:
  typedef std::set<const IList*> emitting_set;
  typedef emitting_set::const_iterator emitting_iter;

  OutputStrategy(Log& log, const FWTree& x);
  virtual ~OutputStrategy() {}

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

class OutputStrategySingle : public OutputStrategy {
public:
  OutputStrategySingle(Log& log, const FWTree& x);
  virtual ~OutputStrategySingle() {}

  virtual void Clear() {}
  virtual void Update();

protected:
  IList m_onode;   // Single output list node
};

class OutputStrategyValue : public OutputStrategySingle {
public:
  OutputStrategyValue(Log& log, const FWTree& x)
    : OutputStrategySingle(log, x) {}
  virtual ~OutputStrategyValue() {}

  virtual void Update();
};

class OutputStrategyWinner : public OutputStrategy {
public:
  OutputStrategyWinner(Log& log, const FWTree& x)
    : OutputStrategy(log, x),
      m_winner(NULL) {}
  virtual ~OutputStrategyWinner() {}

  virtual void Clear();
  virtual void Reset();
  virtual void Update();

private:
  const FWTree* m_winner;
};

class OutputStrategySequence : public OutputStrategy {
public:
  OutputStrategySequence(Log& log, const FWTree& x)
    : OutputStrategy(log, x) {}
  virtual ~OutputStrategySequence() {}

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

#endif // _OutputStrategy_h_
