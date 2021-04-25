#ifndef __PARSER_H_
#define __PARSER_H_

#include <map>
#include <optional>
#include <string>

#include "../lexer/lexer.hh"

namespace parser {

class Parser {

private:
  void fail(std::string expected);

public:
  explicit Parser(std::string filename) : lex{lexer::Lexer(filename)} {
    parse_program();
  };
  lexer::Lexer lex;
  lexer::Token curr_tok;               /**< Current Token*/
  std::optional<lexer::Token> ll1_tok; /**< Single Lookahead*/

  void parse_program();
  void parse_expression();
  void parse_simple_expression();
  void parse_term();
  void parse_factor();
  void parse_actual_params();
  void parse_formal_params();

  void parse_block();
  void parse_statement();
  void parse_var_decl();
  void parse_assignment();
  void parse_print();
  void parse_return();
  void parse_function_decl();

  void parse_while();
  void parse_for();
  void parse_if();
};

} // namespace parser

#endif // __PARSER_H_
