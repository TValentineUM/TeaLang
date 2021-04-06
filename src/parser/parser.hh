#ifndef __PARSER_H_
#define __PARSER_H_

#include <map>
#include <string>

#include "../lexer/lexer.hh"

namespace parser {

class Parser {

public:
  explicit Parser(std::string filename) : lex{lexer::Lexer(filename)} {
    std::cout << lex << std::endl;
    do {
      curr_tok = lex.getNxtToken();
      parse_statement();
    } while (curr_tok.type != lexer::tok_end);
  };
  lexer::Lexer lex;
  lexer::Token curr_tok; /**< Current Token*/
  lexer::Token ll1_tok;  /**< Single Lookahead*/
  lexer::Token ll2_tok;  /**< Double Lookahead*/

  void parse_statement();
  void parse_var_decl();
  void parse_expression();
  void parse_simple_expression();
  void parse_term();
  void parse_factor();
};

} // namespace parser

#endif // __PARSER_H_
