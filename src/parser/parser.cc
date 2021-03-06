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

ASTProgram *Parser::parse_program() {
  ASTProgram *node = new ASTProgram();
  do {
    if (ll1_tok.has_value()) {
      curr_tok = ll1_tok.value();
      ll1_tok.reset();
    } else {
      // std::cout << "ll1 has no value" << std::endl;
      curr_tok = lex.getNxtToken();
    }
    node->statements.push_back(parse_statement());
  } while (curr_tok.type != lexer::tok_end);
  return node;
}

ASTStatement *Parser::parse_statement() {
  // std::cout << curr_tok << std::endl;
  switch (curr_tok.type) {
  case lexer::tok_let:
    // std::cout << "Parsing let" << std::endl;
    return parse_var_decl();
    break;
  case lexer::tok_end:
    // std::cout << "Reached End" << std::endl;
    break;
  case lexer::tok_print:
    // std::cout << "Parsing print" << std::endl;
    return parse_print();
    break;
  case lexer::tok_return:
    // std::cout << "Parsing Return" << std::endl;
    return parse_return();
    break;
  case lexer::tok_if:
    // std::cout << "Parsing If" << std::endl;
    return parse_if();
    break;
  case lexer::tok_for:
    // std::cout << "Parsing For" << std::endl;
    return parse_for();
    break;
  case lexer::tok_while:
    // std::cout << "Parsing While" << std::endl;
    return parse_while();
    break;
  case lexer::tok_iden: {
    // std::cout << "Parsing Assignment" << std::endl;
    auto x = parse_assignment();

    if (curr_tok.type != lexer::tok_semicolon) {
      fail(";");
    }
    return x;
    break;
  }
  case lexer::tok_type_int:
  case lexer::tok_type_float:
  case lexer::tok_type_bool:
  case lexer::tok_type_string:
    // std::cout << "Parsing Function Decleration" << std::endl;
    return parse_function_decl();
    break;
  case lexer::tok_curly_left:
    // std::cout << "Parsing Block" << std::endl;
    return parse_block();
    break;
  default:
    // std::cout << curr_tok << std::endl;
    std::cout << "Not Implemented Yet" << std::endl;
  }
  return nullptr;
}

ASTExpression *Parser::parse_expression() {
  ASTExpression *x = parse_simple_expression();
  if (curr_tok.type == lexer::tok_relational) {
    ASTBinOp *node = new ASTBinOp();
    node->left = x;
    node->value = curr_tok.value;
    node->op = tok_to_op[curr_tok.value];
    node->right = parse_expression();
    return node;
  } else {
    return x;
  }
}

ASTExpression *Parser::parse_simple_expression() {
  ASTExpression *x = parse_term();
  if (curr_tok.type == lexer::tok_add_op) {
    ASTBinOp *node = new ASTBinOp();
    node->left = x;
    node->value = curr_tok.value;
    node->op = tok_to_op[curr_tok.value];
    node->right = parse_simple_expression();
    return node;
  } else {
    return x;
  }
}

ASTExpression *Parser::parse_term() {
  ASTExpression *x = parse_factor();
  if (curr_tok.type == lexer::tok_multi_op) {
    ASTBinOp *node = new ASTBinOp();
    node->left = x;
    node->value = curr_tok.value;
    node->op = tok_to_op[curr_tok.value];
    node->right = parse_term();
    return node;
  } else {
    return x;
  }
}

std::vector<ASTExpression *> Parser::parse_actual_params() {
  std::vector<ASTExpression *> x;
  do {
    x.push_back(parse_expression());
  } while (curr_tok.type == lexer::tok_comma);
  if (curr_tok.type != lexer::tok_round_right) {
    std::stringstream ss;
    ss << "Missing ')' after function call on line: " << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }
  return x;
}

