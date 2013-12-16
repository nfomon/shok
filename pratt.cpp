void Expression::PrattParse(Node* node) {
  Variable* var = dynamic_cast<Variable*>(node);
  Operator* op = dynamic_cast<Operator*>(node);

  Node* stackTop = NULL;
  if (m_stack.size() > 0) {
    Node* stackTop = m_stack.top();   // no pop
  }
  Operator* stackOp = dynamic_cast<Operator*>(stackTop);
  Variable* stackVar = dynamic_cast<Variable*>(stackTop);

  // Ignore the stack; what can we do here
  // We "know" it can't be a binary operator... huh...
  if (!m_infixing) {
    if (var) {
      m_stack.push_back(node);
      m_infixing = true;
      return;
    } else if (op && Operator::CouldBeUnary(op->name)) {
      op->setUnary();
      m_stack.push_back(node);
      // short-circuit the infixing
      return;
    }
    throw EvalError("");
  } else {
    if (op2 && Operator::CouldBeBinary(op2->name)) {
      op2->setBinary();
      Operator::op_prec infix_prec = op2->precedence();
      if (p >= infix_prec.priority) {
        // if we're at the top of the stack, uhh, that's bad?
        // else return top of stack; i.e. skip this guy
        m_infixing = false;
      } else {
        op->addChild(stackTop);
        stackTop->parent = op;
        m_stack.push_back(op, op->priority - (int)infix_prec.assoc);
      }
    } else if (var) {
      m_stack.push_back
    } else if (op) {
    throw EvalError("");
  }

  // Now we want to lookahead.  But what if we were already looking ahead?
  // If the next guy's not a binary op with good precedence, we return
  // ourselves.  ok...


  if (stackOp && stackOp->isUnary) {
    if (op && op->isUnary) {
    }
    // are we at an infix?  it needs to pass the priority check
  }

}

bool m_infixing = false;
std::vector<std::pair<Node*,Operator::op_priority> m_stack;
