##
# TeaLang Interpreter
#
# @file
# @version 0.1

CXX=g++
CXXFLAGS=-I -Wall -std=c++17 -g
DEPS = lexer/lexer.hh lexer/token.hh parser/parser.hh parser/AST.hh visitor/visitor.hh visitor/xmlvisitor.hh  visitor/semantic_visitor.hh
OBJ =  lexer/lexer.o lexer/token.o parser/parser.o parser/AST.o visitor/xmlvisitor.o visitor/semantic_visitor.o
MAINOBJ = main.o $(OBJ)

OUT_DIR = ./bin/
MKDIR_P = mkdir -p

%.o: %.cc $(DEPS) $(TESTDEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -rf **/*.o

# end
