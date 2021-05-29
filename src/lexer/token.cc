#include "token.hh"
using namespace lexer;

#include <algorithm>
#include <map>
#include <regex>
#include <string>

std::map<std::string, tl_token> identifiers = {{"float", tok_type_float},
                                               {"int", tok_type_int},
                                               {"bool", tok_type_bool},
                                               {"string", tok_type_string},
                                               {"true", tok_lit_bool},
                                               {"false", tok_lit_bool},
                                               {"not", tok_unary},
                                               {"let", tok_let},
                                               {"print", tok_print},
                                               {"return", tok_return},
                                               {"if", tok_if},
                                               {"else", tok_else},
                                               {"for", tok_for},
                                               {"while", tok_while},
                                               {"or", tok_add_op},
                                               {"and", tok_multi_op},
                                               {"char", tok_type_char},
                                               {"auto", tok_type_auto},
                                               {"tlstruct", tok_struct}};

void Token::match_token(std::string value, int state) {

  switch (state) {
  case 1:
    type = tok_lit_int;
    break;
  case 3:
    type = tok_lit_float;
    break;
  case 4:
    type = identifiers[value];
    // If the keyword lookup fails it defaults to 0 which is tok_iden
    break;
  case 5:
    type = tok_assign;
    break;
  case 7:
  case 8:
    type = tok_relational;
    break;
  case 9:
    switch (value.at(0)) {
    case '{':
      type = tok_curly_left;
      break;
    case '}':
      type = tok_curly_right;
      break;
    case ';':
      type = tok_semicolon;
      break;
    case ':':
      type = tok_colon;
      break;
    case '(':
      type = tok_round_left;
      break;
    case ')':
      type = tok_round_right;
      break;
    case ',':
      type = tok_comma;
      break;
    case '[':
      type = tok_square_left;
      break;
    case ']':
      type = tok_square_right;
      break;
    case '.':
      type = tok_decimal;
    }
    break;
  case 10:
    switch (value.at(0)) {
    case '+':
    case '-':
      type = tok_add_op;
      break;
    case '*':
      type = tok_multi_op;
      break;
    }
    break;
  case 13:
    type = tok_lit_string;
    value.erase(value.begin());
    value.erase(value.end() - 1, value.end());
    eval_escape_codes();
    break;
  case 14:
    type = tok_multi_op;
    break;
  case 17:
    type = tok_comment;
    break;
  case 21:
    type = tok_lit_char;
    value.erase(value.begin());
    value.erase(value.end() - 1, value.end());
    break;
  case -1:
    type = tok_end;
    break;
  default:
    std::cout << "Unknown Production @ Line: " << line_number << std::endl;
    // throw std::invalid_argument("Unkown Production");
  }
  this->value = value;
}

void Token::eval_escape_codes() {
  value = std::regex_replace(value, std::regex("\\n"), "\n");
}
