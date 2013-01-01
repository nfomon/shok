{
module Scanner where
}

%wrapper "basic"

$digit = 0-9      -- digits
$alpha = [a-zA-Z]    -- alphabetic characters
$graphic    = $printable # $white

@string     = \" ($graphic # \")* \"



-- Define tokens.
-- LHS: A regexp to match from the input
-- RHS: anonymous function that takes a string and returns a Token
-- Later we define the Token datatype that must have any of these return values
-- as one of its possible values

tokens :-

  $white+   ;

  "new"     { \s -> TNew }

-- Types
  "bool"    { \s -> TBool }
  "int"     { \s -> TInt }
  "str"     { \s -> TStr}

  "void"    { \s -> TVoid }
  "return"  { \s -> TReturn }

-- Control flow
  "if"      { \s -> TIf }
  "elif"    { \s -> TElif }
  "else"    { \s -> TElse }
  "while"   { \s -> TWhile }

-- Literals
  "true"    { \s -> TTrue }
  "false"   { \s -> TFalse }
  $digit+   { \s -> TIntLiteral (read s) }
  @string   { \s -> TStrLiteral (init (tail s)) -- remove the leading " and trailing " }
  -- TODO: fixed-point literal, path literal

-- Identifiers
  $alpha[$alpha $digit \_ \']*    { \s -> TIdent s }

-- Symbols and operators
  "."       { \s -> TPeriod }
  "!"       { \s -> TNot }
  "and"     { \s -> TAnd }
  "xor"     { \s -> TXor }
  "or"      { \s -> TOr }
  [\+\-\*\/]    { \s -> TOp (head s) }
  "<"       { \s -> TLessThan (head s) }
  "=="      { \s -> TEquals }
  "="       { \s -> TAssign }
  ";"       { \s -> TSemiColon }
  "("       { \s -> TLeftParen }
  ")"       { \s -> TRightParen }
  "{"       { \s -> TLeftBrace }
  "}"       { \s -> TRightBrace }
  ","       { \s -> TComma }
  "["       { \s -> TLeftBracket }
  "]"       { \s -> TRightBracket }

-- Built-ins (temporary)
  "print"   { \s -> TPrint }
{
-- Each action has type :: String -> Token

-- The token type:
data Token =
    TNew

  | TBool
  | TInt
  | TStr

  | TVoid
  | TReturn

  | TIf
  | TElif
  | TElse
  | TWhile

  | TTrue
  | TFalse
  | TIntLiteral Int
  | TStrLiteral String

  | TIdent String

  | TPeriod
  | TNot
  | TAnd
  | TXor
  | TOr

  | TOp Char
  | TLessThan Char

  | TEquals
  | TAssign
  | TSemiColon

  | TLeftParen
  | TRightParen

  | TLeftBrace
  | TRightBrace
  | TComma
  | TLeftBracket
  | TRightBracket
  | TPrint
  deriving (Eq,Show)

--main = do
--  s <- getContents
--  print (alexScanTokens s)

}
