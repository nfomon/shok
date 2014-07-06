// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Shok_h_
#define _Shok_h_

/* shok grammar parser */

#include "Token.h"

#include <boost/fusion/tuple.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <iostream>
#include <ostream>
#include <string>

namespace parser {

template <typename Iterator>
struct ShokParser : qi::grammar<Iterator, void(), ascii::space_type> {
public:
  ShokParser(std::ostream& out)
    : ShokParser::base_type(shok_, "shok"),
      EXIT_("EXIT"),
      NEW_("NEW"), RENEW_("RENEW"), DEL_("DEL"),
      NEW_n("NEW"), RENEW_n("RENEW"), DEL_n("DEL"),
      ISVAR_("ISVAR"), TYPEOF_("TYPEOF"),
      ISVAR_n("ISVAR"), TYPEOF_n("TYPEOF"),
      AT_("AT"), RETURN_("RETURN"),
      AT_n("AT"), RETURN_n("RETURN"),
      IF_("IF"), ELIF_("ELIF"), ELSE_("ELSE"),
      IF_n("IF"), ELIF_n("ELIF"), ELSE_n("ELSE"),
      WHILE_("WHILE"), LOOP_("LOOP"), TIMES_("TIMES"),
      WHILE_n("WHILE"), LOOP_n("LOOP"), TIMES_n("TIMES"),
      NOT_("NOT"), NOR_("NOR"), AND_("AND"), OR_("OR"),
      NOT_n("NOT"), NOR_n("NOR"), AND_n("AND"), OR_n("OR"),
      XOR_("XOR"), XNOR_("XNOR"),
      XOR_n("XOR"), XNOR_n("XNOR"),
      LT_("LT"), LE_("LE"), GT_("GT"), GE_("GE"), EQ_("EQ"), NE_("NE"),
      LT_n("LT"), LE_n("LE"), GT_n("GT"), GE_n("GE"), EQ_n("EQ"), NE_n("NE"),
      PLUS_("PLUS"), MINUS_("MINUS"), STAR_("STAR"), SLASH_("SLASH"),
      PERCENT_("PERCENT"), CARAT_("CARAT"),
      PIPE_("PIPE"), AMP_("AMP"), DOUBLETILDE_("DOUBLETILDE"), TILDE_("TILDE"),
      EQUALS_("EQUALS"), PLUSEQUALS_("PLUSEQUALS"), MINUSEQUALS_("MINUSEQUALS"),
      STAREQUALS_("STAREQUALS"), SLASHEQUALS_("SLASHEQUALS"),
      PERCENTEQUALS_("PERCENTEQUALS"), CARATEQUALS_("CARATEQUALS"),
      PIPEEQUALS_("PIPEEQUALS"), AMPEQUALS_("AMPEQUALS"),
      DOUBLETILDEEQUALS_("DOUBLETILDEEQUALS"), TILDEEQUALS_("TILDEEQUALS"),
      ARROW_("ARROW"),
      LPAREN_("LPAREN"), RPAREN_("RPAREN"),
      LBRACKET_("LBRACKET"), RBRACKET_("RBRACKET"),
      LBRACE_("LBRACE"), RBRACE_("RBRACE"),
      COMMA_("COMMA"), DOT_("DOT"), COLON_("COLON"),
      SEMI_("SEMI"), NEWL_("NEWL"),
      INT_("INT"), STR_("STR"), ID_("ID"),
      WS_("WS") {
    using phoenix::ref;
    using phoenix::val;
    using qi::_1;
    using qi::_r1;
    using qi::_val;
    using qi::lit;

    // Token groups
    keyword_ %= NEW_n | RENEW_n | DEL_n | ISVAR_n | TYPEOF_n
      | RETURN_n // | VOID_n | YIELD_n
      | IF_n | ELIF_n | ELSE_n
      // | SWITCH_n | CASE_n | DEFAULT_n
      | WHILE_n | LOOP_n | TIMES_n
      // | EACH_n | IN_n | WHERE_n
      // | BREAK_n | CONTINUE_n
      | NOT_n | NOR_n | AND_n | OR_n | XOR_n | XNOR_n
    ;
    op_ = LT_ | LE_ | GT_ | GE_ | EQ_ | NE_
      | PLUS_ | MINUS_ | STAR_ | SLASH_ | PERCENT_ | CARAT_
      | PIPE_ | AMP_ | DOUBLETILDE_ | TILDE_
      | EQUALS_ | PLUSEQUALS_ | MINUSEQUALS_ | STAREQUALS_ | SLASHEQUALS_
      | PERCENTEQUALS_ | CARATEQUALS_ | PIPEEQUALS_ | AMPEQUALS_ | TILDEEQUALS_
      | ARROW_
      | LPAREN_ | RPAREN_ | LBRACKET_ | RBRACKET_ | LBRACE_ | RBRACE_
      | COMMA_ | DOT_ | COLON_
    ;
    cmdop_ =
      // EQUALITY OPERATORS -- DISALLOW lt, le, gt, ge
      EQ_[_val = "=="] | NE_[_val = "!="]
      // NUMERIC OPERATORS
      | PLUS_[_val = "+"] | MINUS_[_val = "-"]
      | STAR_[_val = "*"] | SLASH_[_val = "/"]
      | PERCENT_[_val = "%"] | CARAT_[_val = "^"]
      // OBJECT OPERATORS -- DISALLOW pipe, amp
      | DOUBLETILDE_[_val = "~~"] | TILDE_[_val = "~"]
      // ASSIGNMENT OPERATORS -- DISALLOW pipeequals, ampequals
      | EQUALS_[_val = "="]
      | PLUSEQUALS_[_val = "+="] | MINUSEQUALS_[_val = "-="]
      | STAREQUALS_[_val = "*="] | SLASHEQUALS_[_val = "/="]
      | PERCENTEQUALS_[_val = "%="] | CARATEQUALS_[_val = "^="]
      | PIPEEQUALS_[_val = "|="] | AMPEQUALS_[_val = "&="]
      | TILDEEQUALS_[_val = "~="]
      // CAST -- DISALLOW arrow
      // DELIMETERS: DISALLOW () {}
      | LBRACKET_[_val = "["] | RBRACKET_[_val = "]"] | COMMA_[_val = ","]
      | DOT_[_val = "."] | COLON_[_val = ":"]
    ;
    pathop_ =
      // EQUALITY OPERATORS -- DISALLOW < <= > >= == !=
      // NUMERIC OPERATORS -- DISALLOW + - * % ^
      SLASH_[_val = "/"]
      // OBJECT OPERATORS -- DISALLOW | & ~ ~~
      // ASSIGNMENT OPERATORS -- DISALLOW = += -= *= /= %= ^= |= &= ~=
      // CAST -- DISALLOW arrow
      // DELIMETERS: DISALLOW () [] {} , :
      | DOT_[_val = "."]
    ;

    // Whitespace helpers
    w_ = *WS_;
    ws_ = (+WS_)[_val = " "];
    wn_ = w_ >> NEWL_;
    n_ = *(w_ >> *NEWL_);

    // Basic constructs
    end_ = w_ >> (NEWL_ | SEMI_);
    endl_ = end_ | (w_ >> (RBRACE_[_val = ";"]));

    // Variable or object property accessor
    var1_ %= ID_ > *(DOT_[_val += " "] > n_ > ID_[_val += _1]);
    var_ %= var1_[_val = val("(var ") + _1 + val(")")];

    // Expression components
    cmdliteral_ %= INT_
      //| FIXED_
      | STR_ | ID_;
    pathsubtoken_ %= keyword_ | cmdliteral_;
    pathtoken_ %= keyword_ | pathop_ | cmdliteral_;
    pathpart_ %= *pathtoken_;
    path1_ %=
      (SLASH_[_val = "/"] > pathpart_)
      | +(pathsubtoken_ > SLASH_[_val = "/"])
      | (DOT_ > SLASH_[_val = "./"] > pathpart_)
      | (DOT_ > DOT_ > SLASH_[_val = "../"] > pathpart_)
      | (TILDE_ > SLASH_[_val += "~/"] > pathpart_)
    ;
    path_ = path1_[_val = val("(path ") + _1 + val(")")];
    literal_ %= INT_
      //| FIXED_
      | STR_ | path_
      //| REGEXP_ | LABEL_
      | var_;
    prefixop_ = MINUS_[_val = "MINUS"];
    binop_ = LT_ | LE_ | GT_ | GE_ | EQ_ | NE_
      | PLUS_ | MINUS_ | STAR_ | SLASH_ | PERCENT_ | CARAT_
      | PIPE_ | AMP_ | DOUBLETILDE_ | TILDE_
      // | USEROP_
    ;
    list_ = LBRACKET_[_val = "(list "]
      > n_ > explist_[_val += _1] > n_
      > RBRACKET_[_val += ")"]
    ;
    parens_ = LPAREN_[_val = "(paren "]
      > n_ > explist_[_val += _1] > n_
      > RPAREN_[_val += ")"]
    ;

/*
    // Object literals
    memberext_ %=
      (w_ >> SEMI_[_val = "; "] > n_ > objectbody_)
      | (wn_[_val = "; "] > n_ > objectbody_)
    ;
    member_ %= newassign_ > -memberext_;
    objectbody_ %= -(n_ >> member_);
    object_ %= LBRACE_[_val = "(object "]
      > n_ > objectbody_[_val += val("{") + _1 + val("}")] > w_
      > RBRACE_[_val += ")"]
    ;
*/

    // Function literals
/*
    arg_ %= (
      type_
      | (ID_ > w_ > colon_[_val += " "] > n_ > type_)
    )[_val = val("(arg ") + _1 + val(")")];
    arglist_ %= (arg_ > *(w_ > comma_[_val += " "] > n_ > arg_));
    functionargs_ %= lparen_ > n_ > -arglist_[_val += val(" ") + _1] > rparen_;
    functionreturn_ %= arrow_ > n_ > type_;
    function1_ > at_ > w_ >
      -functionargs_[_val = val("(args") + _1 _ ")"]
      > w_ > -functionreturn_[_val = val("(returns ") + _1 + ")"]
      > w_ > codeblock_
    ;
    function_ = function1_[_val = val("(func ") + _1 + ")"];
*/

    // Expressions
    atom_ %= literal_
      //| list_ | parens_ | object_ | function_
    ;
    prefixexp_ %= prefixop_[_val = _1 + val(" ")] > n_;
    binopexp_ %= binop_[_val = val(" ") + _1] > n_ > subexp_[_val += val(" ") + _1];
    subexp_ %= -prefixexp_ > atom_ > w_ > -binopexp_;
    exp_ %= subexp_[_val = val("(exp ") + _1 + val(")")];
    type_ %= subexp_[_val = val("(type ") + _1 + val(")")];

/*
    // New statements
    newassign1_ %= ID_
      > w_ > -(COLON_[_val += " "] > n_ > type_)
      > w_ > -(EQUALS_[_val += " "] > n_ > exp_)
    ;
    newassign_ = newassign1_[_val = val("(init ") + _1 + val(")")];
    new_ %= NEW_ > n_ > newassign_ > *(w_ > COMMA_[_val += " "] > n_ > newassign_)[_val = val("(new ") + _1 + val(")")];
    renew_ %= RENEW_ > n_ > newassign_ > *(w_ > COMMA_[_val += " "] > n_ > newassign_)[_val = val("(renew ") + _1 + val(")")];
    del_ %= DEL_ > n_ > ID_ > *(w_ > COMMA_[_val += " "] > n_ > ID_)[_val = val("(del ") + _1 + ")"];
    stmtnew_ %= new_ | renew_ | del_;

    // Misc statements
    stmtisvar_ = (ISVAR_ > n_ > ID_ > *(w_ > DOT_[_val += " "] > n_ > ID_))[_val = val("(isvar ") + _1 + val(")")];
    stmttypeof_ = TYPEOF_[_val = "(typeof "] > n_ > type_[_val += _1 + val(")")];
    stmtassign_ = var_[_val = val("(assign ") + _1] > w_ > (
      EQUALS_
      | PLUSEQUALS_
      | MINUSEQUALS_
      | STAREQUALS_
      | SLASHEQUALS_
      | PERCENTEQUALS_
      | CARATEQUALS_
      | PIPEEQUALS_
      | AMPEQUALS_
      | TILDEEQUALS_
    )[_val +=
      //_1 +
      val(" ")] > n_ > exp_;
*/

    // Procedure call statements
    explist_ %= exp_[_val = val(" ") + _1] > *(w_ > COMMA_[_val += " "] > n_ > exp_);
/*
    stmtproccall_ %= var_[_val = val("(call ") + _1] > w_ > LPAREN_ > n_ > -explist_ > n_ > RPAREN_[_val += ")"];

    // Branch constructs
    branchpred_ %=
      (w_ > COMMA_ > w_ > stmt_)
      | (n_ > codeblock_)
    ;
    if_ %= IF_[_val = "(if "] > n_ > exp_[_val += _1 + val(" ")] > branchpred_[_val += _1 + val(")")];
    elif_ %= ELIF_[_val = "(if "] > n_ > exp_[_val += _1 + val(" ")] > branchpred_[_val += _1 + val(")")];
    elsepred_ %=
      (w_ > stmt_)
      | (n_ > codeblock_)
    ;
    else_ %= ELSE_[_val = "(else "] > elsepred_[_val += _1 + val(")")];

    // Loop constructs
    looppred_ %= branchpred_;
    looptimes_ %= w_ > exp_ > w_ > TIMES_;
    loopcond_ %=
      (looptimes_[_val = val("(times ") + _1 + ") "] > looppred_)
      | looppred_
    ;
    loop_ = LOOP_[_val = "(loop "] > loopcond_[_val += _1 + val(")")];
    stmtloop_ %= loop_;

    // Statements
    stmt_ = stmtnew_ | stmtisvar_ | stmttypeof_ | stmtassign_
      | stmtproccall_ | stmtloop_
    ;

    // Blocks
    expblock_ = LBRACE_[_val = "{"] > w_ > exp_ > w_ > RBRACE_[_val += "}"];
*/
    expblock_ %= LBRACKET_ >> RBRACKET_;
/*
    stmtext_ =
      (w_ > SEMI_[_val = ";"] > n_ > codeblockbody_)
      | (wn_[_val = ";"] > n_ > codeblockbody_)
    ;
    ifstmtext_ =
      (w_ > SEMI_[_val = ";"] > n_ > ifpostcodeblockbody_)
      | (wn_[_val = ";"] > n_ > ifpostcodeblockbody_)
    ;
    codeblockstmt_ =
      (stmt_ > -stmtext_)
      | (if_ > -ifstmtext_)
    ;
    ifpostcodeblockstmt_ =
      (stmt_ > -stmtext_)
      | (if_ > -ifstmtext_)
      | (elif_ > -ifstmtext_)
      | (else_ > -stmtext_)
    ;

    codeblockcodeblock_ %= codeblock_ > w_ > -SEMI_[_val += ";"] > n_ > codeblockbody_;
    codeblockitem_ %= codeblockstmt_ | codeblockcodeblock_;
    ifpostcodeblockitem_ %= ifpostcodeblockstmt_ | codeblockcodeblock_;
    codeblockbody_ %= -(n_ > codeblockitem_);
    ifpostcodeblockbody_ %= -(n_ > ifpostcodeblockitem_);
*/
    //codeblock_ %= LBRACE_[_val = "{"] > n_ > codeblockbody_ > w_ > RBRACE_[_val += "}"];
    codeblock_ %= LBRACE_[ref(out) << "{"] >> RBRACE_[ref(out) << "}"];

    // Program invocation
    // Basic program building block; cannot contain an expblock
    programbasic_ %= keyword_ | cmdop_ | cmdliteral_;
    // A not-the-first part of a program (an argument piece, maybe an expblock)
    programexts_ %= *(programbasic_ | expblock_);
    programargs_ %= *(ws_ >> programexts_);
    // Just the name of the program to invoke, without arguments
    // Starts with the program name (not an expblock).  The programexts_ is
    // just part of the program name (not an arg) which may contain expblocks.
    program_ %= programbasic_ >> programexts_;
    // A program invocation: the program and any arguments
    //programinvocation_ %= program_ >> programargs_;
    programinvocation_ %= program_;

    cmdline_ %= wn_
      | (w_[ref(out) << "["] >> (programinvocation_ | codeblock_)[ref(out) << _1] >> end_[ref(out) << "]"]);
    shok_ %= *cmdline_;

    BOOST_SPIRIT_DEBUG_NODE(programbasic_);
    BOOST_SPIRIT_DEBUG_NODE(programexts_);
    BOOST_SPIRIT_DEBUG_NODE(programargs_);
    BOOST_SPIRIT_DEBUG_NODE(program_);
    BOOST_SPIRIT_DEBUG_NODE(cmdline_);
    //BOOST_SPIRIT_DEBUG_NODE(shok_);
  }

private:
  typedef TokenParser<Iterator> T;
  typedef NTokenParser<Iterator> N;
  typedef VTokenParser<Iterator> V;

