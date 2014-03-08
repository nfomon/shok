// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Executor.h"

#include "VMError.h"

#include "util/Util.h"

#include <string>
#include <utility>
using std::pair;
using std::string;

using namespace vm;

Executor::Executor(Log& log)
  : m_log(log),
    m_symbolTable(log),
    m_instruction(NULL) {
}

void Executor::exec(const string& line) {
  m_log.debug("Executing line: " + line);
  if (m_instruction) {
    if (m_instruction->isDone()) {
      throw VMError("Executor's instruction is already done");
    }
    m_log.debug("Providing line (" + line + ") to existing instruction");
    m_instruction->insert(line);
  } else {
    pair<string,string> words = Util::break_word(line);
    m_log.debug("Making instruction for (" + words.first + ") with: (" + words.second + ")");
    m_instruction = Instruction::MakeInstruction(m_log, words.first);
    m_instruction->insert(words.second);
  }
  if (m_instruction->isDone()) {
    m_log.debug("Executing instruction");
    m_instruction->execute(m_symbolTable);
    delete m_instruction;
    m_instruction = NULL;
  }
  m_log.debug("Done executing line: " + line);
}
