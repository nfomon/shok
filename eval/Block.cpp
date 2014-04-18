// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Block.h"

#include "CompileError.h"
#include "Expression.h"
#include "Scope.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace compiler;

/* public */

Block::~Block() {
  log.debug("Destroying Block " + print());
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->cancelParentScopeNode();
  }
  m_scope.reset();
  log.debug(" - done destroying Block " + print());
}

void Block::initScope(Scope* scopeParent) {
  m_scope.init(scopeParent);
}

void Block::initScope(Scope* scopeParent, Function* function) {
  m_function = function;
  m_scope.init(scopeParent, function);
}

void Block::initScope(Scope* scopeParent, ObjectLiteral* object) {
  m_object = object;
  m_scope.init(scopeParent, object);
}

void Block::setup() {
  Brace::setup();

  if (m_exp) {
    throw CompileError("Block.m_exp should not already be set when setting up");
  }
  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    m_exp = dynamic_cast<Expression*>(children.front());
  }
  if (!m_exp) {
    vector<child_mod_iter> instants;
    for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
      Statement* statement = dynamic_cast<Statement*>(*i);
      if (!statement) continue; // Note: not all Block's children are Statements
      // "Instant" statements are builtins that have already been compiled, and
      // can be discarded.
      if (statement->isInstant()) {
        instants.push_back(i);
      } else {
        m_statements.push_back(statement);
      }
    }
    for (vector<child_mod_iter>::const_reverse_iterator i = instants.rbegin();
         i != instants.rend(); ++i) {
      delete **i;
      children.erase(*i);
    }
  }
}

void Block::compile() {
  codegen();
  // OLD: llvm-evaluate the m_bytecode
  /*
  // We used to keep objects in our scope if we're an ObjectLiteral block
  if (!m_object) {
    m_scope.reset();
  }
  */
}

string Block::cmdText() const {
  if (!m_exp) {
    throw CompileError("Cannot get cmdText of a code block");
  } else if (!isCompiled) {
    throw CompileError("Cannot get cmdText of a non-compiled block");
  }
  return m_exp->cmdText();
}

vector<NewInit*> Block::getInits() const {
  if (!m_object) {
    throw CompileError("Cannot get NewInits out of non-ObjectLiteral Block " + print());
  }
  std::vector<NewInit*> iv;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    NewInit* init = dynamic_cast<NewInit*>(*i);
    if (!init) {
      throw CompileError("ObjectLiteral Block " + print() + " must only contain NewInits");
    }
    iv.push_back(init);
  }
  return iv;
}

/* private */

void Block::codegen() {
  // Write out LLVM bytecode for this block
  if (isCodeBlock()) {
    if (0 == depth()) {
      if (m_function) {
      } else if (m_object) {
      } else {
        // create a new main() returning void
      }
    }
    // setup block
    // codegen each child and insert its code into the block
    // Note: not all our children are statements, sooo... hmm... we'll need to cast each and every one to find its individual codegen().  which is fine.
    // Really, we should probably separate the different types of block into
    // different classes.  Oh well, let's not bother with that yet.
    // Finish the block
  } else {
    // codegen the expression, including a final ->str call on it.
    // we should, like, hang on to the llvm var that will contain our result,
    // or something.
    // m_something = m_exp->codegen();    // ?
  }
}
