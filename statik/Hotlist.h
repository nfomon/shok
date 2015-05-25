// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Hotlist_h_
#define _statik_Hotlist_h_

/* A set of IList nodes that are "hot", meaning their done-ness has changed.
 * They can be Insert()ed or Delete()d on the next Connector.
 */

#include <set>
#include <string>
#include <utility>
#include <vector>

namespace statik {

struct IList;

class Hotlist {
public:
  enum HOT_OP {
    OP_INSERT,
    OP_UPDATE,
    OP_DELETE
  };

  static std::string UnMapHotOp(const HOT_OP& hotop);

  typedef std::pair<const IList*, HOT_OP> hotpair;
  typedef std::vector<hotpair> hotlist_vec;
  typedef hotlist_vec::const_iterator hotlist_iter;
  typedef std::set<const IList*> hot_set;
  typedef hot_set::const_iterator hotset_iter;

  const hotlist_vec& GetHotlist() const { return m_hotlist; }
  const hot_set& GetInserted() const { return m_inserted; }
  const hot_set& GetDeleted() const { return m_deleted; }
  size_t Size() const { return m_hotlist.size(); }
  bool IsEmpty() const { return m_hotlist.empty(); }

  void Insert(const IList& inode);
  void Update(const IList& inode);
  void Delete(const IList& inode);
  void Accept(const hotlist_vec& hotlist);
  void Clear();

  std::string Print() const;

private:
  hotlist_vec m_hotlist;
  hot_set m_inserted;
  hot_set m_deleted;
};

}

#endif // _statik_Hotlist_h_
