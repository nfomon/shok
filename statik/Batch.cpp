// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Batch.h"

#include "List.h"
#include "SLog.h"

#include <ostream>
#include <string>
using std::ostream;
using std::string;

using namespace statik;

/* BatchItem public */

std::string Batch::BatchItem::Print() const {
  if (!node) {
    throw SError("Cannot print defective BatchItem");
  }
  string s(UnMapBatchOp(op) + " " + string(*node));
  if (pos) {
    s += " @ " + string(*pos);
  }
  return s;
}

/* Batch public */

std::string Batch::UnMapBatchOp(const BATCH_OP& op) {
  switch (op) {
  case OP_INSERT: return "OP_INSERT";
  case OP_UPDATE: return "OP_UPDATE";
  case OP_DELETE: return "OP_DELETE";
  default: throw SError("Cannot unmap unknown BATCH_OP");
  }
}

void Batch::Insert(const List& inode, const List* pos = NULL) {
  m_batch.push_back(BatchItem(inode, OP_INSERT, pos));
}

void Batch::Delete(const List& inode) {
  m_batch.push_back(BatchItem(inode, OP_DELETE));
}

void Batch::Update(const List& inode) {
  m_batch.push_back(BatchItem(inode, OP_UPDATE));
}

void Batch::Accept(const Batch& batch) {
  for (batch_iter i = batch.begin(); i != batch.end(); ++i) {
    m_batch.push_back(*i);
  }
}

void Batch::Clear() {
  g_log.debug() << "Batch: Clearing " << m_batch.size() << " items out of batch";
  m_batch.clear();
}

string Batch::Print() const {
  string s;
  for (batch_iter i = m_batch.begin(); i != m_batch.end(); ++i) {
    s += "\n" + i->Print();
  }
  return s;
}

/* non-member */

ostream& statik::operator<< (ostream& out, const Batch::BatchItem& item) {
  out << item.Print();
  return out;
}

ostream& statik::operator<< (ostream& out, const Batch& batch) {
  out << batch.Print();
  return out;
}
