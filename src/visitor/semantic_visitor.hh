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
    std::string code;
    parser::Tealang_t return_type;
    std::string type_name;
    std::vector<std::tuple<std::string, parser::Tealang_t, std::string>>
        arguments;
    std::string get_code();
  };

  class Variable {
  public:
    parser::Tealang_t type; /**< Variable Type, supports base and array types */
    std::string name;       /**< Variable Name*/

    std::optional<std::string> type_name; /**< Struct typename*/
    std::optional<std::map<std::string, Variable>>
        members; /**< Collection of member variables */
  };

  // Base class used so store the default variables and functions for a struct
  class Struct {
  public:
    std::string name;
    std::map<std::string, Variable>
        base_variables; /**< Provides the default initialised variables*/
    std::map<std::string, Function>
        base_functions; /**< Provides all the member functions of a struct */
  };

  class Scope {

  public:
    std::string
        code_generator(std::string,
                       std::vector<std::tuple<parser::Tealang_t, std::string>>);

    Scope()
        : variable_scope{std::map<std::string, Variable>()},
          function_scope{std::map<std::string, Function>()} {}

    Function get_func(std::string); /**< Finds Function*/

    Variable get_var(std::string); /**< Find variable starting from top scope */

    Struct get_struct(std::string); /**< Get Base for Struct of given name*/

    void add_var(Variable);

    void add_func(Function);

    void add_struct(Struct);

    void edit_func(Function);

    std::vector<std::map<std::string, Variable>>
        variable_scope; /**< The Vector stores all
                           current variable
                           scopes, where each
                           scope is a mapping from
                           a string to its type*/

    std::vector<std::map<std::string, Function>> function_scope;
    /**< Function Scope is instantiated as a stack of scopes due to structs */

    std::map<std::string, Struct>
        struct_scope; /**< Map containing all base structs */
  };

  void token_setter(parser::Tealang_t);

  parser::Tealang_t token_type;
  std::string token_name;
  std::optional<std::tuple<parser::Tealang_t, std::string, bool>> function_type;
  Scope scope;

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
  void visit(parser::ASTStructAccess *) override;
  void visit(parser::ASTStructFunc *) override;
  void visit(parser::ASTStructDefn *) override;
  void visit(parser::ASTStructAssign *) override;
  void visit(parser::ASTStructDecl *) override;
  void eval_function_body(parser::ASTBlock *, std::string);
};

} // namespace visitor

#endif // SEMANTIC_VISITOR_H_
