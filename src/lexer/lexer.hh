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
  S11,    /**< Main Body of String Literal*/
  S12,    /**< Escaped Character*/
  S13,    /**< Acceptance State for String Literal*/
  S14,    /**< Acceptance State for Division*/
  S15,    /**< Opening Multiline Comment*/
  S16,    /**< Asterist in Comments*/
  S17,    /**< Acceptance State for Block Comment*/
  S18,    /**< Single Line Comment*/
  S19,    /**< Single Quote */
  S20,    /**< Single Character */
  S21,    /**< Valid Quote */
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
  Punctuation, /**< ,:;(){}[] */
  FSlash,      /**< (/) */
  PlusMinus,   /**< (+| -)*/
  Asterisk,    /**< (*) */
  BSlash,      /**< \\ aka Backslash*/
  DQuote,      /**< (")*/
  SQuote,      /**< Singe Quotation Character*/
  Newline,     /** Newline Character*/
  Printable,   /**< All Printable Ascii Characters*/
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
  bool SA[24] = {0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0,
                 /* s1 S2 S3 S4 S5 S6 S7 S8 S9 S10 S11   */
                 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0};
  /*             12 13 14 15 16 17 18 19 20 21 SE BAD
   *
   */

  /** @brief DFSA Transition Table Definition */
  dfa_state transition_table[23][16] = {
      /*       Di   De   Id   <>   =    !    };   FS   PM   AS   BS   DQ   SQ
         \n   Pr  Ukn*/
      /*S0 */ {S1, SE, S4, S7, S5, S6, S9, S14, S10, S10, SE, S11, S19, SE, SE,
               SE},
      /*S1 */ {S1, S2, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S2 */ {S3, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S3 */ {S3, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S4 */ {S4, SE, S4, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S5 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S6 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S7 */ {SE, SE, SE, SE, S8, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S8 */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S9 */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S10*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S11*/
      {S11, S11, S11, S11, S11, S11, S11, S11, S11, S11, S12, S13, SE, SE, S11,
       SE},
      /*S12*/
      {S11, S11, S11, S11, S11, S11, S11, S11, S11, S11, S11, S11, SE, SE, S11,
       SE},
      /*S13*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S14*/
      {SE, SE, SE, SE, SE, SE, SE, S18, SE, S15, SE, SE, SE, SE, SE, SE},
      /*S15*/
      {S15, S15, S15, S15, S15, S15, S15, S15, S15, S16, S15, S15, SE, S15, S15,
       SE},
      /*S16*/
      {S15, S15, S15, S15, S15, S15, S15, S17, S15, S15, S15, S15, SE, S15, S15,
       SE},
      /*S17*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*S18*/
      {S18, S18, S18, S18, S18, S18, S18, S18, S18, S18, S18, S18, SE, S17, S18,
       SE},
      /*S19*/
      {S20, S20, S20, S20, S20, S20, S20, S20, S20, S20, S20, S20, S20, SE, S20,
       SE},
      /*S20*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, S21, SE, SE, SE},
      /*S21*/ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
      /*SE */ {SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE, SE},
  };

public:
  Lexer(std::string file);
  ~Lexer(){};
  friend std::ostream &operator<<(std::ostream &out, const Lexer &lex) {
    for (auto i : lex.tokens) {
      out << i << std::endl;
    }
    return out;
  }

  Token getNxtToken();
  Token peakNextToken();
};

} // namespace lexer

#endif // __LEXER_H_
