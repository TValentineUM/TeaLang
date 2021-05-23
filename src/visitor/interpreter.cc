#include "interpreter.hh"

//#include "semantic_visitor.hh"
#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>

std::string
Interpreter::Scope::code_generator(std::string name,
                                   std::vector<parser::Tealang_t> args) {
  std::string code = name + "$";
  for (auto arg : args) {
    code += std::to_string(arg);
  }
  return code;
}

std::string Interpreter::Function::get_code() {
  if (code == "") {
    code = name + "$";
    for (auto arg : arguments) {
      code += std::to_string(std::get<1>(arg));
    }
    return code;
  } else {
    return code;
  }
}

Interpreter::Function Interpreter::Scope::get_func(std::string str) {
  return function_scope.at(str);
}

Interpreter::Variable Interpreter::Scope::get_var(std::string str) {
  for (auto i = variable_scope.size(); i > 0; i--) {
    auto current_scope = variable_scope[i - 1];
    try {
      auto y = current_scope.at(str);
      return y;
    } catch (...) {
    }
  }
  throw std::runtime_error("Unable to locate variable!");
}

void Interpreter::Scope::add_var(Variable var) {
  auto current_scope = variable_scope.back();
  variable_scope.pop_back();
  current_scope.insert({var.name, var});
  variable_scope.push_back(current_scope);
};

void Interpreter::Scope::add_func(Function func) {
  function_scope.insert({func.get_code(), func});
}

void Interpreter::Scope::update_var(std::string str, Variable var) {
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

// DONE
void Interpreter::visit(parser::ASTProgram *x) {

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
  case parser::tea_char:
    token_value = x->value.at(0);
    break;
  default:
    throw std::runtime_error("Unkown literal type");
  }
}

void Interpreter::visit(parser::ASTIdentifier *x) {
  auto var = scope.get_var(x->name);
  token_type = var.type;
  token_value = var.value;
}

void Interpreter::visit(parser::ASTFunctionCall *x) {
  // auto func = scope.get_func(x->name);
  // std::map<std::string, Variable> func_scope;
  // auto func_args = func.arguments;
  std::vector<Variable> evaluated_args;
  std::vector<parser::Tealang_t> arg_types;
  for (int i = 0; i < x->args.size(); i++) {
    x->args[i]->accept(this);
    Variable temp;
    temp.type = token_type;
    temp.value = token_value;
    switch (token_type) {
    case parser::tea_arr_bool:
      temp.size = std::any_cast<std::vector<bool>>(token_value).size();
      break;
    case parser::tea_arr_int:
      temp.size = std::any_cast<std::vector<int>>(token_value).size();
      break;
    case parser::tea_arr_float:
      temp.size = std::any_cast<std::vector<float>>(token_value).size();
      break;
    case parser::tea_arr_char:
      temp.size = std::any_cast<std::vector<char>>(token_value).size();
      break;
    case parser::tea_arr_string:
      temp.size = std::any_cast<std::vector<std::string>>(token_value).size();
      break;
    }
    arg_types.push_back(token_type);
    evaluated_args.push_back(temp);
  }

  std::string code = scope.code_generator(x->name, arg_types);
  Function func = scope.get_func(code);
  std::map<std::string, Variable> func_scope;
  auto func_args = func.arguments;
  for (int i = 0; i < func_args.size(); i++) {
    evaluated_args[i].name = std::get<0>(func_args[i]);
    func_scope.insert({evaluated_args[i].name, evaluated_args[i]});
  }

  if (scope.function_call) {
    // Removing current top scope to emulate stack frames in recursive calls
    //
    auto current_scope = scope.variable_scope.back();
    scope.variable_scope.pop_back();

    // Adding Function call vars to a new scope and appending it to the current
    // scopes
    scope.variable_scope.push_back(func_scope);
    func.function_body->accept(this);
    scope.variable_scope.pop_back();
    token_type = return_type.value();
    token_value = return_value.value();
    scope.variable_scope.push_back(current_scope);
  } else {

    scope.function_call = true; // Denoting Initial Call
    scope.variable_scope.push_back(func_scope);
    func.function_body->accept(this);
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
  case parser::tea_char:
    std::cout << std::any_cast<char>(token_value) << std::endl;
    break;
  case parser::tea_arr_bool: {
    std::vector<bool> temp = std::any_cast<std::vector<bool>>(token_value);
    std::cout << "{";
    for (auto item : temp) {
      std::cout << std::boolalpha << item << ",";
    }
    std::cout << "}" << std::endl;
    break;
  }
  case parser::tea_arr_int: {
    std::vector<int> temp = std::any_cast<std::vector<int>>(token_value);
    std::cout << "{";
    for (auto item : temp) {
      std::cout << item << ",";
    }
    std::cout << "}" << std::endl;
    break;
  }
  case parser::tea_arr_float: {
    std::vector<float> temp = std::any_cast<std::vector<float>>(token_value);
    std::cout << "{";
    for (auto item : temp) {
      std::cout << item << ",";
    }
    std::cout << "}" << std::endl;
    break;
  }
  case parser::tea_arr_string: {
    std::vector<std::string> temp =
        std::any_cast<std::vector<std::string>>(token_value);
    std::cout << "{";
    for (auto item : temp) {
      std::cout << item << ",";
    }
    std::cout << "}" << std::endl;
    break;
  }
  case parser::tea_arr_char: {
    std::vector<char> temp = std::any_cast<std::vector<char>>(token_value);
    std::cout << "{";
    for (auto item : temp) {
      std::cout << item << ",";
    }
    std::cout << "}" << std::endl;
    break;
  }
  }
}

