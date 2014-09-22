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
using std::multiset;
using std::pair;
using std::string;

using namespace fw;

/* public */

template <>
const TreeDS* Connector<ListDS>::Insert(const ListDS& inode) {
  m_log.info("Inserting inode " + string(inode));

  const TreeDS* ancestor = NULL;

  // If !inode.left, just reposition the root.
  if (!inode.left) {
    RepositionNode(m_root, inode);
    ancestor = &m_root;
  } else {
    ancestor = UpdateListeners(inode);
  }

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    State& state = m_root.GetState();
    state.ok = false;
    state.bad = true;
    state.done = false;
    ancestor = &m_root;
  }
  // Stronger check: every inode has at least one listener :)
  return ancestor;
}

template <>
const TreeDS* Connector<ListDS>::Delete(const ListDS& inode) {
  m_log.info("Deleting inode " + string(inode));

  m_listeners.RemoveAllListeners(&inode);

  const TreeDS* ancestor = NULL;
  if (!inode.left && !inode.right) {
    ClearNode(m_root);
    ancestor = &m_root;
  } else {
    ancestor = UpdateListeners(inode);
  }
  return ancestor;
}

template <>
const TreeDS* Connector<TreeDS>::Update(const TreeDS& inode) {
  // TODO
  throw FWError("Connector<TreeDS>::Update(const TreeDS&) is unimplemented");
}

/* private */

template <>
const TreeDS* Connector<ListDS>::UpdateListeners(const ListDS& inode) {
  typedef multiset<TreeDS*, TreeDSInverseDepthComparator> depth_set;
  typedef depth_set::const_iterator depth_iter;
  depth_set listeners;

  // TODO Unnecessary copy
  listener_set left_listeners = m_listeners.GetListeners(inode.left);
  listeners.insert(left_listeners.begin(), left_listeners.end());

  // If inode.right and inode.right != inode.left, merge the left and right listeners, depth-ordered.
  if (inode.right && inode.right != inode.left) {
    // TODO Unnecessary copy
    listener_set right_listeners = m_listeners.GetListeners(inode.right);
    listeners.insert(right_listeners.begin(), right_listeners.end());
  }

  const TreeDS* ancestor = NULL;
  while (!listeners.empty()) {
    // Update the deepest depth of our listeners set
    int depth = (*listeners.begin())->depth;
    m_log.debug("Connector: Updating listeners at depth " + lexical_cast<string>(depth));
    pair<depth_iter, depth_iter> iters = listeners.equal_range(*listeners.begin());
    for (depth_iter i = iters.first; i != iters.second; ++i) {
      bool changed = UpdateNode(**i, inode);
      if (changed) {
        if (!ancestor) {
          ancestor = *i;
        } else {
          const TreeDS* search = *i;
          while (search != ancestor) {
            if (search->depth > ancestor->depth) {
              search = search->parent;
            } else {
              ancestor = ancestor->parent;
            }
            if (!search || !ancestor) {
              throw FWError("Connector nodes did not share an ancestor");
            }
          }
        }
        //m_log.debug(string(" - ") + string(**i) + " changed; update its parent?");
        TreeDS* parent = (*i)->parent;
        if (parent) {
          //m_log.debug(" - - yes!");
          listeners.insert(parent);
        }
      }
    }
    listeners.erase(iters.first, iters.second);
  }
  return ancestor;
}

template <>
const TreeDS* Connector<TreeDS>::UpdateListeners(const TreeDS& inode) {
  // TODO
  throw FWError("Connector<TreeDS>::UpdateListeners(const TreeDS&) is unimplemented");
}
