#ifndef VISITOR_H_
#define VISITOR_H_

namespace parser {

class AST;
class ASTLiteral;
class ASTExpression;
class ASTIdentifier;
class ASTFunctionCall;
class ASTArrayAccess;
class ASTSubExpression;
class ASTBinOp;
class ASTProgram;
class ASTStatement;
class ASTBlock;
class ASTVariableDecl;
class ASTArrayDecl;
class ASTPrintStatement;
class ASTAssignment;
class ASTIfStatement;
class ASTForStatement;
class ASTWhileStatement;
class ASTReturn;
class ASTFunctionDecl;
class ASTUnary;
class ASTArrayLiteral;
class ASTArrayAssignment;

} // namespace parser

namespace visitor {

// Visitor abstract class (Interface). All visitor implementations (extensions
// to this class) have to handle.
class Visitor {

public:
  virtual void visit(parser::ASTLiteral *) = 0;
  virtual void visit(parser::ASTArrayLiteral *) = 0;
  virtual void visit(parser::ASTIdentifier *) = 0;
  virtual void visit(parser::ASTFunctionCall *) = 0;
  virtual void visit(parser::ASTArrayAccess *) = 0;
  virtual void visit(parser::ASTSubExpression *) = 0;
  virtual void visit(parser::ASTUnary *) = 0;
  virtual void visit(parser::ASTBinOp *) = 0;
  virtual void visit(parser::ASTProgram *) = 0;
  virtual void visit(parser::ASTBlock *) = 0;
  virtual void visit(parser::ASTVariableDecl *) = 0;
  virtual void visit(parser::ASTPrintStatement *) = 0;
  virtual void visit(parser::ASTAssignment *) = 0;
  virtual void visit(parser::ASTIfStatement *) = 0;
  virtual void visit(parser::ASTForStatement *) = 0;
  virtual void visit(parser::ASTWhileStatement *) = 0;
  virtual void visit(parser::ASTReturn *) = 0;
  virtual void visit(parser::ASTFunctionDecl *) = 0;
  virtual void visit(parser::ASTArrayDecl *) = 0;
  virtual void visit(parser::ASTArrayAssignment *) = 0;
};

} // namespace visitor

#endif // VISITOR_H_
