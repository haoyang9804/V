#include "ASTVisualizer.h"
#include "parser.h"
#include "tokenizer.h"
#include "codegen.hpp"
#include <assert.h>

int main(int argc, char **argv) {
  assert(argc == 2);
  char *code = argv[1];
  tokenizer(code);
  AST();
  visualize();
  codegen();
}