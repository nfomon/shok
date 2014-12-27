// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "FWTree.h"
#include "Hotlist.h"
#include "IList.h"
#include "ListenerTable.h"
#include "Rule.h"

#include "util/Log.h"

#include <map>
#include <set>

namespace fw {

class Grapher;

class Connector {
public:
  typedef typename ListenerTable<const IList*, FWTree*>::listener_set listener_set;
  typedef typename ListenerTable<const IList*, FWTree*>::listener_iter listener_iter;

  Connector(Log& log, const Rule& rule, const std::string& name = "", Grapher* grapher = NULL);

  const FWTree& GetRoot() const { return m_root; }
  const Hotlist::hotlist_vec& GetHotlist() const { return m_root.GetOutputStrategy().GetHotlist(); }
  std::string PrintHotlist() const { return m_root.GetOutputStrategy().PrintHotlist(); }
  listener_set GetListeners(const IList& x) const {
    return m_listeners.GetListeners(&x);
  }

  // These return the common ancestor of all nodes that were changed, if any.
  // Insert() a new inode, AFTER attaching its connections in the input list.
  void Insert(const IList& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const IList& inode);

  void UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist);

  // Reposition or Update a single node.  Either called internally by the
  // Connector or by a rule that wants to process a relative.

  // Set the "starting" inode.  The rule's Reposition() may RepositionNode() on
  // its children.
  void RepositionNode(FWTree& x, const IList& inode);

  // Recalculate state based on a change to its children
  bool UpdateNode(FWTree& x);

  void ClearNode(FWTree& x);

  // Called from a rule/state regarding its DS node.
  // Listens for updates to this inode.
  void Listen(FWTree& x, const IList& inode);
  void Unlisten(FWTree& x, const IList& inode);

  // Called from a rule to tell us that a node was updated and thus needs to
  // have its OutputStrategies reset once the whole Tree is done updating
  void AddNodeToReset(FWTree& x);

private:
  // Convenience core for Insert() and Delete().  Updates all listeners to the
  // left (and if any and distinct, to the right) of the inode, about the
  // inode.  For FWTree, updates all listeners of the parent of the inode.
  void UpdateListeners(const IList& inode, bool updateNeighbourListeners);
  void DrawGraph(const FWTree& onode, const IList* inode = NULL);

  void ResetNodes();    // Reset any nodes-to-reset once a Tree update is done

  Log& m_log;
  // Root of the output tree.  Tells us the root of the rule tree.
  FWTree m_root;
  std::string m_name;
  Grapher* m_grapher;
  const IList* m_istart;    // Start of the input list
  ListenerTable<const IList*, FWTree*> m_listeners;
  std::set<FWTree*> m_nodesToReset;
};

}

#endif // _Connector_h_
