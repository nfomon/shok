#include "New.h"

#include "EvalError.h"
#include "NewInit.h"

#include <string>
using std::string;

using namespace eval;

void New::complete() {
  // Children are inits
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    NewInit* init = dynamic_cast<NewInit*>(*i);
    if (!init) {
      throw EvalError("New statement's children must all be NewInit nodes");
    }
  }
  m_isComplete = true;
}

void New::evaluate() {
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->evaluate();
  }
}
