#ifndef __PARSER_H_
#define __PARSER_H_

#include <map>
#include <optional>
#include <string>

#include "../lexer/lexer.hh"

#include "AST.hh"

namespace parser {

class Parser {

private:
  void fail(std::string expected);

public:
  explicit Parser(std::string filename) : lex{lexer::Lexer{filename}} {
    tree = parse_program();
  };
  lexer::Lexer lex;
  lexer::Token curr_tok;               /**< Current Token*/
  std::optional<lexer::Token> ll1_tok; /**< Single Lookahead*/

  ASTProgram *tree;

  ASTProgram *parse_program();
  ASTExpression *parse_expression();
  ASTExpression *parse_simple_expression();
  ASTExpression *parse_term();
  ASTExpression *parse_factor();
  ASTUnary *parse_unary();
  ASTArrayLiteral *parse_array_literal();
  ASTExpression *parse_sub_expression();
  ASTFunctionCall *parse_function_call();
  ASTArrayAccess *parse_array_access();
  ASTExpression *parse_struct_operator();

  std::vector<ASTExpression *> parse_actual_params();

  std::vector<std::tuple<std::string, Tealang_t, std::string>>
  parse_formal_params();

  ASTBlock *parse_block();
  ASTStatement *parse_statement();
  ASTStatement *parse_decl();
  ASTVariableDecl *parse_var_decl(); /**< Also Handles Array Declerations*/
  ASTArrayAssignment *parse_arr_assign();
  ASTAssignment *parse_assignment();
  ASTPrintStatement *parse_print();
  ASTReturn *parse_return();
  ASTFunctionDecl *parse_function_decl();
  ASTWhileStatement *parse_while();
  ASTForStatement *parse_for();
  ASTIfStatement *parse_if();
  ASTArrayDecl *parse_array_decl();
  ASTStructDecl *parse_struct_decl(); // Creating a variable of type struct
  ASTStructDefn *parse_struct_def();  // Defining the struct class
  ASTStructAssign *
  parse_struct_assign(); // Assigning to one of the struct members
};

} // namespace parser

#endif // __PARSER_H_
