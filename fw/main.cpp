// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* shok compiler framework example runner */

#include "Compiler.h"
#include "Connector.h"
#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
#include "Lexer.h"
#include "Parser.h"
#include "Rule.h"

#include "util/Log.h"

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

    bool isGraphing = true;

    // Lexer
    std::auto_ptr<Rule> lexer = CreateLexer_Simple(log);
    log.info("Lexer: " + lexer->Print());
    std::auto_ptr<Grapher> lexerGrapher;
    if (isGraphing) {
      lexerGrapher.reset(new Grapher(log, GRAPHDIR, "lexer_"));
      lexerGrapher->AddMachine("Lexer", *lexer.get());
      lexerGrapher->SaveAndClear();
    }
    Connector lexerConnector(log, *lexer.get(), "Lexer", lexerGrapher.get());
    log.info("Made a lexer connector");

    // Parser
    std::auto_ptr<Rule> parser = CreateParser_Simple(log);
    log.info("Parser: " + parser->Print());
    std::auto_ptr<Grapher> parserGrapher;
    if (isGraphing) {
      parserGrapher.reset(new Grapher(log, GRAPHDIR, "parser_"));
      parserGrapher->AddMachine("Parser", *parser.get());
      parserGrapher->SaveAndClear();
    }
    Connector parserConnector(log, *parser.get(), "Parser", parserGrapher.get());

    // Compiler
/*
    std::auto_ptr<Rule> compiler = CreateCompiler_Simple(log);
    log.info("Compiler: " + compiler->Print());
    std::auto_ptr<Grapher> compilerGrapher;
    if (isGraphing) {
      compilerGrapher.reset(new Grapher(log, GRAPHDIR, "compiler_"));
      compilerGrapher->AddMachine("Compiler", *compiler.get());
      compilerGrapher->SaveAndClear();
    }
    Connector compilerConnector(log, *compiler.get(), "Compiler", compilerGrapher.get());
*/

    IList* start = NULL;
    IList* prev = NULL;
    string line;
    while (getline(cin, line)) {
      //line += "\n";
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
          log.info("* main: Lexer returned hotlist; sending to parser.  Hotlist:" + lexerConnector.PrintHotlist());
          parserConnector.UpdateWithHotlist(tokenHotlist);
/*
          const Hotlist::hotlist_vec& astHotlist = parserConnector.GetHotlist();
          if (!astHotlist.empty()) {
            log.info("* main: Parser returned hotlist; sending to compiler.  Hotlist:" + parserConnector.PrintHotlist());
            compilerConnector.UpdateWithHotlist(astHotlist);
            log.info("* main: No compiler consumer; done with input character.");
          } else {
            log.info("* main: Parser returned no hotlist items.");
          }
*/
        } else {
          log.info("* main: Lexer returned no hotlist items.");
        }
        continue;
      }
      for (size_t i=0; i < line.size(); ++i) {
        IList* c = new IList("", string(1, line.at(i)));
        if (!start) { start = c; }
        if (prev) {
          prev->right = c;
          c->left = prev;
        }
        log.info("");
        log.info("* main: Inserting character '" + c->Print() + "' into lexer");
        lexerConnector.Insert(*c);
        const Hotlist::hotlist_vec& tokenHotlist = lexerConnector.GetHotlist();
        if (!tokenHotlist.empty()) {
          log.info("* main: Lexer returned hotlist; sending to parser.  Hotlist:" + lexerConnector.PrintHotlist());
          parserConnector.UpdateWithHotlist(tokenHotlist);
/*
          const Hotlist::hotlist_vec& astHotlist = parserConnector.GetHotlist();
          if (!astHotlist.empty()) {
            log.info("* main: Parser returned hotlist; sending to compiler.  Hotlist:" + parserConnector.PrintHotlist());
            compilerConnector.UpdateWithHotlist(astHotlist);
            log.info("* main: No compiler consumer; done with input character.");
          } else {
            log.info("* main: Parser returned no hotlist items.");
          }
*/
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
