#ifndef SEMANTIC_VISITOR_H_
#define SEMANTIC_VISITOR_H_

#include "visitor.hh"
using namespace visitor;
#include "../parser/AST.hh"
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace visitor {

class SemanticVisitor : public Visitor {

private:
  class Function {
  public:
    std::string name;
    parser::Tealang_t return_type;
    std::vector<std::tuple<std::string, parser::Tealang_t>> arguments;
  };

  class Variable {
  public:
    parser::Tealang_t type; /**< Variable Type, supports base and array types */
    std::optional<int> size; /**< Optional Parameter for arrays */
    std::string name;        /**< Variable Name*/
  };

  class Scope {

  public:
    Scope() : variable_scope{std::map<std::string, Variable>()} {}

    Function get_func(std::string); /**< Finds Function*/

    Variable get_var(std::string); /**< Find variable starting from top scope */

    void add_var(Variable);

    void add_func(Function);

    void edit_func(Function);

    std::vector<std::map<std::string, Variable>>
        variable_scope; /**< The Vector stores all
                           current variable
                           scopes, where each
                           scope is a mapping from
                           a string to its type*/

    std::map<std::string, Function>
        function_scope; /**< Maps function names to a tuple
                           containing the function return
                           type and the argument types */
  };

  parser::Tealang_t token_type;
  std::optional<std::tuple<parser::Tealang_t, bool>> function_type;
  Scope current_scope;

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
  void visit(parser::ASTArrayAccess *) override;
  void visit(parser::ASTArrayDecl *) override;
  void visit(parser::ASTArrayAssignment *) override;
  void visit(parser::ASTArrayLiteral *) override;
  void eval_function_body(parser::ASTBlock *, std::string);
};

} // namespace visitor

#endif // SEMANTIC_VISITOR_H_
