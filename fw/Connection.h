// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connection_h_
#define _Connection_h_

#include "Hotlist.h"
#include "IList.h"
#include "OData.h"
#include "Rule.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace fw {

struct IConnection {
  IConnection()
    : istart(NULL),
      iend(NULL),
      size(0) {}
  const IList* istart;  // starting inode
  const IList* iend;    // inode that makes us bad after we're done; NULL => eoi
  size_t size;    // number of inodes that make us good (count: iend - istart)

  void Clear() {
    istart = NULL;
    iend = NULL;
    size = 0;
  }
};

class OConnection {
public:
  typedef std::set<const IList*> emitting_vec;
  typedef emitting_vec::const_iterator emitting_iter;

  OConnection(Log& log, const FWTree& x);

  // Wipe out state
  virtual void Clear() = 0;

  IList* OStart() { return m_ostart; }
  IList* OEnd() { return m_oend; }
  const emitting_vec& Emitting() const { return m_emitting; }
  const emitting_vec& WasEmitting() const { return m_wasEmitting; }
  const Hotlist::hotlist_vec& GetHotlist() const { return m_hotlist.GetHotlist(); }

  // Clears the hotlist and flips the Emitting list as appropriate.
  // Called by the Connector between full-tree updates.
  virtual void Reset() {
    m_wasEmitting = m_emitting;
    m_hotlist.Clear();
  }

  std::string DrawOConnection(const std::string& context) const;

protected:
  void ApproveChild(const FWTree& child);
  void InsertChild(const FWTree& child);
  void DeleteChild(const FWTree& child);

  Log& m_log;
  const FWTree& m_node;
  IList* m_ostart;    // first emittable olist node that is spanned
  IList* m_oend;      // last emittable olist node that is spanned
  emitting_vec m_emitting;    // ONodes that we are currently happy to emit
  emitting_vec m_wasEmitting; // ONodes that we emit last tree-cycle
  Hotlist m_hotlist;  // active olist updates
};

class OConnectionSingle : public OConnection {
public:
  OConnectionSingle(Log& log, const FWTree& x);

  virtual void Clear() {}

  IList& GetONode() const;
  void ONodeUpdated();

  template <typename DataType>
  void ONodeUpdate() {
    GetONode().GetData<DataType>().Update();
    ONodeUpdated();
  }

  template <typename DataType, typename T1>
  void ONodeUpdate(T1& t1) {
    GetONode().GetData<DataType>().Update(t1);
    ONodeUpdated();
  }

private:
  std::auto_ptr<IList> m_onode;   // Single output list node
};

class OConnectionWinner : public OConnection {
public:
  OConnectionWinner(Log& log, const FWTree& x)
    : OConnection(log, x),
      m_winner(NULL) {}

  void Clear() {
    m_winner = NULL;
    m_ostart = NULL;
    m_oend = NULL;
  }
  virtual void Reset() {
    m_wasEmitting = m_emitting;
    m_emitting.clear();
    m_hotlist.Clear();
  }

  const FWTree* GetWinner() const { return m_winner; }

  // Or: Declare a specific child the full winner
  void DeclareWinner(const FWTree& winner);

private:
  const FWTree* m_winner;
};

class OConnectionSequence : public OConnection {
public:
  OConnectionSequence(Log& log, const FWTree& x)
    : OConnection(log, x) {}

  void Clear() {
    m_emitting.clear();
    m_ostart = NULL;
    m_oend = NULL;
  }
  virtual void Reset() {
    m_wasEmitting = m_emitting;
    m_emitting.clear();
    m_hotlist.Clear();
    m_ostart = NULL;
    m_oend = NULL;
  }

  // Sequence: Declare the first and last children
  void AddNextChild(const FWTree& child, bool isNew);
};

}

#endif // _Connection_h_
