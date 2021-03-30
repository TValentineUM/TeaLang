#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <iostream>
#include <string>

namespace lexer {

enum tl_token { tl_int = 0, tl_double } typedef tl_token;

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
    out << '{' << tok.type << ", " << tok.value << "}";
    return out;
  }
};

} // namespace lexer
#endif // __TOKEN_H_