// DONE
void Interpreter::visit(parser::ASTSubExpression *x) { x->expr->accept(this); }

void Interpreter::eval_bin_op(int left_val, int right_val,
                              parser::Operators op) {
  switch (op) {
  case parser::op_times:
    token_value = (left_val) * (right_val);
    break;
  case parser::op_div:
    token_value = (left_val) / (right_val);
    break;
  case parser::op_minus:
    token_value = (left_val) - (right_val);
    break;
  case parser::op_less:
    token_value = (left_val) < (right_val);
    break;
  case parser::op_grtr:
    token_value = (left_val) > (right_val);
    break;
  case parser::op_le:
    token_value = (left_val) <= (right_val);
    break;
  case parser::op_ge:
    token_value = (left_val) >= (right_val);
    break;
  case parser::op_plus:
    token_value = (left_val) + (right_val);
    break;
  case parser::op_eql:
    token_value = (left_val) == (right_val);
    break;
  case parser::op_neql:
    token_value = (left_val) != (right_val);
    break;
  }
}

void Interpreter::eval_bin_op(float left_val, float right_val,
                              parser::Operators op) {
  switch (op) {
  case parser::op_times:
    token_value = (left_val) * (right_val);
    break;
  case parser::op_div:
    token_value = (left_val) / (right_val);
    break;
  case parser::op_minus:
    token_value = (left_val) - (right_val);
    break;
  case parser::op_less:
    token_value = (left_val) < (right_val);
    break;
  case parser::op_grtr:
    token_value = (left_val) > (right_val);
    break;
  case parser::op_le:
    token_value = (left_val) <= (right_val);
    break;
  case parser::op_ge:
    token_value = (left_val) >= (right_val);
    break;
  case parser::op_plus:
    token_value = (left_val) + (right_val);
    break;
  case parser::op_eql:
    token_value = (left_val) == (right_val);
    break;
  case parser::op_neql:
    token_value = (left_val) != (right_val);
    break;
  }
}

void Interpreter::eval_bin_op(std::string left_val, std::string right_val,
                              parser::Operators op) {
  switch (op) {
  case parser::op_eql:
    token_value = (left_val) == (right_val);
    break;
  case parser::op_neql:
    token_value = (left_val) != (right_val);
    break;
  case parser::op_plus:
    token_value = (left_val) + (right_val);
    break;
  }
}

