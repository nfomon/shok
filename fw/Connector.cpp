// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"

#include "DS.h"
#include "Rule.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <set>
#include <string>
#include <utility>
using std::make_pair;
using std::multiset;
using std::pair;
using std::string;

using namespace fw;

/* public */

void Connector::Insert(const IList& inode) {
  m_log.info("Connector: Inserting IList: " + string(inode));

  if (inode.left) {
    m_log.debug(" - Found left inode: updating listeners of " + string(*inode.left));
    UpdateListeners(*inode.left);
  } else {
    // Just reposition the root.
    m_log.debug(" - No left inode: just repositioning the root");
    RepositionNode(m_root, inode);
  }

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    State& state = m_root.GetState();
    state.ok = false;
    state.bad = true;
    state.done = false;
  }
  // Stronger check: every inode has at least one listener :)

  m_oconnection = m_root.oconnection;
  m_log.debug("Connector: Insert done, hotlist now has size " + boost::lexical_cast<string>(m_oconnection.hotlist.size()));
}

void Connector::Delete(const IList& inode) {
  m_log.info("Deleting list inode " + string(inode));

  m_listeners.RemoveAllListeners(&inode);

  if (!inode.left && !inode.right) {
    ClearNode(m_root);
  } else {
    UpdateListeners(inode);
  }

  m_oconnection = m_root.oconnection;
}

/* private */

void Connector::UpdateListeners(const IList& inode) {
  typedef std::pair<TreeDS*, const TreeDS*> change_pair;
  typedef std::vector<change_pair> change_vec;
  typedef change_vec::const_iterator change_iter;
  typedef std::map<TreeDS::depth_t, change_vec> change_map;
  change_map changes_by_depth;

  // TODO Unnecessary copy
  listener_set left_listeners = m_listeners.GetListeners(inode.left);
  for (listener_iter i = left_listeners.begin(); i != left_listeners.end(); ++i) {
    changes_by_depth[(*i)->depth].push_back(change_pair(*i, NULL));
  }

  // If inode.right and inode.right != inode.left, merge the left and right listeners, depth-ordered.
  if (inode.right && inode.right != inode.left) {
    // TODO Unnecessary copy
    listener_set right_listeners = m_listeners.GetListeners(inode.right);
    for (listener_iter i = right_listeners.begin(); i != right_listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].push_back(change_pair(*i, NULL));
    }
  }

  while (!changes_by_depth.empty()) {
    // Update the deepest depth of our changeset
    int depth = changes_by_depth.rbegin()->first;
    change_vec changes = changes_by_depth.rbegin()->second;
    m_log.debug("Connector: Updating changes at depth " + lexical_cast<string>(depth));
    for (change_iter i = changes.begin(); i != changes.end(); ++i) {
      // Clear all the nodes' hot lists at this level.  They can't do it in
      // Update() because they might have multiple child updates coming in.
      // TODO but eventually we'll trust them to clear their children, so that
      // the Connector only needs to clear the root when it takes from it.
      // (That might already be the case!)
      i->first->oconnection.hotlist.clear();
      bool changed = UpdateNode(*i->first, i->second);
      if (changed) {
        //m_log.debug(string(" - ") + string(*i->first) + " changed; update its parent?");
        TreeDS* parent = i->first->parent;
        if (parent) {
          //m_log.debug(" - - yes!");
          changes_by_depth[parent->depth].push_back(make_pair(parent, i->first));
        }
      }
    }
    changes_by_depth.erase(depth);
  }
}
