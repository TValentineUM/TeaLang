#include "lexer.hh"
using namespace lexer;
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>

Token Lexer::getNxtToken() {
  if (token_counter != tokens.end()) {
    return *token_counter++;
  } else {
    throw std::runtime_error("Tokens Expired");
  }
}

void Lexer::tokenize(std::string input) {
  std::string::iterator next_char;
  do {
    dfa_state state = S0;
    std::string lexeme = "";
    std::stack<dfa_state> buffer;
    buffer.push(BAD);
    next_char = input.begin();
    while (state != SE) {
      if (SA[state]) {
        while (!buffer.empty()) {
          buffer.pop();
        };
      }
      buffer.push(state);
      char_class cat = char_cat(*next_char);
      state = transition(state, cat);
      next_char++;
      if (next_char == input.end()) {
        break;
      }
    }
    while (!SA[state] && state != BAD) {
      state = buffer.top();
      buffer.pop();
      next_char--;
    }
    if (SA[state]) {
      std::cout << "Hello" << std::endl;
      lexeme = std::string(input.begin(), next_char);
      std::cout << lexeme << std::endl;

      Token temp(lexeme, state);
      tokens.push_back(temp);
    } else {
      std::cout << tokens.size() << std::endl;
      std::cout << "error" << std::endl;
      throw std::invalid_argument("Unkown token provided");
    }
    std::cout << (next_char != input.end()) << std::endl;
  } while (next_char != input.end());
}

dfa_state Lexer::transition(dfa_state state, char_class x) {
  return transition_table[state][x];
}

char_class Lexer::char_cat(char x) {
  if (isdigit(x)) {
    return Digit;
  } else {
    switch (x) {
    case '.':
      return Decimal;
    default:
      return Err;
    }
  }
}
