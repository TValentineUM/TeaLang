#include "parser.hh"
using namespace parser;

#include <iostream>
#include <sstream>
#include <stdexcept>

void Parser::fail(std::string expected) {

  std::stringstream ss;
  ss << "Unexpected token line: " << curr_tok.line_number << ". Found '"
     << curr_tok.value << "' Expected '" << expected << "'";
  throw std::runtime_error(ss.str());
}

void Parser::parse_program() {
  do {
    if (ll1_tok.has_value()) {
      curr_tok = ll1_tok.value();
      ll1_tok.reset();
    } else {
      // std::cout << "ll1 has no value" << std::endl;
      curr_tok = lex.getNxtToken();
    }
    parse_statement();
  } while (curr_tok.type != lexer::tok_end);
}

void Parser::parse_statement() {
  // std::cout << curr_tok << std::endl;

  switch (curr_tok.type) {
  case lexer::tok_let:

    // std::cout << "Parsing let" << std::endl;
    parse_var_decl();
    break;
  case lexer::tok_end:
    // std::cout << "Reached End" << std::endl;
    break;
  case lexer::tok_print:
    // std::cout << "Parsing print" << std::endl;
    parse_print();
    break;
  case lexer::tok_return:
    // std::cout << "Parsing Return" << std::endl;
    parse_return();
    break;
  case lexer::tok_if:
    // std::cout << "Parsing If" << std::endl;
    parse_if();
    break;
  case lexer::tok_for:
    // std::cout << "Parsing For" << std::endl;
    parse_for();
    break;
  case lexer::tok_while:
    // std::cout << "Parsing While" << std::endl;
    parse_while();
    break;
  case lexer::tok_iden:
    // std::cout << "Parsing Assignment" << std::endl;
    parse_assignment();
    break;
  case lexer::tok_type_int:
  case lexer::tok_type_float:
  case lexer::tok_type_bool:
  case lexer::tok_type_string:
    // std::cout << "Parsing Function Decleration" << std::endl;
    parse_function_decl();
    break;
  case lexer::tok_curly_left:
    // std::cout << "Parsing Block" << std::endl;
    parse_block();
    break;
  default:
    // std::cout << curr_tok << std::endl;
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
    // std::cout << "matched literal" << std::endl;
    curr_tok = lex.getNxtToken();
    break;
  case lexer::tok_iden:
    ll1_tok = lex.getNxtToken();
    if (ll1_tok->type == lexer::tok_round_left) {
      // parse function call
      // std::cout << "function call!" << std::endl;
      parse_actual_params();
      curr_tok = lex.getNxtToken();
    } else {
      // std::cout << "identifier" << std::endl;
      curr_tok = ll1_tok.value();
    }
    ll1_tok.reset();
    break;
  case lexer::tok_round_left:
    // std::cout << "matched subexpression" << std::endl;
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
    // std::cout << "matched unary" << std::endl;
    parse_expression();
    break;
  default:
    // std::cout << curr_tok << std::endl;
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
    fail("Identifier");
  }

  // let x

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_colon) {
    fail(":");
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
    fail("Type Decleration");
  }

  // let x : int

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_assign) {
    fail("=");
  }

  // let x : int =

  parse_expression();

  // lex x: int = 5
  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
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
    fail(";");
  } else {
    // std::cout << "Print success" << std::endl;
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
    fail(";");
  } else {
    // std::cout << "Return Success" << std::endl;
  }
}

void Parser::parse_assignment() {
  curr_tok = lex.getNxtToken();

  if (curr_tok.type != lexer::tok_assign) {
    fail("=");
  }

  try {
    parse_expression();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse expression after assignment on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  } else {
    // std::cout << "Assignment Success" << std::endl;
  }
}

void Parser::parse_block() {
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_right) {
    do {
      parse_statement();
      if (ll1_tok.has_value()) {
        curr_tok = ll1_tok.value();
        ll1_tok.reset();
      } else {
        // std::cout << "ll1 has no value" << std::endl;
        curr_tok = lex.getNxtToken();
      }
      if (curr_tok.type == lexer::tok_curly_right) {
        break;
      }
    } while (curr_tok.type != lexer::tok_end);
    if (curr_tok.type != lexer::tok_curly_right) {
      fail("}");
    }
  }
}

// Needs to start with identifier
void Parser::parse_formal_params() {
  do {
    curr_tok = lex.getNxtToken();
    if (curr_tok.type != lexer::tok_iden) {
      fail("Identifier");
    }
    curr_tok = lex.getNxtToken();
    if (curr_tok.type != lexer::tok_colon) {
      fail(":");
    }
    curr_tok = lex.getNxtToken();
    switch (curr_tok.type) {
    case lexer::tok_type_bool:
    case lexer::tok_type_float:
    case lexer::tok_type_int:
    case lexer::tok_type_string:
      break;
    default:
      fail("Type Decleration");
    }
    curr_tok = lex.getNxtToken();
  } while (curr_tok.type == lexer::tok_comma);
}

void Parser::parse_function_decl() {

  // int

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_iden) {
    fail("Identifier");
  }

  // int fib

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  // int fib (
  parse_formal_params();

  // int fib (x:int

  if (curr_tok.type != lexer::tok_round_right) {
    fail(")");
  }

  // int fib (x:int)
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }
  parse_block();
  // std::cout << "AFTER FUNCTION BLOCK: " << curr_tok << std::endl;
  // int fib (x:int){}
}

void Parser::parse_while() {
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  try {
    parse_expression();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse expression in while on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }

  if (curr_tok.type != lexer::tok_round_right) {
    fail(")");
  }

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }
  parse_block();
}

void Parser::parse_for() {
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  curr_tok = lex.getNxtToken();
  try {
    parse_var_decl();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse variable decleration in for loop on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }

  curr_tok = lex.getNxtToken();

  try {
    parse_expression();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse expression in for loop on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  }
  curr_tok = lex.getNxtToken();

  try {
    parse_assignment();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse assignment in for loop on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }
  if (curr_tok.type != lexer::tok_round_right) {
    fail(")");
  }
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }
  parse_block();
}

void Parser::parse_if() {

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }
  try {
    parse_expression();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse expression in for loop on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }

  parse_block();
  if (curr_tok.type != lexer::tok_curly_right) {
    fail("}");
  }
  // std::cout << curr_tok << std::endl;
  ll1_tok = lex.getNxtToken();
  // std::cout << ll1_tok->value << std::endl;
  if (ll1_tok->type == lexer::tok_else) {
    // std::cout << "Here" << std::endl;
    ll1_tok.reset();
    curr_tok = lex.getNxtToken();
    parse_block();
  }
}
