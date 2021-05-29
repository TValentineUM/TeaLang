#ifndef XMLVISITOR_H_
#define XMLVISITOR_H_

#include "visitor.hh"
using namespace visitor;
#include "../parser/AST.hh"
#include <fstream>
#include <iostream>
#include <string>

namespace visitor {
class XMLVisitor : public Visitor {

private:
  std::map<std::string, std::string> op_to_string = {
      {"*", "*"},    {"/", "/"},   {"and", "and"}, {"+", "+"},
      {"-", "-"},    {"or", "or"}, {"<", "&lt;"},  {">", "&gt;"},
      {"==", "=="},  {"!=", "ne"}, {"<=", "&le;"}, {">=", "&ge;"},
      {"not", "not"}

  };

  std::string
      indentation;    /**< String containing the current amount of indentation*/
  std::ofstream file; /**< Output file for the XML Generator*/

  inline void indent() {
    indentation.append("\t");
  } /**< Increases the current amount of indentation */
  inline void unindent() {
    indentation.pop_back();
  } /**< Decreases the current amount of indentation */

public:
  XMLVisitor(std::string filename) : file{filename, std::ofstream::out} {}
  ~XMLVisitor() {}
  void visit(parser::ASTLiteral *) override;
  void visit(parser::ASTArrayLiteral *) override;
  void visit(parser::ASTIdentifier *) override;
  void visit(parser::ASTFunctionCall *) override;
  void visit(parser::ASTArrayAccess *) override;
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
  void visit(parser::ASTArrayDecl *) override;
  void visit(parser::ASTArrayAssignment *) override;
  void visit(parser::ASTStructAccess *) override;
  void visit(parser::ASTStructFunc *) override;
  void visit(parser::ASTStructDefn *) override;
  void visit(parser::ASTStructAssign *) override;
  void visit(parser::ASTStructDecl *) override;
};
} // namespace visitor
#endif // XMLVISITOR_H_
