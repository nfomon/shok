// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Seq.h"

#include "Connector.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace fw;

void SeqRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  m_log.debug("Repositioning SeqRule " + string(*this) + " at " + string(x) + " with " + string(inode));
  RepositionFirstChildOfNode(connector, x, inode);
  Update(connector, x, &x.children.at(0));
}

void SeqRule::Update(Connector& connector, FWTree& x, const FWTree* updated_child) const {
  m_log.debug("Updating SeqRule " + string(*this) + " at " + string(x) + " with child " + (updated_child ? string(*updated_child) : "<null>"));

  // Initialize state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  SeqState& state = x.GetState<SeqState>();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " must have children");
  } else if (x.children.size() > m_children.size()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
  }

  // Iterate over node children, either existing or being created, starting at
  // the updated_child, so long as our last child is complete.
  bool finished = false;
  unsigned int child_index = 0;
  FWTree::child_mod_iter child = x.children.begin();
  // Skip ahead to the updated child
  FWTree* prev_child = NULL;
  if (updated_child) {
    for (child = x.children.begin(); child != x.children.end(); ++child, ++child_index) {
      if (updated_child == &*child) { break; }
      State& istate = child->GetState();
      if (istate.IsLocked()) {
        state.Lock();
      }
      if (!istate.IsComplete()) {
        throw FWError("SeqRule " + string(*this) + " found 'skippable' child that was not complete");
      }
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          throw FWError("SeqRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
          //connector.RepositionNode(*child, *prev_child->iconnection.iend);    // Could correct it like this
        }
      }
      x.iconnection.size += child->iconnection.size;
      prev_child = &*child;
    }
  }

  while (!finished) {
    if (child_index >= m_children.size()) {
      throw FWError("SeqRule " + string(*this) + " evaluated too many children beyond the rule");
    }
    bool isNewChild = false;
    if (child != x.children.end()) {
      // Existing child
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          throw FWError("SeqRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
          //connector.RepositionNode(*child, *prev_child->iconnection.iend);    // Could correct it like this
        }
      }
    } else if (x.children.size() > m_children.size()) {
      throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
    } else if (child_index >= m_children.size()) {
      throw FWError("SeqRule::Update: child index >= Rule size");
    } else {
      // New child
      isNewChild = true;
      std::auto_ptr<FWTree> newChild(new FWTree(m_log, m_children.at(child_index).MakeState(), &x));
      x.children.push_back(newChild);
      child = x.children.end() - 1;
      if (prev_child) {
        if (!prev_child->iconnection.iend) {
          throw FWError("SeqRule " + string(*this) + " prev child " + string(*prev_child) + " failed to assign its iend");
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
      m_log.debug("SeqRule " + string(*this) + " has gone bad");
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
        throw FWError("SeqRule found incomplete inode that is only ok or done; not allowed");
      } else if (x.iconnection.iend) {
        throw FWError("OrRule reached eoi but tried to set an iend.. silly internal check");
      }
      if (istate.IsDone() && child_index == m_children.size() - 1) {
        state.GoDone();
      } else {
        state.GoOK();
      }
      finished = true;
    } else {
      throw FWError("SeqRule " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
    ++child_index;
  }

  if (!prev_child) {
    throw FWError("StarRule " + string(*this) + " should have assigned a previous child at some point");
  }

  m_log.debug("SeqRule " + string(*this) + " done update; now has state " + string(x) + " and hotlist size is " + boost::lexical_cast<string>(x.GetOConnection().GetHotlist().size()));
}

std::auto_ptr<State> SeqRule::MakeState() const {
  return std::auto_ptr<State>(new SeqState(*this));
}

std::auto_ptr<OConnection> SeqRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSequence(m_log, x));
}
