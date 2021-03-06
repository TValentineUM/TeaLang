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
  //   static std::map<parser::Operators, std::string> op_to_string = {

  // {"*", op_times},
  // {"/", op_div},
  // {"and", op_and},
  // {"+", op_plus},
  // {"-", op_minus},
  // {"or", op_or},
  // {"<", op_less},
  // {">", op_grtr},
  // {"==", op_eql},
  // {"!=", op_neql},
  // {"<=", op_le},
  // {">=", op_ge},
  // {"not", op_not}

  //       {"*", op_times}, {"/", op_div},   {"and", op_and}, {"+", op_plus},
  //       {"-", op_minus}, {"or", op_or},   {"<", op_less},  {">", op_grtr},
  //       {"==", op_eql},  {"!=", op_neql}, {"<=", op_le},   {">=", op_ge},
  //       {"not", op_not}

  //   };
  //};
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
#endif // XMLVISITOR_H_
