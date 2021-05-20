#include "AST.hh"
using namespace parser;

ASTLiteral::ASTLiteral(lexer::Token tok) {
  switch (tok.type) {
  case lexer::tok_lit_bool:
    type = tea_bool;
    type_name = "Boolean";
    break;
  case lexer::tok_lit_int:
    type = tea_int;
    type_name = "Integer";
    break;
  case lexer::tok_lit_float:
    type = tea_float;
    type_name = "Float";
    break;
  case lexer::tok_lit_string:
    type = tea_string;
    type_name = "String";
    break;
  case lexer::tok_lit_char:
    type = tea_char;
    type_name = "Char";
    break;
  }

  value = tok.value;
}

std::vector<Tealang_t> ASTFunctionDecl::param_types() {
  std::vector<Tealang_t> types;
  types.resize(arguments.size());
  std::transform(arguments.begin(), arguments.end(), types.begin(),
                 [](std::tuple<std::string, Tealang_t> const &tuple) {
                   return std::get<1>(tuple);
                 });
  return types;
};

std::vector<std::string> ASTFunctionDecl::param_names() {
  std::vector<std::string> names;
  names.resize(arguments.size());
  std::transform(arguments.begin(), arguments.end(), names.begin(),
                 [](std::tuple<std::string, Tealang_t> const &tuple) {
                   return std::get<0>(tuple);
                 });
  return names;
}
