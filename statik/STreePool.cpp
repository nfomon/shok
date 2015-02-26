// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "STreePool.h"

#include "SError.h"
#include "STree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

STreePool::~STreePool() {
  for (node_mod_iter i = m_active.begin(); i != m_active.end(); ++i) {
    delete *i;
  }
  for (node_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
    delete *i;
  }
}

STree* STreePool::Insert(auto_ptr<STree> node) {
  g_log.info() << "STreePool: Inserting " << *node;
  STree* i = node.get();
  m_active.insert(node.release());
  return i;
}

void STreePool::Unlink(STree& node) {
  g_log.info() << "STreePool: Unlinking " << node;
  if (m_active.end() == m_active.find(&node)) {
    throw SError("Cannot unlink node " + string(node) + "; node not found in STreePool");
  }
  m_active.erase(&node);
  m_unlinked.insert(&node);
}

void STreePool::Cleanup() {
  if (m_unlinked.size() > 0) {
    g_log.info() << "STreePool: Cleaning up " << m_unlinked.size() << " items";
  } else {
    g_log.debug() << "STreePool: No items to clean.";
  }
  for (node_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
    g_log.debug() << " - cleaning out " << **i;
    delete *i;
  }
  m_unlinked.clear();
}
