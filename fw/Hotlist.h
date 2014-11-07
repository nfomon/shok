// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Hotlist_h_
#define _Hotlist_h_

/* A set of TreeDS leaf nodes that are "hot", meaning their done-ness has
 * changed.  They can be Insert()ed or Delete()d on the next Connector.
 */

#include <set>

namespace fw {

struct TreeDS;

typedef std::set<const TreeDS*> Hotlist;
typedef Hotlist::const_iterator Hotlist_iter;

}

#endif // _Hotlist_h_
