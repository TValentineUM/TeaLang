#include "lexer.hh"
using namespace lexer;
#include <algorithm>
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
    tokenize(input);
    input.close();
  }
  token_counter = tokens.begin();
}

Token Lexer::getNxtToken() {

  if (token_counter != tokens.end()) {
    return *token_counter++;
  } else {
    throw std::runtime_error("Tokens Expired");
  }
}

Token Lexer::peakNextToken() {
  if (token_counter != tokens.end()) {
    return *token_counter;
  } else {
    throw std::runtime_error("Tokens Expired");
  }
}

void Lexer::tokenize(std::ifstream &file) {
  int line_number = 0;
  do {
    char c = file.get();
    dfa_state state = S0;
    std::string lexeme = "";
    std::stack<dfa_state> buffer;
    buffer.push(BAD);
    while (c == ' ' || c == '\n' || c == '\t') {
      if (c == '\n') {
        line_number++;
      }
      c = file.get();
      if (file.eof()) {
        break;
      }
    }

    if (file.peek() == EOF) {
      break;
    } else {
      while (state != SE) {
        if (c == '\n') {
          line_number++;
        }
        if (SA[state]) {
          while (!buffer.empty()) {
            buffer.pop();
          }
        }
        buffer.push(state);
        lexeme.push_back(c);
        char_class cat = char_cat(c);
        state = transition(state, cat);
        if (state != SE) {
          c = file.get();
        }
        if (file.eof()) {
          break;
        }
      }

      while (!SA[state] && state != BAD) {
        state = buffer.top();
        buffer.pop();
        file.unget();
        if (file.peek() == '\n') {
          line_number--;
        }
        lexeme.pop_back();
      }
      if (SA[state]) {
        Token temp(lexeme, state, line_number);
        if (temp.type > -1) {
          tokens.push_back(temp);
        }
      } else {
        std::cout << file.peek();
        std::cout << "Invalid Production on Line: " << line_number << std::endl;
        throw std::invalid_argument("Unkown token provided");
      }
    }
  } while (file.peek() != EOF);
  tokens.push_back(Token("", -1, line_number));
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
  case '=':
    return Equals;
  case '!':
    return Bang;
  case '<': // Waterfall down
  case '>':
    return Comparison;
  case '{':
  case '}':
  case ';':
  case ':':
  case '(':
  case ')':
  case ',':
  case '[':
  case ']':
    return Punctuation;
  case '/':
    return FSlash;
  case '+':
  case '-':
    return PlusMinus;
  case '*':
    return Asterisk;
  case '\\':
    return BSlash;
  case '"':
    return DQuote;
  case '\'':
    return SQuote;
  case '\n':
    return Newline;

  default:
    if (isprint(x)) {
      return Printable;
    } else {
      return Err;
    }
  }
}