ASTExpression *Parser::parse_factor() {

  // std::cout << "identifier:  " << curr_tok.value << std::endl;
  curr_tok = lex.getNxtToken();
  switch (curr_tok.type) {
  case lexer::tok_lit_bool: {
  case lexer::tok_lit_float:
  case lexer::tok_lit_int:
  case lexer::tok_lit_string:
    ASTLiteral *node = new ASTLiteral(curr_tok);
    // std::cout << "matched literal" << std::endl;
    curr_tok = lex.getNxtToken();
    return node;
  }

  case lexer::tok_iden: {

    ll1_tok = lex.getNxtToken();
    if (ll1_tok->type == lexer::tok_round_left) {
      // parse function call
      // std::cout << "function call!" << std::endl;
      ASTFunctionCall *node = new ASTFunctionCall();
      node->name = curr_tok.value;
      node->args = parse_actual_params();
      curr_tok = lex.getNxtToken();
      ll1_tok.reset();
      return node;
    } else {
      ASTIdentifier *node = new ASTIdentifier();
      node->name = curr_tok.value;
      curr_tok = ll1_tok.value();
      ll1_tok.reset();
      return node;
    }
    break;
  }
  case lexer::tok_round_left: {
    // std::cout << "matched subexpression" << std::endl;
    ASTExpression *node = parse_expression();
    if (curr_tok.type != lexer::tok_round_right) {
      std::stringstream ss;
      ss << "Subexpression not terminated on line: " << curr_tok.line_number
         << " after assignment";
      throw std::runtime_error(ss.str());
    }
    curr_tok = lex.getNxtToken();
    return node;
    break;
  }
  case lexer::tok_unary: {
    // std::cout << "matched unary" << std::endl;
    ASTUnary *node = new ASTUnary();
    node->op = tok_to_op[curr_tok.value];
    node->expr = parse_expression();
    return node;
    break;
  }
  default:
    // std::cout << curr_tok << std::endl;
    std::stringstream ss;
    ss << "Unable to match production on line: " << curr_tok.line_number
       << " after assignment";
    throw std::runtime_error(ss.str());
  }
  return nullptr;
}

ASTVariableDecl *Parser::parse_var_decl() {

  ASTVariableDecl *node = new ASTVariableDecl();
  // let

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_iden) {
    fail("Identifier");
  } else {
    node->identifier = curr_tok.value;
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
    node->Type = tea_int;
    break;
  case lexer::tok_type_string:
    node->Type = tea_string;
    break;
  case lexer::tok_type_bool:
    node->Type = tea_bool;
    break;
  case lexer::tok_type_float:
    node->Type = tea_float;
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

  node->value = parse_expression();

  // lex x: int = 5
  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  }
  return node;
}

ASTPrintStatement *Parser::parse_print() {
  ASTPrintStatement *node = new ASTPrintStatement();
  try {
    node->value = parse_expression();
  } catch (...) {
    std::stringstream ss;
    ss << "Unable to parse expression after print statement on line: "
       << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  }
  return node;
  // std::cout << "Print success" << std::endl;
}

ASTReturn *Parser::parse_return() {
  ASTReturn *node = new ASTReturn;
  try {
    node->value = parse_expression();
  } catch (...) {
    std::stringstream ss;
    ss << "Unable to parse expression after return statement on line: "
       << curr_tok.line_number;
    throw std::runtime_error(ss.str());
  }

  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  }
  // std::cout << "Return Success" << std::endl;
  return node;
}

ASTAssignment *Parser::parse_assignment() {
  ASTAssignment *node = new ASTAssignment;
  node->identifier = curr_tok.value;
  curr_tok = lex.getNxtToken();

  if (curr_tok.type != lexer::tok_assign) {
    fail("=");
  }

  try {
    node->value = parse_expression();
  } catch (const std::runtime_error &e) {
    std::stringstream ss;
    ss << "Unable to parse expression after assignment on line: "
       << curr_tok.line_number << std::endl;
    ss << e.what();
    throw std::runtime_error(ss.str());
  } catch (...) {
    throw std::runtime_error("Unknown Exception");
  }

  // std::cout << "Assignment Success" << std::endl;
  return node;
}

