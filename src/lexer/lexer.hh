#ifndef __LEXER_H_
#define __LEXER_H_

#include <iostream>
#include <string>
#include <vector>

#include "token.hh"

namespace lexer {

enum dfa_state { S0 = 0, S1, S2, S3, S4, SE, BAD } typedef dfa_state;
enum char_class { Digit = 0, Decimal, Identifier, Err } typedef char_class;

class Lexer {
private:
  std::vector<Token> tokens;
  std::vector<Token>::iterator
      token_counter; /**< Tracks the next token to be returned */

  void tokenize(std::ifstream &file);
  char_class char_cat(char x);
  dfa_state transition(dfa_state state, char_class x);

  /*           S0 S1 S2 S3 S4 SE           */
  bool SA[6] = {0, 1, 0, 1, 1, 0};

  dfa_state transition_table[6][5] = {
      //      Digit    Decimal  Iden   Else
      /*S0*/ {S1, /**/ SE, /**/ S4, /**/ SE},
      /*S1*/ {S1, /**/ S2, /**/ SE, /**/ SE},
      /*S2*/ {S3, /**/ SE, /**/ SE, /**/ SE},
      /*S3*/ {S3, /**/ SE, /**/ SE, /**/ SE},
      /*S4*/ {S4, /**/ SE, /**/ S4, /**/ SE},
      /*SE*/ {SE, /**/ SE, /**/ SE, /**/ SE},
  };

public:
  Lexer(std::string file);

  friend std::ostream &operator<<(std::ostream &out, const Lexer &lex) {
    for (auto i : lex.tokens) {
      out << i << std::endl;
    }
    return out;
  }

  Token getNxtToken();
};

} // namespace lexer

#endif // __LEXER_H_
