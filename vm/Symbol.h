// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Symbol_h_
#define _Symbol_h_

/* Symbol */

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include <ostream>
#include <string>

namespace vm {

class Object;

typedef boost::ptr_map<std::string,Object> symbol_map;
typedef symbol_map::const_iterator symbol_iter;
typedef symbol_map::iterator symbol_mod_iter;

inline std::ostream& operator<<(std::ostream& out, const symbol_map& smap) {
  out << "symbol map with " << boost::lexical_cast<std::string>(smap.size()) << " members:\n";
  for (symbol_iter i = smap.begin(); i != smap.end(); ++i) {
    out << " - " << i->first << "\n";
  }
  return out;
}

}

#endif // _Symbol_h_
