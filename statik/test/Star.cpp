// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "statik/Batch.h"
#include "statik/IncParser.h"
#include "statik/Keyword.h"
#include "statik/List.h"
#include "statik/Rule.h"
#include "statik/SError.h"
#include "statik/STree.h"
#include "statik/Star.h"
using statik::Batch;
using statik::IncParser;
using statik::KEYWORD;
using statik::List;
using statik::Rule;
using statik::STAR;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik_test;

void Star::run() {
  g_log.info();
  g_log.info() << "Blank Star";
  {
    bool p = false;
    string msg;
    try {
      auto_ptr<Rule> r(STAR("Star"));
      IncParser ip(r, "Star test 1");
      List cx("cx", "x");
      ip.Insert(cx, NULL);
    } catch (const statik::SError& e) {
      p = true;
      msg = e.what();
    }
    test(p, "Empty Star not allowed: " + msg);
  }

  g_log.info();
  g_log.info() << "Star with a Keyword child";
  {
    auto_ptr<Rule> kr(KEYWORD("a"));
    auto_ptr<Rule> r(STAR("Star"));
    r->AddChild(kr);
    IncParser ip(r, "Star test 2");
    const statik::STree& root = ip.GetRoot();

    {
      Batch out_batch;
      List cx("cx", "x");
      ip.Insert(cx, NULL);
      test(root.GetState().IsBad(), "x");
      ip.ExtractChanges(out_batch);
      test(out_batch.IsEmpty(), "no output");
      out_batch.Clear();

      ip.Delete(cx);
      test(root.IsClear(), "clear");
      test(out_batch.IsEmpty(), "no output");
    }

    {
      Batch out_batch;
      List ca("ca", "a");
      ip.Insert(ca, NULL);
      test(root.GetState().IsDone(), "a");
      ip.ExtractChanges(out_batch);

      {
        Batch expected_batch;
        expected_batch.Insert(ca, NULL);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();

      List cb("cb", "b");
      ip.Insert(cb, &ca);
      test(root.GetState().IsComplete(), "ab");

      List cc("cc", "c");
      ip.Insert(cc, &cb);
      test(root.GetState().IsBad(), "abc");

      ip.Delete(cb);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(ca);
      test(root.GetState().IsBad(), "c");

      List ca2("2", "a");
      ip.Insert(ca2, NULL);
      test(root.GetState().IsComplete(), "ac");

      ip.Delete(cc);
      test(root.GetState().IsDone(), "a");

      ip.Delete(ca2);
      test(root.IsClear());
    }
  }
}
