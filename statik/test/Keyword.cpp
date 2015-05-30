// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "statik/Batch.h"
#include "statik/IncParser.h"
#include "statik/Keyword.h"
#include "statik/List.h"
#include "statik/Rule.h"
#include "statik/SError.h"
#include "statik/STree.h"
using statik::KEYWORD;

#include <memory>
using std::auto_ptr;

using namespace statik_test;

void Keyword::run() {
  // Blank keyword
  {
    bool p = false;
    try {
      auto_ptr<statik::Rule> r(KEYWORD(""));
      statik::IncParser ip(r, "test1");
    } catch (const statik::SError& e) {
      p = true;
    }
    test(p, "Empty keyword not allowed");
  }

  // Single-character keyword
  {
    auto_ptr<statik::Rule> r(KEYWORD("a"));
    statik::IncParser ip(r, "test2");
    const statik::STree& root = ip.GetRoot();

    {
      statik::INode cx = ip.Insert(statik::List("cx", "x"), NULL);
      test(root.GetState().IsBad(), "x");

      ip.Delete(cx);
      test(root.IsClear(), "clear");
    }

    {
      statik::INode ca = ip.Insert(statik::List("ca", "a"), NULL);
      test(root.GetState().IsDone(), "a");

      statik::INode cb = ip.Insert(statik::List("cb", "b"), ca);
      test(root.GetState().IsComplete(), "ab");

      statik::INode cc = ip.Insert(statik::List("cc", "c"), cb);
      test(root.GetState().IsBad(), "abc");

      ip.Delete(cb);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(ca);
      test(root.GetState().IsBad(), "c");

      statik::INode ca2 = ip.Insert(statik::List("2", "a"), NULL);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(cc);
      test(root.GetState().IsDone(), "a");

      ip.Delete(ca2);
      test(root.IsClear());
    }

    // Batch updates
    {
      statik::Batch b;
      statik::List ca("ca", "a");
      statik::List cb("cb", "b");

      b.Insert(ca);
      ip.ApplyBatch(b);
      test(root.GetState().IsDone(), "a");

      b.Clear();
      b.Delete(ca);
      ip.ApplyBatch(b);
      test(root.IsClear(), "clear");

      b.Clear();
      b.Insert(ca);
      b.Insert(cb);
      ip.ApplyBatch(b);
      test(root.GetState().IsComplete(), "ab");

      b.Clear();
      b.Delete(ca);
      b.Delete(cb);
      ip.ApplyBatch(b);
      test(root.IsClear(), "clear");
    }
  }
}
