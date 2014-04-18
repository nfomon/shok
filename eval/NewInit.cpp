// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "NewInit.h"

#include "Block.h"
#include "CompileError.h"
#include "Identifier.h"
#include "Type.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace compiler;

NewInit::~NewInit() {
  // Revert our object if we've partially created it.
  // Be paranoid here since this is regarding error conditions.
  if (m_isPrepared && parentScope && m_varname != "") {
    log.debug("Destroying NewInit " + print() + ": reverting " + m_varname);
    parentScope->revertLast();
  }
}

void NewInit::setup() {
  if (!parentScope) {
    throw CompileError("Cannot setup NewInit " + print() + " with no parent scope");
  } else if (children.size() < 1 || children.size() > 3) {
    throw CompileError("NewInit node must have 1, 2, or 3 children");
  } else if (m_identifier || m_exp || m_typeSpec || m_type.get()) {
    throw CompileError("NewInit node " + print() + " is already partially setup");
  }
  m_identifier = dynamic_cast<Identifier*>(children.at(0));
  if (!m_identifier) {
    throw CompileError("NewInit's first child must be an identifier");
  }
  m_varname = m_identifier->getName();
  log.info("NewInit varname is " + m_varname);
  switch (children.size()) {
    // new x -- type and value are both 'object'
    case 1: {
      const Symbol* object = root->getScope()->getSymbol("object");
      if (!object) {
        throw CompileError("Cannot find symbol for the object object.  Uhoh.");
      }
      m_type.reset(new BasicType(log, *object));
      // leave m_typeSpec NULL
      // leave m_exp NULL
      break;
    }

    // new x : y -- type is 'y', initial value is default value of 'y' (it must
    // be a BasicType)
    // new x = y -- initial value is the type of the expression 'y', our type
    // is its type
    case 2: {
      m_typeSpec = dynamic_cast<TypeSpec*>(children.at(1));
      m_exp = dynamic_cast<Expression*>(children.at(1));
      if (m_typeSpec && m_exp) {
        throw CompileError("NewInit " + print() + " somehow has child of both TypeSpec and Exp type");
      } else if (m_typeSpec) {
        m_type = m_typeSpec->getType();
        if (dynamic_cast<OrType*>(m_type.get())) {
          throw CompileError("NewInit " + print() + " has OrType " + m_typeSpec->print() + " but no default value is provided");
        }
      } else if (m_exp) {
        m_type = m_exp->getType();
      } else {
        throw CompileError("NewInit " + print() + " has inappropriate child type; expected TypeSpec or Exp");
      }
      break;
    }

    // new x : y = z -- type is 'y', initial value is 'z'
    case 3: {
      m_typeSpec = dynamic_cast<TypeSpec*>(children.at(1));
      if (!m_typeSpec) {
        throw CompileError("NewInit child " + children.at(1)->print() + " should have been a TypeSpec");
      }
      m_exp = dynamic_cast<Expression*>(children.at(2));
      if (!m_exp) {
        throw CompileError("NewInit child " + children.at(2)->print() + " should have been an Expression");
      }
      m_type = m_typeSpec->getType();

      if (!m_type->isParentOf(m_exp->type())) {
        throw CompileError("Value does not match the type of variable " + m_varname + ".  Type: " + m_type->print() + ", value type: " + m_exp->type().print());
      }

      break;
    }
    default:
      throw CompileError("NewInit node must have 1, 2, or 3 children");
  }
  if (!m_type.get()) {
    throw CompileError("NewInit " + print() + " failed to determine a type during setup()");
  }
}

void NewInit::prepare() {
  if (!parentScope) {
    throw CompileError("Cannot prepare NewInit " + print() + " with no parent scope");
  } else if (parentScope->getLocalSymbol(m_varname)) {
    throw CompileError("Variable " + m_varname + " already exists");
  } else if (!m_type.get()) {
    throw CompileError("Cannot prepare NewInit " + print() + " which has not determined the new object's Type");
  }

  // Add the symbol to our parent scope
  // TODO don't duplicate the type here; just retrieve it if we need to use it
  parentScope->newSymbol(m_varname, m_type->duplicate());
  log.debug("NewInit " + print() + " adding symbol for " + m_varname + " with type " + m_type->print());
  m_isPrepared = true;
}

// Commit the new object to our enclosing scope, and assign its initial value
void NewInit::compile() {
  if (!m_isPrepared) {
    throw CompileError("Cannot compile NewInit " + print() + " until it has been prepared");
  } else if (!m_type.get()) {
    throw CompileError("Cannot compile NewInit " + print() + " that has no type");
  }
  parentScope->initSymbol(m_varname, getObject());
  parentScope->commitFirst();
  m_isPrepared = false;
}

auto_ptr<Type> NewInit::getType() const {
  return m_type->duplicate();
}

auto_ptr<Object> NewInit::getObject() const {
  if (m_exp) {
    return m_exp->getObject(m_varname);
  }
  Symbol* s = parentScope->getSymbol(m_varname);
  if (!s) {
    throw CompileError("Cannot get Object for NewInit " + print() + "; missing symbol for " + m_varname);
  }
  return s->type->makeDefaultObject(m_varname);
}
