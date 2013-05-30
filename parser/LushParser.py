from Parser import Seq, Or, Plus, Star, Action, MakeParser
import logging
logging.basicConfig(filename="LushParser.log", level=logging.DEBUG)   # FAIL

class Future(object):
  def __init__(self,name):
    self.name = name

def Replace(rule,name,new):
  logging.debug("REPLACE r=%s i=%s n=%s new=%s" % (rule.name, rule.items, name, new))
  for i, item in enumerate(rule.items):
    if isinstance(item, Future):
      if item.name == name:
        rule.items[i] = new

# Actions
def BlockStart(parser):
  logging.debug("- - BLOCK START")
  if len(parser.top.stack) > 0 and hasattr(parser.top.stack[-1], 'preblock') and parser.top.stack[-1].preblock:
    parser.top.ast += parser.top.stack[-1].preblock
    parser.top.stack[-1].preblock = ''
  parser.top.stack.append(parser.parent)
  parser.top.ast += '{'

def ExpBlockEnd(parser):
  logging.debug("- - EXP BLOCK END")
  parser.top.ast += "(exp " + parser.parent.displays[0] + ')}'
  parser.top.stack.pop()

def BlockLazyEnd(parser):
  logging.debug("- - BLOCK LAZY END")
  parser.top.codeblocklazyends += 1

def CodeBlockEnd(parser):
  logging.debug("- - BLOCK END")
  parser.top.ast += '}'
  block = parser.top.stack.pop()
  logging.debug("- - - signalling restart of block '%s'" % block)
  block.signalRestart = True

# Print out the statement
# If we were signalled by BlockLazyEnd, output the }
def StmtEnd(parser):
  logging.debug("- - STMT END: %s" % parser.name)
      #if parser.active.parsers[0].name != "stmt":
  if len(parser.top.stack) > 0 and hasattr(parser.top.stack[-1], 'preblock') and parser.top.stack[-1].preblock:
    parser.top.ast += parser.top.stack[-1].preblock
    parser.top.stack[-1].preblock = ''
  parser.top.ast += parser.display()
  if parser.top.codeblocklazyends > 0:
    parser.top.codeblocklazyends -= 1
    parser.top.ast += '}'
    block = parser.top.stack.pop()
    logging.debug("- - - signalling restart of block '%s'" % block)
    block.signalRestart = True

def StmtIfCheck(parser):
  logging.debug("- - STMT IF CHECK")
  stmt = parser.active.doneparsers[0].name
  if stmt == "if" or stmt == "elif":
    parser.top.stack[-1].ifstatement = True
  else:
    parser.top.stack[-1].ifstatement = False

def CmdEnd(parser):
  logging.debug("- - CMD END")
  parser.top.ast += parser.display()

def PreBlock(parser):
  logging.debug("- - PRE BLOCK")
  parser.top.stack[-1].preblock = parser.display()

def ElifCheck(parser):
  logging.debug("- - ELIF ACTION")
  if not hasattr(parser.top.stack[-1], 'ifstatement'):
    raise Exception("Cannot elif without a preceding if or elif statement")
  ifstmt = parser.top.stack[-1].ifstatement
  if not ifstmt:
    raise Exception("Cannot elif without a preceding if or elif statement")

def ElseCheck(parser):
  logging.debug("- - ELSE ACTION")
  if not hasattr(parser.top.stack[-1], 'ifstatement'):
    raise Exception("Cannot else without a preceding if or elif statement")
  ifstmt = parser.top.stack[-1].ifstatement
  if not ifstmt:
    raise Exception("Cannot else without a preceding if or elif statement")

# Basic parsing constructs
n = Star('n',
  'NEWL',
)

Endl = Or('endl', [
  'SEMI',
  'NEWL',
  Action('RBRACE', BlockLazyEnd),
])


# Object property access
# Currently only goes through an identifier
IdProp = Seq('idprop',
  ['ID', Star('props', Seq('prop', ['DOT', 'ID'], '.%s', [1]))],
  '%s%s', [0, 1]
)

# Expressions
Literal = Or('literal', [
  'INT',
  'FIXED',
  'STR',
  'REGEXP',
  'PATH',
  'LABEL',
])

PrefixOp = Or('prefixop', [
  '-',
])

BinOp = Or('binop', [
  '+',
  '-',
  '*',
  '/',
  'USEROP',
])

