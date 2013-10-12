# Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
# directory of this distribution and at http://lush-shell.org/copyright.html

# Parser for a mini language, to experiment with the parser framework.

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

# Some silly token-construction convenience "macros"
# Terminal constructor with second arg bound to 'msg'
class TMsg(Terminal):
  def __init__(self,name,msg):
    Terminal.__init__(self, name, None, msg)

# Language token groups
Keyword = Or('keyword', [
  # Symbol table modifiers
  TMsg('NEW', 'new'), TMsg('RENEW', 'renew'), TMsg('DEL', 'del'),
  TMsg('ISVAR', 'isvar'), TMsg('TYPEOF', 'typeof'),
  # Functions
  TMsg('VOID', 'void'), TMsg('RETURN', 'return'), TMsg('YIELD', 'yield'),
  # Branch constructs
  TMsg('IF', 'if'), TMsg('ELIF', 'elif'), TMsg('ELSE', 'else'),
  TMsg('SWITCH', 'switch'), TMsg('CASE', 'case'), TMsg('DEFAULT', 'default'),
  # Loop constructs
  TMsg('WHILE', 'while'), TMsg('LOOP', 'loop'), TMsg('TIMES', 'times'),
  TMsg('EACH', 'each'), TMsg('IN', 'in'), TMsg('WHERE', 'where'),
  TMsg('BREAK', 'break'), TMsg('CONTINUE', 'continue'),
  # Logical operators
  TMsg('NOT', 'not'), TMsg('NOR', 'nor'), TMsg('AND', 'and'), TMsg('OR', 'or'),
  TMsg('XOR', 'xor'), TMsg('XNOR', 'xnor'),
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
  'LPAREN', 'RPAREN', 'LBRACKET', 'RBRACKET', 'LBRACE', 'RBRACE',
  'COMMA', 'DOT', 'COLON',
])

CmdOp = Or('cmdop', [
  # Equality operators -- disallow LT, LE, GT, GE
  TMsg('EQ', '=='), TMsg('NE', '!='),
  # Numeric operators
  TMsg('PLUS', '+'), TMsg('MINUS', '-'), TMsg('STAR', '*'), TMsg('SLASH', '/'),
  TMsg('PERCENT', '%'), TMsg('CARAT', '^'),
  # Object operators -- disallow PIPE, AMP
  TMsg('TILDE', '~'), TMsg('DOUBLETILDE', '~~'),
  # Assignment operators -- disallow PIPEEQUALS, AMPEQUALS
  TMsg('EQUALS', '='), TMsg('PLUSEQUALS', '+='), TMsg('MINUSEQUALS', '-='),
  TMsg('STAREQUALS', '*='), TMsg('SLASHEQUALS', '/='),
  TMsg('PERCENTEQUALS', '%='), TMsg('CARATEQUALS', '^='),
  TMsg('PIPEEQUALS', '|='), TMsg('AMPEQUALS', '&='), TMsg('TILDEEQUALS', '~='),
  # Cast -- disallow ARROW
  # Delimeters: disallow () {}
  TMsg('LBRACKET', '['), TMsg('RBRACKET', ']'), TMsg('COMMA', ','),
  TMsg('DOT', '.'), TMsg('COLON', ':'),
])

# Operators that are safe for use by path literals in code
# Anything we allow here will be eaten aggressively by a path literal,
# overriding any possible other meaning.  If you actually want to use the
# regular / or . operators directly beside a path literal, enclose the path
# literal in ()'s.
PathOp = Or('cmdop', [
  # Equality operators -- disallow < <= > >= == !=
  # Numeric operators -- disallow + - * % ^
  TMsg('SLASH', '/'),
  # Object operators -- disallow | & ~ ~~
  # Assignment operators -- disallow = += -= *= /= %= ^= |= &= ~=
  # Cast -- disallow ARROW
  # Delimeters: disallow () [] {} , :
  TMsg('DOT', '.'),
])

# Whitespace
# w: optional whitespace (non-newline)
w = Star('w',
  'WS',
  ''
)

