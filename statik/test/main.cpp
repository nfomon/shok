// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* statik compiler framework tests */

#include "STError.h"
#include "STLog.h"
#include "StatikBattery.h"

#include "statik/Connector.h"
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

using namespace statik_test;

namespace {
  const string PROGRAM_NAME = "statik_test";
}

int main(int argc, char *argv[]) {
  try {
    // Retrieve program options
    string logfile;
    string loglevel = Log::UnMapLevel(Log::INFO);
    string slogfile;
    string sloglevel = Log::UnMapLevel(Log::INFO);
    po::options_description desc(PROGRAM_NAME + " usage");
    desc.add_options()
      ("help,h", "show help message")
      ("logfile,f", po::value<string>(&logfile), "output log file")
      ("loglevel,L", po::value<string>(&loglevel), "log level: debug, info, warning, error")
      ("slogfile", po::value<string>(&slogfile), "statik log file")
      ("sloglevel", po::value<string>(&sloglevel), "statik log level: debug, info, warning, error")
    ;
    po::variables_map vm;

    try {
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
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
    if (!slogfile.empty()) {
      statik::g_log.setLevel(sloglevel);
      statik::g_log.Init(slogfile);
      if (Log::DEBUG == statik::g_log.getLevel()) {
        statik::g_san.setLevel(Log::DEBUG);
        statik::g_san.Init(slogfile + "_sanity");
      }
    }

    StatikBattery b("statik", cin, cout);
    b.Run();

  } catch (const STError& e) {
    g_log.error() << "statik test error: " << e.what() << endl;
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
