#include "token.hh"
using namespace lexer;

#include <map>
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
                                               {"while", tok_while}};

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
  }
  this->value = value;
}
