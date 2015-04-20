// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

/* Interactive client for statik compilers */

#include "ISError.h"
#include "ISLog.h"
#include "IStatik.h"

#include "statik/SLog.h"

#include <boost/program_options.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;
namespace po = boost::program_options;

#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace istatik;

namespace {
  const string PROGRAM_NAME = "istatik";
}

int main(int argc, char *argv[]) {
  try {
    // Retrieve program options
    string compilerName;
    string logfile;
    string loglevel = Log::UnMapLevel(Log::INFO);
    string slogfile;
    string sloglevel = Log::UnMapLevel(Log::INFO);
    string graphdir;
    po::options_description desc(PROGRAM_NAME + " usage");
    desc.add_options()
      ("help,h", "show help message")
      ("compiler,c", po::value<string>(&compilerName), "compiler name")
      ("logfile,f", po::value<string>(&logfile), "output log file")
      ("loglevel,L", po::value<string>(&loglevel), "log level: debug, info, warning, error")
      ("slogfile", po::value<string>(&slogfile), "statik log file")
      ("sloglevel", po::value<string>(&sloglevel), "statik log level: debug, info, warning, error")
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
    if (!slogfile.empty()) {
      statik::g_log.setLevel(sloglevel);
      statik::g_log.Init(slogfile);
      if (Log::DEBUG == g_log.getLevel()) {
        statik::g_san.setLevel(Log::DEBUG);
        statik::g_san.Init(slogfile + "_sanity");
      }
    }

    IStatik istatik(compilerName, graphdir);
    istatik.run();
  } catch (const ISError& e) {
    g_log.error() << "IStatik error: " << e.what() << endl;
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
