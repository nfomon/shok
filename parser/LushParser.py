import MakeRule
from Parser import MakeParser
from ActionParser import Action
from OrParser import Or
from PlusParser import Plus
from SeqParser import Seq
from StarParser import Star
from TerminalParser import Terminal
import logging
logging.basicConfig(filename="LushParser.log", level=logging.DEBUG)

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
  parser.top.ast += "(exp " + parser.parent.displays[1] + ')}'
  parser.top.stack.pop()

def BlockLazyEnd(parser):
  logging.debug("- - BLOCK LAZY END")
  parser.top.codeblocklazyends += 1

def CodeBlockEnd(parser):
  logging.debug("- - BLOCK END")
  parser.top.ast += '}'
  block = parser.top.stack.pop()
  logging.debug("- - - signalling block '%s' to go done" % block)
  block.goDone()

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
    CodeBlockEnd(parser)

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

# Whitespace
# w: optional whitespace (non-newline)
w = Star('w',
  'WS',
  '', []
)

# ws: mandatory whitespace (non-newline)
ws = Plus('ws',
  'WS',
  '', []
)

# wn: optional whitespace preceding a single newline
wn = Seq('wn',
  [w, 'NEWL'],
  '', []
)

# nw: a single newline optionally followed by whitespace
nw = Seq('nw',
  ['NEWL', w],
  '', []
)

# n: any amount of optional whitespace and newlines.  Greedy!
n = Star('n',
  [w, Star('nn', 'NEWL', '', [])],
  '', []
)

# Basic parsing constructs
Endl = Or('endl', [
  Seq('endsemi', [w, 'SEMI'], '', []),
  wn,
  Action(Seq('endbrace', [w, 'RBRACE'], '', []), BlockLazyEnd),
])

CmdCodeBlockEndl = Or('cmdcodeblockendl', [
  Seq('cmdcodeblockendsemi', [w, 'SEMI'], '', []),
  wn,
  Action(Seq('cmdcodeblockendbrace', [w, 'RBRACE', wn], '', []), BlockLazyEnd),
])


# Object property access
# Currently only goes through an identifier
IdProp = Seq('idprop',
  ['ID', Star('props', Seq('prop', [w, 'DOT', n, 'ID'], '.%s', [3]))],
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
  'MINUS',
])

BinOp = Or('binop', [
  'PLUS',
  'MINUS',
  'MULT',
  'DIV',
  'MOD',
  'POWER',
  'PIPE',
  'AMP',
  'TILDE',
  'DOUBLETILDE',
  'LT',
  'LE',
  'GT',
  'GE',
  'EQ',
  'NE',
  'USEROP',
])

List = Seq('list',
  ['LBRACKET', n, Future('ExpList'), n, 'RBRACKET'],
  '(list %s)', [2]
)

Parens = Seq('parens',
  ['LPAREN', n, Future('Exp'), n, 'RPAREN'],
  '(paren %s)', [2]
)

# Object literals don't need to push themselves on a stack or anything,
# since their AST will not come out until at least the whole expression
# is done.
# Care is required because there may be an ambiguity between object
# literals and expblocks.  What do we allow in an object literal?  Just
# new statements?

#Object = Seq('object',
#  ['LBRACE', n, ObjectBody, n, 'RBRACE'],
#  '{obj %s}', [2]
#)

Atom = Or('atom', [
  Literal,
  'ID',
  List,
  Parens,
  #Object,
])

PrefixExp = Seq('prefix',
  [PrefixOp, n, Atom],
  '(%s %s)', [0,2]
)

BinopExp = Seq('binop',
  [Atom, w, BinOp, n, Future('Exp')],
  '(%s %s %s)', [2,0,4]
)

PrefixBinopExp = Seq('prefixbinop',
  [PrefixOp, n, Atom, w, BinOp, n, Future('Exp')],
  '(%s (%s %s) %s)', [4,0,2,6]
)

Exp = Or('exp', [
  Atom,
  PrefixExp,
  BinopExp,
  PrefixBinopExp,
])


# New statements
Assign1 = Seq('assign1',
  ['ID', w, 'EQUALS', n, Exp],
  '(= %s %s)', [0, 4]
)

Assign2 = Seq('assign2',
  ['ID', w, 'EQUALS', n, Exp, w, 'EQUALS', n, Exp],
  '(= %s %s %s)', [0, 4, 8]
)

# New
NewAssign = Or('newassign', [
  'ID',
  Assign1,
  Assign2,
])

New = Seq('new',
  ['NEW', n, NewAssign,
    Star('news', Seq('commanew', [w, 'COMMA', n, NewAssign], ' %s', [3]))],
  '(new %s%s);', [2, 3]
)

# Renew
Renew = Seq('renew',
  ['RENEW', n, Assign1,
    Star('renews', Seq('commarenew', [w, 'COMMA', n, Assign1], ' %s', [3]))],
  '(renew %s%s);', [2, 3]
)

# Del
Del = Seq('del',
  ['DEL', n, 'ID',
    Star('dels', Seq('commadel', [w, 'COMMA', n, 'ID'], ' %s', [3]))],
  '(del %s%s);', [2, 3]
)


StmtNew = Or('stmtnew', [
  New,
  Renew,
  Del,
])


