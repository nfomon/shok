# Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
# directory of this distribution and at http://shok.io/code/copyright.html

# Parser for a mini language, to experiment with the parser framework.

import MakeRule
from Rule import Opt, Rule
from Parser import MakeParser
from ActionParser import Action
from OrParser import Or
from PlusParser import Plus
from SeqParser import Seq
from StarParser import Star
from TerminalParser import Terminal, ValueTerminal
import logging

class Future(object):
  def __init__(self,name):
    self.name = name
  def __repr__(self):
    return 'Future(%s)' % self.name

# most pythonic routine evahr!!11
def Replace(rule,name,new):
  if isinstance(name, str):
    logging.debug("REPLACE r=%s i=%s n=%s new=%s" % (rule.name, rule.items, name, new))
    for i, item in enumerate(rule.items):
      if isinstance(item, Future):
        if item.name == name:
          rule.items[i] = new
  else:
    if not isinstance(rule.items[name][0], Future):
      raise Exception("rule %s tuple at index %s is type %s not Future" % (rule.name, name, type(rule.items[name][0])))
    rule.items[name] = (new, rule.items[name][1])

# Language token groups
Keyword = Or('keyword', [
  # Symbol table modifiers
  ('NEW', 'new'), ('RENEW', 'renew'), ('DEL', 'del'),
  ('ISVAR', 'isvar'), ('TYPEOF', 'typeof'),
  # Functions
  ('VOID', 'void'), ('RETURN', 'return'), ('YIELD', 'yield'),
  # Branch constructs
  ('IF', 'if'), ('ELIF', 'elif'), ('ELSE', 'else'),
  ('SWITCH', 'switch'), ('CASE', 'case'), ('DEFAULT', 'default'),
  # Loop constructs
  ('WHILE', 'while'), ('LOOP', 'loop'), ('TIMES', 'times'),
  ('EACH', 'each'), ('IN', 'in'), ('WHERE', 'where'),
  ('BREAK', 'break'), ('CONTINUE', 'continue'),
  # Logical operators
  ('NOT', 'not'), ('NOR', 'nor'), ('AND', 'and'), ('OR', 'or'),
  ('XOR', 'xor'), ('XNOR', 'xnor'),
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
  ('EQ', '=='), ('NE', '!='),
  # Numeric operators
  ('PLUS', '+'), ('MINUS', '-'), ('STAR', '*'), ('SLASH', '/'),
  ('PERCENT', '%'), ('CARAT', '^'),
  # Object operators -- disallow PIPE, AMP
  ('TILDE', '~'), ('DOUBLETILDE', '~~'),
  # Assignment operators -- disallow PIPEEQUALS, AMPEQUALS
  ('EQUALS', '='), ('PLUSEQUALS', '+='), ('MINUSEQUALS', '-='),
  ('STAREQUALS', '*='), ('SLASHEQUALS', '/='),
  ('PERCENTEQUALS', '%='), ('CARATEQUALS', '^='),
  ('PIPEEQUALS', '|='), ('AMPEQUALS', '&='), ('TILDEEQUALS', '~='),
  # Cast -- disallow ARROW
  # Delimeters: disallow () {}
  ('LBRACKET', '['), ('RBRACKET', ']'), ('COMMA', ','),
  ('DOT', '.'), ('COLON', ':'),
])

# Operators that are safe for use by path literals in code
# Anything we allow here will be eaten aggressively by a path literal,
# overriding any possible other meaning.  If you actually want to use the
# regular / or . operators directly beside a path literal, enclose the path
# literal in ()'s.
PathOp = Or('cmdop', [
  # Equality operators -- disallow < <= > >= == !=
  # Numeric operators -- disallow + - * % ^
  ('SLASH', '/'),
  # Object operators -- disallow | & ~ ~~
  # Assignment operators -- disallow = += -= *= /= %= ^= |= &= ~=
  # Cast -- disallow ARROW
  # Delimeters: disallow () [] {} , :
  ('DOT', '.'),
])

# Whitespace
# w: optional whitespace (non-newline)
w = Star('w',
  ('WS', '')
)

# ws: mandatory whitespace (non-newline)
ws = Plus('ws',
  ('WS', ' ')
)

# wn: optional whitespace preceding a single newline
wn = Seq('wn', [w, ('NEWL','')])

# n: any amount of optional whitespace and newlines.  Greedy!
n = Star('n',
  [w, Star('nn', ('NEWL', ''))]
)

# Basic parsing constructs
End = Or('end', [
  wn,
  Seq('endsemi', [w, ('SEMI','')]),
])

