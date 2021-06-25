#include "lexer/lexer.hh"
#include "parser/parser.hh"
#include "visitor/interpreter.hh"
#include "visitor/semantic_visitor.hh"
#include "visitor/xmlvisitor.hh"
#include <cstdio>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>

using namespace std;

void display_help();

int main(int argc, char *argv[]) {

  map<char, string> args; // Map of Arguments
  char c;

  static struct option long_options[] = {
      {"xml", required_argument, NULL, 'x'},
      {"lexer", required_argument, NULL, 'l'},
      {"output", required_argument, NULL, 'o'},
      {"help", no_argument, NULL, 'h'},
      {NULL, 0, NULL, 0}};

  int option_index = 0;
  while ((c = getopt_long(argc, argv, "l:x:o:h", long_options,
                          &option_index)) != -1) {
    switch (c) {
    case 'l':
    case 'x':
    case 'o':
      args.insert({c, optarg});
      break;
    case 'h':
      display_help();
      return 0;
    default:
      cerr << "Unknown flag provided, for more information run " << argv[0]
           << " again with '--help'" << endl;
      return 0;
    }
  }

  vector<string> extra_args;
  for (; optind < argc; optind++) {
    extra_args.push_back(argv[optind]);
  }
  if (extra_args.size() == 0) {
    cerr << "Missing input file, for more information run " << argv[0]
         << "again with '--help'" << endl;
  }

  lexer::Lexer l(extra_args[0]);
  if (args.find('l') != args.end()) {
    ofstream file;
    file.open(args.at('l'));
    file << l;
    file.close();
  }

  parser::Parser p(extra_args[0]);
  if (args.find('x') != args.end()) {
    XMLVisitor xml(args.at('x'), p.tree);
  }

  SemanticVisitor semantic_visitor;
  semantic_visitor.visit(p.tree);

  Interpreter interpreter;
  if (args.find('o') != args.end()) {
    auto fp = freopen(args.at('o').c_str(), "w", stdout);
    interpreter.visit(p.tree);
    fclose(fp);
  } else {
    interpreter.visit(p.tree);
  }

  return 0;
}

void display_help() {
  cout << "Welcome to the Tealang interpreter" << endl;
  cout << "Usage: tealang [options] file" << endl;
  cout << "Options:" << endl;
  cout << " -l <file> \t Places the lexed input into <file>" << endl;
  cout << " -x <file> \t Places the parsed input in xml form into <file>"
       << endl;
  cout << " -o <file> \t Redirects the output of the interpreter into <file>"
       << endl;
}
