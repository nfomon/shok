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
    auto_ptr<Rule> r(STAR("Star"));
    auto_ptr<Rule> kr(KEYWORD("a"));
    r->AddChild(kr);
    IncParser ip(r, "Star test 2");
    const statik::STree& root = ip.GetRoot();

    Batch empty_batch;
    {
      Batch out_batch;
      List cx("cx", "x");
      ip.Insert(cx, NULL);
      test(root.GetState().GetStation(), statik::State::ST_COMPLETE, "x");
      ip.ExtractChanges(out_batch);
      test(out_batch, empty_batch, "no output");
      out_batch.Clear();

      ip.Delete(cx);
      test(root.IsClear(), "clear");
      test(out_batch, empty_batch, "no output");
      out_batch.Clear();
    }

    {
      Batch out_batch;
      List ca("ca", "a");
      ip.Insert(ca, NULL);
      test(root.GetState().IsDone(), "a");
      ip.ExtractChanges(out_batch);
      {
        Batch expected_batch;
        List xca("a", "");
        expected_batch.Insert(xca, NULL);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();

      List cb("cb", "b");
      ip.Insert(cb, &ca);
      test(root.GetState().GetStation(), statik::State::ST_COMPLETE, "ab");
      ip.ExtractChanges(out_batch);
      test(out_batch, empty_batch, "no output");

      ip.Delete(cb);
      test(root.GetState().GetStation(), statik::State::ST_DONE, "a");
      test(out_batch, empty_batch, "no output");
      out_batch.Clear();

      ip.Delete(ca);
      test(root.IsClear(), "clear");
      // Really, we don't want the OItem to have been Deleted here?  Let's suppose no...
      test(out_batch, empty_batch, "no output");
      out_batch.Clear();

      List ca2("2", "a");
      ip.Insert(ca2, NULL);
      test(root.GetState().GetStation(), statik::State::ST_DONE, "a");
      ip.ExtractChanges(out_batch);
      const List* xca_pos = NULL;
      {
        Batch expected_batch;
        List xca("a", "");
        expected_batch.Insert(xca, NULL);
        expected_batch.Delete(ca);  // why does delete come after insert?
        if (test(out_batch, expected_batch, "output")) {
          xca_pos = out_batch.begin()->node;
        }
      }
      out_batch.Clear();

      List ca3("3", "a");
      ip.Insert(ca3, &ca2);
      test(root.GetState().GetStation(), statik::State::ST_DONE, "aa");
      ip.ExtractChanges(out_batch);
      {
        Batch expected_batch;
        List xca("a", "");
        expected_batch.Insert(xca, xca_pos);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();

      ip.Delete(ca2);
      test(root.GetState().GetStation(), statik::State::ST_DONE, "a");
      ip.ExtractChanges(out_batch);
      {
        // Both items are deleted, then the remaining one is added back.
        // That seems ok.
        Batch expected_batch;
        List xca("a", "");
        expected_batch.Insert(xca, NULL);
        expected_batch.Delete(ca3);
        expected_batch.Delete(ca2);
        test(out_batch, expected_batch, "output");
      }
      out_batch.Clear();
    }
  }
}
