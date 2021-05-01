#include "lexer/lexer.hh"
#include "parser/parser.hh"
#include "visitor/semantic_visitor.hh"
#include "visitor/xmlvisitor.hh"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
  string source = "example";
  string infile = source + ".tl";
  string outfile = source + ".xml";

  // lexer::Lexer lexed(infile);
  // ofstream file;
  // file.open(outfile);
  // file << lexed;
  // file.close();
  XMLVisitor xml(outfile);
  parser::Parser test(infile);
  // cout << test.lex << endl;
  SemanticVisitor sem;
  xml.visit(test.tree);

  cout << "Done from xml" << endl;

  sem.visit(test.tree);
  cout << "Done from semantic";
  return 0;
}