ASTBlock *Parser::parse_block() {
  ASTBlock *node = new ASTBlock();
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_right) {
    do {
      node->source.push_back(parse_statement());
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
  return node;
}

// Needs to start with identifier
std::vector<std::tuple<std::string, Tealang_t>> Parser::parse_formal_params() {
  std::vector<std::tuple<std::string, Tealang_t>> args;
  do {
    curr_tok = lex.getNxtToken();

    auto x = curr_tok.value;
    if (curr_tok.type != lexer::tok_iden) {
      fail("Identifier");
    }

    curr_tok = lex.getNxtToken();

    if (curr_tok.type != lexer::tok_colon) {
      fail(":");
    }
    curr_tok = lex.getNxtToken();
    Tealang_t y;
    switch (curr_tok.type) {
    case lexer::tok_type_int:
      y = tea_int;
      break;
    case lexer::tok_type_string:
      y = tea_string;
      break;
    case lexer::tok_type_bool:
      y = tea_bool;
      break;
    case lexer::tok_type_float:
      y = tea_float;
      break;
    default:
      fail("Type Decleration");
    }
    args.push_back({x, y});
    curr_tok = lex.getNxtToken();
  } while (curr_tok.type == lexer::tok_comma);
  return args;
}

ASTFunctionDecl *Parser::parse_function_decl() {

  ASTFunctionDecl *node = new ASTFunctionDecl();

  switch (curr_tok.type) {
  case lexer::tok_type_bool:
    node->type = tea_bool;
    break;
  case lexer::tok_type_float:
    node->type = tea_float;
    break;
  case lexer::tok_type_int:
    node->type = tea_int;
    break;
  case lexer::tok_type_string:
    node->type = tea_string;
    break;
  default:
    fail("Type Decleration");
  }
  // int

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_iden) {
    fail("Identifier");
  } else {
    node->identifier = curr_tok.value;
  }

  // int fib

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  // int fib (
  node->arguments = parse_formal_params();

  // int fib (x:int

  if (curr_tok.type != lexer::tok_round_right) {
    fail(")");
  }

  // int fib (x:int)
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }
  node->body = parse_block();
  // std::cout << "AFTER FUNCTION BLOCK: " << curr_tok << std::endl;
  // int fib (x:int){}
  return node;
}

ASTWhileStatement *Parser::parse_while() {
  ASTWhileStatement *node = new ASTWhileStatement();
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  try {
    node->condition = parse_expression();
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
  node->eval = parse_block();
  return node;
}

ASTForStatement *Parser::parse_for() {
  ASTForStatement *node = new ASTForStatement();
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }

  curr_tok = lex.getNxtToken();
  if (curr_tok.type == lexer::tok_let) {
    try {
      node->init = parse_var_decl();
    } catch (const std::runtime_error &e) {
      std::stringstream ss;
      ss << "Unable to parse variable decleration in for loop on line: "
         << curr_tok.line_number << std::endl;
      ss << e.what();
      throw std::runtime_error(ss.str());
    } catch (...) {
      throw std::runtime_error("Unknown Exception");
    }
  }
  if (curr_tok.type != lexer::tok_semicolon) {
    fail(";");
  }

  try {
    node->condition = parse_expression();
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
  if (curr_tok.type == lexer::tok_iden) {
    try {
      node->assign = parse_assignment();
    } catch (const std::runtime_error &e) {
      std::stringstream ss;
      ss << "Unable to parse assignment in for loop on line: "
         << curr_tok.line_number << std::endl;
      ss << e.what();
      throw std::runtime_error(ss.str());
    } catch (...) {
      throw std::runtime_error("Unknown Exception");
    }
  }
  if (curr_tok.type != lexer::tok_round_right) {
    fail(")");
  }

  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_curly_left) {
    fail("{");
  }
  node->eval = parse_block();
  return node;
}

ASTIfStatement *Parser::parse_if() {
  ASTIfStatement *node = new ASTIfStatement();
  curr_tok = lex.getNxtToken();
  if (curr_tok.type != lexer::tok_round_left) {
    fail("(");
  }
  try {
    node->eval = parse_expression();
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

  node->eval_true = parse_block();
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
    node->eval_false = parse_block();
  }
  return node;
}
