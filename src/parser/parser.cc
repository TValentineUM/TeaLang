#include "parser.hh"
using namespace parser;

#include <iostream>
#include <sstream>
#include <stdexcept>

void Parser::parse_statement() {
  switch (curr_tok.type) {
  case lexer::tok_let:

    std::cout << "Parsing let" << std::endl;
    parse_var_decl();
    break;
  case lexer::tok_end:
    std::cout << "Reached End" << std::endl;
    break;
  case lexer::tok_print:
    std::cout << "Parsing print" << std::endl;
    parse_print();
    break;
  case lexer::tok_return:
    std::cout << "Parsing Return" << std::endl;
    parse_return();
    break;
  case lexer::tok_if:
  case lexer::tok_for:
  case lexer::tok_while:
  case lexer::tok_iden:
    std::cout << "Parsing Assignment" << std::endl;
    parse_assignment();
    break;
  case lexer::tok_type_int:
  case lexer::tok_type_float:
  case lexer::tok_type_bool:
  case lexer::tok_type_string:
  case lexer::tok_curly_left:

  default:
    std::cout << curr_tok << std::endl;
    std::cout << "Not Implemented Yet" << std::endl;
  }
}

void Parser::parse_expression() {
  parse_simple_expression();
  if (curr_tok.type == lexer::tok_relational) {
    parse_simple_expression();
  }
}

void Parser::parse_simple_expression() {
  parse_term();
  if (curr_tok.type == lexer::tok_add_op) {
    parse_term();
  }
}

void Parser::parse_term() {
  parse_factor();
  if (curr_tok.type == lexer::tok_multi_op) {
    parse_factor();
  }
}

void Parser::parse_actual_params() {
  do {
    parse_expression();
  } while (curr_tok.type == lexer::tok_comma);
  if (curr_tok.type != lexer::tok_round_right) {
    std::stringstream ss;
    ss << "Missing ')' after function call on line: " << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }
}

void Parser::parse_factor() {
  curr_tok = lex.getNxtToken();
  switch (curr_tok.type) {
  case lexer::tok_lit_bool:
  case lexer::tok_lit_float:
  case lexer::tok_lit_int:
  case lexer::tok_lit_string:
    std::cout << "matched literal" << std::endl;
    curr_tok = lex.getNxtToken();
    break;
  case lexer::tok_iden:
    ll1_tok = lex.getNxtToken();
    if (ll1_tok.type == lexer::tok_round_left) {
      // parse function call
      std::cout << "function call!" << std::endl;

      parse_actual_params();
      curr_tok = lex.getNxtToken();
    } else {
      std::cout << "identifier" << std::endl;
      curr_tok = std::move(ll1_tok);
    }
    break;
  case lexer::tok_round_left:
    std::cout << "matched subexpression" << std::endl;
    parse_expression();
    if (curr_tok.type != lexer::tok_round_right) {
      std::stringstream ss;
      ss << "Subexpression not terminated on line: " << curr_tok.line_number
         << " after assignment";
      throw std::runtime_error(ss.str());
    }
    curr_tok = lex.getNxtToken();
    break;
  case lexer::tok_unary:
    std::cout << "matched unary" << std::endl;
    parse_expression();
    break;
  default:
    std::cout << curr_tok << std::endl;
    std::stringstream ss;
    ss << "Unable to match production on line: " << curr_tok.line_number
       << " after assignment";
    throw std::runtime_error(ss.str());
  }
}

void Parser::parse_var_decl() {

  // let

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_iden) {
    std::stringstream ss;
    ss << "Expected Identifier on line: " << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  // let x

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_colon) {
    std::stringstream ss;
    ss << "Expected ':' on line: " << curr_tok.line_number
       << " after identifier";
    throw std::runtime_error(ss.str());
  }

  // let x :

  curr_tok = lex.getNxtToken();
  switch (curr_tok.type) {
  case lexer::tok_type_int:
  case lexer::tok_type_string:
  case lexer::tok_type_bool:
  case lexer::tok_type_float:
    break;
  default:
    std::stringstream ss;
    ss << "Expected type decleration on line: " << curr_tok.line_number
       << " after ':' ";
    throw std::runtime_error(ss.str());
  }

  // let x : int

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_assign) {
    std::stringstream ss;
    ss << "Expected assignment operator on line: " << curr_tok.line_number
       << " after type decleration ";
    throw std::runtime_error(ss.str());
  }

  // let x : int =

  parse_expression();

  // lex x: int = 5
  if (curr_tok.type != lexer::tok_semicolon) {
    std::stringstream ss;
    ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
       << curr_tok.value << "' Expected ';'";
    throw std::runtime_error(ss.str());
  }
}

void Parser::parse_print() {
  try {
    parse_expression();
  } catch (...) {
    std::stringstream ss;
    ss << "Unable to parse expression after print statement on line: "
       << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    std::stringstream ss;
    ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
       << curr_tok.value << "' Expected ';'";
    throw std::runtime_error(ss.str());
  } else {
    std::cout << "Print success" << std::endl;
  }
}

void Parser::parse_return() {
  try {
    parse_expression();
  } catch (...) {
    std::stringstream ss;
    ss << "Unable to parse expression after return statement on line: "
       << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    std::stringstream ss;
    ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
       << curr_tok.value << "' Expected ';'";
    throw std::runtime_error(ss.str());
  } else {
    std::cout << "Return Success" << std::endl;
  }
}

void Parser::parse_assignment() {
  curr_tok = lex.getNxtToken();

  if (curr_tok.type != lexer::tok_assign) {
    std::stringstream ss;
    ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
       << curr_tok.value << "' Expected '='";
    throw std::runtime_error(ss.str());
  }

  try {
    parse_expression();
  } catch (...) {
    std::stringstream ss;
    ss << "Unable to parse expression after assignment on line: "
       << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    std::stringstream ss;
    ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
       << curr_tok.value << "' Expected ';'";
    throw std::runtime_error(ss.str());
  } else {
    std::cout << "Assignment Success" << std::endl;
  }
}
