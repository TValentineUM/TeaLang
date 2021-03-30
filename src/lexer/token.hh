#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <iostream>
#include <string>

namespace lexer {

enum tl_token {
  tok_iden = 0,
  tok_type_int,
  tok_type_float,
  tok_type_bool,
  tok_type_string,
  tok_lit_bool,
  tok_lit_int,
  tok_lit_float,
  tok_lit_string,
  tok_multi_op,
  tok_add_op,
  tok_curly_left,
  tok_curly_right,
  tok_round_left,
  tok_round_right,
  tok_semicolon,
  tok_colon,
  tok_comma,
  tok_unary,
  tok_let,
  tok_print,
  tok_return,
  tok_if,
  tok_else,
  tok_for,
  tok_while,
  tok_relational,
  tok_assign,
} typedef tl_token;

class Token {

private:
  void match_token(std::string value, int state);

public:
  Token() {}
  Token(std::string value, int state) { match_token(value, state); }
  std::string value; /**< Character stream that produced token*/
  tl_token type;     /**< Denotes the type of the token as specified in
                        documentation */

  friend std::ostream &operator<<(std::ostream &out, const Token &tok) {
    out << '{' << tok.type << "," << tok.value << "}";
    return out;
  }
};

} // namespace lexer
#endif // __TOKEN_H_
