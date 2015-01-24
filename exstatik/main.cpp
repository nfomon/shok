// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* statik compiler framework runner */

#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

#include "statik/Connector.h"
#include "statik/Grapher.h"
#include "statik/Hotlist.h"
#include "statik/IList.h"
#include "statik/Rule.h"
#include "statik/SLog.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <memory>
#include <string>
using std::auto_ptr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

using namespace exstatik;
using namespace statik;

namespace {
  const string PROGRAM_NAME = "exstatik";
}

int main(int argc, char *argv[]) {
  try {
    // Retrieve program options
    po::options_description desc(PROGRAM_NAME + " usage");
    desc.add_options()
      ("help,h", "show help message")
      ("logfile,f", po::value<string>(), "output log file")
      ("loglevel,L", po::value<string>(), "log level: debug, info, warning, error")
      ("graphdir,g", po::value<string>(), "output graph directory")
    ;
    po::positional_options_description p;
    po::variables_map vm;
    string logfile;
    string loglevel = Log::UnMapLevel(Log::INFO);
    string graphdir;

    try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);

      if (vm.count("help")) {
        cout << desc;
        return 0;
      }
      if (vm.count("logfile")) {
        logfile = vm["logfile"].as<string>();
      }
      if (vm.count("loglevel")) {
        loglevel = vm["loglevel"].as<string>();
      }
      if (vm.count("graphdir")) {
        graphdir = vm["graphdir"].as<string>();
      }
    } catch (po::error& e) {
      cout << desc;
      return 1;
    }

    // Initialize logging
    if (!logfile.empty()) {
      if (!loglevel.empty()) {
        g_log.setLevel(loglevel);
      }
      g_log.Init(logfile);
    }

    // Lexer
    auto_ptr<Rule> lexer = CreateLexer_Nifty();
    g_log.info() << "Lexer: " << lexer->Print();
    Connector lexerConnector(*lexer.get(), "Lexer", graphdir);
    g_log.info() << "Made a lexer connector";

    // Parser
    auto_ptr<Rule> parser = CreateParser_Nifty();
    g_log.info() << "Parser: " + parser->Print();
    auto_ptr<Grapher> parserGrapher;
    Connector parserConnector(*parser.get(), "Parser", graphdir);

    // Compiler
    auto_ptr<Rule> compiler = CreateCompiler_Nifty();
    g_log.info() << "Compiler: " + compiler->Print();
    auto_ptr<Grapher> compilerGrapher;
    Connector compilerConnector(*compiler.get(), "Compiler", graphdir);

    IList* start = NULL;
    IList* prev = NULL;
    string line;
    while (getline(cin, line)) {
      //line += "\n";
      // check for delete
      if (line.size() >= 2 && line.size() <= 3 && line[0] == 'D' && line[1] >= '0' && line[1] <= '9') {
        if (!start) {
          throw SError("Cannot delete start of input: no input yet");
        }
        int delnum = line[1] - '0';
        if (line.size() == 3 && line[2] >= '0' && line[2] <= '9') {
          delnum *= 10;
          delnum += line[2] - '0';
        }
        IList* s = start;
        if (!s) {
          throw SError("Cannot delete entry without start");
        }
        if (0 == delnum) {
          start = start->right;
        }
        for (int i = 0; i < delnum; ++i) {
          s = s->right;
          if (!s) {
            throw SError("Reached end of input before reaching deletion index");
          }
        }
        g_log.info();
        g_log.info() << "* main: Deleting character '" << *s << "' from lexer";
        if (s->left) {
          s->left->right = s->right;
        }
        if (s->right) {
          s->right->left = s->left;
        }
        lexerConnector.Delete(*s);
        const Hotlist& tokenHotlist = lexerConnector.GetHotlist();
        if (!tokenHotlist.IsEmpty()) {
          g_log.info() << "* main: Lexer returned hotlist; sending to parser.  Hotlist:" << tokenHotlist.Print();
          parserConnector.UpdateWithHotlist(tokenHotlist.GetHotlist());
          lexerConnector.ClearHotlist();
          const Hotlist& astHotlist = parserConnector.GetHotlist();
          if (!astHotlist.IsEmpty()) {
            g_log.info() << "* main: Parser returned hotlist; sending to compiler.  Hotlist:" << astHotlist.Print();
            compilerConnector.UpdateWithHotlist(astHotlist.GetHotlist());
            parserConnector.ClearHotlist();
            g_log.info() << "* main: No compiler consumer; done with input character.";
          } else {
            g_log.info() << "* main: Parser returned no hotlist items.";
          }
        } else {
          g_log.info() << "* main: Lexer returned no hotlist items.";
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
        g_log.info();
        g_log.info() << "* main: Inserting character '" << c->Print() << "' into lexer";
        lexerConnector.Insert(*c);
        const Hotlist& tokenHotlist = lexerConnector.GetHotlist();
        if (!tokenHotlist.IsEmpty()) {
          g_log.info() << "* main: Lexer returned hotlist; sending to parser.  Hotlist:" << tokenHotlist.Print();
          parserConnector.UpdateWithHotlist(tokenHotlist.GetHotlist());
          lexerConnector.ClearHotlist();
          const Hotlist& astHotlist = parserConnector.GetHotlist();
          if (!astHotlist.IsEmpty()) {
            g_log.info() << "* main: Parser returned hotlist; sending to compiler.  Hotlist:" << astHotlist.Print();
            compilerConnector.UpdateWithHotlist(astHotlist.GetHotlist());
            parserConnector.ClearHotlist();
            g_log.info() << "* main: No compiler consumer; done with input character.";
          } else {
            g_log.info() << "* main: Parser returned no hotlist items.";
          }
        } else {
          g_log.info() << "* main: Lexer returned no hotlist items.";
        }

        prev = c;
      }
    }
    g_log.info() << "Clearing input";
    IList* i = start;
    while (i) {
      IList* j = i->right;
      delete i;
      i = j;
    }

  } catch (const SError& e) {
    g_log.error() << "Compilation framework error: " << e.what() << endl;
    return 1;
  } catch (const std::exception& e) {
    g_log.error() << "Unknown error: " << e.what() << endl;
    return 1;
  } catch (...) {
    g_log.error() << "Unknown error" << endl;
    return 1;
  }

  return 0;
}
