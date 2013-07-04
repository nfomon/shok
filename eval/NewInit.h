#ifndef _NewInit_h_
#define _NewInit_h_

/* New statement's variable-initialization helper */

#include "Log.h"
#include "Node.h"
#include "Object.h"
#include "Token.h"
#include "Type.h"
#include "Variable.h"

#include <string>

namespace eval {

class NewInit : public Node {
public:
  NewInit(Log& log, const Token& token)
    : Node(log, token),
      m_var(NULL),
      m_value(NULL) {}
  virtual void complete();
  virtual void evaluate();

private:
  Variable* m_var;
  //Type m_type;
  Variable* m_value;
};

};

#endif // _NewInit_h_
