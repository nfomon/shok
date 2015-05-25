// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_IConnection_h_
#define _statik_IConnection_h_

#include "IList.h"

namespace statik {

class IConnection {
public:
  IConnection();

  void Clear();
  bool IsClear() const;

  const IList& Start() const;
  const IList& End() const;

  void Restart(const IList& istart, bool total = true);
  void SetEnd(const IList& iend);

private:
  const IList* m_istart;  // starting inode
  const IList* m_iend;    // inode that makes us bad after we're done, or last
};

}

#endif // _statik_IConnection_h_
