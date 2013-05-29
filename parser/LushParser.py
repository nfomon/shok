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
  parser.top.stack.append(parser.parent)
  parser.top.ast += '{'

def ExpBlockEnd(parser):
  logging.debug("- - EXP BLOCK END")
  parser.top.ast += "exp(" + parser.parent.displays[0] + ')}'
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
  logging.debug("- - STMT END")
  parser.top.ast += parser.display()
  if parser.top.codeblocklazyends > 0:
    parser.top.codeblocklazyends -= 1
    parser.top.ast += '}'
    block = parser.top.stack.pop()
    logging.debug("- - - signalling restart of block '%s'" % block)
    block.signalRestart = True

def CmdEnd(parser):
  parser.top.ast += parser.display()

def ElifAction(parser):
  pass

def ElseAction(parser):
  pass

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
  ['ID', Star('prop', ['DOT', 'ID'], '.%s', [1])],
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
  'list(%s)', [1]
)

Parens = Seq('parens',
  ['LPAREN', Future('Exp'), 'RPAREN'],
  '(%s)', [1]
)

Object = Seq('object',
  ['LBRACE', Future('Exp'), 'RBRACE'],
  'obj{%s}', [1]
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
  Seq('prefix', [PrefixOp, n, Atom], '%s(%s)', [0,2]),
  Seq('binop', [Atom, BinOp, n, Future('Exp')], '%s(%s,%s)', [1,0,3]),
  Seq('prefixbinop', [PrefixOp, n, Atom, BinOp, n, Future('Exp')], '%s(%s(%s),%s)', [3,0,2,5])
])


# New statement
Assign1 = Seq('assign1',
  ['ID', 'EQUALS', n, Exp],
  '=(%s,%s)', [0, 3]
)

Assign2 = Seq('assign2',
  ['ID', 'EQUALS', n, Exp, 'EQUALS', n, Exp],
  '=(%s,%s,%s)', [0, 3, 6]
)

NewAssign = Or('newassign', [
  'ID',
  Assign1,
  Assign2,
])

New = Seq('new',
  ['NEW', n, NewAssign,
    Star('news', ['COMMA', n, NewAssign], ',%s', [2])],
  'new(%s%s);', [2, 3]
)

Renew = Seq('renew',
  ['RENEW', n, Assign1,
    Star('renews', ['COMMA', n, Assign1], ',%s', [2])],
  'renew(%s%s);', [2, 3]
)

Del = Seq('del',
  ['DEL', n, 'ID',
    Star('dels', ['COMMA', n, 'ID'], ',%s', [2])],
  'del(%s%s);', [2, 3]
)


StmtNew = Or('stmtnew', [
  New,
  Renew,
  Del,
])


# Assignment statement
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
  '%s(%s,%s);', [1, 0, 3]
)


# Procedure call statement
ExpList = Seq('explist',
  [Exp, Star('explists', ['COMMA', n, Exp], ',%s', [2])]
)

StmtProcCall = Seq('stmtproccall',
  [IdProp, 'LPAREN', n, ExpList, n, 'RPAREN'],
  'call(%s,%s);', [0, 3]
)


# Branch construct
IfPred = Or('ifpred', [
  Future('Stmt'),
  Seq('ifblock', [n, Future('CodeBlock')], '%s', [1]),
])

If = Seq('if',
  ['IF', n, Exp, IfPred],
  'if(%s,%s);', [2, 3]
)


ElifPred = []
# on 'ELIF' or 'ELSE', check if 'IF' just happened.
Elif = Seq('elif',
  [Action('ELIF', ElifAction), Exp, ElifPred],
  'elif(%s,%s);', [2, 3]
)

ElsePred = []
Else = Seq('else',
  [Action('ELSE', ElseAction), ElsePred],
  'else(%s);', [1]
)

StmtBranch = Or('stmtbranch', [
  If,
#  Future('Switch'),
])


# Loop construct
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


# Break construct
StmtBreak = Or('stmtbreak', [
  'BREAK',
  Seq('breaklabel',
    ['BREAK', 'LABEL'],
    '%s(%s);', [0, 1]),
  'CONTINUE',
  Seq('continuelabel',
    ['CONTINUE', 'LABEL'],
    '%s(%s);', [0, 1]),
  'RETURN',
  Seq('returnexp',
    ['RETURN', Exp],
    '%s(%s);', [0, 1]),
  Seq('yield',
    ['YIELD', Exp],
    '%s(%s);', [0, 1]),
])


# Statement
Stmt = Or('stmt', [
  StmtNew,
  StmtAssign,
  StmtProcCall,
  Future('CodeBlock'),
  StmtBranch,
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  #StmtLoop,
  #StmtBreak,
])
#Stmt = Seq('stmtstmtnew', [StmtNew, Endl])


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
    'exp{%s};', [1]),
])

ProgramArgs = Star('programargs',
  ProgramArg,
  ',%s'
)

ExpBlockProgram = Seq('expblockprogram',
  [Exp, Action('RBRACE', ExpBlockEnd), ProgramArgs, Endl],
  '%s', [2]
)

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
  'cmd(%s%s);', [0, 1]
)

CmdLine = Or('cmdline', [
  'NEWL',
  Action(ProgramInvocation, CmdEnd),
  CmdBlock,   # Note: may be an ExpBlock as part of a program invocation
])


# Refresh missed rule dependencies
Replace(Exp, 'Exp', Exp)
Replace(List, 'Exp', Exp)
Replace(Parens, 'Exp', Exp)
Replace(Object, 'Exp', Exp)
Replace(IfPred, 'Stmt', Stmt)
Replace(IfPred, 'CodeBlock', CodeBlock)
Replace(Stmt, 'CodeBlock', CodeBlock)


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

