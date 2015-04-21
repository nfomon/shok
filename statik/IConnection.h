// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IConnection_h_
#define _IConnection_h_

#include "IList.h"
#include "SError.h"

namespace statik {

class IConnection {
public:
  IConnection()
    : m_istart(NULL),
      m_iend(NULL),
      m_size(0) {}

  void Clear() {
    m_istart = NULL;
    m_iend = NULL;
    m_size = 0;
  }
  bool IsClear() const { return !m_istart; }

  const IList& Start() const {
    if (!m_istart) {
      throw SError("Cannot get start of uninitialized IConnection");
    }
    return *m_istart;
  }

  const IList& End() const {
    if (!m_iend) {
      throw SError("Cannot get end of uninitialized IConnection");
    }
    return *m_iend;
  }

  size_t Size() const { return m_size; }

  void Restart(const IList& istart) {
    m_istart = &istart;
    m_iend = &istart;
    m_size = 0;
  }

  void SetEnd(const IList& iend, size_t size) {
    if (!m_istart) {
      throw SError("Cannot set end of IConnection that has not been started");
    }
    m_iend = &iend;
    m_size = size;
  }

private:
  const IList* m_istart;  // starting inode
  const IList* m_iend;    // inode that makes us bad after we're done, or last
  size_t m_size;          // number of inodes that are spanned, including IEnd if possible
};

}

#endif // _IConnection_h_
