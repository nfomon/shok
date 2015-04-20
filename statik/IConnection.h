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
      m_size(0),
      m_tentative(NULL) {}

  void Clear() {
    m_istart = NULL;
    m_iend = NULL;
    m_size = 0;
    m_tentative = NULL;
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

  const IList* TentativeStart() const { return m_tentative; }

  void Restart(const IList& istart) {
    m_tentative = &istart;
    m_istart = NULL;
    m_iend = NULL;
    m_size = 0;
  }

  void ConfirmStart() {
    if (!IsClear()) {
      throw SError("Cannot confirm start of IConnection that is not clear");
    } else if (!m_tentative) {
      throw SError("Cannot confirm start of IConnection that has no tentative start");
    }
    m_istart = m_tentative;
    m_iend = m_tentative;
    m_size = 1;
    m_tentative = NULL;
  }

  void SetEnd(const IList& iend, size_t size) {
    if (m_tentative || !m_istart) {
      throw SError("Cannot set end of IConnection that does not have a confirmed start");
    }
    m_iend = &iend;
    m_size = size;
  }

private:
  const IList* m_istart;  // starting inode
  const IList* m_iend;    // inode that makes us bad after we're done, or last
  size_t m_size;          // number of inodes that are spanned, including IEnd if possible
  const IList* m_tentative;  // tentative starting inode
};

}

#endif // _IConnection_h_
