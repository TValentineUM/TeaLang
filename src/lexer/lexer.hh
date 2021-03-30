#ifndef __LEXER_H_
#define __LEXER_H_

/**
 * @file lexer.hh
 * @author Tony Valentine
 * @brief Table Driven Lexer
 *
 * This header file contains the main implementation of a table driven lexer for
 * the TeaLang programming language. The implementation is heavily based of the
 * examples given in the "Engineering a Compiler" book.
 *
 * @date 30 March 2020
 */

#include <iostream>
#include <string>
#include <vector>

#include "token.hh"

namespace lexer {

enum dfa_state {
  S0 = 0, /**< Initial System State */
  S1,     /**< Acceptance State for Integer*/
  S2,     /**< Invalid State: [0-9]*(.) */
  S3,     /**< Acceptance State for Floats*/
  S4,     /**< Acceptance State for Identifiers and Keywords*/
  S5,     /**< Acceptance State for Assignment =*/
  S6,     /**< Invalid State: (!)*/
  S7,     /**< Acceptance State for Comparison*/
  S8,     /**< Acceptance State for Comparison and or Equality*/
  S9,     /** Acceptance State for Single Character Punctuation*/
  S10,    /**< Acceptance State for (+|-|*)*/
  SE,     /**< Invalid State*/
  BAD     /**< State used for lexer algorithm*/
} typedef dfa_state;

enum char_class {
  Digit = 0,   /**< [0-9]*/
  Decimal,     /**< (.)*/
  Identifier,  /**< (_)|[A-z]*/
  Comparison,  /**< (<|>)*/
  Equals,      /**< (=)*/
  Bang,        /**< (!)*/
  Punctuation, /**< (\(|\)|\{|\}|:|;) */
  FSlash,      /**< (/) */
  PlusMinus,   /**< (+|-)*/
  Asterisk,    /**< (*) */
  Err          /**< Unkown Character Type*/
} typedef char_class;

class Lexer {
private:
  std::vector<Token> tokens;
  std::vector<Token>::iterator
      token_counter; /**< Tracks the next token to be returned */

  /**
   * @brief Main Lexing Function
   *
   * This function implements the main lexing loop based of the sample code
   * found in page 61 of "Enginerring a Compiler".
   *
   * @param file A valid input file stream
   */
  void tokenize(std::ifstream &file);
  /**
   * @brief Determines Character Categories
   *
   * @param x input character
   *
   * @return char_class
   */
  char_class char_cat(char x);

  /**
   * @brief DFA \delta transition function
   *
   * Implements the functionality of the \delta transition function for DFSA's
   *
   * @param state The current state of the system
   * @param x The character class of the input
   *
   * @return The updated DFA state
   *
   */
  dfa_state transition(dfa_state state, char_class x);

  /** @brief Accepting States for the DFSA */
  bool SA[12] = {0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0};
  /*             S0 S1 S2 S3 S4 S5 S6 S7 S8 S9 S10 SE            */

  /** @brief DFSA Transition Table Definition */
  dfa_state transition_table[12][11] = {
      /*       Di  De  Id  <>  =   !   };  FS  PM  AS Ukn*/
      /*S0 */ {S1, SE, S4, S7, S5, S6, S9, SE, S10, S10, SE},
      /*S1 */ {S1, S2, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S2 */ {S3, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S3 */ {S3, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S4 */ {S4, SE, S4, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S5 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE},
      /*S6 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE},
      /*S7 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE},
      /*S8 */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S9 */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S10*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*SE */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
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
