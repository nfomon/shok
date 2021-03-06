// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Returns_h_
#define _Returns_h_

/* Returns
 *
 * Function return type, which (eventually) might have a name.
 */

#include "RootNode.h"
#include "Token.h"
#include "TypeSpec.h"

#include "util/Log.h"

namespace compiler {

class Returns : public Node {
public:
  Returns(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      m_typeSpec(NULL) {}
  virtual void setup();

  std::string getName() const;
  std::auto_ptr<Type> getType() const;

private:
  TypeSpec* m_typeSpec;
};

}

#endif // _Returns_h_
