// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Hotlist_h_
#define _Hotlist_h_

/* A set of IList nodes that are "hot", meaning their done-ness has changed.
 * They can be Insert()ed or Delete()d on the next Connector.
 */

#include <set>
#include <utility>

namespace fw {

struct IList;

enum HOT_OP {
  OP_INSERT,
  OP_DELETE
};

typedef std::pair<const IList*, HOT_OP> hotpair;

typedef std::set<hotpair> Hotlist;
typedef Hotlist::const_iterator Hotlist_iter;

}

#endif // _Hotlist_h_
