#include "lexer/lexer.hh"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
  string source = "test";
  string infile = source + ".tl";
  string outfile = source + ".to";

  lexer::Lexer lexed(infile);
  ofstream file;
  file.open(outfile);
  file << lexed;
  file.close();
  cout << "Done";

  return 0;
}
