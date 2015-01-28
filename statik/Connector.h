// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "Hotlist.h"
#include "IList.h"
#include "ListenerTable.h"

#include <map>
#include <set>

namespace statik {

class Grapher;
class Rule;
class STree;

class Connector {
public:
  typedef typename ListenerTable<const IList*, STree*>::listener_set listener_set;
  typedef typename ListenerTable<const IList*, STree*>::listener_iter listener_iter;

  Connector(const Rule& rule, const std::string& name = "", const std::string& graphdir = "");

  const Hotlist& GetHotlist() const;
  void ClearHotlist();
  listener_set GetListeners(const IList& x) const {
    return m_listeners.GetListeners(&x);
  }

  // Insert() a new inode.  Call this AFTER attaching its connections in the
  // input list.
  void Insert(const IList& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const IList& inode);

  // Apply a bunch of inode insertions/updates/deletions
  void UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist);

  // Called from a rule/state regarding its DS node.
  // Listens for updates to this inode.
  void Listen(STree& x, const IList& inode);
  void Unlisten(STree& x, const IList& inode);

  // Remove any listening connections regarding a node
  void ClearNode(STree& x);

  void DrawGraph(const STree& onode, const IList* inode = NULL);

private:
  const STree& GetRoot() const;

  // Convenience core for Insert() and Delete().  Updates all listeners to the
  // left (and if any and distinct, to the right) of the inode, about the
  // inode.  For STree, updates all listeners of the parent of the inode.
  void UpdateListeners(const IList& inode, bool updateNeighbourListeners);

  // Root of the Rule tree
  const Rule& m_rule;
  // Root of the output tree
  std::auto_ptr<STree> m_root;
  std::string m_name;
  std::auto_ptr<Grapher> m_grapher;
  ListenerTable<const IList*, STree*> m_listeners;
  Hotlist m_hotlist;
};

}

#endif // _Connector_h_