#include "lexer.hh"
using namespace lexer;
#include <fstream>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>

Lexer::Lexer(std::string file) {
  std::ifstream input;
  input.open(file);
  if (!input) {
    throw std::invalid_argument("File not found");
  } else {
    for (std::string line; std::getline(input, line);) {
      tokenize(line);
    }
  }
}

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
    while (*next_char == ' ') {
      input.erase(next_char);
      next_char = input.begin();
    }
    if (next_char != input.end()) {
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
        lexeme = std::string(input.begin(), next_char);
        input.erase(input.begin(), next_char);
        Token temp(lexeme, state);
        tokens.push_back(temp);
      } else {
        std::cout << input << std::endl;
        std::cout << "error" << std::endl;
        throw std::invalid_argument("Unkown token provided");
      }
    } else {
      break;
    }
  } while (next_char != input.end());
}

dfa_state Lexer::transition(dfa_state state, char_class x) {
  return transition_table[state][x];
}

char_class Lexer::char_cat(char x) {
  if (isdigit(x)) {
    return Digit;
  }
  if (isalpha(x) || x == '_') {
    return Identifier;
  }
  switch (x) {
  case '.':
    return Decimal;
  default:
    return Err;
  }
}
