#include "interpreter.hh"
using namespace visitor;

std::optional<std::tuple<parser::Tealang_t, std::vector<parser::Tealang_t>>>
Scope::res_func(std::string s) {

  try {
    auto y = function_scope.at(s);
    return y;
  } catch (...) {
    // Not Found
    return {};
  }
}

std::optional<parser::Tealang_t> Scope::res_var_all(std::string s) {
  for (auto i = variable_scope.end(); i != variable_scope.begin(); i--) {
    auto current_scope = variable_scope.back();
    try {
      auto y = current_scope.at(s);
      return y;
    } catch (...) {
    }
  }
  return {};
}

std::optional<parser::Tealang_t> Scope::res_var_local(std::string s) {

  auto current_scope = variable_scope.back();
  try {
    auto y = current_scope.at(s);
    return y;
  } catch (...) {
    // Not Found
    return {};
  }
}