List = Seq('list',
  ['LBRACKET', Future('Exp'), 'RBRACKET'],
  '(list %s)', [1]
)

Parens = Seq('parens',
  ['LPAREN', Future('Exp'), 'RPAREN'],
  '(paren %s)', [1]
)

Object = Seq('object',
  ['LBRACE', Future('Exp'), 'RBRACE'],
  '{obj %s}', [1]
)

Atom = Or('atom', [
  Literal,
  'ID',
  List,
  Parens,
  Object,
])

Exp = Or('exp', [
  Atom,
  Seq('prefix', [PrefixOp, n, Atom], '(%s %s)', [0,2]),
  Seq('binop', [Atom, BinOp, n, Future('Exp')], '(%s %s %s)', [1,0,3]),
  Seq('prefixbinop', [PrefixOp, n, Atom, BinOp, n, Future('Exp')], '(%s (%s %s) %s)', [3,0,2,5])
])


# New statements
Assign1 = Seq('assign1',
  ['ID', 'EQUALS', n, Exp],
  '(= %s %s)', [0, 3]
)

Assign2 = Seq('assign2',
  ['ID', 'EQUALS', n, Exp, 'EQUALS', n, Exp],
  '(= %s %s %s)', [0, 3, 6]
)

# New
NewAssign = Or('newassign', [
  'ID',
  Assign1,
  Assign2,
])

New = Seq('new',
  ['NEW', n, NewAssign,
    Star('news', Seq('commanew', ['COMMA', n, NewAssign], ' %s', [2]))],
  '(new %s%s);', [2, 3]
)

# Renew
Renew = Seq('renew',
  ['RENEW', n, Assign1,
    Star('renews', Seq('commarenew', ['COMMA', n, Assign1], ' %s', [2]))],
  '(renew %s%s);', [2, 3]
)

# Del
Del = Seq('del',
  ['DEL', n, 'ID',
    Star('dels', Seq('commadel', ['COMMA', n, 'ID'], ' %s', [2]))],
  '(del %s%s);', [2, 3]
)


StmtNew = Or('stmtnew', [
  New,
  Renew,
  Del,
])


# Assignment statements
StmtAssign = Seq('stmtassign',
  [IdProp, Or('assignop', [
    'EQUALS',
    'PLUSEQUALS',
    'MINUSEQUALS',
    'MULTEQUALS',
    'DIVEQUALS',
    'MODEQUALS',
    'POWEREQUALS',
    'PIPEEQUALS',
    'AMPEQUALS',
    'TILDEEEQUALS',
  ]), n, Exp],
  '(%s %s %s);', [1, 0, 3]
)


# Procedure call statements
ExpList = Seq('explist',
  [Exp, Star('explists', Seq('commaexp', ['COMMA', n, Exp], ' %s', [2]))],
  '%s%s', [0, 1]
)

StmtProcCall = Or('stmtproccall', [
  Seq('proccallargs',
    [IdProp, 'LPAREN', n, ExpList, n, 'RPAREN'],
    '(call %s %s);', [0, 3]),
  Seq('proccallvoid',
    [IdProp, 'LPAREN', n, 'RPAREN'],
    '(call %s);', [0]),
])


# Branch constructs
# If
IfPred = Or('ifpred', [
  Seq('ifline', ['COMMA', Future('Stmt')], '%s', [1]),
  Seq('ifblock', [n, Future('CodeBlock')], '%s', [1]),
])

If = Seq('if',
  [Action(Seq('ifstart', ['IF', n, Exp], '(if %s ', [2]), PreBlock), IfPred],
  '%s);', [1]
)

# Elif
ElifPred = Or('elifpred', [
  Seq('elifline', ['COMMA', Future('Stmt')], '%s', [1]),
  Seq('elifblock', [n, Future('CodeBlock')], '%s', [1]),
])

Elif = Seq('elif',
  [Action(Seq('elifstart', [Action('ELIF', ElifCheck), n, Exp], '(elif %s ', [2]), PreBlock), ElifPred],
  '%s);', [1]
)

# Else
ElsePred = Or('elsepred', [
  Future('Stmt'),
  Seq('elseblock', [n, Future('CodeBlock')], '%s', [1]),
])

Else = Seq('else',
  [Action(Action('ELSE', ElseCheck, '(else ', []), PreBlock), ElsePred],
  '%s);', [1]
)


# Loop constructs
Loop = ''
LabelLoop = ''
Repeat = ''
While = ''
Each = ''

