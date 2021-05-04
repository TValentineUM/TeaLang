#ifndef INTERPRETER_H_
#define INTERPRETER_H

#include "visitor.hh"
using namespace visitor;
#include "../parser/AST.hh"
#include <any>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <variant>

namespace visitor {

class Function {
public:
  std::string name;
  parser::Tealang_t return_type;
  std::vector<std::tuple<std::string, parser::Tealang_t>> arguments;
  parser::ASTBlock *function_body;
};

class Variable {
public:
  parser::Tealang_t var_type;
  std::string name;
  std::any value; /**< Using the enum for the get*/
};

// Scope is gonna be more of a problem now since we have stack frames with
// recursive calls
//
//

class Scope {
public:
  Scope() : function_call{false} {
    std::map<std::string, Variable> temp;
    variable_scope.push_back(temp);
  }

  Function get_func(std::string); /**< Finds Function*/

  Variable get_var(std::string); /**< Find variable starting from top scope */

  void update_var(std::string,
                  Variable); /**< Updates a variable starting from top scope*/

  void add_var(Variable);
  std::map<std::string, Function>
      function_scope; /**< Maps function names to a tuple containing the
                         function return type and the argument types */

  std::vector<std::map<std::string, Variable>>
      variable_scope; /**< The Vector stores all current variable scopes, where
                         each scope is a mapping from a string to its type*/

  bool function_call;
};

class Interpreter : public Visitor {

private:
  Scope scope;

  parser::Tealang_t token_type;
  std::any token_value;

  std::optional<parser::Tealang_t> return_type;
  std::optional<std::any> return_value;

public:
  void visit(parser::ASTLiteral *) override;
  void visit(parser::ASTIdentifier *) override;
  void visit(parser::ASTFunctionCall *) override;
  void visit(parser::ASTSubExpression *) override;
  void visit(parser::ASTBinOp *) override;
  void visit(parser::ASTProgram *) override;
  void visit(parser::ASTBlock *) override;
  void visit(parser::ASTUnary *) override;
  void visit(parser::ASTVariableDecl *) override;
  void visit(parser::ASTPrintStatement *) override;
  void visit(parser::ASTAssignment *) override;
  void visit(parser::ASTIfStatement *) override;
  void visit(parser::ASTForStatement *) override;
  void visit(parser::ASTWhileStatement *) override;
  void visit(parser::ASTReturn *) override;
  void visit(parser::ASTFunctionDecl *) override;
  Variable eval_function(parser::ASTFunctionCall *);
};

} // namespace visitor
#endif // INTERPRETER_H_
