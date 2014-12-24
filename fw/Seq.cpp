// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Seq.h"

#include "Connector.h"

#include "util/Graphviz.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

void SeqRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  m_log.debug("Repositioning Seq " + string(*this) + " at " + string(x) + " with " + string(inode));
  RepositionFirstChildOfNode(connector, x, inode);
  Update(connector, x);
}

void SeqRule::Update(Connector& connector, FWTree& x) const {
  m_log.debug("Updating Seq " + string(*this) + " at " + string(x));

  // Initialize state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  State& state = x.GetState();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " must have children");
  } else if (x.children.size() > m_children.size()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
  }

  // Iterate over node children, either existing or being created, so long as
  // our last child is complete.
  bool finished = false;
  unsigned int child_index = 0;
  FWTree::child_mod_iter child = x.children.begin();
  FWTree* prev_child = NULL;
  while (!finished) {
    if (child_index >= m_children.size()) {
      throw FWError("Seq " + string(*this) + " evaluated too many children beyond the rule");
    }
    bool isNewChild = false;
    if (child != x.children.end()) {
      // Existing child
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          m_log.info("Seq " + string(*this) + " child " + string(*child) + " needs to be repositioned to the prev child's end");
          connector.RepositionNode(*child, *prev_child->iconnection.iend);
        }
      }
    } else if (x.children.size() > m_children.size()) {
      throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
    } else if (child_index >= m_children.size()) {
      throw FWError("SeqRule::Update: child index >= Rule size");
    } else {
      // New child
      isNewChild = true;
      x.children.push_back(auto_ptr<FWTree>(new FWTree(m_children.at(child_index), &x)));
      child = x.children.end() - 1;
      if (prev_child) {
        if (!prev_child->iconnection.iend) {
          throw FWError("Seq " + string(*this) + " prev child " + string(*prev_child) + " failed to assign its iend");
        }
        connector.RepositionNode(*child, *prev_child->iconnection.iend);
      } else {
        connector.RepositionNode(*child, *x.iconnection.istart);
      }
    }
    x.iconnection.size += child->iconnection.size;

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();

    if (istate.IsLocked()) {
      state.Lock();
    }

    if (istate.IsEmitting()) {
      x.GetOConnection<OConnectionSequence>().AddNextChild(*child, isNewChild);
    }

    if (istate.IsBad()) {
      m_log.debug("Seq " + string(*this) + " has gone bad");
      state.GoBad();
      x.iconnection.iend = child->iconnection.iend;
      // Clear any subsequent children
      for (FWTree::child_mod_iter i = child+1; i != x.children.end(); ++i) {
        connector.ClearNode(*i);
      }
      x.children.erase(child+1, x.children.end());
      finished = true;
    } else if (istate.IsComplete()) {
      // Are we complete at the end of our sequence?
      if (child_index == m_children.size() - 1) {
        state.GoComplete();
        x.iconnection.iend = child->iconnection.iend;
        finished = true;
      } else {
        // Cool, keep going!
      }
    } else if (istate.IsAccepting()) {
      if (child->iconnection.iend != NULL) {
        throw FWError("Seq found incomplete inode that is only ok or done; not allowed");
      } else if (x.iconnection.iend) {
        throw FWError("Seq reached eoi but tried to set an iend.. silly internal check");
      }
      if (istate.IsDone() && child_index == m_children.size() - 1) {
        state.GoDone();
      } else {
        state.GoOK();
      }
      finished = true;
    } else {
      throw FWError("Seq " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
    ++child_index;
  }

  if (!prev_child) {
    throw FWError("Seq " + string(*this) + " should have assigned a previous child at some point");
  }

  m_log.debug("Seq " + string(*this) + " done update; now has state " + string(x) + " and hotlist size is " + boost::lexical_cast<string>(x.GetOConnection().GetHotlist().size()));
  m_log.debug(" - and it has istart " + (x.iconnection.istart ? string(*x.iconnection.istart) : "<null>") + " and iend " + (x.iconnection.iend ? string(*x.iconnection.iend): "<null>"));
}

auto_ptr<OConnection> SeqRule::MakeOConnection(const FWTree& x) const {
  return auto_ptr<OConnection>(new OConnectionSequence(m_log, x));
}
