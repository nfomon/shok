// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Changeset_h_
#define _Changeset_h_

/* A TreeChangeset tracks a set of tree changes that have occurred.  This is
 * used in the context of a Connector (updating output tree nodes based on
 * input node changes), where it tracks all the changes it is doing to its
 * output: what tree nodes have been added, changed, or removed.  This lets us
 * feed those updates as input to the next Connector.
 */

#include "State.h"
#include "FWError.h"

#include <vector>

namespace fw {

struct TreeChange {
  enum ChangeType {
    INSERT,
    UPDATE,
    DELETE
  };

  TreeChange(ChangeType type, const TreeDS& node)
    : type(type), node(&node) {}

  ChangeType type;
  const TreeDS* node;
};

class TreeChangeset {
public:
  typedef std::vector<TreeChange> change_vec;
  typedef change_vec::const_iterator change_iter;
  typedef std::map<TreeDS::depth_t, change_vec> changeset_map;
  typedef changeset_map::const_iterator changeset_iter;
  typedef changeset_map::const_reverse_iterator changeset_rev_iter;
  typedef changeset_map::iterator changeset_mod_iter;

  TreeChangeset() {}

  void AddTreeChange(const TreeChange& change) {
    if (!change.node) {
      throw FWError("Cannot add TreeChange to TreeChangeset with invalid node");
    }
    TreeDS::depth_t depth = change.node->depth;
    m_changeset[depth].push_back(change);
  }

  void Clear() { m_changeset.clear(); }

  const changeset_map& GetChangeset() const { return m_changeset; }

private:
  changeset_map m_changeset;
};

}

#endif // _Changeset_h_
