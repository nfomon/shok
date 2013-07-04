import MakeRule
from Parser import MakeParser
from ActionParser import Action
from OrParser import Or
from PlusParser import Plus
from SeqParser import Seq
from StarParser import Star
from TerminalParser import Terminal
from ValueParser import ValueTerminal
import logging

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
def PreBlockCheck(parser):
  if len(parser.top.stack) > 0 and hasattr(parser.top.stack[-1], 'preblock') and parser.top.stack[-1].preblock:
    parser.top.ast += parser.top.stack[-1].preblock
    parser.top.stack[-1].preblock = ''

def BlockStart(parser):
  logging.debug("- - BLOCK START")
  PreBlockCheck(parser)
  parser.top.stack.append(parser.parent)
  parser.top.ast += '{'

def CmdBlockStart(parser):
  logging.debug("- - CMD BLOCK START")
  parser.top.stack.append(parser.parent)
  parser.top.ast += '[{'

def CmdEnd(parser):
  logging.debug("- - CMD END")
  parser.top.ast += parser.display()

# ExpBlockEnd gets called by the closing brace of an ExpBlockProgram, as soon
# as the closing brace happens -- but not before the rule is actually done.  We
# can pull out index [1] from the rule which is the Exp -- it will have already
# been parsed at the time of the ExpBlockEnd call.
def ExpBlockEnd(parser):
  logging.debug("- - EXP BLOCK END")
  parser.top.ast += '(exp ' + parser.parent.displays[1] + ')}'
  parser.top.stack.pop()

def BlockLazyEnd(parser):
  logging.debug("- - BLOCK LAZY END")
  parser.top.codeblocklazyends += 1

def CodeBlockEnd(parser):
  logging.debug("- - CODE BLOCK END")
  parser.top.ast += '}'
  block = parser.top.stack.pop()
  logging.debug("- - - signalling block '%s' to go done" % block)
  if block.name == 'cmdblock':
    parser.top.ast += ']'
  else:
    parser.top.stack[-1].codeblock = True
  block.goDone()

# Something happened that indicates a codeblock's end has *not* just happened,
# so we should deny a semicolon that might have otherwise been allowed after a
# codeblockend.
def NoCodeBlockEndSemi(parser):
  logging.debug("- - NO CODE BLOCK END SEMI")
  parser.top.stack[-1].codeblock = False

# Print out the statement
# If we were signalled by BlockLazyEnd, output the } and maybe ]
def StmtEnd(parser):
  logging.debug("- - STMT END: %s" % parser.name)
      #if parser.active.parsers[0].name != "stmt":
  PreBlockCheck(parser)
  parser.top.ast += parser.display()
  NoCodeBlockEndSemi(parser)
  if parser.top.codeblocklazyends > 0:
    parser.top.codeblocklazyends -= 1
    CodeBlockEnd(parser)

def PreBlock(parser):
  logging.debug("- - PRE BLOCK")
  parser.top.stack[-1].preblock = parser.display()

# Whether or not an If or Elif statement just happened: record this in the top
# of stack parser
def StmtIfCheck(parser):
  logging.debug("- - STMT IF CHECK")
  stmt = parser.active.doneparsers[0].name
  if stmt == "if" or stmt == "elif":
    parser.top.stack[-1].ifstatement = True
  else:
    parser.top.stack[-1].ifstatement = False

# An Elif statement just happened: ensure it was preceded by an If or Elif
def ElifCheck(parser):
  logging.debug("- - ELIF CHECK")
  if not hasattr(parser.top.stack[-1], 'ifstatement'):
    raise Exception("Cannot elif without a preceding if or elif statement")
  ifstmt = parser.top.stack[-1].ifstatement
  if not ifstmt:
    raise Exception("Cannot elif without a preceding if or elif statement")

# An Else statement just happened: ensure it was preceded by an If or Elif
def ElseCheck(parser):
  logging.debug("- - ELSE CHECK")
  if not hasattr(parser.top.stack[-1], 'ifstatement'):
    raise Exception("Cannot else without a preceding if or elif statement")
  ifstmt = parser.top.stack[-1].ifstatement
  if not ifstmt:
    raise Exception("Cannot else without a preceding if or elif statement")

