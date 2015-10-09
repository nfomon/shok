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

  auto_ptr<Rule> identifier_(META("identifier", "ID"));   // typedef_name_

  auto_ptr<Rule> unary_operator_(OR("unary-operator"));
  unary_operator_->AddChild(META("&", "&"));
  unary_operator_->AddChild(META("*", "*"));
  unary_operator_->AddChild(META("+", "+"));
  unary_operator_->AddChild(META("-", "-"));
  unary_operator_->AddChild(META("~", "~"));
  unary_operator_->AddChild(META("!", "!"));

  auto_ptr<Rule> assignment_operator_(OR("assignment-operator"));
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

  auto_ptr<Rule> goto_statement_(SEQ("goto-statement"));  // jump_statement_
  goto_statement_->AddChild(META("goto", "goto"));
  //goto_statement_->AddChild(identifier_);
  goto_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> continue_statement_(SEQ("continue-statement"));  // jump_statement_
  continue_statement_->AddChild(META("continue", "continue"));
  continue_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> break_statement_(SEQ("break-statement"));  // jump_statement_
  break_statement_->AddChild(META("break", "break"));
  break_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> return_statement_(SEQ("return-statement"));  // jump_statement_
  return_statement_->AddChild(META("return", "return"));
  //return_statement_->AddChild(OPT(expression_));
  return_statement_->AddChild(META(";", ";"));

  auto_ptr<Rule> jump_statement_(OR("jump-statement")); // statement_
  jump_statement_->AddChild(goto_statement_);
  jump_statement_->AddChild(continue_statement_);
  jump_statement_->AddChild(break_statement_);
  jump_statement_->AddChild(return_statement_);


  // type-qualifier-list
  // parameter-type-list
  // parameter-list
  // parameter-declaration
  // identifier-list
  // initializer
  // initializer-list
  // type-name
  // abstract-declarator
  // direct-abstract-declarator
  // statement
  // labeled-statement
  // expression-statement
  // compound-statement
  // statement-list
  // selection-statement
  // iteration-statement
  // v

  auto_ptr<Rule> statement_(OR("statement"));
  //statement_->AddChild(labeled_statement_);
  //statement_->AddChild(expression_statement_);
  //statement_->AddChild(compound_statement_);
  //statement_->AddChild(selection_statement_);
  //statement_->AddChild(iteration_statement_);
  statement_->AddChild(jump_statement_);

  // ^
  // direct-declarator
  // declarator

/*
  auto_ptr<Rule> enumerator_1(SEQ("enumerator-1"));
  enumerator_1->AddChild(META("=", "="));
  enumerator_1->AddChild(constant_expression_);

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

/*
  auto_ptr<Rule> struct_declarator_1(SEQ("struct-declarator-1"));
  struct_declarator_1->AddChild(OPT(declarator_));
  struct_declarator_1->AddChild(META(":", ":"));
  struct_declarator_1->AddChild(constant_expression_);

  auto_ptr<Rule> struct_declarator_(OR("struct-declarator"));
  struct_declarator_->AddChild(declarator_);
  struct_declarator_->AddChild(struct_declarator_1);

  auto_ptr<Rule> struct_declarator_list_1(OR("struct-declarator-list-1"));
  struct_declarator_list_1->AddChild(META(",", ","));
  struct_declarator_list_1->AddChild(struct_declarator_);

  auto_ptr<Rule> struct_declarator_list_(SEQ("struct-declarator-list"));
  struct_declarator_list_->AddChild(struct_declarator_);
  struct_declarator_list_->AddChild(struct_declarator_list_1);

  auto_ptr<Rule> specifier_qualifier_list_1(OR("specifier-qualifier-list-1"));
  specifier_qualifier_list_1->AddChild(type_specifier_);
  specifier_qualifier_list_1->AddChild(type_qualifier_);

  auto_ptr<Rule> specifier_qualifier_list_(OR("specifier-qualifier-list"));
  specifier_qualifier_list_->AddChild(specifier_qualifier_list_1);
  specifier_qualifier_list_->AddChild(OPT(specifier_qualifier_list_);

  auto_ptr<Rule> struct_declaration_(SEQ("struct-declaration"));
  struct_declaration_->AddChild(specifier_qualifier_list_);
  struct_declaration_->AddChild(struct_declarator_list_);
  struct_declaration_->AddChild(META(";", ";"));

  auto_ptr<Rule> init_declarator_1(OR("init-declarator-1"));
  init_declarator_1->AddChild(declarator_);
  init_declarator_1->AddChild(META("=", "="));
  init_declarator_1->AddChild(initializer_);

  auto_ptr<Rule> init_declarator_(OR("init-declarator"));
  init_declarator_->AddChild(declarator_);
  init_declarator_->AddChild(init_declarator_1);

  auto_ptr<Rule> init_declarator_list_1(SEQ("init-declarator-list-1"));
  init_declarator_list_1->AddChild(META(",", ","));
  init_declarator_list_1->AddChildRecursive(init_declarator_);

  auto_ptr<Rule> init_declarator_list_(SEQ("init-declarator-list"));
  init_declarator_list_->AddChild(init_declarator_);
  init_declarator_list_->AddChild(STAR(init_declarator_list_1));

  auto_ptr<Rule> struct_or_union_(OR("struct-or-union"));
  struct_or_union_->AddChild(META("struct", "struct"));
  struct_or_union_->AddChild(META("union", "union"));

  auto_ptr<Rule> struct_or_union_specifier_1(SEQ("struct-or-union-specifier-1"));
  struct_or_union_specifier_1->AddChild(struct_or_union_);
  struct_or_union_specifier_1->AddChild(OPT(identifier_));
  struct_or_union_specifier_1->AddChild(META("{", "{"));
  struct_or_union_specifier_1->AddChild(PLUS(struct_declaration_));
  struct_or_union_specifier_1->AddChild(META("}", "}"));

  auto_ptr<Rule> struct_or_union_specifier_2(SEQ("struct-or-union-specifier-2"));
  struct_or_union_specifier_2->AddChildRecursive(struct_or_union_);
  struct_or_union_specifier_1->AddChildRecursive(identifier_);

  auto_ptr<Rule> struct_or_union_specifier_(OR("struct-or-union-specifier"));
  struct_or_union_specifier_->AddChild(struct_or_union_specifier_1);
  struct_or_union_specifier_->AddChild(struct_or_union_specifier_2);
*/

