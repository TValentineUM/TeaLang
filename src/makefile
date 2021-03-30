##
# TeaLang Interpreter
#
# @file
# @version 0.1

CXX=g++
CXXFLAGS=-I -Wall -std=c++17
DEPS = lexer/lexer.hh lexer/token.hh
OBJ =  lexer/lexer.o lexer/token.o
MAINOBJ = main.o $(OBJ)

OUT_DIR = ./bin/
MKDIR_P = mkdir -p

%.o: %.cc $(DEPS) $(TESTDEPS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(MAINOBJ)
		$(CXX) -o $(OUT_DIR)$@ $^ $(CXXFLAGS)

clean:
	rm -rf **/*.o

# end
