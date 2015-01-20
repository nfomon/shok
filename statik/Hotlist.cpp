// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Hotlist.h"

#include "IList.h"

#include <string>
using std::string;

using namespace statik;

std::string Hotlist::UnMapHotOp(const HOT_OP& hotop) {
  switch (hotop) {
  case OP_INSERT: return "OP_INSERT";
  case OP_UPDATE: return "OP_UPDATE";
  case OP_DELETE: return "OP_DELETE";
  default: throw SError("Cannot unmap unknown HOT_OP");
  }
}

void Hotlist::Insert(const IList& inode) {
  m_hotlist.push_back(std::make_pair(&inode, OP_INSERT));
  m_inserted.insert(&inode);
}

void Hotlist::Delete(const IList& inode) {
  m_hotlist.push_back(std::make_pair(&inode, OP_DELETE));
  m_deleted.insert(&inode);
}

void Hotlist::Update(const IList& inode) {
  m_hotlist.push_back(std::make_pair(&inode, OP_UPDATE));
}

void Hotlist::Accept(const hotlist_vec& hotlist) {
  for (hotlist_iter h = hotlist.begin(); h != hotlist.end(); ++h) {
    m_hotlist.push_back(*h);
    switch (h->second) {
    case OP_INSERT: m_inserted.insert(h->first); break;
    case OP_DELETE: m_deleted.insert(h->first); break;
    default: break;
    }
  }
}

void Hotlist::Clear() {
  m_hotlist.clear();
  m_inserted.clear();
  m_deleted.clear();
}

string Hotlist::Print() const {
  string s;
  for (hotlist_iter i = m_hotlist.begin(); i != m_hotlist.end(); ++i) {
    s += "\n" + UnMapHotOp(i->second) + " - " + string(*i->first);
  }
  return s;
}
