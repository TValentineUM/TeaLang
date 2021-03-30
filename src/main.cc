#include "lexer/lexer.hh"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
  string temp = "test.tl";

  lexer::Lexer lexed(temp);
  cout << lexed;

  return 0;
}
