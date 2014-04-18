// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ObjectLiteral_h_
#define _ObjectLiteral_h_

/* Object Literals */

#include "Block.h"
#include "NewInit.h"
#include "Token.h"
#include "TypedNode.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace compiler {

class ObjectLiteral : public TypedNode {
public:
  ObjectLiteral(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_body(NULL) {}

  virtual void setup();
  virtual void compile();
  std::auto_ptr<Object> makeObject(const std::string& newName) const;

private:
  typedef std::vector<NewInit*> init_vec;
  typedef init_vec::const_iterator init_iter;

  // from TypedNode
  virtual void computeType();
  Block* m_body;
  init_vec m_inits;
};

}

#endif // _ObjectLiteral_h_
