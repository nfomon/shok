// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _WindowResponse_h_
#define _WindowResponse_h_

#include "WindowAction.h"

#include "statik/Hotlist.h"

#include <boost/ptr_container/ptr_vector.hpp>

namespace istatik {

struct WindowResponse {
  WindowResponse()
    : hotlist(NULL) {}
  typedef boost::ptr_vector<WindowAction> action_vec;
  typedef action_vec::const_iterator action_iter;
  action_vec actions;
  const statik::Hotlist* hotlist;
};

}

#endif // _WindowResponse_h_