void Interpreter::eval_bin_op(bool left_val, bool right_val,
                              parser::Operators op) {
  switch (op) {
  case parser::op_eql:
    token_value = (left_val) == (right_val);
    break;
  case parser::op_neql:
    token_value = (left_val) != (right_val);
    break;
  case parser::op_plus:
    token_value = (left_val) + (right_val);
    break;
  case parser::op_and:
    token_value = (left_val) && (right_val);
    break;
  case parser::op_or:
    token_value = (left_val) || (right_val);
  }
}

void Interpreter::eval_bin_op(char left_val, char right_val,
                              parser::Operators op) {
  switch (op) {
  case parser::op_eql:
    token_value = (left_val) == (right_val);
    break;
  case parser::op_neql:
    token_value = (left_val) != (right_val);
    break;
  }
}

// DONE
void Interpreter::visit(parser::ASTBinOp *x) {

  x->left->accept(this);
  parser::Tealang_t left_type = token_type;
  auto left_val = token_value;
  x->right->accept(this);
  parser::Tealang_t right_type = token_type;
  auto right_val = token_value;

  // Could have made this more compact but this makes it alot easier to debug

  switch (left_type) {
  case parser::tea_bool:
    eval_bin_op(std::any_cast<bool>(left_val), std::any_cast<bool>(right_val),
                x->op);
    break;
  case parser::tea_float:
    eval_bin_op(std::any_cast<float>(left_val), std::any_cast<float>(right_val),
                x->op);
    break;
  case parser::tea_int:
    eval_bin_op(std::any_cast<int>(left_val), std::any_cast<int>(right_val),
                x->op);
    break;
  case parser::tea_char:
    eval_bin_op(std::any_cast<char>(left_val), std::any_cast<char>(right_val),
                x->op);
    break;
  case parser::tea_string:
    eval_bin_op(std::any_cast<std::string>(left_val),
                std::any_cast<std::string>(right_val), x->op);
    break;
  default:
    throw std::runtime_error("Unexpected type in binary expression");
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
  default:
    throw std::invalid_argument("Cannot use unary operators on arrays");
  }
}

