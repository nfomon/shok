// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ObjectPool_h_
#define _ObjectPool_h_

#include "SError.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <set>
#include <string>

namespace statik {

template <typename T>
class ObjectPool {
public:
  ~ObjectPool() {
    for (item_mod_iter i = m_active.begin(); i != m_active.end(); ++i) {
      delete *i;
    }
    for (item_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
      delete *i;
    }
  }

  T* Insert(std::auto_ptr<T> item) {
    T* i = item.get();
    m_active.insert(item.release());
    return i;
  }

  void Unlink(T& item) {
    if (m_active.end() == m_active.find(&item)) {
      throw SError("Cannot unlink " + std::string(item) + "; item not found in pool");
    }
    m_active.erase(&item);
    m_unlinked.insert(&item);
  }

  void Cleanup() {
    for (item_mod_iter i = m_unlinked.begin(); i != m_unlinked.end(); ++i) {
      g_log.debug() << "Object pool deleting " << **i << " - " << *i;
      g_san.debug() << "Object pool deleting " << **i << " - " << *i;
      delete *i;
    }
    m_unlinked.clear();
  }

  operator std::string() const {
    return std::string("Object pool has " + boost::lexical_cast<std::string>(m_active.size()) + " active and " + boost::lexical_cast<std::string>(m_unlinked.size()) + " unlinked");
  }

private:
  typedef std::set<T*> item_set;
  typedef typename item_set::iterator item_mod_iter;
  item_set m_active;
  item_set m_unlinked;
};

}

#endif // _ObjectPool_h_