StmtLoop = Or('stmtloop', [
  Loop,
  LabelLoop,
  Repeat,
  While,
  Each,
])


# Break constructs
StmtBreak = Or('stmtbreak', [
  'BREAK',
  Seq('breaklabel',
    ['BREAK', 'LABEL'],
    '(%s %s);', [0, 1]),
  'CONTINUE',
  Seq('continuelabel',
    ['CONTINUE', 'LABEL'],
    '(%s %s);', [0, 1]),
  'RETURN',
  Seq('returnexp',
    ['RETURN', Exp],
    '(%s %s);', [0, 1]),
  Seq('yield',
    ['YIELD', Exp],
    '(%s %s);', [0, 1]),
])


# Statements
Stmt1 = Or('stmt1', [
  StmtNew,
  StmtAssign,
  StmtProcCall,
  Future('CodeBlock'),
  If,
  Elif,
  Else,
  #Future('Switch'),
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  #StmtLoop,
  #StmtBreak,
])
Stmt = Action(Stmt1, StmtIfCheck)


# Blocks
CodeBlockStmtOrEnd = Or('codeblockstmtorend', [
  'NEWL',
  Action('RBRACE', CodeBlockEnd),
  Action(Seq('codeblockstmt', [Stmt, Endl]), StmtEnd),
])

# The command-line, which can invoke code-blocks or expression-blocks

# Require at least one stmt.  Each stmt could end with an RBRACE as part of its
# Endl -- in this case, call StmtEnd which will pop the stack and emit a }.
CodeBlockBody = Seq('codeblockbody',
  [n, Action(Seq('codeblockbodystmt', [Stmt, Endl]), StmtEnd), Star('codeblockstmts', CodeBlockStmtOrEnd)],
  '', []
)

CodeBlock = Seq('codeblock',
  [Action('LBRACE', BlockStart), n, CodeBlockBody],
  '', []
)


# Program invocation
# Necessarily starts with a program name (not an ExpBlock; those are handled by
# CmdBlock).  ProgramArgs are allowed to be ExpBlocks.
Program = 'ID'

ProgramArg = Or('programarg', [
  'ID',
  Seq('programargexpblock',
    ['LBRACE', Exp, 'RBRACE'],
    '{(exp %s)};', [1]),
])

ProgramArgs = Star('programargs',
  ProgramArg,
  ' %s'
)

ExpBlockProgram = Seq('expblockprogram',
  [Exp, Action('RBRACE', ExpBlockEnd), ProgramArgs, Endl],
  '%s', [2]
)

# A CmdBlock is when a { occurs at the start of a commandline.  We don't yet
# know if it's a codeblock (list of statements) or an expression block (single
# expression which will become the name of a program to invoke).
CmdBlock = Seq('cmdblock',
  [Action('LBRACE', BlockStart),    # Emit { and push CmdBlock on stack
  Or('cmdblockbody', [
    Action(ExpBlockProgram, CmdEnd),
    CodeBlockBody,
  ])],
  '', []
)

ProgramInvocation = Seq('programinvocation',
  [Program, ProgramArgs, Endl],
  '(cmd %s%s);', [0, 1]
)

CmdLine = Or('cmdline', [
  'NEWL',
  Action(ProgramInvocation, CmdEnd),
  CmdBlock,   # Note: may be an ExpBlock as part of a program invocation
])


# Refresh missed rule dependencies
Replace(Exp.items[2], 'Exp', Exp)
Replace(Exp.items[3], 'Exp', Exp)
Replace(List, 'Exp', Exp)
Replace(Parens, 'Exp', Exp)
Replace(Object, 'Exp', Exp)
Replace(IfPred.items[0], 'Stmt', Stmt)
Replace(IfPred.items[1], 'CodeBlock', CodeBlock)
Replace(ElifPred.items[0], 'Stmt', Stmt)
Replace(ElifPred.items[1], 'CodeBlock', CodeBlock)
Replace(ElsePred, 'Stmt', Stmt)
Replace(ElsePred.items[1], 'CodeBlock', CodeBlock)
Replace(Stmt1, 'CodeBlock', CodeBlock)


# Lush
# The LushParser holds some "global" state.  It should also probably do
# something smart the moment it turns bad.
Lush = Star('lush', CmdLine)
def LushParser():
  parser = MakeParser(Lush)
  parser.stack = []
  parser.ast = ''
  parser.codeblocklazyends = 0
  return parser

