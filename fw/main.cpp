// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok compiler framework example runner */

#include "Char.h"
#include "Connector.h"
#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
#include "Keyword.h"
#include "Meta.h"
#include "Or.h"
#include "Regexp.h"
#include "Rule.h"
#include "Seq.h"
#include "Star.h"

#include "util/Log.h"

#include <boost/regex.hpp>

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace fw;

namespace {
  const string PROGRAM_NAME = "shok_fw";
  const string LOGFILE = "fw.log";
  const string GRAPHDIR = "graphs";
}

class Lexer {
public:
  // Lexer Tree:
  //       Star
  //         |
  //        Or
  //   |-----|-------|
  //  new   del  identifier

  Lexer(Log& log, const std::string& name)
    : m_log(log),
      m_name(name),
      m_machine(log, name) {
    std::auto_ptr<Rule> or_(new OrRule(log, "or"));
    addKeyword(*or_.get(), "new");
    addKeyword(*or_.get(), "del");
    addRegexp(*or_.get(), "ID", "[A-Za-z_][0-9A-Za-z_]*");
    addRegexp(*or_.get(), "INT", "[0-9]+");
    addRegexp(*or_.get(), "WS", "[ \t\r]+");
    addKeyword(*or_.get(), ";");
    m_machine.AddChild(or_);
  }

  Rule& Machine() { return m_machine; }

private:
  void addKeyword(Rule& or_, const std::string& keyword) {
    std::auto_ptr<Rule> kw(new KeywordRule(m_log, keyword));
    or_.AddChild(kw);
  }
  void addRegexp(Rule& or_, const std::string& name, const std::string& regexp) {
    std::auto_ptr<Rule> re(new RegexpRule(m_log, name, boost::regex(regexp)));
    or_.AddChild(re);
  }

  Log& m_log;
  string m_name;
  StarRule m_machine;
};

class Parser {
public:
  // Parser Tree:
  //       Star
  //        |
  //        Or
  //   |----------|
  //  Seq1       Seq2
  // |----|     |----|
  // new  id    del  id

  Parser(Log& log, const std::string& name)
    : m_log(log),
      m_name(name),
      m_machine(log, name) {
    std::auto_ptr<Rule> or_(new OrRule(log, "or"));
    std::auto_ptr<Rule> seq1_(new SeqRule(log, "seq1"));
    std::auto_ptr<Rule> new_(new MetaRule(log, "new", "new"));
    std::auto_ptr<Rule> x1_(new MetaRule(log, "ID", "identifier"));
    std::auto_ptr<Rule> semi1_(new MetaRule(log, ";", ";"));
    seq1_->AddChild(new_);
    seq1_->AddChild(x1_);
    seq1_->AddChild(semi1_);
    or_->AddChild(seq1_);
    std::auto_ptr<Rule> seq2_(new SeqRule(log, "seq2"));
    std::auto_ptr<Rule> del_(new MetaRule(log, "del", "del"));
    std::auto_ptr<Rule> x2_(new MetaRule(log, "ID", "identifier"));
    std::auto_ptr<Rule> semi2_(new MetaRule(log, ";", ";"));
    seq2_->AddChild(del_);
    seq2_->AddChild(x2_);
    seq2_->AddChild(semi2_);
    or_->AddChild(seq2_);
    m_machine.AddChild(or_);
  }

  Rule& Machine() { return m_machine; }

private:
  Log& m_log;
  string m_name;
  StarRule m_machine;
};

