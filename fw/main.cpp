// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok compiler framework example runner */

#include "Char.h"
#include "Connector.h"
#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
#include "Keyword.h"
#include "Name.h"
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
    std::auto_ptr<Rule> new_(new KeywordRule(log, "new"));
    or_->AddChild(new_);
    std::auto_ptr<Rule> del_(new KeywordRule(log, "del"));
    or_->AddChild(del_);
    std::auto_ptr<Rule> identifier_(new RegexpRule(log, "ID", boost::regex("[A-Za-z_][0-9A-Za-z_]*")));
    or_->AddChild(identifier_);
    m_machine.AddChild(or_);
  }

  Rule& Machine() { return m_machine; }

private:
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
    std::auto_ptr<Rule> new_(new KeywordMetaRule(log, "new", "new"));
    std::auto_ptr<Rule> x1_(new NameRule(log, "ID", "identifier"));
    seq1_->AddChild(new_);
    seq1_->AddChild(x1_);
    or_->AddChild(seq1_);
    std::auto_ptr<Rule> seq2_(new SeqRule(log, "seq2"));
    std::auto_ptr<Rule> del_(new KeywordMetaRule(log, "del", "del"));
    std::auto_ptr<Rule> x2_(new NameRule(log, "ID", "identifier"));
    seq2_->AddChild(del_);
    seq2_->AddChild(x2_);
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

    // Lexer
    Lexer lexer(log, "Star: lexer");
    log.info("Lexer: " + lexer.Machine().print());
    Grapher lexerGrapher(GRAPHDIR, "lexer_");
    lexerGrapher.AddMachine("Lexer", lexer.Machine());
    lexerGrapher.SaveAndClear();
    Connector lexerConnector(log, lexer.Machine(), "Lexer", &lexerGrapher);
    log.info("Made a lexer connector");

    // Parser
    Parser parser(log, "Star: parser");
    log.info("Parser: " + parser.Machine().print());
    Grapher parserGrapher(GRAPHDIR, "parser_");
    parserGrapher.AddMachine("Parser", parser.Machine());
    parserGrapher.SaveAndClear();
    Connector parserConnector(log, parser.Machine(), "Parser", &parserGrapher);

    IList* start = NULL;
    IList* prev = NULL;
    const IList* astStart = NULL;
    string line;
    while (getline(cin, line)) {
      line += "\n";
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
          log.info("* main: Lexer returned " + boost::lexical_cast<string>(tokenHotlist.size()) + " hotlist items: sending to parser");
          for (Hotlist::hotlist_iter i = tokenHotlist.begin(); i != tokenHotlist.end(); ++i) {
            log.debug("Hotlist item: " + string(*i->first) + ", op: " + (i->second == Hotlist::OP_INSERT ? "insert" : "update|delete"));
          }
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
