#include "semantic_visitor.hh"
#include <algorithm>

std::string SemanticVisitor::Function::get_code() {
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

void SemanticVisitor::Scope::add_var(Variable var) {
  auto current_scope = variable_scope.back();
  variable_scope.pop_back();
  if (current_scope.find(var.name) == current_scope.end()) {
    current_scope.insert({var.name, var});
    variable_scope.push_back(current_scope);
  } else {
    throw std::invalid_argument("Cannot redeclare variable with name: " +
                                var.name + " in the current scope");
  }
};

void SemanticVisitor::Scope::add_func(Function func) {
  if (function_scope.find(func.get_code()) == function_scope.end()) {
    // None there can just insert
    std::cout << "Adding func: " << func.get_code() << std::endl;
    function_scope.insert({func.get_code(), func});
  } else {
    throw std::invalid_argument(
        "Cannot redeclare function with the same name and parameters");
  }
}

void SemanticVisitor::Scope::edit_func(Function func) {

  if (function_scope.find(func.get_code()) != function_scope.end()) {
    function_scope.erase(func.get_code());
    function_scope.insert({func.get_code(), func});
  } else {
    throw std::runtime_error("Function not found");
  }
}

SemanticVisitor::Variable SemanticVisitor::Scope::get_var(std::string str) {

  for (int i = variable_scope.size(); i > 0; i--) {
    try {
      return variable_scope[i - 1].at(str);
    } catch (...) {
    }
  }
  std::cout << variable_scope.size() << std::endl;
  throw std::invalid_argument("Variable " + str +
                              " does not exist in any scope");
}

SemanticVisitor::Function SemanticVisitor::Scope::get_func(std::string str) {
  try {
    return function_scope.at(str);
  } catch (...) {
    throw std::invalid_argument("Function " + str +
                                " does not exist in any scope");
  }

  return {};
};

void SemanticVisitor::visit(parser::ASTProgram *x) {

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
  Variable var = current_scope.get_var(x->name);
  token_type = var.type;
}

void SemanticVisitor::visit(parser::ASTFunctionCall *x) {
  std::string code = x->name;
  code += "$";
  for (int i = 0; i < x->args.size(); i++) {
    x->args[i]->accept(this);
    code += std::to_string(token_type);
  }
  current_scope.get_func(code);
}

void SemanticVisitor::visit(parser::ASTReturn *x) {
  if (function_type.has_value()) {
    x->value->accept(this);
    if (token_type != std::get<0>(*function_type) &&
        std::get<0>(*function_type) != parser::tea_auto) {
      throw std::invalid_argument(
          "Return type does not match function signature");
    } else {
      if (std::get<0>(*function_type) == parser::tea_auto &&
          token_type != parser::tea_auto) {
        std::get<0>(*function_type) = token_type;
      }
      std::get<1>(*function_type) = true;
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
  parser::Tealang_t left = token_type;
  x->right->accept(this);
  parser::Tealang_t right = token_type;

  if (left != right) {
    throw std::invalid_argument(
        "Binary Operator Nodes can only work on same types");
  } else {
    switch (token_type) {
    case parser::tea_bool: {
      switch (x->op) {
      case parser::op_eql:
      case parser::op_neql:
      case parser::op_and:
      case parser::op_or:
        break;
      default:
        throw std::invalid_argument("Boolean Values can only be compared "
                                    "with ['==','!=', 'and', 'or'] operators");
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
    case parser::tea_char: {
      switch (x->op) {
      case parser::op_eql:
      case parser::op_neql:
        break;
      default:
        throw std::invalid_argument("Char can only be operated on "
                                    "with ['==','!='] operators");
      }
      break;
    }
    default:
      // Case for arrays
      throw std::invalid_argument("Unsupported types used in binary operator");
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

  switch (token_type) {
  case parser::tea_bool:
    if (x->op == parser::op_minus) {
      throw std::invalid_argument("Cannot use minus operator on boolean");
    }
    break;
  case parser::tea_int:
  case parser::tea_float:
    if (x->op == parser::op_not) {
      throw std::invalid_argument(
          "Cannot use 'not' operator on numerical type");
    }
    break;
  case parser::tea_string:
    throw std::invalid_argument("Cannot use unary operators on strings");
  case parser::tea_char:
    throw std::invalid_argument("Cannot use unary operators on char");
  }
}

void SemanticVisitor::visit(parser::ASTVariableDecl *x) {

  Variable var;
  var.name = x->identifier;
  x->value->accept(this);
  if (x->Type == parser::tea_auto) {
    x->Type = token_type;
  } else {
    if (token_type != x->Type) {
      throw std::invalid_argument(
          "Variable Declaration and Assignment have incompatible types");
    }
  }
  var.type = token_type;
  current_scope.add_var(var);
}

void SemanticVisitor::visit(parser::ASTAssignment *x) {

  auto var = current_scope.get_var(x->identifier);
  x->value->accept(this);
  if (var.type != token_type) {
    throw std::invalid_argument("Variable assignment types dont match");
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
  if (x->init) {
    x->init->accept(this);
  }
  x->condition->accept(this);
  if (token_type != parser::tea_bool) {
    throw std::invalid_argument("Need bool for for");
  }
  if (x->assign) {
    x->assign->accept(this);
  }
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

  Function func;
  func.name = x->identifier;
  func.return_type = x->type;
  func.arguments = x->arguments;
  current_scope.add_func(func);
  function_type = {x->type, false};
  std::vector<parser::Tealang_t> y;

  std::map<std::string, Variable> new_scope;
  auto param_names = x->param_names();
  auto param_types = x->param_types();
  for (int i = 0; i < param_names.size(); i++) {
    Variable var;
    var.name = param_names[i];
    var.type = param_types[i];
    new_scope.insert({var.name, var});
  }
  current_scope.variable_scope.push_back(new_scope);

  eval_function_body(x->body, func.get_code());
  // x->body->accept(this);
  current_scope.variable_scope.pop_back();
  if (!std::get<1>(*function_type)) {
    throw std::invalid_argument("Function does not return");
  } else {
    if (func.return_type == parser::tea_auto) {
      if (std::get<0>(*function_type) == parser::tea_auto) {
        throw std::invalid_argument("Function type was never inferred");
      } else {
        func.return_type = std::get<0>(*function_type);
      }
    }
  }
  function_type.reset();
}

void SemanticVisitor::eval_function_body(parser::ASTBlock *x,
                                         std::string name) {

  bool is_auto = std::get<0>(*function_type) == parser::tea_auto;
  if (!is_auto) {
    x->accept(this);
    return;
  }
  for (int i = 0; i != x->source.size(); i++) {
    if (x->source[i] != nullptr) {
      try {
        x->source[i]->accept(this);
      } catch (...) {
      }
      if (is_auto) {
        if (std::get<0>(*function_type) != parser::tea_auto) {
          Function func = current_scope.get_func(name);
          func.return_type = std::get<0>(*function_type);
          current_scope.edit_func(func);
          is_auto = false;
        }
      }
    }
  }
  if (is_auto) {
    throw std::runtime_error(
        "Unable to determine function return type on initial pass");
  } else {
    // Running again with inferred type
    x->accept(this);
  }
}

void SemanticVisitor::visit(parser::ASTArrayAccess *x) {
  Variable arr = current_scope.get_var(x->name);
  switch (arr.type) {
  case parser::tea_arr_bool:
  case parser::tea_arr_float:
  case parser::tea_arr_int:
  case parser::tea_arr_string:
  case parser::tea_arr_char:
    break;
  default:
    throw std::invalid_argument(
        "Cannot use array access operators on non-array type");
  }
  x->index->accept(this);
  if (token_type != parser::tea_int) {
    throw std::invalid_argument("Cannot use a non integer index");
  }
  token_type = arr.type;
  switch (arr.type) {
  case parser::tea_arr_bool:
    token_type = parser::tea_bool;
    break;
  case parser::tea_arr_float:
    token_type = parser::tea_float;
    break;
  case parser::tea_arr_int:
    token_type = parser::tea_int;
    break;
  case parser::tea_arr_string:
    token_type = parser::tea_string;
    break;
  case parser::tea_arr_char:
    token_type = parser::tea_char;
    break;
  }
}

void SemanticVisitor::visit(parser::ASTArrayDecl *x) {
  Variable arr;
  arr.name = x->identifier;
  if (x->value) {
    x->value->accept(this);
    switch (token_type) {
    case parser::tea_arr_bool:
    case parser::tea_arr_int:
    case parser::tea_arr_float:
    case parser::tea_arr_string:
    case parser::tea_arr_char:
      break;
    default:
      throw std::invalid_argument(
          "Can only initialize an array with an array literal or no value");
    }
  }
  if (x->Type == parser::tea_auto) {
    x->Type = token_type;
    arr.type = token_type;
  } else {
    if (x->Type != token_type) {
      throw std::invalid_argument("Array type and Literal type do not match");
    }
  }
  current_scope.add_var(arr);
}

void SemanticVisitor::visit(parser::ASTArrayAssignment *x) {
  Variable arr = current_scope.get_var(x->identifier);
  x->index->accept(this);
  switch (arr.type) {
  case parser::tea_arr_bool:
  case parser::tea_arr_float:
  case parser::tea_arr_int:
  case parser::tea_arr_string:
  case parser::tea_arr_char:
    break;
  default:
    throw std::invalid_argument(
        "Cannot use array access operators on non-array type");
  }
  if (token_type != parser::tea_int) {
    throw std::invalid_argument("Cannot use a non integer index");
  }
  x->value->accept(this);
  if ((arr.type - parser::tea_arr_float) != token_type) {
    throw std::invalid_argument("Array assignment types dont match");
  }
}

void SemanticVisitor::visit(parser::ASTArrayLiteral *x) {

  parser::Tealang_t arr_lit_type;
  if (x->values.size()) {
    x->values[0]->accept(this);
    arr_lit_type = token_type;
  } else {
    throw std::invalid_argument("Cannot assign empty list");
  }
  for (int i = 0; i < x->values.size(); i++) {
    x->values[i]->accept(this);
    if (token_type != arr_lit_type) {
      throw std::invalid_argument("Cannot have mixed types inside array");
    }
  }
  switch (token_type) {
  case parser::tea_bool:
    token_type = parser::tea_arr_bool;
    break;
  case parser::tea_char:
    token_type = parser::tea_arr_char;
    break;
  case parser::tea_float:
    token_type = parser::tea_arr_float;
    break;
  case parser::tea_int:
    token_type = parser::tea_arr_int;
    break;
  case parser::tea_string:
    token_type = parser::tea_arr_string;
    break;
  }
}
