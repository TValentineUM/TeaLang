#include "semantic_visitor.hh"
#include <algorithm>

void SemanticVisitor::Scope::add_var(Variable var) {
  auto current_scope = variable_scope.back();
  variable_scope.pop_back();
  if (current_scope.find(var.name) == current_scope.end()) {
    current_scope.insert({var.name, var});
    std::cout << "Adding " << var.name << " to scope " << variable_scope.size()
              << std::endl;
    variable_scope.push_back(current_scope);
  } else {
    throw std::invalid_argument("Cannot redeclare variable with name: " +
                                var.name + " in the current scope");
  }
};

void SemanticVisitor::Scope::add_arr(Array arr) {
  auto current_scope = array_scope.back();
  array_scope.pop_back();
  if (current_scope.find(arr.name) == current_scope.end()) {
    current_scope.insert({arr.name, arr});
    array_scope.push_back(current_scope);
  } else {
    throw std::invalid_argument("Cannot redeclare array with name: " +
                                arr.name + " in the current scope");
  }
};

void SemanticVisitor::Scope::add_func(Function func) {
  if (function_scope.find(func.name) == function_scope.end()) {
    function_scope.insert({func.name, func});
  } else {
    throw std::invalid_argument("Cannot redeclare function with name: " +
                                func.name + " in the current scope");
  }
};

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

SemanticVisitor::Array
SemanticVisitor::SemanticVisitor::Scope::get_arr(std::string str) {
  for (int i = array_scope.size(); i > 0; i--) {
    try {
      return array_scope[i - 1].at(str);
    } catch (...) {
    }
  }
  throw std::invalid_argument("Array " + str + " does not exist in any scope");
  return {};
}

SemanticVisitor::Function SemanticVisitor::Scope::get_func(std::string str) {
  try {
    return function_scope.at(str);
  } catch (...) {
  }

  throw std::invalid_argument("Function " + str +
                              " does not exist in any scope");
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
  token_type = var.var_type;
}

void SemanticVisitor::visit(parser::ASTFunctionCall *x) {
  Function func = current_scope.get_func(x->name);
  auto params = func.arguments;
  auto args = x->args;
  if (params.size() != args.size()) {
    throw std::invalid_argument("Unable to determine function call");
  } else {
    for (int i = 0; i < params.size(); i++) {
      args[i]->accept(this);
      if (token_type != std::get<1>(params[i])) {
        throw std::invalid_argument(
            "Function argument did not match parameter type");
      }
      token_type = func.return_type;
    }
  }
}

void SemanticVisitor::visit(parser::ASTReturn *x) {
  if (function_type.has_value()) {
    x->value->accept(this);
    if (token_type != std::get<0>(*function_type)) {
      throw std::invalid_argument(
          "Return type does not match function signature");
    } else {
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
  if (is_array == true) {
    throw std::invalid_argument("Cannot use a binary operator on an array");
  }
  x->right->accept(this);
  if (is_array == true) {
    throw std::invalid_argument("Cannot use a binary operator on an array");
  }
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
  if (token_type != x->Type) {
    throw std::invalid_argument(
        "Variable Declaration and Assignment have incompatible types");
  }
  var.var_type = token_type;
  current_scope.add_var(var);
}

void SemanticVisitor::visit(parser::ASTAssignment *x) {

  auto var = current_scope.get_var(x->identifier);
  x->value->accept(this);
  if (var.var_type != token_type) {
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
    var.var_type = param_types[i];
    new_scope.insert({var.name, var});
  }
  current_scope.variable_scope.push_back(new_scope);
  x->body->accept(this);
  current_scope.variable_scope.pop_back();
  if (!std::get<1>(*function_type)) {
    throw std::invalid_argument("Function does not return");
  }
  function_type.reset();
}

void SemanticVisitor::visit(parser::ASTArrayAccess *x) {
  Array arr = current_scope.get_arr(x->name);
  x->index->accept(this);
  if (token_type != parser::tea_int) {
    throw std::invalid_argument("Cannot use a non integer index");
  }
  token_type = arr.arr_type;
}

void SemanticVisitor::visit(parser::ASTArrayDecl *x) {
  Array arr;
  arr.name = x->identifier;
  arr.arr_type = x->Type;
  if (x->value) {
    x->value->accept(this);
    if (!is_array) {
      throw std::invalid_argument(
          "Can only initialize an array with an array literal or no value");
    }
  }
  if (x->Type != token_type) {
    throw std::invalid_argument("Array type and Literal type do not match");
  }
  current_scope.add_arr(arr);
  is_array = false;
}

void SemanticVisitor::visit(parser::ASTArrayAssignment *x) {
  Array arr = current_scope.get_arr(x->identifier);
  x->index->accept(this);
  if (token_type != parser::tea_int) {
    throw std::invalid_argument("Cannot use a non integer index");
  }
  x->value->accept(this);
  if (arr.arr_type != token_type) {
    throw std::invalid_argument("Array assignment types dont match");
  }
}

void SemanticVisitor::visit(parser::ASTArrayLiteral *x) {

  is_array = true;
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
}