  // Shell control
  T EXIT_;
  // Symbol table modifiers
  T NEW_, RENEW_, DEL_, ISVAR_, TYPEOF_;
  N NEW_n, RENEW_n, DEL_n, ISVAR_n, TYPEOF_n;
  // Functions
  T AT_, RETURN_; // VOID_, YIELD_
  N AT_n, RETURN_n; // VOID_n, YIELD_n
  // Branch constructs
  T IF_, ELIF_, ELSE_;
  N IF_n, ELIF_n, ELSE_n;
    // SWITCH_, CASE_, DEFAULT_
    // SWITCH_n, CASE_n, DEFAULT_n
  // Loop constructs
  T WHILE_, LOOP_, TIMES_;
    // EACH_, IN_, WHERE_
    // BREAK_, CONTINUE_
  N WHILE_n, LOOP_n, TIMES_n;
    // EACH_n, IN_n, WHERE_n
    // BREAK_n, CONTINUE_n
  // Logical operators
  T NOT_, NOR_, AND_, OR_, XOR_, XNOR_;
  N NOT_n, NOR_n, AND_n, OR_n, XOR_n, XNOR_n;
  // Equality operators
  T LT_, LE_, GT_, GE_, EQ_, NE_;
  N LT_n, LE_n, GT_n, GE_n, EQ_n, NE_n;
  // Numeric operators
  T PLUS_, MINUS_, STAR_, SLASH_, PERCENT_, CARAT_;
  // Object operators
  T PIPE_, AMP_, DOUBLETILDE_, TILDE_;
  // Assignment operators
  T EQUALS_, PLUSEQUALS_, MINUSEQUALS_, STAREQUALS_, SLASHEQUALS_;
  T PERCENTEQUALS_, CARATEQUALS_, PIPEEQUALS_, AMPEQUALS_;
  T DOUBLETILDEEQUALS_, TILDEEQUALS_;
  // Cast: ->
  T ARROW_;
  // Delimeters: () [] {} , . :
  T LPAREN_, RPAREN_;
  T LBRACKET_, RBRACKET_;
  T LBRACE_, RBRACE_;
  T COMMA_, DOT_, COLON_;
  // End of line: ; \n
  T SEMI_, NEWL_;
  // Literals
  V INT_; // FIXED_
  V STR_; // REGEXP_, LABEL_, USEROP_
  V ID_;
  // Whitespace
  T WS_;

