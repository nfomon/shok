from Parser import Seq, Or, Plus, Star, Action, MakeParser
import logging
logging.basicConfig(filename="LushParser.log", level=logging.DEBUG)

# Actions
def BlockStart(parser):
  logging.debug("- - BLOCK START")
  parser.top.stack.append(parser.parent)
  parser.top.ast += '{'

def ExpBlockEnd(parser):
  logging.debug("- - EXP BLOCK END")
  parser.top.ast += "exp(" + parser.active.display() + ')}'
  parser.top.stack.pop()

def BlockLazyEnd(parser):
  logging.debug("- - BLOCK LAZY END")
  parser.top.codeblocklazyends += 1

def CodeBlockEnd(parser):
  logging.debug("- - BLOCK END")
  parser.top.ast += '}'
  block = parser.top.stack.pop()
  logging.debug("- - - TOP WAS '%s'" % block)
  block.done = True

# Print out the statement
# If we were signalled by BlockLazyEnd, output the }
def StmtEnd(parser):
  logging.debug("- - STMT END")
  parser.top.ast += parser.display()
  if parser.top.codeblocklazyends > 0:
    parser.top.codeblocklazyends -= 1
    parser.top.ast += '}'
    block = parser.top.stack.pop()
    logging.debug("- - - TOP WAS '%s'" % block)
    block.done = True


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

Exp = []
Atom = Or('atom', [
  Literal,
  'ID',
  Seq('list', ['LBRACKET', Exp, 'RBRACKET'], 'list(%s)', [1]),
  Seq('parens', ['LPAREN', Exp, 'RPAREN'], '(%s)', [1]),
  Seq('object', ['LBRACE', Exp, 'RBRACE'], 'obj{%s}', [1]),
])

Exp = Or('exp', [
  Atom,
  Seq('prefix', [PrefixOp, n, Atom], '%s(%s)', [0,2]),
  Seq('binop', [Atom, BinOp, n, Exp], '%s(%s,%s)', [1,0,3]),
  Seq('prefixbinop', [PrefixOp, n, Atom, BinOp, n, Exp], '%s(%s(%s),%s)', [3,0,2,5])
])


# New statement
Assign1 = Seq('assign1',
  ['ID', 'ASSIGN', n, Exp],
  '=(%s,%s)', [0, 3]
)

Assign2 = Seq('assign2',
  ['ID', 'ASSIGN', n, Exp, 'ASSIGN', n, Exp],
  '=(%s,%s,%s)', [0, 3, 6]
)

NewAssign = Or('newassign', [
  'ID',
  Assign1,
  Assign2,
])

New = Seq('new',
  ['NEW', n, NewAssign,
    Star('news', Seq('newcomma', ['COMMA', n, NewAssign], ',%s', [2]))],
  'new(%s%s);', [2, 3]
)

Renew = Seq('renew',
  ['RENEW', n, Assign1,
    Star('renews', Seq('renewcomma', ['COMMA', n, Assign1], ',%s', [2]))],
  'renew(%s%s);', [2, 3]
)

Del = Seq('del',
  ['DEL', n, 'ID',
    Star('dels', Seq('delcomma', ['COMMA', n, 'ID'], ',%s', [2]))],
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
  [Exp, Star('expliststar', Seq('explistpred', ['COMMA', n, Exp], ',%s', [2]))
])

StmtProcCall = Seq('stmtproccall',
  [IdProp, 'LPAREN', n, ExpList, n, 'RPAREN'],
  'call(%s,%s);', [0, 3]
)


# Branch construct
#Stmt = []

# Care is needed to get this right re: newlines and display()
#If = Or('if', [
#  Seq('if1',
#    ['IF', n, Exp, 'COMMA', Stmt],
#    'if(%s,%s);', [2, 4]),
#  Seq('if2',
#    ['IF', n, Exp, 'COMMA', Stmt, Endl, n, Elif],
#    'if(%s,%s,elif(%s));', [2, 4, 7]),
#  Seq('if3',
#    ['IF', n, Exp, n, CodeBlock
#])
#
#StmtBranch = Or('stmtbranch', [
#  If,
#  Switch,
#])


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
  Seq('stmtstmtnew', [StmtNew, Endl]),
  Seq('stmtstmtassign', [StmtAssign, Endl]),
  Seq('stmtstmtproccall', [StmtProcCall, Endl]),
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  #StmtLoop,
  #StmtBreak,
])
#Stmt = Seq('stmtstmtnew', [StmtNew, Endl])


# Blocks
CodeBlockStmtOrEnd = Or('codeblockstmtorend', [
  'NEWL',
  Action('RBRACE', CodeBlockEnd),
  Action(Stmt, StmtEnd),
])

# The command-line, which can invoke code-blocks or expression-blocks

# Require at least one stmt.  Each stmt could end with an RBRACE as part of its
# Endl -- in this case, call StmtEnd which will pop the stack and emit a }.
CodeBlockBody = Seq('codeblockbody',
  [n, Action(Stmt, StmtEnd), Star('codeblockstmts', CodeBlockStmtOrEnd)],
  '', []
)

CodeBlock = Seq('codeblock',
  [Action('LBRACE', BlockStart), n, CodeBlockBody],
  '', []
)

CmdBlock = Seq('cmdblock',
  [Action('LBRACE', BlockStart),    # Emit { and push CmdBlock on stack
  Or('cmdblockbody', [
    Action(Seq('expblockbody', [Exp, 'RBRACE'], '%s', [0]), ExpBlockEnd),
    CodeBlockBody,
  ])],
  '%s', [1]   # Only used by ExpBlock
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

ProgramInvocation = Seq('programinvocation',
  [Program, Star('programargs', ProgramArg, ',%s'), Endl],
  'cmd(%s%s);', [0, 1]
)

CmdLine = Or('cmdline', [
  'NEWL',
  ProgramInvocation,
  CmdBlock,   # Note: may be an ExpBlock as part of a program invocation
])


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

