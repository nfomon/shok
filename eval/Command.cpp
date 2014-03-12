// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Command.h"

#include "Block.h"
#include "CommandFragment.h"
#include "EvalError.h"
#include "Node.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Command::setup() {
}

void Command::evaluate() {
  // Codegen all the child blocks.  If it's a single code block, run it.
  // Otherwise, run all the expression blocks, take their cmdtext, and tell
  // the shell what command it should run on our behalf.
  if (1 == children.size()) {
    Block* block = dynamic_cast<Block*>(children.front());
    if (block && block->isCodeBlock()) {
      block->evaluateNode();
      return;
    }
  }
  string cmd;
  for (Node::child_iter i = children.begin(); i != children.end(); ++i) {
    Block* block = dynamic_cast<Block*>(*i);
    // Code blocks should have already been dealt with
    if (block && block->isCodeBlock()) {
      throw EvalError("Command " + print() + " found inappropriately-positioned code block");
    }
    CommandFragment* frag = dynamic_cast<CommandFragment*>(*i);
    if (frag) {
      frag->evaluateNode();
      cmd += frag->cmdText();
    } else if (block) {
      block->evaluateNode();
      cmd += block->cmdText();
    } else {
      throw EvalError("Command has an unsupported child: " + string(**i));
    }
  }
  log.info("RUNNING CMD: <" + cmd + ">");
  std::cout << "CMD:" << cmd << std::endl;
  string line;
  std::getline(std::cin, line);
  int returnCode = boost::lexical_cast<int>(line);
  log.info("RETURN CODE: " + boost::lexical_cast<string>(returnCode));
}
