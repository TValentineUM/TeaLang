#include "interpreter.hh"

#include "semantic_visitor.hh"
#include <algorithm>
#include <sstream>
#include <string>

Function Scope::get_func(std::string str) { return function_scope.at(str); }

Variable Scope::get_var(std::string str) {
  for (auto i = variable_scope.size(); i > 0; i--) {
    auto current_scope = variable_scope[i - 1];
    try {
      auto y = current_scope.at(str);
      return y;
    } catch (...) {
    }
  }
}

void Scope::update_var(std::string str, Variable var) {

  for (auto i = variable_scope.size(); i > 0; i--) {
    auto current_scope = variable_scope[i - 1];
    try {
      auto temp = current_scope.at(str);

      current_scope.erase(str);
      current_scope.insert({var.name, var});
      variable_scope[i - 1] = current_scope;
      return;
    } catch (...) {
    }
  }
}

void Scope::add_var(Variable var) {
  auto current_scope = variable_scope.back();
  variable_scope.pop_back();
  current_scope.insert({var.name, var});
  variable_scope.push_back(current_scope);
};

// DONE
void Interpreter::visit(parser::ASTProgram *x) {

  // std::map<std::string, parser::Tealang_t> temp;
  for (int i = 0; i < x->statements.size(); i++) {
    if (x->statements[i] != nullptr) {

      x->statements[i]->accept(this);
    }
  }
}

// DONE
void Interpreter::visit(parser::ASTBlock *x) {
  for (int i = 0; i != x->source.size(); i++) {
    if (x->source[i] != nullptr) {
      x->source[i]->accept(this);
      if (return_value.has_value()) {
        token_type = *return_type;
        token_value = *return_value;
        break;
      }
    }
  }
}

// DONE
void Interpreter::visit(parser::ASTLiteral *x) {
  token_type = x->type;
  switch (token_type) {
  case parser::tea_bool:
    bool temp;
    std::istringstream(x->value) >> std::boolalpha >> temp;
    token_value = temp;
    break;
  case parser::tea_int:
    token_value = std::stoi(x->value);
    break;
  case parser::tea_float:
    token_value = std::stof(x->value);
    break;
  case parser::tea_string:
    token_value = x->value;
    break;
  }
}

// DONE
void Interpreter::visit(parser::ASTIdentifier *x) {
  auto var = scope.get_var(x->name);
  token_type = var.var_type;
  // std::cout << "before val" << std::endl;
  token_value = var.value;
  //  std::cout << "after val" << std::endl;
}

void Interpreter::visit(parser::ASTFunctionCall *x) {
  auto func = scope.get_func(x->name);
  if (scope.function_call) {
    //  std::cout << "Recursive Statment" << std::endl;

    //  std::cout << "1" << std::endl;
    std::map<std::string, Variable> func_scope;
    auto func_args = func.arguments;

    //  std::cout << "2 Re" << std::endl;
    // create scope
    for (int i = 0; i < x->args.size(); i++) {

      x->args[i]->accept(this);
      Variable temp;

      //     std::cout << "2.5 Re" << std::endl;
      temp.name = std::get<0>(func_args[i]);

      //  std::cout << temp.name << std::endl;
      temp.var_type = std::get<1>(func_args[i]);

      temp.value = token_value;

      func_scope.insert({temp.name, temp});
    }
    auto current_scope = scope.variable_scope.back();
    scope.variable_scope.pop_back();
    // Creating New Scope
    scope.variable_scope.push_back(func_scope);
    // std::cout << "3" << std::endl;
    func.function_body->accept(this);
    // Removing new scope
    scope.variable_scope.pop_back();
    token_type = return_type.value();
    token_value = return_value.value();
    scope.variable_scope.push_back(current_scope);
    //  std::cout << "end Recursive Statment" << std::endl;
  } else {
    //  std::cout << "Else Statment" << std::endl;
    scope.function_call = true;
    std::map<std::string, Variable> func_scope;
    auto func_args = func.arguments;
    // create scope
    for (int i = 0; i < x->args.size(); i++) {
      x->args[i]->accept(this);
      Variable temp;
      temp.name = std::get<0>(func_args[i]);
      temp.var_type = std::get<1>(func_args[i]);
      temp.value = token_value;
      func_scope.insert({temp.name, temp});
    }
    // Creating New Scope
    scope.variable_scope.push_back(func_scope);

    func.function_body->accept(this);
    // Removing new scope
    scope.variable_scope.pop_back();
    token_type = *return_type;
    token_value = *return_value;
    scope.function_call = false;
  }
  return_type.reset();
  return_value.reset();
}

