#ifndef __PARSER_H_
#define __PARSER_H_

/**
 * @file parser.hh
 * @author Tony Valentine
 * @brief LL Recursive Descent Parser
 *
 * The file contains the Parser class definition for TealangV1. The parser is
 * implemented as a generic LLk recursive descent parser. The first set for
 * Tealang is unique for all instaces outside of identifiers and function calls,
 * and as such the majority of the implementation code take an LL1 approach.
 *
 * The implementation is heavily based on that found in the "Parsing" chapter of
 * "Engineering a Compiler" with all the functions wrapped inside 1 class that
 * generates the AST for the program.
 *
 * @date 30 March 2020
 */

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
  lexer::Lexer lex;      /**< Lexer class used to tokenize the input file*/
  lexer::Token curr_tok; /**< Current Token */
  std::optional<lexer::Token> ll1_tok; /**< Single Lookahead */

  ASTProgram *tree; /**< Final AST Tree generated after parsing */

  ASTProgram *parse_program();
  ASTExpression *parse_expression();
  ASTExpression *parse_simple_expression();
  ASTExpression *parse_term();
  ASTExpression *parse_factor(); /**< Parses Literals, Function Calls,
                                   SubExpressions, Unaries and Literals */
  std::vector<ASTExpression *> parse_actual_params();
  std::vector<std::tuple<std::string, Tealang_t>> parse_formal_params();

  ASTBlock *parse_block();
  ASTStatement *parse_statement();
  ASTVariableDecl *parse_var_decl();
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
