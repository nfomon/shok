// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object */

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/variant.hpp>

#include <memory>
#include <string>

namespace vm {

class Object;

typedef boost::ptr_map<std::string,Object> symbol_map;
typedef symbol_map::const_iterator symbol_iter;

class Object {
public:
  void assign(const std::string& name, std::auto_ptr<Object> value);

private:
  symbol_map m_members;
};

}

#endif // _Object_h_