# A bare SEMI just happened: ensure it was preceded by the } of a CodeBlock
def SemiCheck(parser):
  logging.debug("- - SEMI CHECK")
  if not hasattr(parser.top.stack[-1], 'codeblock') or not parser.top.stack[-1].codeblock:
    parser.parse = parser.goBad
    parser.bad = True
    parser.done = False


# Language token groups
Keyword = Or('keyword', [
  # Symbol table modifiers
  'NEW', 'RENEW', 'DEL', 'ISVAR',
  # Functions
  'VOID', 'RETURN', 'YIELD',
  # Branch constructs
  'IF', 'ELIF', 'ELSE', 'SWITCH', 'CASE', 'DEFAULT',
  # Loop constructs
  'WHILE', 'LOOP', 'TIMES', 'EACH', 'IN', 'WHERE', 'BREAK', 'CONTINUE',
  # Logical operators
  'NOT', 'NOR', 'AND', 'OR', 'XOR', 'XNOR',
  # Keyword operators
  'TYPEOF',
])

Op = Or('op', [
  # Equality operators
  'LT', 'LE', 'GT', 'GE', 'EQ', 'NE',
  # Numeric operators
  'PLUS', 'MINUS', 'STAR', 'SLASH', 'PERCENT', 'CARAT',
  # Object operators
  'PIPE', 'AMP', 'TILDE', 'DOUBLETILDE',
  # Assignment operators
  'EQUALS', 'PLUSEQUALS', 'MINUSEQUALS', 'STAREQUALS', 'SLASHEQUALS',
  'PERCENTEQUALS', 'CARATEQUALS', 'PIPEEQUALS', 'AMPEQUALS', 'TILDEEQUALS',
  # Cast
  'ARROW',
  # Delimeters
  'LPAREN', 'RPAREN',
  'LBRACKET', 'RBRACKET',
  'LBRACE', 'RBRACE',
  'COMMA', 'DOT', 'COLON',
])

class CmdT(Terminal):
  def __init__(self,name,msg):
    Terminal.__init__(self, name, None, msg)

CmdOp = Or('cmdop', [
  # Equality operators -- disallow LT, LE, GT, GE
  CmdT('EQ', '=='),
  CmdT('NE', '!='),
  # Numeric operators
  CmdT('PLUS', '+'),
  CmdT('MINUS', '-'),
  CmdT('STAR', '*'),
  CmdT('SLASH', '/'),
  CmdT('PERCENT', '%'),
  CmdT('CARAT', '^'),
  # Object operators -- disallow PIPE, AMP
  CmdT('TILDE', '~'),
  CmdT('DOUBLETILDE', '~~'),
  # Assignment operators -- disallow PIPEEQUALS, AMPEQUALS
  CmdT('EQUALS', '='),
  CmdT('PLUSEQUALS', '+='),
  CmdT('MINUSEQUALS', '-='),
  CmdT('STAREQUALS', '*='),
  CmdT('SLASHEQUALS', '/='),
  CmdT('PERCENTEQUALS', '%='),
  CmdT('CARATEQUALS', '^='),
  CmdT('PIPEEQUALS', '|='),
  CmdT('AMPEQUALS', '&='),
  CmdT('TILDEEQUALS', '~='),
  # Cast -- disallow ARROW
  # Delimeters: disallow () {}
  CmdT('LBRACKET', '['),
  CmdT('RBRACKET', ']'),
  CmdT('COMMA', ','),
  CmdT('DOT', '.'),
  CmdT('COLON', ':'),
])

class PathT(ValueTerminal):
  def __init__(self,name,msg):
    ValueTerminal.__init__(self, name, None, msg)

# Operators that are safe for use by path literals in code
# Anything we allow here will be eaten aggressively by a path literal,
# overriding any possible other meaning.  If you actually want to use the
# regular / or . operators directly beside a path literal, enclose the path
# literal in ()'s.
PathOp = Or('cmdop', [
  # Equality operators -- disallow < <= > >= == !=
  # Numeric operators -- disallow + - * % ^
  PathT('SLASH', '/'),
  # Object operators -- disallow | & ~ ~~
  # Assignment operators -- disallow = += -= *= /= %= ^= |= &= ~=
  # Cast -- disallow ARROW
  # Delimeters: disallow () [] {} , :
  PathT('DOT', '.'),
])

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
End = Or('end', [
  wn,
  Seq('endsemi', [w, 'SEMI'], '', []),
])