# Assignment statements
StmtAssign = Seq('stmtassign',
  [IdProp, w, Or('assignop', [
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
  '(%s %s %s);', [2, 0, 4]
)


# Procedure call statements
ExpList = Seq('explist',
  [Exp, Star('explists', Seq('commaexp', [w, 'COMMA', n, Exp], ' %s', [3]))],
  '%s%s', [0, 1]
)

# Debate: Allow whitespace after the function name (IdProp) but before
# the first paren?
StmtProcCall = Or('stmtproccall', [
  Seq('proccallargs',
    [IdProp, w, 'LPAREN', n, ExpList, n, 'RPAREN'],
    '(call %s %s);', [0, 4]),
  Seq('proccallvoid',
    [IdProp, w, 'LPAREN', n, 'RPAREN'],
    '(call %s);', [0]),
])


# Branch constructs
# If
IfPred = Or('ifpred', [
  Seq('ifline', ['COMMA', w, Future('Stmt')], '%s', [2]),
  Seq('ifblock', [n, Future('CodeBlock')], '%s', [1]),
])

If = Seq('if',
  [Action(Seq('ifstart', ['IF', n, Exp], '(if %s ', [2]), PreBlock), IfPred],
  '%s);', [1]
)

# Elif
ElifPred = Or('elifpred', [
  Seq('elifline', ['COMMA', w, Future('Stmt')], '%s', [2]),
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
    ['BREAK', ws, 'LABEL'],
    '(%s %s);', [0, 2]),
  'CONTINUE',
  Seq('continuelabel',
    ['CONTINUE', ws, 'LABEL'],
    '(%s %s);', [0, 2]),
  'RETURN',
  Seq('returnexp',
    ['RETURN', ws, Exp],
    '(%s %s);', [0, 2]),
  Seq('yield',
    ['YIELD', ws, Exp],
    '(%s %s);', [0, 2]),
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
  StmtBreak,
])
Stmt = Action(Stmt1, StmtIfCheck)


# Blocks
# A code block may have 0 or more statements.  Each stmt could end with an RBRACE as part of its
# Endl -- in this case, call StmtEnd which will pop the stack and emit a }.
CodeBlockBody = Star('codeblockbody',
  Or('codeblockbodystmts', [
    n,
    Action('RBRACE', CodeBlockEnd),
    Action(Seq('codeblockbodystmt', [Stmt, Endl]), StmtEnd),
  ]),
  '', []
)

# A codeblock issued from a commandline requires a newline after the end
# of the block.
CmdCodeBlockBody = Star('cmdcodeblockbody',
  Or('cmdcodeblockbodystmts', [
    n,
    Action(Seq('cmdcodeblockend', ['RBRACE', wn], '', []), CodeBlockEnd),
    Action(Seq('cmdcodeblockbodystmt', [Stmt, CmdCodeBlockEndl]), StmtEnd),
  ]),
  '', []
)

CodeBlock = Seq('codeblock',
  [Action('LBRACE', BlockStart), CodeBlockBody],
  '', []
)


# Program invocation
# We should be getting the raw text from the user instead of
# back-tracking from these tokens.  Eventually...
ProgramArg = Or('programarg', [
  'ID',
  'PLUS',
  'MINUS',
  'INT',
  Seq('programargexpblock',
    ['LBRACE', w, Exp, w, 'RBRACE'],
    '{(exp %s)}', [2]),
])

ProgramArgs = Star('programargs',
  Seq('wsarg', [ws, Plus('arg', ProgramArg, ' %s')], '%s', [1]),
  ',%s'
)

ProgramMore = Star('programmore',
  ProgramArg,
  ' %s'
)

# Necessarily starts with a program name (not an ExpBlock; those are handled by
# CmdBlock).  ProgramArgs are allowed to be ExpBlocks.
Program = Seq('program',
  ['ID', ProgramMore],
  '%s%s', [0, 1]
)

ExpBlockProgram = Seq('expblockprogram',
  [w, Exp, w, Action('RBRACE', ExpBlockEnd), ProgramMore, ProgramArgs, Endl],
  '%s%s;', [4, 5]
)

# A CmdBlock is when a { occurs at the start of a commandline.  We don't yet
# know if it's a codeblock (list of statements) or an expression block (single
# expression which will become the name of a program to invoke).
CmdBlock = Seq('cmdblock',
  [w,
  Action('LBRACE', BlockStart),    # Emit { and push CmdBlock on stack
  Or('cmdblockbody', [
    Action(ExpBlockProgram, CmdEnd),
    CmdCodeBlockBody,
  ])],
  '', []
)

ProgramInvocation = Seq('programinvocation',
  [w, Program, ProgramArgs, Endl],
  '(cmd %s%s);', [1, 2]
)

CmdLine = Or('cmdline', [
  wn,
  Action(ProgramInvocation, CmdEnd),
  CmdBlock,   # Note: may be an ExpBlock as part of a program invocation
])


# Refresh missed rule dependencies
Replace(BinopExp, 'Exp', Exp)
Replace(PrefixBinopExp.items[3], 'Exp', Exp)
Replace(List, 'ExpList', ExpList)
Replace(Parens, 'Exp', Exp)
#Replace(Object, 'Exp', Exp)
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
Lush = Star('lush', CmdLine, neverGoBad=True)
def LushParser():
  parser = MakeParser(Lush)
  parser.stack = []
  parser.ast = ''
  parser.codeblocklazyends = 0
  return parser

