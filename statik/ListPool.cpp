// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ListPool.h"

#include "SError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

ListPool::~ListPool() {
  for (node_mod_iter i = m_active.begin(); i != m_active.end(); ++i) {
    delete *i;
  }
  for (node_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
    delete *i;
  }
}

IList* ListPool::Insert(auto_ptr<IList> inode) {
  IList* i = inode.get();
  m_active.insert(inode.release());
  return i;
}

void ListPool::Unlink(IList& inode) {
  if (m_active.end() == m_active.find(&inode)) {
    throw SError("Cannot unlink inode " + string(inode) + "; node not found in ListPool");
  }
  m_active.erase(&inode);
  m_unlinked.insert(&inode);
}

void ListPool::Cleanup() {
  for (node_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
    delete *i;
  }
  m_unlinked.clear();
}
