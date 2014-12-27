// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IConnection_h_
#define _IConnection_h_

#include "IList.h"

namespace fw {

struct IConnection {
  IConnection()
    : istart(NULL),
      iend(NULL),
      size(0) {}
  const IList* istart;  // starting inode
  const IList* iend;    // inode that makes us bad after we're done; NULL => eoi
  size_t size;    // number of inodes that make us good (count: iend - istart)

  void Clear() {
    istart = NULL;
    iend = NULL;
    size = 0;
  }
};

}

#endif // _IConnection_h_
