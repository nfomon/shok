// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operator */

#include "CompileError.h"

#include <string>
#include <vector>

namespace compiler {

class Operator {
public:
  typedef int Priority;
  static const Priority NO_PRIORITY = -1;

  Operator(const std::string& name, Priority priority)
    : m_name(name), m_priority(priority) {}
  virtual ~Operator() {}

  Priority priority() const {
    return m_priority;
  }

private:
  std::string m_name;
  Priority m_priority;
};

class PrefixOperator : public Operator {
private:
  static Priority priority(const std::string& name) {
    if ("PLUS" == name || "MINUS" == name) {
      return 12;
    }
    throw CompileError("Priority unknown for prefix operator " + name);
  }

public:
  PrefixOperator(const std::string& name)
    : Operator(name, priority(name)) {}
};

class InfixOperator : public Operator {
public:
  enum Assoc {
    LEFT_ASSOC,
    RIGHT_ASSOC
  };

private:
  static Priority priority(const std::string& name) {
    if ("DOT" == name) {
      return 1;
    } else if ("OR" == name || "NOR" == name || "XOR" == name || "XNOR" == name) {
      return 2;
    } else if ("AND" == name) {
      return 3;
    } else if ("EQ" == name || "NE" == name) {
      return 4;
    } else if ("LT" == name || "LE" == name || "GT" == name || "GE" == name) {
      return 5;
    } else if ("USEROP" == name) {
      return 6;
    } else if ("TILDE" == name || "DOUBLETILDE" == name) {
      return 7;
    } else if ("PLUS" == name || "MINUS" == name) {
      return 8;
    } else if ("STAR" == name || "SLASH" == name || "PERCENT" == name) {
      return 9;
    } else if ("CARAT" == name) {
      return 10;
    } else if ("NOT" == name) {
      return 11;
    } else if ("PIPE" == name) {
      return 13;
    } else if ("AMP" == name) {
      return 14;
    } else if ("paren" == name) {
      return 15;
    }
    throw CompileError("Priority unknown for infix operator " + name);
  }

  static Assoc assoc(const std::string& name) {
    if ("CARAT" == name) {
      return RIGHT_ASSOC;
    }
    return LEFT_ASSOC;
  }

public:
  InfixOperator(const std::string& name)
    : Operator(name, priority(name)),
      m_assoc(assoc(name)) {}

  Assoc assoc() const {
    return m_assoc;
  }

private:
  Assoc m_assoc;
};

}

#endif // _Operator_h_