Endl = Or('endl', [
  End,
  Action(Seq('endbrace', [w, 'RBRACE'], '', []), BlockLazyEnd),
])

CmdStmtEndl = Or('cmdstmtendl', [
  End,
  Action(Seq('endbrace', [w, 'RBRACE', End], '', []), BlockLazyEnd),
])

# Object property access
# Currently only goes through an identifier
IdProp = Seq('idprop',
  ['ID', Star('props', Seq('prop', [w, 'DOT', n, 'ID'], '.%s', [3]))],
  '%s%s', [0, 1]
)


# Expressions
CmdLiteral = Or('cmdliteral', [
  # disallow REGEXP, LABEL
  ValueTerminal('INT'),
  ValueTerminal('FIXED'),
  ValueTerminal('STR'),
  ValueTerminal('ID'),
])

PathToken = Or('pathtoken', [
  Keyword,
  PathOp,
  CmdLiteral,
])

PathPart = Star('pathpart', PathToken)

Path = Or('path', [
  Seq('pathstartslash', ['SLASH', PathPart], '/%s', [1]),
  Seq('pathendslash', [PathPart, 'SLASH'], '%s/', [0]),
  Seq('pathstartdotslash', ['DOT', 'SLASH', PathPart], './%s', [2]),
  Seq('pathstartdotdotslash', ['DOT', 'DOT', 'SLASH', PathPart], '../%s', [3]),
  Seq('pathstarttildeslash', ['TILDE', 'SLASH', PathPart], '~/%s', [2]),
], '(path %s)')

Literal = Or('literal', [
  'INT', 'FIXED', 'STR',
  Path,
  'REGEXP', 'LABEL', 'ID',
])

PrefixOp = Or('prefixop', [
  # Numeric prefix unary operators
  'MINUS',
])

BinOp = Or('binop', [
  # Equality binary operators
  'LT', 'LE', 'GT', 'GE', 'EQ', 'NE',
  # Numeric binary operators
  'PLUS', 'MINUS', 'STAR', 'SLASH', 'PERCENT', 'CARAT',
  # Object binary operators
  'PIPE', 'AMP', 'TILDE', 'DOUBLETILDE',
  # User binary operators
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
  '%s %s', [0, 4]
)

Assign2 = Seq('assign2',
  ['ID', w, 'EQUALS', n, Exp, w, 'EQUALS', n, Exp],
  '%s %s %s', [0, 4, 8]
)

# New
NewAssign = Or('newassign', [
  'ID',
  Assign1,
  Assign2,
], '(init %s)')

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
    'STAREQUALS',
    'SLASHEQUALS',
    'PERCENTEQUALS',
    'CARATEQUALS',
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
    '%s %s', [0, 4]),
  Seq('proccallvoid',
    [IdProp, w, 'LPAREN', n, 'RPAREN'])
], '(call %s);')


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
Stmt1 = Or('stmt', [
  StmtNew,
  StmtAssign,
  StmtProcCall,
  If,
  Elif,
  Else,
  #Future('Switch'),
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  #StmtLoop,
  StmtBreak,
])
Stmt = Action(Stmt1, StmtIfCheck)

CodeBlockSemi = Action(Seq('codeblocksemi', [w, 'SEMI'], '', []), SemiCheck)

BlockOrStmt = Or('blockorstmt', [
  Seq('stmtendl', [Stmt, Endl]),
  Future('CodeBlock'),
  CodeBlockSemi,
])

BlockOrCmdStmt = Or('blockorcmdstmt', [
  Seq('stmtcmdstmtendl', [Stmt, CmdStmtEndl]),
  Future('CodeBlock'),
  CodeBlockSemi,
])


# Blocks
# A code block may have 0 or more statements.  Each stmt could end with an
# RBRACE as part of its Endl -- in this case, call StmtEnd which will pop the
# stack and emit a }.
CodeBlockBody = Star('codeblockbody',
  Or('codeblockbodystmts', [
    Action(n, NoCodeBlockEndSemi),
    Action('RBRACE', CodeBlockEnd),
    Action(BlockOrStmt, StmtEnd),
  ]),
  '', []
)

