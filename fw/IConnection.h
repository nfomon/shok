// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IConnection_h_
#define _IConnection_h_

#include "IList.h"

#include "FWError.h"

namespace fw {

class IConnection {
public:
  IConnection()
    : m_istart(NULL),
      m_iend(NULL) {}

  const IList& Start() const {
    if (!m_istart) {
      throw FWError("Cannot get start of uninitialized IConnection");
    }
    return *m_istart;
  }

  const IList& End() const {
    if (!m_iend) {
      throw FWError("Cannot get end of uninitialized IConnection");
    }
    return *m_iend;
  }

  void Restart(const IList& istart) {
    m_istart = &istart;
    m_iend = &istart;
  }
  void SetEnd(const IList& iend) { m_iend = &iend; }

private:
  const IList* m_istart;  // starting inode
  const IList* m_iend;    // inode that makes us bad after we're done, or last
};

}

#endif // _IConnection_h_