Endl = Or('endl', [
  End,
  Seq('endbrace', [w, ('RBRACE','}')]),
])

# Variable or object property access
# Currently, property access can only go through an identifier.
Var = (Seq('var',
  ['ID', Star('props', Seq('prop', [('DOT',''), n, ('ID',' %s')]))]
), '(var %s)')


# Expressions
CmdLiteral = Or('cmdliteral', [
  # disallow REGEXP, LABEL
  ValueTerminal('INT'),
  ValueTerminal('FIXED'),
  ValueTerminal('STR'),
  ValueTerminal('ID'),
])

PathSubToken = Or('pathsubtoken', [
  Keyword,
  CmdLiteral,
])

PathToken = Or('pathtoken', [
  Keyword,
  PathOp,
  CmdLiteral,
])

PathPart = Star('pathpart', PathToken)

Path = (Or('path', [
  Seq('pathstartslash', [('SLASH','/'), PathPart]),
  Plus('pathendslash', Seq('pathendslashsub', [PathSubToken, ('SLASH','/')])),
  Seq('pathstartdotslash', [('DOT','.'), ('SLASH','/'), PathPart]),
  Seq('pathstartdotdotslash', [('DOT','.'), ('DOT','.'), ('SLASH','/'), PathPart]),
  Seq('pathstarttildeslash', [('TILDE','~'), ('SLASH','/'), PathPart]),
]), '(path %s)')

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

Parens = Seq('parens',
  [('LPAREN','(paren '), n, Future('SubExp'), n, ('RPAREN',')')]
)


# Object literals
# Care is required because there could be an ambiguity between object literals
# and expblocks, depending on what is allowed inside each.
MemberExt = Or('memberext', [
  Seq('memberextsemi', [w, ('SEMI',';'), n, Future('ObjectBody')]),
  Seq('memberextwn', [(wn,';'), n, Future('ObjectBody')]),
])

Member = Seq('member',
  [(Future('NewAssign'),' %s'), Opt(MemberExt)]
)

ObjectBody = Opt(Seq('objectbody',
  [n, Member]
))
Replace(MemberExt.items[0], 'ObjectBody', ObjectBody)
Replace(MemberExt.items[1], 'ObjectBody', ObjectBody)

Object = Seq('object',
  [('LBRACE','(object'), n, ObjectBody, w, ('RBRACE',')')]
)

Arg = (Or('arg', [
  Future('Type'),
  Seq('namedarg', ['ID', w, ('COLON',' '), n, Future('Type')]),
]), '(arg %s)')

ArgList = Seq('arglist',
  [Arg, Star('types', Seq('commatype', [w, ('COMMA',' '), n, Arg]))]
)

FunctionArgs = Seq('functionargs',
  [('LPAREN',''), n, (Opt(ArgList),' %s'), ('RPAREN','')]
)

FunctionReturn = Seq('functionreturn',
  [('ARROW',''), n, Future('Type')]
)

Function = (Seq('function',
  [('AT',''), w, (Opt(FunctionArgs),'(args%s) '), w,
    (Opt(FunctionReturn),'(returns %s) '), w, Future('CodeBlock')]
), '(func %s)')

Atom = Or('atom', [
  Literal,
  List,
  Parens,
  Object,
  Function,
])

PrefixExp = Seq('prefixexp',
  [(PrefixOp, '%s '), n]
)

BinopExp = Seq('binopexp',
  [(BinOp,' %s'), n, (Future('SubExp'),' %s')]
)

SubExp = Seq('subexp',
  [(Opt(PrefixExp),'%s'), Atom, w, Opt(BinopExp)]
)
Replace(Parens, 'SubExp', SubExp)
Replace(BinopExp, 2, SubExp)

Exp = (SubExp, '(exp %s)')
Type = (SubExp, '(type %s)')
Replace(Arg[0], 'Type', Type)
Replace(Arg[0].items[1], 'Type', Type)
Replace(FunctionReturn, 'Type', Type)


# New statements
# New
NewAssign = (Seq('newassign',
  ['ID', w, Opt(Seq('newtype', [('COLON',' '), n, Type])),
         w, Opt(Seq('newvalue', [('EQUALS',' '), n, Exp]))]
), '(init %s)')
Replace(Member, 0, NewAssign)

New = (Seq('new',
  [('NEW',''), n, NewAssign,
  Star('news', Seq('commanew', [w, ('COMMA',' '), n, NewAssign]))
]), '(new %s)')

