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
class OutputFunc;

std::auto_ptr<OutputFunc> MakeOutputFunc_Single(Log& log, const std::string& name);
std::auto_ptr<OutputFunc> MakeOutputFunc_Value(Log& log, const std::string& name);
std::auto_ptr<OutputFunc> MakeOutputFunc_Winner(Log& log);
std::auto_ptr<OutputFunc> MakeOutputFunc_Sequence(Log& log);
std::auto_ptr<OutputFunc> MakeOutputFunc_Winner_Cap(Log& log, const std::string& cap);
std::auto_ptr<OutputFunc> MakeOutputFunc_Sequence_Cap(Log& log, const std::string& cap);

class OutputFunc {
public:
  typedef std::set<const IList*> emitting_set;
  typedef emitting_set::const_iterator emitting_iter;

  OutputFunc(Log& log);
  virtual ~OutputFunc() {}

  virtual void Init(const FWTree& x) { m_node = &x; }

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

  Log& m_log;
  const FWTree* m_node;
  IList* m_ostart;    // first emittable olist node that is spanned
  IList* m_oend;      // last emittable olist node that is spanned
  emitting_set m_emitting;    // ONodes that we are currently happy to emit
  emitting_set m_wasEmitting; // ONodes that we emit last tree-cycle
  Hotlist m_hotlist;  // active olist updates
};

class OutputFunc_Single : public OutputFunc {
public:
  OutputFunc_Single(Log& log, const std::string& name);
  virtual ~OutputFunc_Single() {}

  virtual void Clear() {}
  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Single(m_log, m_onode.name));
  }

protected:
  IList m_onode;   // Single output list node
};

class OutputFunc_Value : public OutputFunc_Single {
public:
  OutputFunc_Value(Log& log, const std::string& name)
    : OutputFunc_Single(log, name) {}
  virtual ~OutputFunc_Value() {}

  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Value(m_log, m_onode.name));
  }
};

class OutputFunc_Winner : public OutputFunc {
public:
  OutputFunc_Winner(Log& log)
    : OutputFunc(log),
      m_winner(NULL) {}
  virtual ~OutputFunc_Winner() {}

  virtual void Clear();
  virtual void Reset();
  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Winner(m_log));
  }

private:
  const FWTree* m_winner;
};

class OutputFunc_Sequence : public OutputFunc {
public:
  OutputFunc_Sequence(Log& log)
    : OutputFunc(log) {}
  virtual ~OutputFunc_Sequence() {}

  virtual void Clear();
  virtual void Reset();
  virtual void operator() ();
  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Sequence(m_log));
  }

private:
  typedef std::set<const FWTree*> emitchildren_set;
  typedef emitchildren_set::const_iterator emitchildren_iter;
  typedef emitchildren_set::iterator emitchildren_mod_iter;
  emitchildren_set m_emitChildren;
};

template <typename OFT>
class OutputFunc_Cap : public OutputFunc {
public:
  OutputFunc_Cap(Log& log, const std::string& cap)
    : OutputFunc(log),
      m_cap(cap),
      m_capStart(cap),
      m_capEnd("/" + cap),
      m_of(log) {
    m_ostart = &m_capStart;
    m_capStart.right = &m_capEnd;
    m_capEnd.left = &m_capStart;
    m_oend = &m_capEnd;
    m_emitting.insert(&m_capStart);
    m_emitting.insert(&m_capEnd);
    m_hotlist.Insert(m_capStart);
    m_hotlist.Insert(m_capEnd);
  }

  virtual ~OutputFunc_Cap() {}

  virtual void Init(const FWTree& x) {
    m_node = &x;
    m_of.Init(x);
  }

  virtual void Clear() {
    m_of.Clear();
  }

  virtual void Reset() {
    m_of.Reset();
    m_wasEmitting = m_emitting;
    m_emitting.clear();
    m_hotlist.Clear();
    m_emitting.insert(&m_capStart);
    m_emitting.insert(&m_capEnd);
  }

  virtual void operator() () {
    m_of();
    if (m_of.OStart()) {
      m_capStart.right = m_of.OStart();
      m_of.OStart()->left = &m_capStart;
    }
    if (m_of.OEnd()) {
      m_of.OEnd()->right = &m_capEnd;
      m_capEnd.left = m_of.OEnd();
    }
    m_emitting = m_of.Emitting();
    m_emitting.insert(&m_capStart);
    m_emitting.insert(&m_capEnd);
    m_hotlist.Accept(m_of.GetHotlist());
  }

  virtual std::auto_ptr<OutputFunc> Clone() {
    return std::auto_ptr<OutputFunc>(new OutputFunc_Cap<OFT>(m_log, m_cap));
  }

protected:
  std::string m_cap;
  IList m_capStart;
  IList m_capEnd;
  OFT m_of;
};

}

#endif // _OutputFunc_h_
