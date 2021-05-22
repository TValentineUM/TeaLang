#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <iostream>
#include <map>
#include <string>

namespace lexer {

enum tl_token {
  tok_iden = 0,
  tok_type_int,
  tok_type_float,
  tok_type_bool,
  tok_type_string,
  tok_type_char,
  tok_lit_bool,
  tok_lit_int,
  tok_lit_float,
  tok_lit_string,
  tok_lit_char,
  tok_multi_op,
  tok_add_op,
  tok_curly_left,
  tok_curly_right,
  tok_round_left,
  tok_round_right,
  tok_square_left,
  tok_square_right,
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
  tok_type_auto,
  tok_end,
  tok_comment = -1,
} typedef tl_token;

static std::map<tl_token, std::string> tok_word = {
    {tok_iden, "tok_iden"},
    {tok_type_int, "tok_type_int"},
    {tok_type_float, "tok_type_float"},
    {tok_type_bool, "tok_type_bool"},
    {tok_type_string, "tok_type_string"},
    {tok_type_char, "tok_type_char"},
    {tok_lit_bool, "tok_lit_bool"},
    {tok_lit_int, "tok_lit_int"},
    {tok_lit_float, "tok_lit_float"},
    {tok_lit_string, "tok_lit_string"},
    {tok_lit_char, "tok_lit_char"},
    {tok_multi_op, "tok_multi_op"},
    {tok_add_op, "tok_add_op"},
    {tok_curly_left, "tok_curly_left"},
    {tok_curly_right, "tok_curly_right"},
    {tok_round_left, "tok_round_left"},
    {tok_round_right, "tok_round_right"},
    {tok_square_left, "tok_square_left"},
    {tok_square_right, "tok_square_right"},
    {tok_semicolon, "tok_semicolon"},
    {tok_colon, "tok_colon"},
    {tok_comma, "tok_comma"},
    {tok_unary, "tok_unary"},
    {tok_let, "tok_let"},
    {tok_print, "tok_print"},
    {tok_return, "tok_return"},
    {tok_if, "tok_if"},
    {tok_else, "tok_else"},
    {tok_for, "tok_for"},
    {tok_while, "tok_while"},
    {tok_relational, "tok_relational"},
    {tok_assign, "tok_assign"},
    {tok_type_auto, "tok_type_auto"},
    {tok_end, "EOF"}};

class Token {

private:
  void match_token(std::string value, int state);

public:
  Token() {}
  Token(std::string value, int state, int line_number)
      : line_number{line_number} {
    match_token(value, state);
  }
  std::string value; /**< Character stream that produced token*/
  tl_token type;     /**< Denotes the type of the token as specified in
                      documentation */
  int line_number;   /**< Denotes the linenumber of the token*/

  friend std::ostream &operator<<(std::ostream &out, const Token &tok) {
    out << '{' << tok_word[tok.type] << "," << tok.value << ","
        << tok.line_number << "}";
    return out;
  }
};

} // namespace lexer
#endif // __TOKEN_H_
