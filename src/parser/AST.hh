#ifndef __AST_H_
#define __AST_H_

#include "../lexer/lexer.hh"

#include <string>
#include <vector>

namespace parser {

enum Operators { Plus, Minus, Division, Multiplication } typedef Operators;

enum Tealang_t { Float, Int, Bool, String } typedef Tealang_t;

class AST {};

class ASTLiteral : public AST {
public:
  Tealang_t type;
  std::string value;
  ASTLiteral(lexer::Token tok);
};

class ASTExpression : public AST {};

class ASTIdentifier : public ASTExpression {
  std::string name;
};

class ASTFunctionCall : public ASTExpression {
  std::string name;
  std::vector<ASTExpression *> args;
};

class ASTSubExpression : public ASTExpression {
  ASTExpression *expr;
};

class ASTBinOp : public ASTExpression {
  ASTExpression *left, *right;
  Operators op;
};

/**
 *
 * Implementing Statement Nodes
 *
 */

class ASTStatement : public AST {};

class ASTProgram : public AST {
  std::vector<ASTStatement *> statements;
};

class ASTBlock : ASTStatement {
  std::vector<ASTStatement *> source;
};

class ASTVariableDecl : public ASTStatement {
  std::string identifier;
  Tealang_t Type;
  ASTExpression *value;
};

class ASTPrintStatement : public ASTStatement {
  ASTExpression *value;
};

class ASTAssignment : public ASTStatement {
  std::string identifier;
  ASTExpression *value;
};

class ASTIfStatement : public ASTStatement {
  ASTExpression *eval;
  ASTBlock *eval_true;
  ASTBlock *eval_false;
};

class ASTForStatement : public ASTStatement {
  ASTVariableDecl *init;
  ASTExpression *condition;
  ASTAssignment *assign;
};

class ASTWhileStatement : public ASTStatement {
  ASTExpression *condition;
  ASTBlock *eval;
};

class ASTReturn : public ASTStatement {
  ASTExpression *value;
};

class ASTFunctionDecl : public ASTStatement {
  Tealang_t type;
  std::string identifier;
  std::vector<std::tuple<std::string, Tealang_t>> arguments;
  ASTBlock *body;
};

} // namespace parser

#endif // __AST_H_
