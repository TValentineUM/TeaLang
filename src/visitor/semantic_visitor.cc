#include "semantic_visitor.hh"
#include <algorithm>

void SemanticVisitor::token_setter(parser::Tealang_t type) {
  token_type = type;
  switch (type) {
  case parser::tea_bool:
    token_name = "bool";
    break;
  case parser::tea_char:
    token_name = "char";
    break;
  case parser::tea_float:
    token_name = "float";
    break;
  case parser::tea_int:
    token_name = "int";
    break;
  case parser::tea_string:
    token_name = "string";
    break;
  case parser::tea_arr_bool:
    token_name = "arr_bool";
    break;
  case parser::tea_arr_float:
    token_name = "arr_float";
    break;
  case parser::tea_arr_int:
    token_name = "arr_int";
    break;
  case parser::tea_arr_string:
    token_name = "arr_string";
    break;
  case parser::tea_arr_char:
    token_name = "arr_char";
    break;
  }
}

std::string SemanticVisitor::Scope::code_generator(
    std::string name,
    std::vector<std::tuple<parser::Tealang_t, std::string>> args) {
  std::string code = name;
  for (auto arg : args) {
    code += "&" + std::get<1>(arg);
  }
  return code;
}

std::string SemanticVisitor::Function::get_code() {
  if (code == "") {
    code = name;
    for (auto arg : arguments) {
      code += "&" + std::get<2>(arg);
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
  auto current_scope = function_scope.back();
  function_scope.pop_back();
  if (current_scope.find(func.get_code()) == current_scope.end()) {
    // None there can just insert
    std::cout << "Adding func: " << func.get_code() << std::endl;
    current_scope.insert({func.get_code(), func});
    function_scope.push_back(current_scope);
  } else {
    throw std::invalid_argument(
        "Cannot redeclare function with the same name and parameters");
  }
}

void SemanticVisitor::Scope::add_struct(Struct temp) {
  if (struct_scope.find(temp.name) == struct_scope.end()) {
    // None there can just insert
    std::cout << "Adding struct: " << temp.name << std::endl;
    struct_scope.insert({temp.name, temp});
  } else {
    throw std::invalid_argument("Cannot redeclare struct with the same name");
  }
}

void SemanticVisitor::Scope::edit_func(Function func) {
  auto current_scope = function_scope.back();
  function_scope.pop_back();
  if (current_scope.find(func.get_code()) != current_scope.end()) {
    current_scope.erase(func.get_code());
    current_scope.insert({func.get_code(), func});
    function_scope.push_back(current_scope);
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
  for (int i = function_scope.size(); i > 0; i--) {
    try {
      return function_scope[i - 1].at(str);
    } catch (...) {
    }
  }
  std::cout << function_scope.size() << std::endl;
  throw std::invalid_argument("Function " + str +
                              " does not exist in any scope");
};

SemanticVisitor::Struct SemanticVisitor::Scope::get_struct(std::string str) {
  try {
    return struct_scope.at(str);
  } catch (...) {
    throw std::invalid_argument("Struct " + str + " does not exist");
  }

  return {};
}

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

void SemanticVisitor::visit(parser::ASTLiteral *x) { token_setter(x->type); }

void SemanticVisitor::visit(parser::ASTIdentifier *x) {
  Variable var = scope.get_var(x->name);
  token_setter(var.type);
  // std::cout << "Token Name: " << x->name
  //           << ", Token Type: " << var.type_name.value() << std::endl;
  if (var.type == parser::tea_struct) {
    token_name = var.type_name.value();
  }
}

void SemanticVisitor::visit(parser::ASTFunctionCall *x) {
  std::vector<std::tuple<parser::Tealang_t, std::string>> args;
  for (int i = 0; i < x->args.size(); i++) {
    x->args[i]->accept(this);
    args.push_back({token_type, token_name});
  }
  Function func = scope.get_func(scope.code_generator(x->name, args));
  token_type = func.return_type;
  token_name = func.type_name;
}

void SemanticVisitor::visit(parser::ASTReturn *x) {
  if (function_type.has_value()) {
    x->value->accept(this);
    if (token_type != std::get<0>(*function_type) &&
        std::get<0>(*function_type) != parser::tea_auto) {
      throw std::invalid_argument(
          "Return type does not match function signature");
    } else if (token_type == parser::tea_struct &&
               std::get<1>(*function_type) != token_name) {

      throw std::invalid_argument(
          "Struct type returned does not match!\n Expected: \"" +
          std::get<1>(*function_type) + "\", Found: \"" + token_name + "\"");

    } else {
      if (std::get<0>(*function_type) == parser::tea_auto &&
          token_type != parser::tea_auto) {
        std::get<0>(*function_type) = token_type;
        std::get<1>(*function_type) = token_name;
      }
      std::get<2>(*function_type) = true;
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
      token_setter(parser::tea_bool);
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
  default:
    throw std::invalid_argument("Unsupported type for unary operator");
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
  var.type_name = token_name;
  std::cout << "Variable :" << var.name << ", Type: " << var.type_name.value()
            << std::endl;
  scope.add_var(var);
}

void SemanticVisitor::visit(parser::ASTAssignment *x) {
  auto var = scope.get_var(x->identifier);
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
  func.type_name = x->type_name;
  func.arguments = x->arguments;
  scope.add_func(func);
  function_type = {x->type, x->type_name, false};
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
  scope.variable_scope.push_back(new_scope);

  eval_function_body(x->body, func.get_code());
  // x->body->accept(this);
  scope.variable_scope.pop_back();
  if (!std::get<2>(*function_type)) {
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

// Auto Determining the return type
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
          Function func = scope.get_func(name);
          func.return_type = std::get<0>(*function_type);
          scope.edit_func(func);
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
  Variable arr = scope.get_var(x->name);
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
  switch (arr.type) {
  case parser::tea_arr_bool:
    token_setter(parser::tea_bool);
    break;
  case parser::tea_arr_float:
    token_setter(parser::tea_float);
    break;
  case parser::tea_arr_int:
    token_setter(parser::tea_int);
    break;
  case parser::tea_arr_string:
    token_setter(parser::tea_string);
    break;
  case parser::tea_arr_char:
    token_setter(parser::tea_char);
    break;
  }
}

void SemanticVisitor::visit(parser::ASTArrayDecl *x) {
  Variable arr;
  arr.name = x->identifier;
  x->size->accept(this);
  if (token_type != parser::tea_int) {
    throw std::invalid_argument("Array must have an integer type");
  }

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
  scope.add_var(arr);
}

void SemanticVisitor::visit(parser::ASTArrayAssignment *x) {
  Variable arr = scope.get_var(x->identifier);
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
    token_setter(parser::tea_arr_bool);

    break;
  case parser::tea_char:

    token_setter(parser::tea_arr_char);
    break;
  case parser::tea_float:

    token_setter(parser::tea_arr_float);
    break;
  case parser::tea_int:

    token_setter(parser::tea_arr_int);
    break;
  case parser::tea_string:

    token_setter(parser::tea_arr_string);
    break;
  }
}

void SemanticVisitor::visit(parser::ASTStructAccess *x) {
  Variable var = scope.get_var(x->name);
  if (var.type != parser::tea_struct) {
    throw std::invalid_argument(
        "Invalid operator on variable, it is not a struct in this scope");
  } else {
    Variable temp = (*var.members).at(x->element);
    token_setter(temp.type);
  }
}

void SemanticVisitor::visit(parser::ASTStructFunc *x) {
  Variable var = scope.get_var(x->name);
  if (var.type != parser::tea_struct) {
    throw std::invalid_argument(
        "Invalid operator on variable, it is not a struct in this scope");
  } else {
    std::vector<std::tuple<parser::Tealang_t, std::string>> args;
    for (int i = 0; i < x->args.size(); i++) {
      x->args[i]->accept(this);
      args.push_back({token_type, token_name});
    }
    Struct base_type = scope.get_struct(*var.type_name);
    try {
      Function func =
          base_type.base_functions.at(scope.code_generator(x->element, args));
      token_setter(func.return_type);
    } catch (...) {
      throw std::invalid_argument("Unknown function \"" + x->element +
                                  "\" called for struct:\"" + x->name +
                                  "\" of type \"" + *var.type_name + "\"");
    }
  }
}

void SemanticVisitor::visit(parser::ASTStructDefn *x) {
  Struct new_struct;
  new_struct.name = x->name;
  scope.variable_scope.push_back(new_struct.base_variables);
  scope.function_scope.push_back(new_struct.base_functions);
  for (auto var : x->vars) {
    var->accept(this);
  }
  for (auto func : x->funcs) {
    func->accept(this);
  }
  new_struct.base_variables = scope.variable_scope.back();
  new_struct.base_functions = scope.function_scope.back();
  scope.variable_scope.pop_back();
  scope.function_scope.pop_back();
  scope.add_struct(new_struct);
}

void SemanticVisitor::visit(parser::ASTStructAssign *x) {
  Variable var = scope.get_var(x->name);
  if (var.type != parser::tea_struct) {
    throw std::invalid_argument(
        "Invalid operator on variable, it is not a struct in this scope");
  } else {
    try {
      Variable temp = var.members->at(x->element);
      token_type = temp.type;
    } catch (...) {
      throw std::invalid_argument("Unkown member call for struct: " +
                                  x->element);
    }
  }
}

void SemanticVisitor::visit(parser::ASTStructDecl *x) {
  Variable var;
  var.name = x->identifier;
  var.type_name = x->struct_name;
  var.type = x->Type;
  Struct temp = scope.get_struct(x->struct_name);
  var.members = temp.base_variables;
  scope.add_var(var);
}