# ws: mandatory whitespace (non-newline)
ws = Plus('ws',
  'WS',
  ' '
)

# wn: optional whitespace preceding a single newline
wn = Seq('wn', [w, ('NEWL','')])

# nw: a single newline optionally followed by whitespace
#nw = Seq('nw', [('NEWL',''), (w,'')])

# n: any amount of optional whitespace and newlines.  Greedy!
n = Star('n',
  [w, Star('nn', 'NEWL', '')],
  ''
)

# Basic parsing constructs
End = Or('end', [
  wn,
  Seq('endsemi', [w, 'SEMI']),
], '')

Endl = Or('endl', [
  End,
  Seq('endbrace', [w, ('RBRACE','}')]),
])

# Variable or object property access
# Currently, property access can only go through an identifier.
Var = Seq('var',
  [('ID','(var %s'),
   (Star('props', Seq('prop', [w, ('DOT',''), n, ('ID','%s')]),' %s'),'%s)')
])


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
  Seq('pathstartslash', [('SLASH','/'), PathPart]),
  #Seq('pathendslash', [PathPart, ('SLASH','/')]),  # requires parser upgrade
  Seq('pathstartdotslash', [('DOT','.'), ('SLASH','/'), PathPart]),
  Seq('pathstartdotdotslash', [('DOT','.'), ('DOT','.'), ('SLASH','/'), PathPart]),
  Seq('pathstarttildeslash', [('TILDE','~'), ('SLASH','/'), PathPart]),
], '(path %s)')

