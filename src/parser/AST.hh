#ifndef __AST_H_
#define __AST_H_

#include "../lexer/lexer.hh"
#include "../visitor/visitor.hh"

#include <algorithm>
#include <string>
#include <vector>

namespace parser {

enum Operators {
  op_times,
  op_div,
  op_and,
  op_plus,
  op_minus,
  op_or,
  op_less,
  op_grtr,
  op_eql,
  op_neql,
  op_le,
  op_ge,
  op_not
} typedef Operators;

static std::map<std::string, Operators> tok_to_op = {
    {"*", op_times}, {"/", op_div},   {"and", op_and}, {"+", op_plus},
    {"-", op_minus}, {"or", op_or},   {"<", op_less},  {">", op_grtr},
    {"==", op_eql},  {"!=", op_neql}, {"<=", op_le},   {">=", op_ge},
    {"not", op_not}

};

enum Tealang_t {
  tea_auto = -1,
  tea_float,
  tea_int,
  tea_bool,
  tea_string,
  tea_char,
  tea_arr_float,
  tea_arr_int,
  tea_arr_bool,
  tea_arr_string,
  tea_arr_char,
  tea_struct

} typedef Tealang_t;

class AST {

  virtual void accept(visitor::Visitor *) = 0;
};

class ASTExpression : public AST {
public:
  virtual void accept(visitor::Visitor *) = 0;
};

class ASTLiteral : public ASTExpression {
public:
  Tealang_t type;
  std::string value;
  std::string type_name;
  ASTLiteral(lexer::Token tok);
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTArrayLiteral : public ASTExpression {
public:
  std::vector<ASTExpression *> values;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTIdentifier : public ASTExpression {
public:
  std::string name;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTArrayAccess : public ASTExpression {
public:
  ASTExpression *index;
  std::string name;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTFunctionCall : public ASTExpression {
public:
  std::string name;
  std::vector<ASTExpression *> args;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTSubExpression : public ASTExpression {
public:
  ASTExpression *expr;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTUnary : public ASTExpression {
public:
  Operators op;
  std::string value;
  ASTExpression *expr;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTBinOp : public ASTExpression {
public:
  ASTExpression *left, *right;
  Operators op;
  std::string value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

/**
 *
 * Implementing Statement Nodes
 *
 */

class ASTStatement : public AST {
public:
  void accept(visitor::Visitor *) override = 0;
};

class ASTProgram : public AST {
public:
  std::vector<ASTStatement *> statements;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTBlock : public ASTStatement {
public:
  std::vector<ASTStatement *> source;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTVariableDecl : public ASTStatement {
public:
  std::string identifier;
  Tealang_t Type;
  ASTExpression *value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTArrayDecl : public ASTVariableDecl {
public:
  ASTExpression *size;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTPrintStatement : public ASTStatement {
public:
  ASTExpression *value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTAssignment : public ASTStatement {
public:
  std::string identifier;
  ASTExpression *value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTArrayAssignment : public ASTAssignment {
public:
  ASTExpression *index;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTIfStatement : public ASTStatement {
public:
  ASTExpression *eval;
  ASTBlock *eval_true;
  ASTBlock *eval_false;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTForStatement : public ASTStatement {
public:
  ASTVariableDecl *init;
  ASTExpression *condition;
  ASTAssignment *assign;
  ASTBlock *eval;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTWhileStatement : public ASTStatement {
public:
  ASTExpression *condition;
  ASTBlock *eval;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTReturn : public ASTStatement {
public:
  ASTExpression *value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTFunctionDecl : public ASTStatement {
public:
  Tealang_t type;
  std::string type_name;
  std::string identifier;
  std::vector<std::tuple<std::string, Tealang_t, std::string>>
      arguments; /**< <Variable Name, Type, Typename (if Struct)>*/

  std::vector<Tealang_t> param_types();
  std::vector<std::string> param_names();
  ASTBlock *body;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

// Struct Stuff
//

class ASTStructAccess : public ASTExpression {
public:
  std::string name;
  std::string element;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTStructFunc : public ASTExpression {
public:
  std::string name;
  std::string element;
  std::vector<ASTExpression *> args;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTStructDefn : public ASTStatement {
public:
  std::string name;
  std::vector<ASTVariableDecl *> vars;
  std::vector<ASTFunctionDecl *> funcs;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTStructAssign : public ASTStatement {
public:
  std::string name;
  std::string element;
  ASTExpression *value;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

class ASTStructDecl : public ASTVariableDecl {
public:
  ASTStructDecl() { Type = tea_struct; }
  std::string struct_name;
  inline void accept(visitor::Visitor *visitor) { visitor->visit(this); }
};

} // namespace parser

#endif // __AST_H_