int main(int argc, char *argv[]) {
  if (argc < 1 || argc > 2) {
    cout << "usage: " << PROGRAM_NAME << " [log level]" << endl;
    return 1;
  }

  Log log(LOGFILE);
  try {
    if (2 == argc) {
      log.setLevel(argv[1]);
    }

    bool isGraphing = false;

    // Lexer
    Lexer lexer(log, "Star: lexer");
    log.info("Lexer: " + lexer.Machine().print());
    std::auto_ptr<Grapher> lexerGrapher;
    if (isGraphing) {
      lexerGrapher.reset(new Grapher(log, GRAPHDIR, "lexer_"));
      lexerGrapher->AddMachine("Lexer", lexer.Machine());
      lexerGrapher->SaveAndClear();
    }
    Connector lexerConnector(log, lexer.Machine(), "Lexer", lexerGrapher.get());
    log.info("Made a lexer connector");

    // Parser
    Parser parser(log, "Star: parser");
    log.info("Parser: " + parser.Machine().print());
    std::auto_ptr<Grapher> parserGrapher;
    if (isGraphing) {
      parserGrapher.reset(new Grapher(log, GRAPHDIR, "parser_"));
      parserGrapher->AddMachine("Parser", parser.Machine());
      parserGrapher->SaveAndClear();
    }
    Connector parserConnector(log, parser.Machine(), "Parser", parserGrapher.get());

    IList* start = NULL;
    IList* prev = NULL;
    const IList* astStart = NULL;
    string line;
    while (getline(cin, line)) {
      // check for delete
      if (line.size() >= 2 && line.size() <= 3 && line[0] == 'D' && line[1] >= '0' && line[1] <= '9') {
        if (!start) {
          throw FWError("Cannot delete start of input: no input yet");
        }
        int delnum = line[1] - '0';
        if (line.size() == 3 && line[2] >= '0' && line[2] <= '9') {
          delnum *= 10;
          delnum += line[2] - '0';
        }
        IList* s = start;
        if (!s) {
          throw FWError("Cannot delete entry without start");
        }
        if (0 == delnum) {
          start = start->right;
        }
        for (int i = 0; i < delnum; ++i) {
          s = s->right;
          if (!s) {
            throw FWError("Reached end of input before reaching deletion index");
          }
        }
        log.info("");
        log.info("* main: Deleting character '" + string(*s) + "' from lexer");
        if (s->left) {
          s->left->right = s->right;
        }
        if (s->right) {
          s->right->left = s->left;
        }
        lexerConnector.Delete(*s);
        const Hotlist::hotlist_vec& tokenHotlist = lexerConnector.GetHotlist();
        if (!tokenHotlist.empty()) {
          log.info("* main: Lexer returned hotlist; sending to parser.  Hotlist: " + lexerConnector.PrintHotlist());
          if (!astStart) {
            astStart = tokenHotlist.begin()->first;
          }
          parserConnector.UpdateWithHotlist(tokenHotlist);
          log.info("* main: No parser consumer; done with input character.");
        } else {
          log.info("* main: Lexer returned no hotlist items.");
        }
        continue;
      }
      for (size_t i=0; i < line.size(); ++i) {
        IList* c = new IList(std::auto_ptr<OData>(new CharData(line.at(i))));
        if (!start) { start = c; }
        if (prev) {
          prev->right = c;
          c->left = prev;
        }
        log.info("");
        log.info("* main: Inserting character '" + c->print() + "' into lexer");
        lexerConnector.Insert(*c);
        const Hotlist::hotlist_vec& tokenHotlist = lexerConnector.GetHotlist();
        if (!tokenHotlist.empty()) {
          log.info("* main: Lexer returned hotlist; sending to parser.  Hotlist: " + lexerConnector.PrintHotlist());
          if (!astStart) {
            astStart = tokenHotlist.begin()->first;
          }
          parserConnector.UpdateWithHotlist(tokenHotlist);
          log.info("* main: No parser consumer; done with input character.");
        } else {
          log.info("* main: Lexer returned no hotlist items.");
        }

        prev = c;
      }
    }
    log.info("Clearing input");
    IList* i = start;
    while (i) {
      IList* j = i->right;
      delete i;
      i = j;
    }

  } catch (const FWError& e) {
    log.error(string("Compilation framework error: ") + e.what());
    return 1;
  } catch (const std::exception& e) {
    log.error(string("Unknown error: ") + e.what());
    return 1;
  } catch (...) {
    log.error("Unknown error");
    return 1;
  }

  return 0;
}