# Renew
Renew = (Seq('renew',
  [('RENEW',''), n, NewAssign,
  Star('renews', Seq('commarenew', [w, ('COMMA',' '), n, NewAssign]))
]), '(renew %s)')

# Del
Del = (Seq('del',
  [('DEL',''), n, 'ID',
  Star('dels', Seq('commadel', [w, ('COMMA',' '), n, 'ID']))
]), '(del %s)')

StmtNew = Or('stmtnew', [
  New,
  Renew,
  Del,
])

# IsVar statement
StmtIsVar = (Seq('isvar',
  [('ISVAR',''), n, 'ID',
  Star('isvarprops', Seq('isvarprop', [w, ('DOT',' '), n, 'ID']))]
), '(isvar %s)')

# Typeof statement
StmtTypeof = Seq('typeof',
  [('TYPEOF','(typeof '), n, (Exp,'%s)')]
)

# Assignment statements
StmtAssign = Seq('stmtassign',
  [(Var,'(assign %s'), w, (Or('assignop', [
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
  ]), ' %s '), n, (Exp,'%s)')],
)


# Procedure call statements
ExpList = Seq('explist',
  [(Exp,' %s'), Star('explists', Seq('commaexp', [w, ('COMMA',' '), n, Exp]))],
)
Replace(List, 'ExpList', ExpList)

StmtProcCall = (Seq('stmtproccall',
  [Var, w, ('LPAREN',''), n, Opt(ExpList), n, ('RPAREN','')]
), '(call %s)')


# Branch constructs
# If
BranchPred = Or('branchpred', [
  Seq('branchline', [w, ('COMMA',''), w, Future('Stmt')]),
  Seq('branchblock', [n, Future('CodeBlock')]),
])

If = (Seq('if',
  [('IF',''), n, (Exp,'%s '), BranchPred]
), '(if %s)')

# Elif
Elif = (Seq('elif',
  [('ELIF',''), n, (Exp,'%s '), BranchPred]
), '(elif %s)')

# Else
ElsePred = Or('elsepred', [
  Seq('elseline', [w, Future('Stmt')]),
  Seq('elseblock', [n, Future('CodeBlock')]),
])

Else = (Seq('else',
  [('ELSE',''), ElsePred]
), '(else %s)')


## Loop constructs
LoopPred = BranchPred

LoopTimes = Seq('looptimes',
  [w, Exp, w, ('TIMES','')]
)
LoopCond = Or('loopcond', [
  Seq('loopcondtimes', [(LoopTimes,'(times %s) '), LoopPred]),
  LoopPred
])

Loop = (Seq('loop',
  [('LOOP',''), LoopCond]
), '(loop %s)')
#LabelLoop = 'SLOOP'   # TODO

While = (Seq('while',
  [('WHILE',''), w, Exp, LoopPred]
), '(while %s)')

#TypedVars = ...
#
#WhereClause = ...
#
#Where = (Seq('where',
#  [w, ('WHERE',''), WhereClause]
#), '(where %s)')
#
#Each = (Seq('each',
#  [('EACH',''), w, TypedVars, ('IN',''), n, Exp, Opt(Where), LoopPred]
#), '(each %s)')

StmtLoop = Or('stmtloop', [
  Loop,
  #LabelLoop,
  While,
  #Each,
])


# Break constructs
StmtBreak = Or('stmtbreak', [
  ('BREAK','break'),
  #Seq('breaklabel',
  #  [('BREAK','break'), ws, ('<LABEL>','<label>')]),
  ('CONTINUE','continue'),
  #Seq('continuelabel',
  #  [('CONTINUE','continue'), ws, ('LABEL','<label>')]),
  Seq('return',
    [('RETURN','return'), Opt(Seq('returnexp', [(ws,''), (Exp,' %s')]))]),
  Seq('yield',
    [('YIELD','yield'), ws, Exp]),
])


# Statements
Stmt = Or('stmt', [
  StmtNew,
  StmtIsVar,
  StmtTypeof,
  StmtAssign,
  StmtProcCall,
  #Future('Switch'),
  #Seq('stmtstmtbranch', [StmtBranch, Endl]),
  StmtLoop,
  StmtBreak,
])
Replace(BranchPred.items[0], 'Stmt', Stmt)
Replace(ElsePred.items[0], 'Stmt', Stmt)

ExpBlock = Seq('expblock',
  [('LBRACE','{'), w, Exp, w, ('RBRACE','}')]
)

