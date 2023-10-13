#include "tokenizer.h"
#include <assert.h>

int main(int argc, char** argv) {
  assert(argc == 2);
  char *code = argv[1];
  Token *token = tokenizer(code);
  check(token);
}