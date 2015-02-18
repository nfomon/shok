// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ListPool_h_
#define _ListPool_h_

#include "IList.h"

#include <set>
#include <memory>

namespace statik {

class ListPool {
public:
  ~ListPool();

  IList* Insert(std::auto_ptr<IList> inode);
  void Unlink(IList& inode);
  void Cleanup();

private:
  typedef std::set<IList*> node_set;
  typedef node_set::iterator node_mod_iter;
  node_set m_active;
  node_set m_unlinked;
};

}

#endif // _ListPool_h_
