// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Parser.h"

#include "statik/Meta.h"
#include "statik/Opt.h"
#include "statik/Or.h"
#include "statik/Seq.h"
#include "statik/Star.h"
using statik::META;
using statik::OPT;
using statik::OR;
using statik::PLUS;
using statik::Rule;
using statik::SEQ;
using statik::STAR;

#include <memory>
using std::auto_ptr;

using namespace exstatik;

// parser = ((new ;)|(del ;))*
auto_ptr<Rule> exstatik::CreateParser_Simple() {
  auto_ptr<Rule> parser(STAR("parser"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  //newstmt_->AddChild(META("x"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  //delstmt_->AddChild(META("x"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();
  return parser;
}

// parser = ((new WS ID ;)|(del WS ID ;))*
auto_ptr<Rule> exstatik::CreateParser_Moderate() {
  auto_ptr<Rule> parser(STAR("parser"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("cmd");
  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  newstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  newstmt_->AddChild(META("identifier", "ID"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();
  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  delstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  delstmt_->AddChild(META("identifier", "ID"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();
  return parser;
}

// parser = ((cmd|codeblock)end)*
auto_ptr<Rule> exstatik::CreateParser_Complex() {
  auto_ptr<Rule> parser(STAR("parser"));
  Rule* line = parser->AddChild(SEQ("line"));
  //line->AddChild(META("WS"));
  Rule* cmdorcode = line->AddChild(OR("Or (cmdorcode)"));
  Rule* end = line->AddChild(OR("end"));
  end->AddChild(META(";"));
  end->AddChild(META("\n"));

  Rule* cmdline = cmdorcode->AddChild(SEQ("cmdline"));
  cmdline->AddChild(META("ID", "program"));
  Rule* cmdargs = cmdline->AddChild(STAR("* (cmdargs)"));
  Rule* cmdarg = cmdargs->AddChild(SEQ("cmdarg"));
  cmdarg->AddChild(META("WS"));
  cmdarg->AddChild(META("arg"));

  Rule* codeblock = cmdorcode->AddChild(SEQ("codeblock"));
  codeblock->AddChild(META("{"));
  Rule* stmts = codeblock->AddChild(STAR("* (stmts)"));
  codeblock->AddChild(META("}"));

  Rule* stmt = stmts->AddChild(OR("Or (stmt)"));
  Rule* newstmt = stmt->AddChild(SEQ("new stmt"));
  newstmt->AddChild(META("new"));
  newstmt->AddChild(META("WS"));
  newstmt->AddChild(META("identifier", "ID"));
  newstmt->AddChild(META(";"));
  Rule* delstmt = stmt->AddChild(SEQ("del stmt"));
  delstmt->AddChild(META("del"));
  delstmt->AddChild(META("WS"));
  delstmt->AddChild(META("identifier", "ID"));
  delstmt->AddChild(META(";"));
  return parser;
}

// Nifty parser
auto_ptr<Rule> exstatik::CreateParser_Nifty() {
  auto_ptr<Rule> parser(STAR("parser"));
  Rule* stmts_ = parser->AddChild(OR("Or (stmts)"))
    ->CapOutput("stmt");

  Rule* cmd_ = stmts_->AddChild(SEQ("cmd stmt"));
  cmd_->AddChild(META("cmd", "ID"));

  Rule* cmdexts_ = cmd_->AddChild(STAR("cmdexts"));
  Rule* cmdext_ = cmdexts_->AddChild(SEQ("cmdext"));
  cmdext_->AddChild(META("cmd", "WS"));
  cmdext_->AddChild(META("cmd", "ID"));

  Rule* newstmt_ = stmts_->AddChild(SEQ("new stmt"));
  newstmt_->AddChild(META("new"));
  newstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  Rule* newor_ = newstmt_->AddChild(STAR("new*"));
  newor_->AddChildRecursive(newstmt_);
  newstmt_->AddChild(META("identifier", "ID"));
  newstmt_->AddChild(META(";"))
    ->SilenceOutput();

  Rule* delstmt_ = stmts_->AddChild(SEQ("del stmt"));
  delstmt_->AddChild(META("del"));
  delstmt_->AddChild(META("WS"))
    ->SilenceOutput();
  delstmt_->AddChild(META("identifier", "ID"));
  delstmt_->AddChild(META(";"))
    ->SilenceOutput();

  return parser;
}

// C parser
auto_ptr<Rule> exstatik::CreateParser_C() {

  auto_ptr<Rule> identifier_(META("identifier", "ID"));   // goto_statement_
  auto_ptr<Rule> integer_constant_(META("integer", "INT"));   // constant_
  auto_ptr<Rule> character_constant_(META("char", "CHAR"));   // constant_
  auto_ptr<Rule> floating_constant_(META("float", "FLOAT"));   // constant_
  auto_ptr<Rule> enumeration_constant_(META("enum", "ENUM"));   // constant_

  auto_ptr<Rule> constant_(OR("constant")); // FAKE constant_expression_
  constant_->AddChild(integer_constant_);
  constant_->AddChild(character_constant_);
  constant_->AddChild(floating_constant_);
  constant_->AddChild(enumeration_constant_);

  /*
    postfix-expression:
      primary-expression
      postfix-expression [ expression ]
      postfix-expression ( argument-expression-list_OPT )
      postfix-expression . identifier
      postfix-expression -> identifier
      postfix-expression ++
      postfix-expression --
    primary-expression:
      identifier
      constant
      string
      ( expression )
    argument-expression-list:
      assignment-expression
      argument-expression-list , assignment-expression
  */

  auto_ptr<Rule> unary_operator_(OR("unary-operator"));
  unary_operator_->AddChild(META("&", "&"));
  unary_operator_->AddChild(META("*", "*"));
  unary_operator_->AddChild(META("+", "+"));
  unary_operator_->AddChild(META("-", "-"));
  unary_operator_->AddChild(META("~", "~"));
  unary_operator_->AddChild(META("!", "!"));

  // FAKE
  auto_ptr<Rule> constant_expression_(constant_);

  /*
    conditional-expression:
      logical-OR-expression
      logical-OR-expression ? expression : conditional-expression
    constant-expression:
      conditional-expression
    logical-OR-expression:
      logical-AND-expression
      logical-OR-expression || logical-AND-expression
    logical-AND-expression:
      inclusive-OR-expression
      logical-AND-expression && inclusive-OR-expression
    inclusive-OR-expression:
      exclusive-OR-expression
      inclusive-OR-expression | exclusive-OR-expression
    exclusive-OR-expression:
      AND-expression
      exclusive-OR-expression ^ AND-expression
    AND-expression:
      equality-expression
      AND-expression & equality-expression
    equality-expression:
      relational-expression
      equality-expression == relational-expression
      equality-expression != relational-expression
    relational-expression:
      shift-expression
      relational-expression < shift-expression
      relational-expression > shift-expression
      relational-expression <= shift-expression
      relational-expression >= shift-expression
    shift-expression:
      additive-expression
      shift-expression << additive-expression
      shift-expression >> additive-expression
    additive-expression:
      multiplicative-expression
      additive-expression + multiplicative-expression
      additive-expression - multiplicative-expression
    multiplicative-expression:
      cast-expression
      multiplicative-expression * cast-expression
      multiplicative-expression / cast-expression
      multiplicative-expression % cast-expression
    cast-expression:
      unary-expression
      ( type-name ) cast-expression
    unary-expression:
      postfix-expression
      ++ unary-expression
      -- unary-expression
      unary-operator cast-expression
      sizeof unary-expression
      sizeof ( type-name )
  */

  auto_ptr<Rule> assignment_operator_(OR("assignment-operator")); // FAKE assignment_expression_
  assignment_operator_->AddChild(META("=", "="));
  assignment_operator_->AddChild(META("*=", "*="));
  assignment_operator_->AddChild(META("/=", "/="));
  assignment_operator_->AddChild(META("%=", "%="));
  assignment_operator_->AddChild(META("+=", "+="));
  assignment_operator_->AddChild(META("-=", "+="));
  assignment_operator_->AddChild(META("<<=", "<<="));
  assignment_operator_->AddChild(META(">>=", ">>="));
  assignment_operator_->AddChild(META("&=", "&="));
  assignment_operator_->AddChild(META("^=", "^="));
  assignment_operator_->AddChild(META("|=", "|="));

  /*
    expression:
      assignment-expression (, assignmentexpression)*
    assignment-expression:
      conditional-expression
      unary-expression assignment-operator assignment-expression
  */

  // FAKE
  auto_ptr<Rule> assignment_expression_(assignment_operator_);

  auto_ptr<Rule> return_statement_(SEQ("return-statement"));  // jump_statement_
  return_statement_->AddChild(META("return", "return"));
  //return_statement_->AddChild(OPT(expression_));
  return_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> break_statement_(SEQ("break-statement"));  // jump_statement_
  break_statement_->AddChild(META("break", "break"));
  break_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> continue_statement_(SEQ("continue-statement"));  // jump_statement_
  continue_statement_->AddChild(META("continue", "continue"));
  continue_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> goto_statement_(SEQ("goto-statement"));  // jump_statement_
  goto_statement_->AddChild(META("goto", "goto"));
  Rule* identifier_REC = goto_statement_->AddChild(identifier_);
  goto_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> jump_statement_(OR("jump-statement")); // statement_
  jump_statement_->AddChild(goto_statement_);
  jump_statement_->AddChild(continue_statement_);
  jump_statement_->AddChild(break_statement_);
  jump_statement_->AddChild(return_statement_);

  /*
    labeled-statement:
      identifier : statement
      case constant-expression : statement
      default : statement
    expression-statement:
      expression_OPT ;
    (compound-statement)
    selection-statement:
      if ( expression ) statement
      if ( expression ) statement else statement
      switch ( expression ) statement
    iteration-statement:
      while ( expression ) statement
      do statement while ( expression ) ;
      for ( expression_OPT ; expression_OPT ; expression_OPT ) statement
  */

  auto_ptr<Rule> statement_(OR("statement")); // compound_statement_
  //statement_->AddChild(labeled_statement_);
  //statement_->AddChild(expression_statement_);
  //statement_->AddChild(selection_statement_);
  //statement_->AddChild(iteration_statement_);
  statement_->AddChild(jump_statement_);

  auto_ptr<Rule> compound_statement_(SEQ("compound-statement"));  // function_definition_
  compound_statement_->AddChild(META("{", "{"));
  Rule* declaration_STAR = compound_statement_->AddChild(STAR("declaration-STAR"));
  Rule* statement_STAR = compound_statement_->AddChild(STAR("statement-STAR"));
  Rule* statement_REC = statement_STAR->AddChild(statement_);
  compound_statement_->AddChild(META("}", "}"));

  /*
    type-name:
      specifier-qualifier-list abstract-declarator_OPT
    abstract-declarator:
      pointer
      pointer_OPT direct-abstract-declarator
    direct-abstract-declarator:
      ( abstract-declarator )
      direct-abstract-declarator_OPT [ constant-expression_OPT ]
      direct-abstract-declarator_OPT ( parameter-type-list_OPT )
    typedef-name:
      identifier
  */

  auto_ptr<Rule> initializer_2(SEQ("initializer-2")); // initializer_1

  auto_ptr<Rule> initializer_1(SEQ("initializer-1")); // initializer_

  auto_ptr<Rule> initializer_(OR("initializer")); // init_declarator_
  initializer_->AddChild(assignment_expression_);
  Rule* initializer_1_REC = initializer_->AddChild(initializer_1);
  Rule* initializer_2_REC = initializer_->AddChild(initializer_2);

  /*
    parameter-type-list:
      parameter-list
      parameter-list , ...
    parameter-list:
      parameter-declaration
      parameter-list , parameter-declaration
    parameter-declaration:
      declaration-specifiers declarator
      declaration-specifiers abstract-declarator_OPT
    identifier-list:
      identifier
      identifier-list , identifier
  */

  auto_ptr<Rule> pointer_1(SEQ("pointer-1")); // pointer_

  auto_ptr<Rule> pointer_(PLUS("pointer")); // declarator_
  Rule* pointer_1_REC = pointer_->AddChild(pointer_1);

/*
  auto_ptr<Rule> direct_declarator_4(SEQ("direct-declarator-4")); // direct_declarator_
  direct_declarator_4->AddChildRecursive(direct_declarator_REC);
  direct_declarator_4->AddChild(META("(", "("));
  //Rule* identifier_list_OPT = direct_declarator_4->AddChild(OPT("identifier-list-OPT"));
  //identifier_list_OPT->AddChild(identifier_list_);
  direct_declarator_4->AddChild(META(")", ")"));
*/

  //auto_ptr<Rule> direct_declarator_3(SEQ("direct-declarator-3")); // direct_declarator_

  //auto_ptr<Rule> direct_declarator_2(SEQ("direct-declarator-2")); // direct_declarator_

  //auto_ptr<Rule> direct_declarator_1(SEQ("direct-declarator-1")); // direct_declarator_

  auto_ptr<Rule> direct_declarator_(OR("direct-declarator")); // declarator_
  direct_declarator_->AddChildRecursive(identifier_REC);
  //Rule* direct_declarator_1_REC = direct_declarator_->AddChild(direct_declarator_1);
  //Rule* direct_declarator_2_REC = direct_declarator_->AddChild(direct_declarator_2);
  //Rule* direct_declarator_3_REC = direct_declarator_->AddChild(direct_declarator_3);
  //Rule* direct_declarator_4_REC = direct_declarator_->AddChild(direct_declarator_4);

  auto_ptr<Rule> declarator_(SEQ("declarator"));  // struct_declarator_1
  Rule* pointer_OPT = declarator_->AddChild(OPT("pointer-OPT"));
  /*Rule* pointer_REC = */ pointer_OPT->AddChild(pointer_);
  Rule* direct_declarator_REC = declarator_->AddChild(direct_declarator_);

/*
  auto_ptr<Rule> enumerator_1(SEQ("enumerator-1"));
  enumerator_1->AddChild(META("=", "="));
  enumerator_1->AddChildRecursive(constant_expression_REC);

  auto_ptr<Rule> enumerator_(SEQ("enumerator"));
  enumerator_->AddChild(identifier_);
  enumerator_->AddChild(OPT(enumerator_1));

  auto_ptr<Rule> enumerator_list_1(SEQ("enumerator-list-1"));
  enumerator_list_1->AddChild(META(",", ","));
  enumerator_list_1->AddChild(enumerator_);

  auto_ptr<Rule> enumerator_list_(SEQ("enumerator-list"));
  enumerator_list->AddChild(enumerator_);
  enumerator_list->AddChild(STAR(enumerator_list_1));

  auto_ptr<Rule> enum_specifier_1(SEQ("enum-specifier-1"));
  enum_specifier_1->AddChild(META("enum", "enum"));
  enum_specifier_1->AddChild(identifier_);

  auto_ptr<Rule> enum_specifier_2(SEQ("enum-specifier-2"));
  enum_specifier_2->AddChild(META("enum", "enum"));
  enum_specifier_2->AddChild(OPT(identifier_));
  enum_specifier_2->AddChild(META("{", "{"));
  enum_specifier_2->AddChild(enumerator_list_);
  enum_specifier_2->AddChild(META("}", "}"));

  auto_ptr<Rule> enum_specifier_(OR("enum-specifier"));
  enum_specifier_->AddChild(enum_specifier_1);
  enum_specifier_->AddChild(enum_specifier_2);
*/

  auto_ptr<Rule> struct_declarator_1(SEQ("struct-declarator-1")); // struct_declarator_
  Rule* declarator_OPT = struct_declarator_1->AddChild(OPT("declarator-OPT"));
  Rule* declarator_REC = declarator_OPT->AddChild(declarator_);

  auto_ptr<Rule> struct_declarator_(OR("struct-declarator")); // struct_declarator_list_
  struct_declarator_->AddChildRecursive(declarator_REC);
  Rule* struct_declarator_1_REC = struct_declarator_->AddChild(struct_declarator_1);

  auto_ptr<Rule> struct_declarator_list_1(OR("struct-declarator-list-1"));  // struct_declarator_list_
  struct_declarator_list_1->AddChild(META(",", ","));
  Rule* struct_declarator_REC = struct_declarator_list_1->AddChild(struct_declarator_);

  auto_ptr<Rule> struct_declarator_list_(SEQ("struct-declarator-list"));  // specifier_qualifier_list_1
  struct_declarator_list_->AddChildRecursive(struct_declarator_REC);
  struct_declarator_list_->AddChild(struct_declarator_list_1);

  auto_ptr<Rule> specifier_qualifier_list_1(SEQ("specifier-qualifier-list-1")); // specifier_qualifier_list_

  auto_ptr<Rule> specifier_qualifier_list_2(SEQ("specifier-qualifier-list-2")); // specifier_qualifier_list_

  auto_ptr<Rule> specifier_qualifier_list_(OR("specifier-qualifier-list")); // struct_declaration_
  Rule* specifier_qualifier_list_1_REC = specifier_qualifier_list_->AddChild(specifier_qualifier_list_1);
  Rule* specifier_qualifier_list_2_REC = specifier_qualifier_list_->AddChild(specifier_qualifier_list_2);

  auto_ptr<Rule> struct_declaration_(SEQ("struct-declaration"));  // struct_or_union_specifier_

  auto_ptr<Rule> init_declarator_1(OR("init-declarator-1"));  // init_declarator_
  init_declarator_1->AddChildRecursive(declarator_REC);
  init_declarator_1->AddChild(META("=", "="));
  Rule* initializer_REC = init_declarator_1->AddChild(initializer_);

  auto_ptr<Rule> init_declarator_(OR("init-declarator")); // init_declarator_list_1
  init_declarator_->AddChildRecursive(declarator_REC);
  init_declarator_->AddChild(init_declarator_1);

  auto_ptr<Rule> init_declarator_list_1(SEQ("init-declarator-list-1")); // init_declarator_list_1_STAR
  init_declarator_list_1->AddChild(META(",", ","));
  Rule* init_declarator_REC = init_declarator_list_1->AddChild(init_declarator_);

  auto_ptr<Rule> init_declarator_list_(SEQ("init-declarator-list"));  // declaration_
  init_declarator_list_->AddChildRecursive(init_declarator_REC);
  Rule* init_declarator_list_1_STAR = init_declarator_list_->AddChild(STAR("init-declarator-list-1-STAR"));
  init_declarator_list_1_STAR->AddChild(init_declarator_list_1);

  auto_ptr<Rule> struct_or_union_(OR("struct-or-union")); // struct_or_union_specifier_1
  struct_or_union_->AddChild(META("struct", "struct"));
  struct_or_union_->AddChild(META("union", "union"));

  auto_ptr<Rule> struct_or_union_specifier_1(SEQ("struct-or-union-specifier-1")); // struct_or_union_specifier_
  Rule* struct_or_union_REC = struct_or_union_specifier_1->AddChild(struct_or_union_);
  Rule* identifier_OPT = struct_or_union_specifier_1->AddChild(OPT("identifier-OPT"));
  identifier_OPT->AddChildRecursive(identifier_REC);
  struct_or_union_specifier_1->AddChild(META("{", "{"));
  Rule* struct_declaration_PLUS = struct_or_union_specifier_1->AddChild(PLUS("struct-declaration-PLUS"));
  Rule* struct_declaration_REC = struct_declaration_PLUS->AddChild(struct_declaration_);
  struct_or_union_specifier_1->AddChild(META("}", "}"));

  auto_ptr<Rule> struct_or_union_specifier_2(SEQ("struct-or-union-specifier-2")); // struct_or_union_specifier_
  struct_or_union_specifier_2->AddChildRecursive(struct_or_union_REC);
  struct_or_union_specifier_1->AddChildRecursive(identifier_REC);

  auto_ptr<Rule> struct_or_union_specifier_(OR("struct-or-union-specifier")); // type_specifier_
  struct_or_union_specifier_->AddChild(struct_or_union_specifier_1);
  struct_or_union_specifier_->AddChild(struct_or_union_specifier_2);

  auto_ptr<Rule> typedef_name_(META("identifier", "ID")); // type_specifier_

  auto_ptr<Rule> type_qualifier_(OR("type-qualifier")); // declaration_specifiers_1
  type_qualifier_->CapOutput("type-qualifier");
  type_qualifier_->AddChild(META("const", "const"));
  type_qualifier_->AddChild(META("volatile", "volatile"));

  auto_ptr<Rule> type_specifier_(OR("type-specifier")); // declaration_specifiers_1
  type_specifier_->CapOutput("type-specifier");
  type_specifier_->AddChild(META("void", "void"));
  type_specifier_->AddChild(META("char", "char"));
  type_specifier_->AddChild(META("short", "short"));
  type_specifier_->AddChild(META("int", "int"));
  type_specifier_->AddChild(META("long", "long"));
  type_specifier_->AddChild(META("float", "float"));
  type_specifier_->AddChild(META("double", "double"));
  type_specifier_->AddChild(META("signed", "signed"));
  type_specifier_->AddChild(META("unsigned", "unsigned"));
  type_specifier_->AddChild(struct_or_union_specifier_);
  //type_specifier_->AddChild(enum_specifier_);
  type_specifier_->AddChild(typedef_name_);

  auto_ptr<Rule> storage_class_specifier_(OR("storage-class-specifier")); // declaration_specifiers_1
  storage_class_specifier_->CapOutput("storage-class-specifier");
  storage_class_specifier_->AddChild(META("auto", "auto"));
  storage_class_specifier_->AddChild(META("register", "register"));
  storage_class_specifier_->AddChild(META("static", "static"));
  storage_class_specifier_->AddChild(META("extern", "extern"));
  storage_class_specifier_->AddChild(META("typedef", "typedef"));

  auto_ptr<Rule> declaration_specifiers_1(OR("declaration-specifiers-1"));  // declaration_specifiers_
  declaration_specifiers_1->AddChild(storage_class_specifier_);
  Rule* type_specifier_REC = declaration_specifiers_1->AddChild(type_specifier_);
  Rule* type_qualifier_REC = declaration_specifiers_1->AddChild(type_qualifier_);

  auto_ptr<Rule> declaration_specifiers_(SEQ("declaration-specifiers"));  // declaration_
  declaration_specifiers_->AddChild(declaration_specifiers_1);
  Rule* declaration_specifiers_OPT = declaration_specifiers_->AddChild(OPT("declaration-specifiers-OPT"));

  auto_ptr<Rule> declaration_(SEQ("declaration"));  // external_declaration_
  declaration_->CapOutput("declaration");
  Rule* declaration_specifiers_REC = declaration_->AddChild(declaration_specifiers_);
  Rule* init_declarator_list_OPT = declaration_->AddChild(OPT("init-declarator-list-OPT"));
  init_declarator_list_OPT->AddChild(init_declarator_list_);
  declaration_->AddChild(META(";", ";"));

  auto_ptr<Rule> function_definition_(SEQ("function-definition"));  // external_declaration_
  function_definition_->CapOutput("function-definition");
  function_definition_->AddChildRecursive(declaration_specifiers_OPT);
  function_definition_->AddChildRecursive(declarator_REC);
  function_definition_->AddChildRecursive(declaration_STAR);
  Rule* compound_statement_REC = function_definition_->AddChild(compound_statement_);

  auto_ptr<Rule> external_declaration_(OR("external-declaration")); // translation_unit_
  external_declaration_->CapOutput("external-declaration");
  external_declaration_->AddChild(function_definition_);
  Rule* declaration_REC = external_declaration_->AddChild(declaration_);

  auto_ptr<Rule> translation_unit_(PLUS("C_parser"));
  translation_unit_->AddChild(external_declaration_);

  // recursive reckoning
  declaration_specifiers_OPT->AddChildRecursive(declaration_specifiers_REC);
  declaration_STAR->AddChildRecursive(declaration_REC);

  specifier_qualifier_list_1_REC->AddChildRecursive(type_specifier_REC);
  Rule* struct_declarator_list_OPT = specifier_qualifier_list_1_REC->AddChild(OPT("struct-declarator-list-OPT"));
  Rule* struct_declarator_list_REC = struct_declarator_list_OPT->AddChild(struct_declarator_list_);

  specifier_qualifier_list_2_REC->AddChildRecursive(type_qualifier_REC);
  specifier_qualifier_list_2_REC->AddChildRecursive(struct_declarator_list_REC);

  struct_declaration_REC->AddChild(specifier_qualifier_list_);
  struct_declaration_REC->AddChildRecursive(struct_declarator_list_REC);
  struct_declaration_REC->AddChild(META(";", ";"));

/*
  direct_declarator_1_REC->AddChild(META("(", "("));
  direct_declarator_1_REC->AddChildRecursive(declarator_REC);
  direct_declarator_1_REC->AddChild(META(")", ")"));

  direct_declarator_2_REC->AddChildRecursive(direct_declarator_REC);
  direct_declarator_2_REC->AddChild(META("[", "["));
  Rule* constant_expression_REC = direct_declarator_2_REC->AddChild(constant_expression_);
  direct_declarator_2_REC->AddChild(META("]", "]"));

  direct_declarator_3_REC->AddChildRecursive(direct_declarator_REC);
  direct_declarator_3_REC->AddChild(META("(", "("));
  //direct_declarator_3_REC->AddChild(parameter_type_list_);
  direct_declarator_3_REC->AddChild(META(")", ")"));
*/

  struct_declarator_1_REC->AddChild(META(":", ":"));
  //struct_declarator_1_REC->AddChildRecursive(constant_expression_REC);

  initializer_2_REC->AddChild(META(",", ","));
  Rule* initializer_OPT = initializer_2_REC->AddChild(OPT("initializer-OPT"));
  initializer_OPT->AddChildRecursive(initializer_REC);

  initializer_1_REC->AddChild(META("{", "{"));
  initializer_1_REC->AddChildRecursive(initializer_REC);
  Rule* initializer_2_OPT = initializer_1_REC->AddChild(OPT("initializer-2-OPT"));
  initializer_2_OPT->AddChild(initializer_2);
  initializer_1_REC->AddChild(META("}", "}"));

  pointer_1_REC->AddChild(META("*", "*"));
  Rule* type_qualifier_STAR = pointer_1_REC->AddChild(STAR("type-qualifier-STAR"));
  type_qualifier_STAR->AddChildRecursive(type_qualifier_REC);

  statement_REC->AddChildRecursive(compound_statement_REC);

  statement_STAR->AddChildRecursive(statement_REC);

  return translation_unit_;
}
