#include "Command.h"

#include "Comma.h"
#include "Node.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Command::complete() {
  log.info("Completing command");
  m_isComplete = true;
}

void Command::evaluate() {
  // There's much duplicated effort here.  We walk across our children,
  // retrieving their cmdtext and shoving it into either the program string, or
  // an element of the args vector.  Comma children become the spaces that
  // separate args.  But at the end, we don't care about this vector, we just
  // want a whole-string commandline.  But let's keep this duplication because
  // it may become useful.
  string program;
  vector<string> args;
  size_t pos = 0;
  for (Node::child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->evaluate();
    string cmdtext = (*i)->cmdText();
    if (dynamic_cast<Comma*>(*i)) {
      ++pos;
      continue;
    }
    if (0 == pos) {
      program += cmdtext;
    } else {
      if (pos > args.size()) {
        args.push_back("");
      }
      args.back() += cmdtext;
    }
  }
  string cmd = program;
  for (vector<string>::const_iterator i = args.begin(); i != args.end(); ++i) {
    cmd += " " + *i;
  }
  log.info("RUNNING CMD: <" + cmd + ">");
  std::cout << "CMD:" << cmd << std::endl;
  log.info("DONE, NOW GET STATUS CODE");
  string line;
  std::getline(std::cin, line);
  int returnCode = boost::lexical_cast<int>(line);
  log.info("RETURN CODE: " + boost::lexical_cast<string>(returnCode));
}
