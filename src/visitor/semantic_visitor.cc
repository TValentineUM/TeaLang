#include "semantic_visitor.hh"
#include <algorithm>

std::optional<std::tuple<parser::Tealang_t, std::vector<parser::Tealang_t>>>
SemanticVisitor::res_func(std::string s) {

  try {
    auto y = function_scope.at(s);
    return y;
  } catch (...) {
    // Not Found
    return {};
  }
}

std::optional<parser::Tealang_t> SemanticVisitor::res_var_all(std::string s) {
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

std::optional<parser::Tealang_t> SemanticVisitor::res_var_local(std::string s) {

  auto current_scope = variable_scope.back();
  try {
    auto y = current_scope.at(s);
    return y;
  } catch (...) {
    // Not Found
    return {};
  }
}

void SemanticVisitor::visit(parser::ASTProgram *x) {

  std::map<std::string, parser::Tealang_t> temp;
  variable_scope.push_back(temp);
  for (int i = 0; i < x->statements.size(); i++) {
    if (x->statements[i] != nullptr) {
      x->statements[i]->accept(this);
      function_type.reset();
    }
  }
}

void SemanticVisitor::visit(parser::ASTBlock *x) {

  for (int i = 0; i != x->source.size(); i++) {
    if (x->source[i] != nullptr) {
      x->source[i]->accept(this);
    }
  }
}

void SemanticVisitor::visit(parser::ASTLiteral *x) { token_type = x->type; }

void SemanticVisitor::visit(parser::ASTIdentifier *x) {
  auto var = res_var_all(x->name);
  if (var.has_value()) {
    token_type = *var;
  } else {
    throw std::invalid_argument("Variable not initialized");
  }
}

void SemanticVisitor::visit(parser::ASTFunctionCall *x) {
  auto func = res_func(x->name);
  if (func.has_value()) {
    auto params = std::get<1>(func.value());
    auto args = x->args;
    if (params.size() != args.size()) {
      throw std::invalid_argument("Unable to determine function call");
    } else {
      for (int i = 0; i < params.size(); i++) {
        args[i]->accept(this);
        if (token_type != params[i]) {
          throw std::invalid_argument(
              "Function argument did not match parameter type");
        }
      }
    }
    token_type = std::get<0>(func.value());
  }
}

void SemanticVisitor::visit(parser::ASTReturn *x) {
  std::get<1>(*function_type) = true;
  if (function_type.has_value()) {
    x->value->accept(this);
    if (token_type != std::get<0>(*function_type)) {
      throw std::invalid_argument(
          "Return type does not match function signature");
    }
  } else {
    throw std::invalid_argument("Cannot Call Return Statement outside block");
  }
}

void SemanticVisitor::visit(parser::ASTPrintStatement *x) {
  x->value->accept(this);
}

void SemanticVisitor::visit(parser::ASTSubExpression *x) {

  x->expr->accept(this);
}

void SemanticVisitor::visit(parser::ASTBinOp *x) {

  x->left->accept(this);
  parser::Tealang_t left = token_type.value();
  x->right->accept(this);
  parser::Tealang_t right = token_type.value();
  if (left != right) {
    throw std::invalid_argument(
        "Binary Operator Nodes can only work on same types");
  } else {
    switch (token_type.value()) {
    case parser::tea_bool: {
      switch (x->op) {
      case parser::op_eql:
      case parser::op_neql:
      case parser::op_and:
      case parser::op_or:
        break;
      default:
        throw std::invalid_argument(
            "Boolean Values can only be compared with '==' or '!=' operators");
      }
      break;
    }
    case parser::tea_float:
    case parser::tea_int: {
      switch (x->op) {
      case parser::op_and:
      case parser::op_or:
        throw std::invalid_argument(
            "Numerical types cannot be operated upon by boolean operators");
      default:
        break;
      }
      break;
    }
    case parser::tea_string: {
      switch (x->op) {
      case parser::op_plus:
      case parser::op_eql:
      case parser::op_neql:
        break;
      default:
        throw std::invalid_argument("Invalid operator on string");
      }
      break;
    }
    }
    switch (x->op) {
    case parser::op_and:
    case parser::op_or:
    case parser::op_less:
    case parser::op_grtr:
    case parser::op_eql:
    case parser::op_neql:
    case parser::op_le:
    case parser::op_ge:
      token_type = parser::tea_bool;
      break;
    default:
      break;
    }
  }
}

void SemanticVisitor::visit(parser::ASTUnary *x) {
  x->expr->accept(this);

  switch (token_type.value()) {
  case parser::tea_bool:
    if (x->op == parser::op_minus) {
      throw std::invalid_argument("Cannot use minus operator on boolean");
    }
    break;
  case parser::tea_int:
  case parser::tea_float:
    if (x->op == parser::op_minus) {
      throw std::invalid_argument(
          "Cannot use 'not' operator on numerical type");
    }
    break;
  case parser::tea_string:
    throw std::invalid_argument("Cannot use unary operators on strings");
  }
}

void SemanticVisitor::visit(parser::ASTVariableDecl *x) {

  auto var = res_var_local(x->identifier);
  if (var.has_value()) {
    throw std::invalid_argument(
        "Cannot redeclare variable with the same name in the same scope");
  } else {
    x->value->accept(this);
    if (token_type != x->Type) {
      throw std::invalid_argument(
          "Variable Declaration and Assignment have incompatible types");
    } else {
      // Update the SCOPE
      auto current_scope = variable_scope.back();
      variable_scope.pop_back();
      current_scope.insert({x->identifier, x->Type});
      variable_scope.push_back(current_scope);
    }
  }
}

void SemanticVisitor::visit(parser::ASTAssignment *x) {

  auto var = res_var_all(x->identifier);
  if (var.has_value()) {
    x->value->accept(this);
    if (var.value() != token_type) {
      throw std::invalid_argument("Variable assignment types dont match");
    }
  } else {
    throw std::invalid_argument("Variable not initialised");
  }
}

void SemanticVisitor::visit(parser::ASTIfStatement *x) {
  x->eval->accept(this);
  if (token_type != parser::tea_bool) {
    throw std::invalid_argument("If statement needs bool");
  } else {
    x->eval_true->accept(this);
    if (x->eval_false) {
      x->eval_false->accept(this);
    }
  }
}

void SemanticVisitor::visit(parser::ASTForStatement *x) {
  x->init->accept(this);
  x->condition->accept(this);
  if (token_type != parser::tea_bool) {
    throw std::invalid_argument("Need bool for for");
  }
  x->assign->accept(this);
  x->eval->accept(this);
}

void SemanticVisitor::visit(parser::ASTWhileStatement *x) {
  x->condition->accept(this);
  if (token_type != parser::tea_bool) {
    throw std::invalid_argument("Need bool for while");
  }
  x->eval->accept(this);
}

void SemanticVisitor::visit(parser::ASTFunctionDecl *x) {

  auto func = res_func(x->identifier);
  if (!func.has_value()) {
    function_type = {x->type, false};
    std::vector<parser::Tealang_t> y;

    function_scope.insert({x->identifier, {x->type, x->param_types()}});
    std::map<std::string, parser::Tealang_t> new_scope;
    auto param_names = x->param_names();
    auto param_types = x->param_types();
    for (int i = 0; i < param_names.size(); i++) {
      new_scope.insert({param_names[i], param_types[i]});
    }
    variable_scope.push_back(new_scope);
    x->body->accept(this);
    variable_scope.pop_back();
    if (!std::get<1>(*function_type)) {
      throw std::invalid_argument("Function does not return");
    }
    function_type.reset();
  } else {
    throw std::invalid_argument("Function with this name already defined");
  }
}