Literal = Or('literal', [
  'INT', 'FIXED', 'STR',
  Path,
  'REGEXP', 'LABEL', Var,
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
  [('LBRACKET','(list '), n, Future('ExpList'), n, ('RBRACKET',')')]
)

#Parens = Seq('parens',
#  [('LPAREN','(paren '), n, Future('SubExp'), n, ('RPAREN',')')]
#)

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

####TypeList = Seq('typelist',
####  [Future('Type'), Star('typelists',
####    Seq('commatype', [w, 'COMMA', n, Future('Type')], ' %s', [3]))
####  ], '%s%s', [0, 1]
####)

####Signature = Or('signatures', [
####  TMsg('AT', '-sigat-'),
#  Seq('sigargs', ['AT', w, TypeList], '(args %s) ', [2]),
#  Seq('sigret',
#    ['AT', w, 'ARROW', w, Future('Type')],
#    '(returns %s) ', [4]),
#  Seq('sigargsret',
#    ['AT', w, TypeList, w, 'ARROW', w, Future('Type')],
#    '(args %s) (returns %s) ', [2, 6]
#  ),
####])

####MooBlock = Seq('mooblock',
####  [w, Future('CodeBlock')],
####  '%s', [1]
####)
####
####Function = Seq('function',
####  [Action(Seq('prefunc', [Signature, w]), PreBlock), Future('CodeBlock')],
####  '(func %s%s)', [0, 1]
####)

#Function = Seq('function',
#  [Action(Signature, PreBlock), MooBlock],
#  '(func %s%s)', [0, 1]
#)

Atom = Or('atom', [
  Literal,
  List,
  #Parens,
  #Object,
])

#PrefixExp = Seq('prefix',
#  [(PrefixOp, n, Future('SubExp')],
#  '(%s %s)', [0,2]
#)
#
#BinopExp = Seq('binop',
#  [Atom, w, BinOp, n, Future('SubExp')],
#  '(%s %s %s)', [2,0,4]
#)
#
#PrefixBinopExp = Seq('prefixbinop',
#  [PrefixOp, n, Atom, w, BinOp, n, Future('SubExp')],
#  '(%s (%s %s) %s)', [4,0,2,6]
#)
#
#SubExp = Or('subexp', [
#  Atom,
#  PrefixExp,
#  BinopExp,
#  PrefixBinopExp,
#])

Exp = Or('exp', [
  Atom,
  #PrefixExp,
  #BinopExp,
  #PrefixBinopExp,
], '(exp %s)')

#Type = Or('type', [
#  Atom,
#  PrefixExp,
#  BinopExp,
#  PrefixBinopExp,
#], '(type %s)')
#
#
## New statements
#Assign1 = Seq('assign1',
#  ['ID', w, 'EQUALS', n, Exp],
#  '%s %s', [0, 4]
#)
#
#Assign2 = Seq('assign2',
#  ['ID', w, 'EQUALS', n, Type, w, 'EQUALS', n, Exp],
#  '%s %s %s', [0, 4, 8]
#)
#
## New
#NewAssign = Or('newassign', [
#  'ID',
#  Assign1,
#  Assign2,
#], '(init %s)')
#
#New = Seq('new',
#  ['NEW', n, NewAssign,
#    Star('news', Seq('commanew', [w, 'COMMA', n, NewAssign], ' %s', [3]))],
#  '(new %s%s);', [2, 3]
#)
#
## Renew
#Renew = Seq('renew',
#  ['RENEW', n, Assign1,
#    Star('renews', Seq('commarenew', [w, 'COMMA', n, Assign1], ' %s', [3]))],
#  '(renew %s%s);', [2, 3]
#)
#
## Del
#Del = Seq('del',
#  ['DEL', n, 'ID',
#    Star('dels', Seq('commadel', [w, 'COMMA', n, 'ID'], ' %s', [3]))],
#  '(del %s%s);', [2, 3]
#)
#
#
#StmtNew = Or('stmtnew', [
#  New,
#  Renew,
#  Del,
#])
#
## IsVar statement
#StmtIsVar = Seq('isvar',
#  ['ISVAR', n, 'ID',
#   Star('isvarprops', Seq('isvarprop', [w, 'DOT', n, 'ID'], ' %s', [3]))],
#  '(isvar %s%s);', [2, 3]
#)
#
#
## Assignment statements
#StmtAssign = Seq('stmtassign',
#  [Var, w, Or('assignop', [
#    'EQUALS',
#    'PLUSEQUALS',
#    'MINUSEQUALS',
#    'STAREQUALS',
#    'SLASHEQUALS',
#    'PERCENTEQUALS',
#    'CARATEQUALS',
#    'PIPEEQUALS',
#    'AMPEQUALS',
#    'TILDEEEQUALS',
#  ]), n, Exp],
#  '(%s %s %s);', [2, 0, 4]
#)


# Procedure call statements
ExpList = Seq('explist',
  [Exp, Star('explists', Seq('commaexp', [w, ('COMMA',' '), n, Exp]))],
)

## Debate: Allow whitespace after the function name (Var) but before the first
## paren?
#StmtProcCall = Or('stmtproccall', [
#  Seq('proccallargs',
#    [Var, w, 'LPAREN', n, ExpList, n, 'RPAREN'],
#    '%s %s', [0, 4]),
#  Seq('proccallvoid',
#    [Var, w, 'LPAREN', n, 'RPAREN'])
#], '(call %s);')
#
#
## Branch constructs
## If
#IfPred = Or('ifpred', [
#  Seq('ifline', ['COMMA', w, Future('Stmt')], '%s', [2]),
#  Seq('ifblock', [n, Future('CodeBlock')], '%s', [1]),
#])
#
#If = Seq('if',
#  [Action(Seq('ifstart', ['IF', n, Exp], '(if %s ', [2]), PreBlock), IfPred],
#  '%s);', [1]
#)
#
## Elif
#ElifPred = Or('elifpred', [
#  Seq('elifline', ['COMMA', w, Future('Stmt')], '%s', [2]),
#  Seq('elifblock', [n, Future('CodeBlock')], '%s', [1]),
#])
#
#Elif = Seq('elif',
#  [Action(Seq('elifstart', [Action('ELIF', ElifCheck), n, Exp], '(elif %s ', [2]), PreBlock), ElifPred],
#  '%s);', [1]
#)
#
## Else
#ElsePred = Or('elsepred', [
#  Future('Stmt'),
#  Seq('elseblock', [n, Future('CodeBlock')], '%s', [1]),
#])
#
#Else = Seq('else',
#  [Action(Action('ELSE', ElseCheck, '(else ', []), PreBlock), ElsePred],
#  '%s);', [1]
#)
#
#
## Loop constructs
Loop = 'LOOP'         # TODO
LabelLoop = 'SLOOP'   # TODO
#Repeat = ''
#While = ''
#Each = ''
#
StmtLoop = Or('stmtloop', [
  Loop,
  LabelLoop,
#  Repeat,
#  While,
#  Each,
])


# Break constructs
StmtBreak = Or('stmtbreak', [
  'BREAK',
  Seq('breaklabel',
    [('BREAK','break'), ws, ('LABEL','<label>')]),
  'CONTINUE',
#  Seq('continuelabel',
#    [('CONTINUE','continue'), ws, ('LABEL','<label>')]),
  'RETURN',
  #Seq('returnexp',
  #  [('RETURN','return'), ws, Exp]),
  #Seq('yield',
  #  [('YIELD','yield'), ws, Exp]),
], 'x%sy')


# Statements
Stmt = Or('stmt', [
  #StmtNew,
  #StmtIsVar,
  #StmtAssign,
  #StmtProcCall,
  #If,
  #Elif,
  #Else,
  #Function,
  #Future('Switch'),
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  StmtLoop,
  StmtBreak,
], 'a%sb')

#CodeBlock = Seq('codeblock',
#  [('LBRACE','{'), Stmt, ('RBRACE','}')],
#)
#
#BlockOrStmt = Or('blockorstmt', [
#  Seq('stmtendl', [Stmt, Endl]),
#  Future('CodeBlock'),
#])

# Blocks
# A code block may have 0 or more statements.
#CodeBlockBody = Star('codeblockbody',
#  Seq('codeblockbodyws', [w, BlockOrStmt]),
#  ''
#)
#
#CodeBlockStatements = Seq('codeblockstatements',
#  [Stmt, Star('codeblockstatementsmore', [(End,';'), Stmt])]
#)

# A codeblock issued from a commandline requires a newline or semicolon after
# the end of the block.
CmdCodeBlock = Seq('cmdcodeblock',
  [w, ('LBRACE','{'),
#  Star('cmdcodeblockitems',
#    Or('cmdcodeblockbody', [
#      wn,
#      Seq('cmdcodeblockbodystmt', [w, Stmt, w, Or('e1', [wn, 'SEMI'], '')]),
#      Seq('cmdcodeblockbodyblock', [w, CodeBlock]),
#    ]),
#  ),
#  Or('cmdcodeblocklast', [
#    Star('cmdcodeblocklastwn', wn),
#    Seq('cmdcodeblocklaststmt', [w, Stmt, w, Or('e1', [n, 'SEMI'], '')]),
#  ]),
  Star('breaks', [Stmt]),
  w, ('RBRACE','}')],
)

# Program invocation

# Basic building block -- cannot contain an ExpBlock
# OK to include "not cmdline approved" symbols (operators) -- these will have
# been parsed out earlier, and asking for their cmdText will fail if we do
# accidentally catch any.
ProgramBasic = Or('programbasic', [
  Keyword,    # These will be output verbatim in both code and cmd mode
  CmdOp,      # Operators spilled out for commands
  CmdLiteral, # Literals spilled out for commands
])

ProgramArg = Star('programarg',
  Or('programarg1', [
    ProgramBasic,
    Seq('programargexpblock',
      [('LBRACE','{'), w, Exp, w, ('RBRACE','}')]
    ),
    # TODO: redirection, pipes, background-job, etc.
  ])
)

ProgramArgs = Star('programargs',
  Seq('wsarg', [ws, ProgramArg]),
)

# Just the name of a program to invoke, without its arguments.
# Necessarily starts with a program name (not an ExpBlock; those are
# handled by CmdBlock).  ProgramArg is allowed to contain ExpBlocks.
Program = Seq('program',
  [ProgramBasic, ProgramArg],
)

ExpBlockProgram = Seq('expblockprogram',
  [w, ('LBRACE', '{'), w, Exp, w, ('RBRACE','}'), ProgramArg, ProgramArgs],
)

# A CmdBlock is when a { occurs at the start of a commandline.  We don't yet
# know if it's a codeblock (list of statements) or an expression block (single
# expression which will become (at least the beginning of) the name of a
# program to invoke).  This specific kind of expression block is an
# ExpBlockProgram.
#
# An ExpBlockProgram can have programargs follow it, which may include
# semicolons.  The semicolons will group things within []'s.  We wait until a
# NEWL to give us the trailing ']'.  CmdCodeBlock may not have anything after
# the '}', just whitespace and a newline.
CmdBlock = Seq('cmdblock',
  [(Or('cmdblockmain', [
    ExpBlockProgram,
    CmdCodeBlock,
    ]), '[%s'),
  ('NEWL', ']'),
])

ProgramInvocation = Seq('programinvocation',
  [w, (Program,'[%s'), ProgramArgs, ('NEWL',']')]
)

CmdLine = Or('cmdline', [
  wn,
  ProgramInvocation,
  CmdBlock,
])
CmdLines = Star('cmdlines', CmdLine)

# Refresh missed rule dependencies
#Replace(TypeList, 'Type', Type)
#Replace(TypeList.items[1].items, 'Type', Type)
##Replace(Signature.items[2], 'Type', Type)
##Replace(Signature.items[3], 'Type', Type)
#Replace(MooBlock, 'CodeBlock', CodeBlock)
#Replace(Function, 'CodeBlock', CodeBlock)
#Replace(PrefixExp, 'SubExp', SubExp)
#Replace(BinopExp, 'SubExp', SubExp)
#Replace(PrefixBinopExp, 'SubExp', SubExp)
Replace(List, 'ExpList', ExpList)
#Replace(Parens, 'SubExp', SubExp)
##Replace(Object, 'Exp', Exp)    # or SubExp ?
#Replace(IfPred.items[0], 'Stmt', Stmt)
#Replace(IfPred.items[1], 'CodeBlock', CodeBlock)
#Replace(ElifPred.items[0], 'Stmt', Stmt)
#Replace(ElifPred.items[1], 'CodeBlock', CodeBlock)
#Replace(ElsePred, 'Stmt', Stmt)
#Replace(ElsePred.items[1], 'CodeBlock', CodeBlock)
#Replace(BlockOrStmt, 'CodeBlock', CodeBlock)
#Replace(BlockOrCmdStmt, 'CodeBlock', CodeBlock)


# Lush
# The Lush parser holds some "global" state.  It should also probably do
# something smart the moment it turns bad.
# The ast string is a buffer consumed by the lush_parser, which resets it to ''
# after any consumption.
#
# NEW: we probably should kill the whole stack thing.  We don't need it -- just
# do two fans.  But maybe a sometimes-used explicit-stack is better, mlehhh, I
# don't like this duality and redundancy of sometimes-implicit and
# sometimes-explicit stack.  Second fan out ftw!

#Lush = Star('lush', CmdLine, neverGoBad=True)
class Lush(object):
  def __init__(self):
    self.stack = []
    self.ast = ''
    self.bad = False
    self.top = self
    self.parser = MakeParser(CmdLines, self)
    self.active = self.parser

  def parse(self,token):
    # assume that WE aren't the one cleaning up after bad branches, for now.
    # A finished element of the stack will pop itself and as many others off as
    # necessary.
    disp = ''
    if self.stack:
      self.active = self.stack[-1]
    else:
      self.active = self.parser
    # parse
    disp = self.active.parse(token)
    if self.active.bad:
      raise Exception("Lush failed to parse token '%s'" % token)
    self.ast += disp

  def finish(self):
    disp = self.active.finish()
    self.ast += disp

def LushParser():
  return Lush()