/*
  auto_ptr<Rule> pointer_1(SEQ("pointer-1"));
  pointer_1->AddChild(META("*", "*"));
  Rule* pointer_type_qualifier_ = pointer_1->AddChild(STAR("pointer-type-qualifier"));
  pointer_type_qualifier_->AddChild(type_qualifier_);

  auto_ptr<Rule> pointer_(PLUS("pointer"));
  pointer_->AddChild(pointer_1);
*/

  auto_ptr<Rule> typedef_name_(identifier_);  // type_specifier_

  auto_ptr<Rule> type_qualifier_(OR("type-qualifier")); // declaration_specifiers_1
  type_qualifier_->AddChild(META("const", "const"));
  type_qualifier_->AddChild(META("volatile", "volatile"));

  auto_ptr<Rule> type_specifier_(OR("type-specifier")); // declaration_specifiers_1
  type_specifier_->AddChild(META("void", "void"));
  type_specifier_->AddChild(META("char", "char"));
  type_specifier_->AddChild(META("short", "short"));
  type_specifier_->AddChild(META("int", "int"));
  type_specifier_->AddChild(META("long", "long"));
  type_specifier_->AddChild(META("float", "float"));
  type_specifier_->AddChild(META("double", "double"));
  type_specifier_->AddChild(META("signed", "signed"));
  type_specifier_->AddChild(META("unsigned", "unsigned"));
  //type_specifier_->AddChild(struct_or_union_specifier_);
  //type_specifier_->AddChild(enum_specifier_);
  type_specifier_->AddChild(typedef_name_);

  auto_ptr<Rule> storage_class_specifier_(OR("storage-class-specifier")); // declaration_specifiers_1
  storage_class_specifier_->AddChild(META("auto", "auto"));
  storage_class_specifier_->AddChild(META("register", "register"));
  storage_class_specifier_->AddChild(META("static", "static"));
  storage_class_specifier_->AddChild(META("extern", "extern"));
  storage_class_specifier_->AddChild(META("typedef", "typedef"));

  auto_ptr<Rule> declaration_specifiers_1(OR("declaration-specifiers-1"));  // declaration_specifiers_
  declaration_specifiers_1->AddChild(storage_class_specifier_);
  declaration_specifiers_1->AddChild(type_specifier_);
  declaration_specifiers_1->AddChild(type_qualifier_);

  auto_ptr<Rule> declaration_specifiers_(SEQ("declaration-specifiers"));  // declaration_
  declaration_specifiers_->AddChild(declaration_specifiers_1);
  Rule* declaration_specifiers_OPT = declaration_specifiers_->AddChild(OPT("declaration-specifiers-OPT"));

  auto_ptr<Rule> declaration_(SEQ("declaration"));  // external_declaration_
  Rule* declaration_specifiers_REC = declaration_->AddChild(declaration_specifiers_);
  //declaration_->AddChild(OPT(init_declarator_list_));
  declaration_->AddChild(META(";", ";"));

  declaration_specifiers_OPT->AddChildRecursive(declaration_specifiers_REC);

/*
  auto_ptr<Rule> function_definition_(SEQ("function-definition"));
  function_definition->AddChild(OPT(declaration_specifiers_));
  function_definition->AddChild(declarator_);
  function_definition->AddChild(PLUS(declaration_));
  function_definition->AddChild(compound_statement_);
*/

  auto_ptr<Rule> external_declaration_(OR("external-declaration")); // translation_unit_
  //external_declaration_->AddChild(function_definition_);
  external_declaration_->AddChild(declaration_);

  auto_ptr<Rule> translation_unit_(PLUS("C_parser"));
  translation_unit_->AddChild(external_declaration_);
  return translation_unit_;
}