# Blocks
# A code block may have 0 or more statements.
# We're very careful about where semicolons and newlines are allowed and/or
# required, including as regards nested blocks.
# Only If and Elif statements can be followed by Elif and Else statements.
StmtExt = Or('stmtext', [
  Seq('stmtextsemi', [w, ('SEMI',';'), n, Future('CodeBlockBody')]),
  Seq('stmtextwn', [(wn,';'), n, Future('CodeBlockBody')]),
])
IfStmtExt = Or('ifstmtext', [
  Seq('ifstmtextsemi', [w, ('SEMI',';'), n, Future('IfPostCodeBlockBody')]),
  Seq('ifstmtextwn', [(wn,';'), n, Future('IfPostCodeBlockBody')]),
])

CodeBlockStmt = Or('codeblockstmt', [
  Seq('codeblockbasicstmt', [Stmt, Opt(StmtExt)]),
  Seq('codeblockifstmt', [If, Opt(IfStmtExt)]),
])
IfPostCodeBlockStmt = Or('ifpostcodeblockstmt', [
  Seq('codeblockbasicstmt', [Stmt, Opt(StmtExt)]),
  Seq('codeblockifstmt', [If, Opt(IfStmtExt)]),
  Seq('codeblockelifstmt', [Elif, Opt(IfStmtExt)]),
  Seq('codeblockelsestmt', [Else, Opt(StmtExt)]),
])

CodeBlockCodeBlock = Seq('codeblockcodeblock',
  [Future('CodeBlock'), w, (Opt(Terminal('SEMI')),';'), n, Future('CodeBlockBody')]
)

CodeBlockItem = Or('codeblockitem', [
  CodeBlockStmt,
  CodeBlockCodeBlock,
])
IfPostCodeBlockItem = Or('ifpostcodeblockitem', [
  IfPostCodeBlockStmt,
  CodeBlockCodeBlock,
])

CodeBlockBody = Opt(Seq('codeblockbody',
  [n, CodeBlockItem]
))
IfPostCodeBlockBody = Opt(Seq('ifpostcodeblockbody',
  [n, IfPostCodeBlockItem]
))
Replace(StmtExt.items[0], 'CodeBlockBody', CodeBlockBody)
Replace(StmtExt.items[1], 'CodeBlockBody', CodeBlockBody)
Replace(IfStmtExt.items[0], 'IfPostCodeBlockBody', IfPostCodeBlockBody)
Replace(IfStmtExt.items[1], 'IfPostCodeBlockBody', IfPostCodeBlockBody)
Replace(CodeBlockCodeBlock, 'CodeBlockBody', CodeBlockBody)

CodeBlock = Seq('codeblock',
  [('LBRACE','{'), n, CodeBlockBody, w, ('RBRACE','}')]
)
Replace(Function[0], 'CodeBlock', CodeBlock)
Replace(CodeBlockCodeBlock, 'CodeBlock', CodeBlock)
Replace(BranchPred.items[1], 'CodeBlock', CodeBlock)
Replace(ElsePred.items[1], 'CodeBlock', CodeBlock)

# Program invocation

# Basic building block -- cannot contain an ExpBlock
# OK to include "not cmdline approved" symbols (operators) -- these will have
# been parsed out earlier, and asking for their cmdText (in the evaluator) will
# fail if we do accidentally catch any.
ProgramBasic = Or('programbasic', [
  Keyword,    # These will be output verbatim in both code and cmd mode
  CmdOp,      # Operators spilled out for commands
  CmdLiteral, # Literals spilled out for commands
])

# A not-the-first part of a program (an argument piece, maybe an exp block)
ProgramExts = Star('programexts',
  Or('programext', [
    ProgramBasic,
    ExpBlock,
    # TODO: redirection, pipes, background-job, etc.
  ])
)

ProgramArgs = Star('programargs',
  Seq('wsarg', [(ws,''), (ProgramExts,' %s')]),
)

# Just the name of a program to invoke, without its arguments.
# Necessarily starts with a program name (not an ExpBlock).  The ProgramArg
# here is just part of the program name, and is allowed to contain ExpBlocks.
Program = Seq('program',
  [ProgramBasic, ProgramExts],
)

ProgramInvocation = Seq('programinvocation',
  [Program, ProgramArgs]
)

CmdLine = Or('cmdline', [
  wn,
  Seq('cmdlinemain',
    [w, (Or('cmd', [ProgramInvocation, CodeBlock]),'[%s'), (End,']')]
  ),
])

CmdLines = Star('cmdlines', CmdLine)


# Shok
def ShokParser():
  parser = MakeParser(CmdLines)
  parser.neverGoBad = True
  return parser

