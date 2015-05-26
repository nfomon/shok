// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* statik compiler framework runner */

#include "Codegen.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

#include "statik/Grapher.h"
#include "statik/Hotlist.h"
#include "statik/IncParser.h"
#include "statik/List.h"
#include "statik/Rule.h"
#include "statik/SError.h"
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
    string compilerName;
    string logfile;
    string loglevel = Log::UnMapLevel(Log::INFO);
    string graphdir;
    po::options_description desc(PROGRAM_NAME + " usage");
    desc.add_options()
      ("help,h", "show help message")
      ("compiler,c", po::value<string>(&compilerName), "compiler name")
      ("logfile,f", po::value<string>(&logfile), "output log file")
      ("loglevel,L", po::value<string>(&loglevel), "log level: debug, info, warning, error")
      ("graphdir,g", po::value<string>(&graphdir), "output graph directory")
    ;
    po::positional_options_description p;
    p.add("compiler", 1);
    po::variables_map vm;

    try {
      po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
      po::notify(vm);

      if (vm.count("help")) {
        cout << desc;
        return 0;
      }
    } catch (po::error& e) {
      cout << desc;
      return 1;
    }

    // Initialize logging
    if (!logfile.empty()) {
      g_log.setLevel(loglevel);
      g_log.Init(logfile);
    }

    auto_ptr<Compiler> compiler = MakeCompiler(compilerName);
    if (!compiler.get() || compiler->empty()) {
      throw statik::SError("Empty compiler; nothing to do");
    }
    typedef boost::ptr_vector<IncParser> parser_vec;
    typedef parser_vec::iterator parser_mod_iter;
    parser_vec parsers;
    for (size_t i = 0; i < compiler->size(); ++i) {
      Compiler::auto_type c = compiler->release(compiler->begin());
      g_log.info() << "Compiler item: " << c->Print();
      string name = c->Name();
      parsers.push_back(new IncParser(auto_ptr<Rule>(c.release()), name, graphdir));
    }

    List* start = NULL;
    List* prev = NULL;
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
        List* s = start;
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
        g_log.info() << "* main: Deleting character '" << *s;
        if (s->left) {
          s->left->right = s->right;
        }
        if (s->right) {
          s->right->left = s->left;
        }
        parsers.at(0).Delete(*s);
        Hotlist hotlist;
        parsers.front().ExtractHotlist(hotlist);
        for (parser_mod_iter i = parsers.begin()+1; i != parsers.end(); ++i) {
          if (hotlist.IsEmpty()) {
            g_log.info() << "* main: IncParser returned no hotlist items.";
            break;
          } else {
            g_log.info() << "* main: IncParser returned hotlist; sending to parser.  Hotlist:" << hotlist.Print();
            i->UpdateWithHotlist(hotlist.GetHotlist());
            i->ExtractHotlist(hotlist);
          }
        }
        continue;
      }
      for (size_t i=0; i < line.size(); ++i) {
        List* c = new List("", string(1, line.at(i)));
        if (!start) { start = c; }
        if (prev) {
          prev->right = c;
          c->left = prev;
        }
        g_log.info();
        g_log.info() << "* main: Inserting character '" << *c;
        parsers.at(0).Insert(*c);
        Hotlist hotlist;
        parsers.front().ExtractHotlist(hotlist);
        for (parser_mod_iter i = parsers.begin()+1; i != parsers.end(); ++i) {
          if (hotlist.IsEmpty()) {
            g_log.info() << "* main: IncParser returned no hotlist items.";
            break;
          } else {
            g_log.info() << "* main: IncParser returned hotlist; sending to parser.  Hotlist:" << hotlist.Print();
            i->UpdateWithHotlist(hotlist.GetHotlist());
            i->ExtractHotlist(hotlist);
          }
        }
        prev = c;
      }
    }
    g_log.info() << "Clearing input";
    List* i = start;
    while (i) {
      List* j = i->right;
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
