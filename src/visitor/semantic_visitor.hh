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
  std::optional<parser::Tealang_t>
      token_type; /**< Optional Token used to track the current type of an
                     expression. This is needed by the operator nodes to ensure
                     type checking*/

  std::optional<std::tuple<parser::Tealang_t, bool>>
      function_type; /**< Optional Token used to track the needed return
                     type of the current function */

  // Working under the assumption that functions can only be declared in the
  // global scope
  std::map<std::string,
           std::tuple<parser::Tealang_t, std::vector<parser::Tealang_t>>>
      function_scope; /**< Maps function names to a tuple containing the
                         function return type and the argument types */

  std::vector<std::map<std::string, parser::Tealang_t>>
      variable_scope; /**< The Vector stores all current variable scopes, where
                         each scope is a mapping from a string to its type*/

  std::optional<std::tuple<parser::Tealang_t, std::vector<parser::Tealang_t>>>
      res_func(std::string); /**< Checks if function exists and optionally
                                returns the parameters and return type*/

  std::optional<parser::Tealang_t>
      res_var_all(std::string); /**< Checks if the variable exits in any scope
                          and optionally returns its type */

  std::optional<parser::Tealang_t>
      res_var_local(std::string); /**< Checks if the variable exits in the
                           current scope and optionally returns its type */

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
};

} // namespace visitor

#endif // SEMANTIC_VISITOR_H_
