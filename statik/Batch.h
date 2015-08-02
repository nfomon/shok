// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Batch_h_
#define _statik_Batch_h_

/* A batch of List node changes (insertions/updates/deletions).  This is the
 * output emit by an IncParser, describing the changes to the output list.  You
 * can also provide a batch of input changes to an IncParser; e.g. to drive one
 * IncParser from the ChangeBatch emit by another.
 *
 * The batch is ordered; the first item (change) should be applied before the
 * subsequent.  The nodes are wired (left/right pointers are set) as if they
 * were connected as a part of the list as it exists when their change would be
 * applied.  However, of course, their neighbours' left/right pointers will
 * reflect the old state of the list.
 */

#include "List.h"

#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace statik {

class Batch {
public:
  enum BATCH_OP {
    OP_INSERT,
    OP_UPDATE,
    OP_DELETE
  };

  static std::string UnMapBatchOp(const BATCH_OP& op);

  struct BatchItem {
    BatchItem(const List& node, BATCH_OP op, const List* pos = NULL)
      : node(&node), op(op), pos(pos) {}
    const List* node;
    BATCH_OP op;
    const List* pos;

    std::string Print() const;
  };

  typedef std::vector<BatchItem> batch_vec;
  typedef batch_vec::const_iterator batch_iter;

  batch_iter begin() const { return m_batch.begin(); }
  batch_iter end() const { return m_batch.end(); }
  size_t Size() const { return m_batch.size(); }
  bool IsEmpty() const { return m_batch.empty(); }

  void Insert(const List& inode, const List* pos);
  void Update(const List& inode);
  void Delete(const List& inode);
  void Accept(const Batch& batch);
  void Clear();

  std::string Print() const;

private:
  batch_vec m_batch;
};

std::ostream& operator<< (std::ostream& out, const Batch::BatchItem& item);
std::ostream& operator<< (std::ostream& out, const Batch& batch);

}

#endif // _statik_Batch_h_
