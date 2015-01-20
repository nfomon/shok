// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ListenerTable_h_
#define _ListenerTable_h_

#include <boost/function.hpp>

#include <map>
#include <set>

namespace statik {

template <typename T1, typename T2>
class ListenerTable {
public:
  typedef typename std::set<T2> listener_set;
  typedef typename listener_set::const_iterator listener_iter;

  ListenerTable() {}

  void AddListener(const T1& t1, const T2& t2) {
    {
      t1_mod_iter i = m_t1_map.find(t1);
      if (m_t1_map.end() == i) {
        std::pair<t1_mod_iter, bool> ins = m_t1_map.insert(std::make_pair(t1, listener_set()));
        ins.first->second.insert(t2);
      } else {
        i->second.insert(t2);
      }
    } {
      t2_mod_iter i = m_t2_map.find(t2);
      if (m_t2_map.end() == i) {
        std::pair<t2_mod_iter, bool> ins = m_t2_map.insert(std::make_pair(t2, t1_set()));
        ins.first->second.insert(t1);
      } else {
        i->second.insert(t1);
      }
    }
  }

  listener_set GetListeners(const T1& t1) const {
    t1_iter i = m_t1_map.find(t1);
    if (m_t1_map.end() == i) {
      return listener_set();
    }
    return i->second;
  }

  bool HasAnyListeners(const T1& t1) const {
    return m_t1_map.find(t1) != m_t1_map.end();
  }

  bool IsListening(const T1& t1, const T2& t2) const {
    t1_iter i = m_t1_map.find(t1);
    if (i != m_t1_map.end()) {
      const listener_set& listeners = i->second;
      listener_iter j = listeners.find(t2);
      if (j != listeners.end()) {
        return true;
      }
    }
    return false;
  }

  void RemoveListener(const T1& t1, const T2& t2) {
    {
      t1_mod_iter i = m_t1_map.find(t1);
      if (m_t1_map.end() == i) {
        throw SError("Cannot remove listener; no listeners for this item");
      }
      listener_set& listeners = i->second;
      listener_iter j = listeners.find(t2);
      if (listeners.end() == j) {
        throw SError("Cannot remove listener; output item not listening to this input item");
      }
      listeners.erase(t2);
    } {
      t2_mod_iter i = m_t2_map.find(t2);
      if (m_t2_map.end() == i) {
        throw SError("Cannot remove listener; somehow not listening to this item");
      }
      t1_set& t1s = i->second;
      t1_set_iter j = t1s.find(t1);
      if (t1s.end() == j) {
        throw SError("Cannot remove listener; input item not being listened by this output item");
      }
      t1s.erase(t1);
    }
  }

  // These return true if any were removed
  bool RemoveAllListeners(const T1& t1) {
    t1_mod_iter i = m_t1_map.find(t1);
    if (m_t1_map.end() == i || i->second.empty()) {
      return false;
    }
    const listener_set& listeners = i->second;
    for (listener_iter j = listeners.begin(); j != listeners.end(); ++j) {
      RemoveListener(t1, *j);
    }
    m_t1_map.erase(i);
    return true;
  }

  bool RemoveAllListenings(const T2& t2) {
    t2_mod_iter i = m_t2_map.find(t2);
    if (m_t2_map.end() == i) {
      return false;
    }
    const t1_set& t1s = i->second;
    for (t1_set_iter j = t1s.begin(); j != t1s.end(); ++j) {
      RemoveListener(*j, t2);
    }
    m_t2_map.erase(i);
    return true;
  }

private:
  // map<T1, set<T2> >
  typedef typename std::map<T1, listener_set> t1_map;
  typedef typename t1_map::const_iterator t1_iter;
  typedef typename t1_map::iterator t1_mod_iter;
  t1_map m_t1_map;

  // map<T2, set<T1> >
  typedef typename std::set<T1> t1_set;
  typedef typename t1_set::const_iterator t1_set_iter;
  typedef typename std::map<T2, t1_set> t2_map;
  typedef typename t2_map::const_iterator t2_iter;
  typedef typename t2_map::iterator t2_mod_iter;
  t2_map m_t2_map;
};

}

#endif // _ListenerTable_h_
