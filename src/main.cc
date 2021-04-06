#include "lexer/lexer.hh"
#include "parser/parser.hh"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
  string source = "test";
  string infile = source + ".tl";
  // string outfile = source + ".to";

  // lexer::Lexer lexed(infile);
  // ofstream file;
  // file.open(outfile);
  // file << lexed;
  // file.close();
  parser::Parser test(infile);
  cout << "Done";

  return 0;
}
