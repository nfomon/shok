#ifndef _Global_h_
#define _Global_h_

/* Global code scope
 *
 * Functionality mirrors that of Block (regarding CodeBlock behaviour); a
 * CodeBlock with no parent block will defer its operations to us.
 */

#include "Log.h"
//#include "Type.h"

#include <set>
#include <string>

namespace eval {

class Variable;

class Global {
public:
  Global();
  ~Global();

  bool hasVariable(Variable*) const;
  void addVariable(Variable*);

private:
  std::set<Variable*> m_variables;
};

};

#endif // _Global_h_
