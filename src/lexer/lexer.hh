#ifndef __LEXER_H_
#define __LEXER_H_

#include <iostream>
#include <string>
#include <vector>

#include "token.hh"

namespace lexer {

enum dfa_state { S0 = 0, S1, S2, S3, SE, BAD } typedef dfa_state;
enum char_class { Digit = 0, Decimal, Err } typedef char_class;

class Lexer {
private:
  std::vector<Token> tokens;
  std::vector<Token>::iterator
      token_counter; /**< Tracks the next token to be returned */

  void tokenize(std::string input);
  char_class char_cat(char x);
  dfa_state transition(dfa_state state, char_class x);

  /*           S0 S1 S2 S3 SE           */
  bool SA[5] = {0, 1, 0, 1, 0};
  // int transition_table[4][5] = {
  //     //           S0  S1  S2  S3  SE
  //     /*Digit  */ {S1, S1, S2, S3, SE},
  //     /*Decimal*/ {SE, S2, SE, SE, SE},
  //     /*Else   */ {SE, SE, SE, SE, SE}};

  dfa_state transition_table[5][4] = {
      //     Digit  Decimal  Else
      /*S0*/ {S1, /**/ SE, /**/ SE},
      /*S1*/ {S1, /**/ S2, /**/ SE},
      /*S2*/ {S3, /**/ SE, /**/ SE},
      /*S3*/ {S3, /**/ SE, /**/ SE},
      /*SE*/ {S3, /**/ SE, /**/ SE},
  };

public:
  Lexer(std::string input) { tokenize(input); }

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
