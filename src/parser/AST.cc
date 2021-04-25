#include "AST.hh"
using namespace parser;

ASTLiteral::ASTLiteral(lexer::Token tok) {
  // Determine Literal Type
  switch (tok.type) {
  case lexer::tok_type_bool:
    type = Bool;
  case lexer::tok_type_int:
    type = Int;
  case lexer::tok_type_float:
    type = Float;
  case lexer::tok_type_string:
    type = String;
  }
  // Extract Literal Value
  value = tok.value;
}