CodeBlock = Seq('codeblock',
  [Action('LBRACE', BlockStart), CodeBlockBody],
  '', []
)

# A codeblock issued from a commandline requires a newline or semicolon after
# the end of the block.
CmdCodeBlock = Star('cmdcodeblock',
  Or('cmdcodeblockbody', [
    Action(n, NoCodeBlockEndSemi),
    Action(Seq('cmdcodeblockend', ['RBRACE', End], '', []), CodeBlockEnd),
    Action(BlockOrCmdStmt, StmtEnd),
  ]),
  '', []
)

# Program invocation

# Basic building block -- cannot contain an ExpBlock
# OK to include "not cmdline approvide" symbols (operators) -- these
# will have been parsed out earlier, and asking for their cmdText will
# fail if we do accidentally catch any.
ProgramBasic = Or('programbasic', [
  Keyword,    # These will be output verbatim in both code and cmd mode
  CmdOp,      # Operators spilled out for commands
  CmdLiteral, # Literals spilled out for commands
])

ProgramArg = Star('programarg',
  Or('programarg1', [
    ProgramBasic,
    Seq('programargexpblock',
      ['LBRACE', w, Exp, w, 'RBRACE'],
      '{(exp %s)}', [2]),
    # TODO: redirection, pipes, background-job, etc.
  ])
)

ProgramArgs = Star('programargs',
  Seq('wsarg', [ws, ProgramArg], ' %s', [1]),
)

# Just the name of a program to invoke, without its arguments.
# Necessarily starts with a program name (not an ExpBlock; those are
# handled by CmdBlock).  ProgramArg is allowed to contain ExpBlocks.
Program = Seq('program',
  [ProgramBasic, ProgramArg],
  '%s%s', [0, 1]
)

ExpBlockProgram = Action(
  Seq('expblockprogram',
    [w, Exp, w, Action('RBRACE', ExpBlockEnd), ProgramArg, ProgramArgs, Endl],
    '%s%s]', [4, 5]
  ), CmdEnd
)

# A CmdBlock is when a { occurs at the start of a commandline.  We don't yet
# know if it's a codeblock (list of statements) or an expression block (single
# expression which will become (at least part of) the name of a program to
# invoke).
#
# Since the CmdBlock is pushed on the stack, it may be "forcefully completed"
# by either the ExpBlockProgram or the CmdCodeBlock.  So don't put an Action on
# either of those guys here -- let them do it themselves, and they will set the
# CmdBlock as complete.
#
# Note though that for any CmdBlock we'll want to emit '}' eventually and ']'
# when it's done :)
CmdBlock = Seq('cmdblock',
  [w,
  Action('LBRACE', CmdBlockStart),    # Emit [{ and push CmdBlock on stack
  Or('cmdblockbody', [
    ExpBlockProgram,
    CmdCodeBlock,
  ])],
  '', []
)

ProgramInvocation = Seq('programinvocation',
  [w, Program, ProgramArgs, Endl],
  '[%s%s]', [1, 2]
)

CmdLine = Or('cmdline', [
  wn,
  Action(ProgramInvocation, CmdEnd),
  CmdBlock,
])


# Refresh missed rule dependencies
Replace(BinopExp, 'Exp', Exp)
Replace(PrefixBinopExp, 'Exp', Exp)
Replace(List, 'ExpList', ExpList)
Replace(Parens, 'Exp', Exp)
#Replace(Object, 'Exp', Exp)
Replace(IfPred.items[0], 'Stmt', Stmt)
Replace(IfPred.items[1], 'CodeBlock', CodeBlock)
Replace(ElifPred.items[0], 'Stmt', Stmt)
Replace(ElifPred.items[1], 'CodeBlock', CodeBlock)
Replace(ElsePred, 'Stmt', Stmt)
Replace(ElsePred.items[1], 'CodeBlock', CodeBlock)
Replace(BlockOrStmt, 'CodeBlock', CodeBlock)
Replace(BlockOrCmdStmt, 'CodeBlock', CodeBlock)


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

