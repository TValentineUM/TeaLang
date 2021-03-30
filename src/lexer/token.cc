#include "token.hh"
using namespace lexer;

#include <string>

void Token::match_token(std::string value, int state) {

  switch (state) {
  case 1:
    type = tl_int;
    break;
  case 3:
    type = tl_double;
    break;
  }
  this->value = value;
}
