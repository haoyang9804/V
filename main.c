#include "ASTVisualizer.h"
#include "ASTVisualizer_play.h"
#include "parser.h"
#include "parser_play.h"
#include "tokenizer.h"
#include <assert.h>

int main(int argc, char **argv) {
  assert(argc == 2);
  char *code = argv[1];
  tokenizer(code);
  AST();
  visualize();
  AST2();
  visualize2();
}