  // Token groups
  qi::rule<Iterator, ascii::space_type, std::string()> keyword_;
  qi::rule<Iterator, ascii::space_type> op_;
  qi::rule<Iterator, ascii::space_type> cmdop_;
  qi::rule<Iterator, ascii::space_type> pathop_;

  // Whitespace helpers
  qi::rule<Iterator, ascii::space_type> w_;
  qi::rule<Iterator, std::string(), ascii::space_type> ws_;
  qi::rule<Iterator, ascii::space_type> wn_;
  qi::rule<Iterator, ascii::space_type> n_;

  // Basic constructs
  qi::rule<Iterator, ascii::space_type> end_;
  qi::rule<Iterator, std::string(), ascii::space_type> endl_;

  // Variable or object property accessor
  qi::rule<Iterator, std::string(), ascii::space_type> var1_;
  qi::rule<Iterator, std::string(), ascii::space_type> var_;

  // Expression components
  qi::rule<Iterator, std::string(), ascii::space_type> cmdliteral_;
  qi::rule<Iterator, std::string(), ascii::space_type> pathsubtoken_;
  qi::rule<Iterator, std::string(), ascii::space_type> pathtoken_;
  qi::rule<Iterator, std::string(), ascii::space_type> pathpart_;
  qi::rule<Iterator, std::string(), ascii::space_type> path1_;
  qi::rule<Iterator, std::string(), ascii::space_type> path_;
  qi::rule<Iterator, std::string(), ascii::space_type> literal_;
  qi::rule<Iterator, std::string(), ascii::space_type> prefixop_;
  qi::rule<Iterator, std::string(), ascii::space_type> binop_;
  qi::rule<Iterator, std::string(), ascii::space_type> list_;
  qi::rule<Iterator, std::string(), ascii::space_type> parens_;

/*
  // Object literals
  qi::rule<Iterator, std::string(), ascii::space_type> memberext_;
  qi::rule<Iterator, std::string(), ascii::space_type> member_;
  qi::rule<Iterator, std::string(), ascii::space_type> objectbody_;
  qi::rule<Iterator, std::string(), ascii::space_type> object_;

  // Function literals
  qi::rule<Iterator, std::string(), ascii::space_type> arg_;
  qi::rule<Iterator, std::string(), ascii::space_type> arglist_;
  qi::rule<Iterator, std::string(), ascii::space_type> functionargs_;
  qi::rule<Iterator, std::string(), ascii::space_type> functionreturn_;
  qi::rule<Iterator, std::string(), ascii::space_type> function_;
*/

