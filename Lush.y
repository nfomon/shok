{
module Main where
import Scanner
}


%name lush
%tokentype { Token }
%error { parseError }
%token
  "type"        { TType }
  "new"          { TNew }
  "str"        { TStr }
  "void"        { TVoid }
  "return"      { TReturn }
  "int"                { TInt }
  "bool"        { TBool }
  "if"                { TIf }
  "elif"        { TElif }
  "else"        { TElse }
  "true"        { TTrue }
  "false"        { TFalse }
  "while"        { TWhile }
  integer_literal      { TIntLiteral $$ }
  ident         { TIdent $$ }
  "{"               { TLeftBrace }
  "}"          { TRightBrace }
  ","          { TComma }
  "["          { TLeftBracket }
  "]"          { TRightBracket }
  op           { TOp $$}
  lessthan        { TLessThan $$ }
  "("          { TLeftParen }
  ")"          { TRightParen }
  ";"          { TSemiColon }
  "."          { TPeriod }
  "!"          { TNot }
  "=="         { TEquals }
  "="          { TAssign }
  "print"      { TPrint }
%%

Exp : 
    Exp op Exp                        { ExpOp $1 $2 $3}
    | Exp lessthan Exp                   { ExpLessThan $1 $2 $3}
    | Exp "[" Exp "]"                 { ExpArray $1 $3}
--    | Exp "." "length"                { ExpLength $1}
    | Exp "." ident "(" ExpList ")"   { ExpFCall $1 $3 $5}
    | integer_literal                 { ExpInt $1}
    | "true"                          { ExpBool True}
    | "false"                         { ExpBool False}
    | ident                           { ExpIdent $1}
--    | "this"                          { ExpThis }
    | "new" "int" "[" Exp "]"         { ExpNewInt $4 }  
    | "new" ident "(" ")"             { ExpNewIdent $2}
    | "!" Exp                         { ExpNot $2}
    | "(" Exp ")"                     { ExpExp $2}

ExpList :
        Exp            { ExpListExp $1 }
        | Exp ExpRest  { ExpList $1 $2 }
        |              { ExpListEmpty }

ExpRest : 
     "," Exp      { ExpRest $2 }

-- Scope :
--   "{" StatementList "}"   { ; }
-- 
-- Statement :
--     Block                 { Block }
--   | IfStatement           { If }
--   | WhileStatement        { While }
-- 
-- StatementList :
--     Statement       { StatementList Empty $1 }
--   | StatementList Statement   { StatementList $1 $2 }
-- 
{
parseError :: [Token] -> a
parseError _ = error "Parse error"

-- Datastructures that we are parsing

-- data Cd 
--     = Cd DirList
--       deriving (Show, Eq)

data Exp
    = Exp String
    | ExpOp Exp Char Exp
    | ExpLessThan Exp Char Exp
    | ExpArray Exp Exp -- "Exp [ Exp ]"
    | ExpFCall Exp Ident ExpList -- Exp . Ident ( ExpList )
    | ExpInt Int
    | ExpNewInt Exp
    | ExpBool Bool -- True or False
    | ExpIdent Ident
    | ExpNewIdent Ident -- new Ident ()
    | ExpExp Exp -- Exp ( Exp )
    | ExpThis
    | ExpNot Exp
    | ExpLength Exp
    | ExpError
    deriving (Show, Eq)

data Op
     = And
     | LessThan
     | Plus
     | Minus
     | Times
     deriving (Show, Eq)

type Ident = String
type Integer_Literal = Int
data ExpList
    = ExpList Exp ExpRest
    | ExpListEmpty
    | ExpListExp Exp
    deriving (Show, Eq)
data ExpRest
    = ExpRest Exp
    deriving (Show, Eq)



main = do 
  inStr <- getContents
  let parseTree = lush (alexScanTokens inStr)  
  putStrLn ("parseTree: " ++ show(parseTree))
  print "done"
}
