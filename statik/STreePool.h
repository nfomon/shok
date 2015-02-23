// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _STreePool_h_
#define _STreePool_h_

#include <set>
#include <memory>

namespace statik {

class STree;

class STreePool {
public:
  ~STreePool();

  STree* Insert(std::auto_ptr<STree> node);
  void Unlink(STree& node);
  void Cleanup();

private:
  typedef std::set<STree*> node_set;
  typedef node_set::iterator node_mod_iter;
  node_set m_active;
  node_set m_unlinked;
};

}

#endif // _STreePool_h_