  // Expressions
  qi::rule<Iterator, std::string(), ascii::space_type> atom_;
  qi::rule<Iterator, std::string(), ascii::space_type> prefixexp_;
  qi::rule<Iterator, std::string(), ascii::space_type> binopexp_;
  qi::rule<Iterator, std::string(), ascii::space_type> subexp_;
  qi::rule<Iterator, std::string(), ascii::space_type> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> type_;

/*
  // New statements
  qi::rule<Iterator, std::string(), ascii::space_type> newassign1_;
  qi::rule<Iterator, std::string(), ascii::space_type> newassign_;
  qi::rule<Iterator, std::string(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> renew_;
  qi::rule<Iterator, std::string(), ascii::space_type> del_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtnew_;

  // Misc statements
  qi::rule<Iterator, std::string(), ascii::space_type> stmtisvar_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmttypeof_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtassign_;

  // Procedure call statements
*/
  qi::rule<Iterator, std::string(), ascii::space_type> explist_;
/*
  qi::rule<Iterator, std::string(), ascii::space_type> stmtproccall_;

  // Branch constructs
  qi::rule<Iterator, std::string(), ascii::space_type> branchpred_;
  qi::rule<Iterator, std::string(), ascii::space_type> if_;
  qi::rule<Iterator, std::string(), ascii::space_type> elif_;
  qi::rule<Iterator, std::string(), ascii::space_type> elsepred_;
  qi::rule<Iterator, std::string(), ascii::space_type> else_;

  // Loop constructs
  qi::rule<Iterator, std::string(), ascii::space_type> looppred_;
  qi::rule<Iterator, std::string(), ascii::space_type> looptimes_;
  qi::rule<Iterator, std::string(), ascii::space_type> loopcond_;
  qi::rule<Iterator, std::string(), ascii::space_type> loop_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtloop_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtbreak_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtcmd_;

  // Statements
  qi::rule<Iterator, std::string(), ascii::space_type> stmt_;
*/

  // Blocks
  qi::rule<Iterator, std::string(), ascii::space_type> expblock_;
  qi::rule<Iterator, std::string(), ascii::space_type> stmtext_;
  qi::rule<Iterator, std::string(), ascii::space_type> ifstmtext_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblockstmt_;
  qi::rule<Iterator, std::string(), ascii::space_type> ifpostcodeblockstmt_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblockcodeblock_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblockitem_;
  qi::rule<Iterator, std::string(), ascii::space_type> ifpostcodeblockitem_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblockbody_;
  qi::rule<Iterator, std::string(), ascii::space_type> ifpostcodeblockbody_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblock_;

  // Program invocation
  qi::rule<Iterator, std::string(), ascii::space_type> programbasic_;
  qi::rule<Iterator, std::string(), ascii::space_type> programexts_;
  qi::rule<Iterator, std::string(), ascii::space_type> programargs_;
  qi::rule<Iterator, std::string(), ascii::space_type> program_;
  qi::rule<Iterator, std::string(), ascii::space_type> programinvocation_;

  qi::rule<Iterator, void(), ascii::space_type> cmdline_;
  qi::rule<Iterator, void(), ascii::space_type> shok_;
};

}

#endif // _Shok_h_