void Interpreter::visit(parser::ASTReturn *x) {

  x->value->accept(this);
  return_type = token_type;
  return_value = token_value;
}

// DONE
void Interpreter::visit(parser::ASTPrintStatement *x) {
  x->value->accept(this);
  switch (token_type) {
  case parser::tea_bool:
    std::cout << std::boolalpha << std::any_cast<bool>(token_value)
              << std::endl;
    break;
  case parser::tea_int:
    std::cout << std::any_cast<int>(token_value) << std::endl;
    break;
  case parser::tea_float:
    std::cout << std::any_cast<float>(token_value) << std::endl;
    break;
  case parser::tea_string:
    std::cout << std::any_cast<std::string>(token_value) << std::endl;
    break;
  }
}

// DONE
void Interpreter::visit(parser::ASTSubExpression *x) { x->expr->accept(this); }

// DONE
void Interpreter::visit(parser::ASTBinOp *x) {

  x->left->accept(this);
  parser::Tealang_t left_type = token_type;
  auto left_val = token_value;
  x->right->accept(this);
  parser::Tealang_t right_type = token_type;
  auto right_val = token_value;

  // Could have made this more compact but this makes it alot easier to debug

  // Performing Operations on types
  switch (left_type) {
  case parser::tea_bool: {
    switch (x->op) {
    case parser::op_eql:
      token_value =
          std::any_cast<bool>(left_val) == std::any_cast<bool>(right_val);
      break;
    case parser::op_neql:
      token_value =
          std::any_cast<bool>(left_val) != std::any_cast<bool>(right_val);
      break;
    case parser::op_and:
      token_value =
          std::any_cast<bool>(left_val) && std::any_cast<bool>(right_val);
      break;
    case parser::op_or:
      token_value =
          std::any_cast<bool>(left_val) || std::any_cast<bool>(right_val);
      break;
    default:
      throw std::invalid_argument(
          "Boolean Values can only be compared with '==' or '!=' operators");
    }
    break;
  }
  case parser::tea_float: {
    switch (x->op) {
    case parser::op_times:
      token_value =
          std::any_cast<float>(left_val) * std::any_cast<float>(right_val);
      break;
    case parser::op_div:

      token_value =
          std::any_cast<float>(left_val) / std::any_cast<float>(right_val);
      break;
    case parser::op_plus:
      token_value =
          std::any_cast<float>(left_val) + std::any_cast<float>(right_val);
      break;
    case parser::op_minus:
      token_value =
          std::any_cast<float>(left_val) - std::any_cast<float>(right_val);
      break;
    case parser::op_less:
      token_value =
          std::any_cast<float>(left_val) < std::any_cast<float>(right_val);
      break;
    case parser::op_grtr:
      token_value =
          std::any_cast<float>(left_val) > std::any_cast<float>(right_val);
      break;
    case parser::op_eql:
      token_value =
          std::any_cast<float>(left_val) == std::any_cast<float>(right_val);
      break;
    case parser::op_neql:
      token_value =
          std::any_cast<float>(left_val) != std::any_cast<float>(right_val);
      break;
    case parser::op_le:
      token_value =
          std::any_cast<float>(left_val) <= std::any_cast<float>(right_val);
      break;
    case parser::op_ge:
      token_value =
          std::any_cast<float>(left_val) >= std::any_cast<float>(right_val);
      break;
    default:
      throw std::invalid_argument("Unexpected Type");
    }
    break;
  }
  case parser::tea_int: {
    switch (x->op) {
    case parser::op_times:
      token_value =
          std::any_cast<int>(left_val) * std::any_cast<int>(right_val);
      break;
    case parser::op_div:
      token_value =
          std::any_cast<int>(left_val) / std::any_cast<int>(right_val);
      break;
    case parser::op_plus:
      token_value =
          std::any_cast<int>(left_val) + std::any_cast<int>(right_val);
      break;
    case parser::op_minus:
      token_value =
          std::any_cast<int>(left_val) - std::any_cast<int>(right_val);
      break;
    case parser::op_less:
      token_value =
          std::any_cast<int>(left_val) < std::any_cast<int>(right_val);
      break;
    case parser::op_grtr:
      token_value =
          std::any_cast<int>(left_val) > std::any_cast<int>(right_val);
      break;
    case parser::op_eql:
      token_value =
          std::any_cast<int>(left_val) == std::any_cast<int>(right_val);
      break;
    case parser::op_neql:
      token_value =
          std::any_cast<int>(left_val) != std::any_cast<int>(right_val);
      break;
    case parser::op_le:
      token_value =
          std::any_cast<int>(left_val) <= std::any_cast<int>(right_val);
      break;
    case parser::op_ge:
      token_value =
          std::any_cast<int>(left_val) >= std::any_cast<int>(right_val);
      break;
    default:
      throw std::invalid_argument("Unexpected Type");
    }
    break;
  }
  case parser::tea_string: {
    switch (x->op) {
    case parser::op_plus:
      token_value = std::any_cast<std::string>(left_val) +
                    std::any_cast<std::string>(right_val);
      break;
    case parser::op_eql:
      token_value = std::any_cast<std::string>(left_val) ==
                    std::any_cast<std::string>(right_val);
      break;

    case parser::op_neql:
      token_value = std::any_cast<std::string>(left_val) !=
                    std::any_cast<std::string>(right_val);
      break;

      break;
    default:
      throw std::invalid_argument("Invalid operator on string");
    }
    break;
  }
  }
  // Updating Types for certain operators
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

// DONE
void Interpreter::visit(parser::ASTUnary *x) {
  x->expr->accept(this);

  switch (token_type) {
  case parser::tea_bool:
    token_value = !std::any_cast<bool>(token_value);
    break;
  case parser::tea_int:
    token_value = -std::any_cast<int>(token_value);
    break;
  case parser::tea_float:
    token_value = -std::any_cast<float>(token_value);
    break;
  case parser::tea_string:
    throw std::invalid_argument("Cannot use unary operators on strings");
  }
}

// DONE
void Interpreter::visit(parser::ASTVariableDecl *x) {
  Variable var;
  x->value->accept(this);
  var.var_type = token_type;
  var.value = token_value;
  var.name = x->identifier;
  scope.add_var(var);
}

// Done
void Interpreter::visit(parser::ASTAssignment *x) {

  auto var = scope.get_var(x->identifier);
  x->value->accept(this);
  switch (token_type) {
  case parser::tea_bool:
    var.value = std::any_cast<bool>(token_value);
    break;
  case parser::tea_int:
    var.value = std::any_cast<int>(token_value);
    break;
  case parser::tea_float:
    var.value = std::any_cast<float>(token_value);
    break;
  case parser::tea_string:
    var.value = std::any_cast<std::string>(token_value);
    break;
  }

  scope.update_var(x->identifier, var);
}

// DONE
void Interpreter::visit(parser::ASTIfStatement *x) {
  x->eval->accept(this);
  if (std::any_cast<bool>(token_value)) {
    x->eval_true->accept(this);
  } else {
    // If there is an else statement then use it
    if (x->eval_false) {
      x->eval_false->accept(this);
    }
  }
}

// DONE
void Interpreter::visit(parser::ASTForStatement *x) {

  // Create new scope for loop
  std::map<std::string, Variable> new_scope;
  scope.variable_scope.push_back(new_scope);
  x->init->accept(this);

  x->condition->accept(this);
  while (std::any_cast<bool>(token_value)) {
    x->eval->accept(this);
    if (return_value.has_value()) {
      break;
    }
    x->assign->accept(this);
    x->condition->accept(this);
  }
  // Delete scope

  scope.variable_scope.pop_back();
}

// DONE
void Interpreter::visit(parser::ASTWhileStatement *x) {
  std::map<std::string, Variable> new_scope;
  scope.variable_scope.push_back(new_scope);

  x->condition->accept(this);
  while (std::any_cast<bool>(token_value)) {
    x->eval->accept(this);
    if (return_value.has_value()) {
      break;
    }
    x->condition->accept(this);
  }
  scope.variable_scope.pop_back();
}

// Done
void Interpreter::visit(parser::ASTFunctionDecl *x) {
  Function func;
  func.name = x->identifier;
  func.return_type = x->type;
  func.arguments = x->arguments;
  func.function_body = x->body;
  scope.function_scope.insert({func.name, func});
}
