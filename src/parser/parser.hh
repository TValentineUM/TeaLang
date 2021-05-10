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
  std::vector<ASTExpression *> parse_actual_params();
  std::vector<std::tuple<std::string, Tealang_t>> parse_formal_params();

  ASTBlock *parse_block();
  ASTStatement *parse_statement();
  ASTVariableDecl *parse_var_decl(); /**< Also Handles Array Declerations*/
  ASTAssignment *parse_assignment();
  ASTPrintStatement *parse_print();
  ASTReturn *parse_return();
  ASTFunctionDecl *parse_function_decl();
  ASTWhileStatement *parse_while();
  ASTForStatement *parse_for();
  ASTIfStatement *parse_if();
};

} // namespace parser

#endif // __PARSER_H_
