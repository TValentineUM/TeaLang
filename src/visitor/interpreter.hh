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

class Interpreter : public Visitor {

private:
  class Function {
  public:
    std::string name;
    std::string code;
    parser::Tealang_t return_type;
    std::string type_name;
    std::vector<std::tuple<std::string, parser::Tealang_t, std::string>>
        arguments;
    parser::ASTBlock *function_body;
    std::string get_code();
  };

  class Variable {
  public:
    parser::Tealang_t type; /**< Variable Type, supports base and array*/
    std::string name;       /**< Variable Name*/
    std::any value;         /**< Using the enum for the get*/
    std::string type_name;
    std::optional<int>
        size; /**< Optional integer parameter used when evaulating arrays*/
    std::optional<std::map<std::string, Variable>>
        members; /**< Optional collection of memeber variables to be used when
                    evaulating structs*/
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
    Scope()
        : function_call{false},
          variable_scope{std::map<std::string, Variable>()},
          function_scope{std::map<std::string, Function>()} {}

    Function get_func(std::string); /**< Finds Function*/

    Variable get_var(std::string); /**< Find variable starting from top scope */

    Struct get_struct(std::string);

    void add_var(Variable);

    void add_func(Function);

    void add_struct(Struct);

    void update_var(std::string,
                    Variable); /**< Updates a variable starting from top scope*/

    std::string
        code_generator(std::string,
                       std::vector<std::tuple<parser::Tealang_t, std::string>>);

    std::string code_generator(std::string, std::vector<Variable>);

    std::string code_generator(std::string, std::vector<parser::Tealang_t>);

    std::vector<std::map<std::string, Variable>>
        variable_scope; /**< The Vector stores all current variable scopes,
                           where each scope is a mapping from a string to its
                           type*/

    std::vector<std::map<std::string, Function>> function_scope;
    /**< Function Scope is instantiated as a stack of scopes due to structs */

    std::map<std::string, Struct>
        struct_scope; /**< Map containing all base structs */

    bool function_call;
  };

  void token_setter(parser::Tealang_t);

  Scope scope;

  parser::Tealang_t
      token_type;         /**< Stores the type of an evaluated expression */
  std::string token_name; /**< Stores the typename of an evaluated expression */
  std::any token_value;   /**< Stores the value of an evaulated expression*/

  std::optional<parser::Tealang_t>
      return_type; /**< Optional parameter for evaluated function return*/
  std::optional<std::string>
      return_typename; /**< Optional parameter for evaluated function return*/
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
  void visit(parser::ASTArrayAccess *) override;
  void visit(parser::ASTArrayDecl *) override;
  void visit(parser::ASTArrayAssignment *) override;
  void visit(parser::ASTArrayLiteral *) override;
  void visit(parser::ASTStructAccess *) override;
  void visit(parser::ASTStructFunc *) override;
  void visit(parser::ASTStructDefn *) override;
  void visit(parser::ASTStructAssign *) override;
  void visit(parser::ASTStructDecl *) override;
  void eval_bin_op(int left_val, int right_val, parser::Operators op);
  void eval_bin_op(float left_val, float right_val, parser::Operators op);
  void eval_bin_op(bool left_val, bool right_val, parser::Operators op);
  void eval_bin_op(char left_val, char right_val, parser::Operators op);
  void eval_bin_op(std::string left_val, std::string right_val,
                   parser::Operators op);
  template <typename T> void eval_arr_lit(std::vector<parser::ASTExpression *>);
  Variable eval_function(parser::ASTFunctionCall *);
};

} // namespace visitor
#endif // INTERPRETER_H_
