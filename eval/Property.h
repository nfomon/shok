// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Property_h_
#define _Property_h_

/* Object Property
 *
 * I'm using "Property" to mean either a member-lookup on an object:
 *    date.year
 * Or a nested-deeper member lookup which needs more machinery:
 *    simba.father.isKing
 *
 * A Property, like a Variable, is really only used for lookups; it
 * stores the names of the objects, but an operation or statement will
 * actually look up the objects and perform whatever operation with
 * them.  So the Property provides getObjectName(), getPropertyName(),
 * and getSubProperty().  getPropertyName() should only be called if
 * isTerminal() is true, and the opposite for getSubProperty().
 */

#include "Log.h"
#include "Node.h"
#include "Object.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class Property : public Node {
public:
  Property(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      m_subProperty(NULL) {}
  virtual void setup();
  virtual void evaluate();

  std::string getObjectName() const { return m_objectName; }
  bool isTerminal() const { return !m_subProperty; }
  std::string getPropertyName() const;
  Property* getSubProperty() const;

private:
  std::string m_objectName;
  std::string m_propertyName;
  Property* m_subProperty;
};

};

#endif // _Property_h_
