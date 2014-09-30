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

template <>
void Connector<ListDS>::Insert(const ListDS& inode) {
  m_log.info("Inserting list inode " + string(inode));

  if (inode.left) {
    UpdateListeners(*inode.left);
  } else {
    // Just reposition the root.
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
}

template <>
void Connector<TreeDS>::Insert(const TreeDS& inode) {
  m_log.info("Inserting tree inode " + string(inode));

  // If we are the leftmost child of any ancestor, reposition that ancestor.
  // Oldest ancestor possible, going up to the root.
  // TODO FIXME...
  const TreeDS* child = &inode;
  const TreeDS* ancestor = child->parent;
  while (ancestor && child == &ancestor->children.front()) {
    child = ancestor;
    ancestor = child->parent;
  }
  if (ancestor == NULL) {
  }

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    State& state = m_root.GetState();
    state.ok = false;
    state.bad = true;
    state.done = false;
  }
  // Stronger check: every inode has at least one listener :)
}

template <>
void Connector<ListDS>::Delete(const ListDS& inode) {
  m_log.info("Deleting list inode " + string(inode));

  m_listeners.RemoveAllListeners(&inode);

  if (!inode.left && !inode.right) {
    ClearNode(m_root);
  } else {
    UpdateListeners(inode);
  }
}

template <>
void Connector<TreeDS>::Delete(const TreeDS& inode) {
  // TODO
  throw FWError("Connector<TreeDS>::Delete(const TreeDS&) is unimplemented");
}

/* private */

template <>
void Connector<ListDS>::UpdateListeners(const ListDS& inode) {
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
      bool changed = UpdateNode(*i->first, i->second);
      if (changed) {
        //m_log.debug(string(" - ") + string(i->node) + " changed; update its parent?");
        TreeDS* parent = i->first->parent;
        if (parent) {
          //m_log.debug(" - - yes!");
          changes_by_depth[parent->depth].push_back(make_pair(parent, i->first));
        }
      }
    }
    changes_by_depth.erase(depth);
  }

/*
    pair<depth_iter, depth_iter> iters = listeners.equal_range(*listeners.begin());
    for (depth_iter i = iters.first; i != iters.second; ++i) {
      bool changed = UpdateNode(**i, inode);
      if (changed) {
        //m_log.debug(string(" - ") + string(**i) + " changed; update its parent?");
        TreeDS* parent = (*i)->parent;
        if (parent) {
          //m_log.debug(" - - yes!");
          // Instead of receiving the inode, these might want to receive which
          // of their children have changed.  Note that OrRule::Update and Keyword::Update both ignore the inode, currently.  Maybe they do just want "which child caused this Update(), or NULL if we got this Update via subscription".  But note that multiple children may have changed, and these should I guess cause multiple Update() calls, which is ok.
          // So Reposition() takes an inode, whereas Update() takes a child.  Makes sense I think!!
          // Is it only leaves that ever subscribe?  We might want to re-think the whole flow, and separate leaves from bodynodes.
          listeners.insert(parent);
        }
      }
    }
    listeners.erase(iters.first, iters.second);
*/
}

template <>
void Connector<TreeDS>::UpdateListeners(const TreeDS& inode) {
  // TODO
  throw FWError("Connector<TreeDS>::UpdateListeners(const TreeDS&) is unimplemented");
}