// DONE
void Interpreter::visit(parser::ASTVariableDecl *x) {
  Variable var;
  x->value->accept(this);
  var.type = token_type;
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
  default:
    throw std::invalid_argument("Cannot assign an array type");
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
  if (x->init) {
    x->init->accept(this);
  }

  x->condition->accept(this);
  while (std::any_cast<bool>(token_value)) {
    x->eval->accept(this);
    if (return_value.has_value()) {
      break;
    }
    if (x->assign) {
      x->assign->accept(this);
    }
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
  scope.function_scope.insert({func.get_code(), func});
}

// DONE
void Interpreter::visit(parser::ASTArrayAccess *x) {
  Variable arr = scope.get_var(x->name);
  x->index->accept(this);
  auto index = std::any_cast<int>(token_value);
  if (index > *arr.size || index < 0) {
    throw std::runtime_error("Invalid index");
  }
  switch (arr.type) {
  case parser::tea_arr_bool:
    token_value = std::any_cast<std::vector<bool>>(arr.value)[index];
    token_type = parser::tea_bool;
    break;
  case parser::tea_arr_float:
    token_value = std::any_cast<std::vector<float>>(arr.value)[index];
    token_type = parser::tea_float;
    break;
  case parser::tea_arr_int:
    token_value = std::any_cast<std::vector<int>>(arr.value)[index];
    token_type = parser::tea_int;
    break;
  case parser::tea_arr_string:
    token_value = std::any_cast<std::vector<std::string>>(arr.value)[index];
    token_type = parser::tea_string;
    break;
  case parser::tea_arr_char:
    token_value = std::any_cast<std::vector<char>>(arr.value)[index];
    token_type = parser::tea_char;
    break;
  }
}

// DONE
void Interpreter::visit(parser::ASTArrayAssignment *x) {
  Variable arr = scope.get_var(x->identifier);
  x->index->accept(this);
  auto index = std::any_cast<int>(token_value);
  if (index > arr.size || index < 0) {
    throw std::runtime_error("Invalid index");
  }
  x->value->accept(this);
  switch (token_type) {
  case parser::tea_bool:
    std::any_cast<std::vector<bool>>(arr.value)[index] =
        std::any_cast<bool>(token_value);
    break;
  case parser::tea_float:
    std::any_cast<std::vector<float>>(arr.value)[index] =
        std::any_cast<float>(token_value);
    break;
  case parser::tea_int: {
    auto temp_arr = std::any_cast<std::vector<int>>(arr.value);
    temp_arr[index] = std::any_cast<int>(token_value);
    arr.value = temp_arr;
    break;
  }
  case parser::tea_string:
    std::any_cast<std::vector<std::string>>(arr.value)[index] =
        std::any_cast<std::string>(token_value);
    break;
  case parser::tea_arr_char:
    std::any_cast<std::vector<char>>(arr.value)[index] =
        std::any_cast<char>(token_value);
    break;
  }
  scope.update_var(arr.name, arr);
}

void Interpreter::visit(parser::ASTArrayLiteral *x) {
  switch (token_type) {
  case parser::tea_bool:

    eval_arr_lit<bool>(x->values);
    token_type = parser::tea_arr_bool;
    break;
  case parser::tea_float:
    eval_arr_lit<float>(x->values);
    token_type = parser::tea_arr_float;
    break;
  case parser::tea_int:
    eval_arr_lit<int>(x->values);
    token_type = parser::tea_arr_int;
    break;
  case parser::tea_string:
    eval_arr_lit<std::string>(x->values);
    token_type = parser::tea_arr_string;
    break;
  case parser::tea_char:
    eval_arr_lit<char>(x->values);
    token_type = parser::tea_arr_char;
    break;
  }
}

template <typename T>
void Interpreter::eval_arr_lit(std::vector<parser::ASTExpression *> x) {
  std::vector<T> arr;
  for (int i = 0; i < x.size(); i++) {
    x[i]->accept(this);
    arr.push_back(std::any_cast<T>(token_value));
  }
  token_value = arr;
}

void Interpreter::visit(parser::ASTArrayDecl *x) {
  Variable arr;
  arr.name = x->identifier;
  arr.type = x->Type;
  x->size->accept(this);
  auto size = std::any_cast<int>(token_value);
  if (size <= 0) {
    throw std::runtime_error("Invalid index");
  }
  arr.size = size;
  switch (arr.type) {
  case parser::tea_arr_bool:
    arr.value = std::vector<bool>(*arr.size);
    break;
  case parser::tea_arr_int:
    arr.value = std::vector<int>(*arr.size);
    break;
  case parser::tea_arr_float:
    arr.value = std::vector<float>(*arr.size);
    break;
  case parser::tea_arr_string:
    arr.value = std::vector<std::string>(*arr.size);
    break;
  case parser::tea_arr_char:
    arr.value = std::vector<char>(*arr.size);
    break;
  default:
    throw std::invalid_argument(
        "Can only initialize an array with an array literal or no value");
  }

  if (x->value) {
    x->value->accept(this);
    switch (token_type) {
    case parser::tea_arr_bool: {
      arr.value = std::any_cast<std::vector<bool>>(token_value);
    }
    case parser::tea_arr_int: {
      arr.value = std::any_cast<std::vector<int>>(token_value);
      break;
    }
    case parser::tea_arr_float: {
      arr.value = std::any_cast<std::vector<float>>(token_value);
      break;
    }
    case parser::tea_arr_string: {
      arr.value = std::any_cast<std::vector<std::string>>(token_value);
      break;
    }
    case parser::tea_arr_char: {
      arr.value = std::any_cast<std::vector<char>>(token_value);
      break;
    }
    default:
      throw std::invalid_argument(
          "Can only initialize an array with an array literal or no value");
    }
  }
  scope.add_var(arr);
}
