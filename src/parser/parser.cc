#include "parser.hh"
using namespace parser;

#include <iostream>
#include <sstream>
#include <stdexcept>

void Parser::parse_statement() {
  switch (curr_tok.type) {
  case lexer::tok_let:
    parse_var_decl();
    std::cout << "Parsing let" << std::endl;
    break;
  case lexer::tok_end:
    std::cout << "Reached End" << std::endl;
    break;
  case lexer::tok_print:
  case lexer::tok_return:
  case lexer::tok_if:
  case lexer::tok_for:
  case lexer::tok_while:
  case lexer::tok_iden:
  case lexer::tok_type_int:
  case lexer::tok_type_float:
  case lexer::tok_type_bool:
  case lexer::tok_type_string:
  case lexer::tok_curly_left:

  default:
    std::cout << "Not Implemented Yet" << std::endl;
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
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_semicolon) {
    std::stringstream ss;
    ss << "Missing ; on line: " << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }
}

void Parser::parse_expression() {
  parse_simple_expression();
  // curr_tok = lex.getNxtToken();
  // if (curr_tok.type == lexer::tok_relational) {
  //   parse_simple_expression();
  // }
}

void Parser::parse_simple_expression() {
  parse_term();
  // curr_tok = lex.getNxtToken();
  // if (curr_tok.type == lexer::tok_add_op) {
  //   parse_term();
  // }
}

void Parser::parse_term() {
  parse_factor();
  // curr_tok = lex.getNxtToken();
  // if (curr_tok.type == lexer::tok_multi_op) {
  //   parse_factor();
  // }
}

void Parser::parse_factor() {
  curr_tok = lex.getNxtToken();
  switch (curr_tok.type) {
  case lexer::tok_lit_bool:
  case lexer::tok_lit_float:
  case lexer::tok_lit_int:
  case lexer::tok_lit_string:
    std::cout << "matched literal" << std::endl;
    break;
  case lexer::tok_iden:
    ll1_tok = lex.peakNextToken();
    if (ll1_tok.type == lexer::tok_round_left) {
      // parse function call
      std::cout << "function call" << std::endl;
    } else {
      std::cout << "Just identifier" << std::endl;
    }
  case lexer::tok_round_left:
    curr_tok = lex.getNxtToken();
    parse_expression();
    curr_tok = lex.getNxtToken();
    if (curr_tok.type != lexer::tok_round_right) {
      std::stringstream ss;
      ss << "Subexpression not terminated on line: " << curr_tok.line_number
         << " after assignment";
      throw std::runtime_error(ss.str());
    }
  case lexer::tok_unary:
  default:
    std::stringstream ss;
    ss << "Unable to match production on line: " << curr_tok.line_number
       << " after assignment";
    throw std::runtime_error(ss.str());
  }
}
