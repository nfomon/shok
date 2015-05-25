// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_IConnection_h_
#define _statik_IConnection_h_

#include "List.h"

namespace statik {

class IConnection {
public:
  IConnection();

  void Clear();
  bool IsClear() const;

  const List& Start() const;
  const List& End() const;

  void Restart(const List& istart, bool total = true);
  void SetEnd(const List& iend);

private:
  const List* m_istart;  // starting inode
  const List* m_iend;    // inode that makes us bad after we're done, or last
};

}

#endif // _statik_IConnection_h_
