#include "lexer/lexer.hh"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
  string temp = "12345.789";

  lexer::Lexer lexed(temp);
  cout << lexed << endl;

  return 0;
}
