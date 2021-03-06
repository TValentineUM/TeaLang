#include "xmlvisitor.hh"
#include "../parser/AST.hh"
using namespace visitor;

void XMLVisitor::visit(parser::ASTLiteral *x) {

  file << indentation;
  // std::cout << x->value << " : " << (x->val) << std::endl;
  switch (x->type) {
  case parser::tea_int:
    file << "<IntConst>" << x->value << "</IntConst>" << std::endl;
    break;
  case parser::tea_float:
    file << "<FloatConst>" << x->value << "</FloatConst>" << std::endl;
    break;
  case parser::tea_string:
    file << "<StringConst>" << x->value << "</StringConst>" << std::endl;
    break;
  case parser::tea_bool:
    file << "<BoolConst>" << x->value << "</BoolConst>" << std::endl;
    break;
  }
}

void XMLVisitor::visit(parser::ASTIdentifier *x) {
  file << indentation << "<Identifier>" << x->name << "</Identifier>"
       << std::endl;
}

void XMLVisitor::visit(parser::ASTSubExpression *x) {
  file << indentation << "<SubExpr>" << std::endl;
  indent();
  x->expr->accept(this);
  unindent();
  file << indentation << "</SubExpr>" << std::endl;
}

void XMLVisitor::visit(parser::ASTBinOp *x) {

  file << indentation << "<BinExprNode Op=\"" << x->value << "\">" << std::endl;
  indent();
  x->left->accept(this);
  x->right->accept(this);
  unindent();
  file << indentation << "</BinExprNode>" << std::endl;
}

void XMLVisitor::visit(parser::ASTProgram *x) {
  file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  file << "<root description=\"Tealang AST Generation\">" << std::endl;
  indent();
  file << indentation << "<Prog>" << std::endl;
  indent();

  for (int i = 0; i != x->statements.size(); i++) {
    if (x->statements[i] != nullptr) {
      x->statements[i]->accept(this);
    }
  }
  unindent();
  file << indentation << "</Prog>" << std::endl;
  unindent();
  file << "</root>" << std::endl;
}

void XMLVisitor::visit(parser::ASTBlock *x) {
  file << indentation << "<Block>" << std::endl;
  indent();
  for (auto item : x->source) {
    item->accept(this);
  }
  unindent();
  file << indentation << "</Block>" << std::endl;
}

void XMLVisitor::visit(parser::ASTVariableDecl *x) {
  file << indentation << "<Decl>" << std::endl;
  indent();
  file << indentation << "<Var Type=\"" << x->Type << "\">" << x->identifier
       << "</Var>" << std::endl;
  indent();

  x->value->accept(this);
  unindent();
  unindent();
  file << indentation << "</Decl>" << std::endl;
}

void XMLVisitor::visit(parser::ASTAssignment *x) {
  file << indentation << "<Assign>" << std::endl;
  indent();
  file << indentation << "<Var>" << x->identifier << "</Var>" << std::endl;
  x->value->accept(this);
  unindent();
  file << indentation << "</Assign>" << std::endl;
}

void XMLVisitor::visit(parser::ASTPrintStatement *x) {
  file << indentation << "<Print>" << std::endl;
  indent();
  x->value->accept(this);
  unindent();
  file << indentation << "</Print>" << std::endl;
}

void XMLVisitor::visit(parser::ASTReturn *x) {
  file << indentation << "<Return>" << std::endl;
  indent();
  x->value->accept(this);
  unindent();
  file << indentation << "</Return>" << std::endl;
}

void XMLVisitor::visit(parser::ASTUnary *x) {
  file << indentation << "<Unary Op=\"" << x->op << "\">" << std::endl;
  indent();
  x->expr->accept(this);
  unindent();
  file << indentation << "</Unary>" << std::endl;
}

void XMLVisitor::visit(parser::ASTIfStatement *x) {
  file << indentation << "<If>" << std::endl;
  indent();
  file << indentation << "<Condition>" << std::endl;
  indent();
  x->eval->accept(this);
  unindent();
  file << indentation << "</Condition>" << std::endl;
  file << indentation << "<EvalTrue>" << std::endl;
  indent();
  x->eval_true->accept(this);
  unindent();
  file << indentation << "</EvalTrue>" << std::endl;
  file << indentation << "<Else>" << std::endl;
  indent();
  if (x->eval_false) {
    x->eval_false->accept(this);
  }
  unindent();
  file << indentation << "</Else>" << std::endl;
  unindent();
  file << indentation << "</If>" << std::endl;
}

void XMLVisitor::visit(parser::ASTWhileStatement *x) {
  file << indentation << "<While>" << std::endl;
  indent();
  file << indentation << "<Condition>" << std::endl;
  indent();
  x->condition->accept(this);
  unindent();
  file << indentation << "</Condition>" << std::endl;
  file << indentation << "<EvalTrue>" << std::endl;
  indent();
  x->eval->accept(this);
  unindent();
  file << indentation << "</EvalTrue>" << std::endl;
  unindent();
  file << indentation << "</While>" << std::endl;
}

void XMLVisitor::visit(parser::ASTForStatement *x) {
  file << indentation << "<For>" << std::endl;
  indent();
  if (x->init) {
    x->init->accept(this);
  }
  file << indentation << "<Condition>" << std::endl;
  indent();
  if (x->condition) {
    x->condition->accept(this);
  }
  unindent();
  file << indentation << "</Condition>" << std::endl;
  x->assign->accept(this);
  file << indentation << "<EvalTrue>" << std::endl;
  indent();
  x->eval->accept(this);
  unindent();
  file << indentation << "</EvalTrue>" << std::endl;
  unindent();
  file << indentation << "</For>" << std::endl;
}

void XMLVisitor::visit(parser::ASTFunctionDecl *x) {
  file << indentation << "<FuncDecl>" << std::endl;
  indent();
  file << indentation;
  file << "<Var Type=\"" << x->type << "\">" << x->identifier << "</Var>"
       << std::endl;
  for (auto item : x->arguments) {
    file << indentation << "<Arg Type=\"" << std::get<1>(item) << "\">"
         << std::get<0>(item) << "</Arg>" << std::endl;
  }
  x->body->accept(this);
  unindent();
  file << indentation << "</FuncDecl>" << std::endl;
}

void XMLVisitor::visit(parser::ASTFunctionCall *x) {
  file << indentation << "<FuncCall Id=\"" << x->name << "\">" << std::endl;
  indent();
  int i = 0;
  while (i < x->args.size()) {
    if (x->args[i] != nullptr) {

      x->args[i]->accept(this);
      i++;
    } else {
      break;
    }
  }

  unindent();
  file << indentation << "</FuncCall>" << std::endl;
}
