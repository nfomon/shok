#ifndef _Variable_h_
#define _Variable_h_

/* Variable */

#include "Log.h"
#include "Node.h"
#include "Object.h"
#include "Token.h"
//#include "Type.h"

#include <string>

namespace eval {

class Variable : public Node {
public:
  Variable(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void setup();
  virtual void analyze();
  virtual void evaluate();

  //std::string varname() { return value; }

private:
  Object* m_object;
  //Type m_type;
};

};

#endif // _Variable_h_
