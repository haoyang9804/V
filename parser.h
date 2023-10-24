// A parser for a Non-left-recursive grammar (in grammar.bnf)
#pragma once

#include "tokenizer.h"

typedef enum { ND_NUM, ND_ADD, ND_SUB, ND_MUL, ND_DIV, ND_LE, ND_LT, ND_GE, ND_GT, ND_EQ, ND_NE} NodeKind;

typedef struct Node Node;

struct Node {
  union {
    int val;
    char *op;
  } content;
  Node *left;
  Node *right;
  NodeKind kind;
};

/*! \brief check the token stream*/
void AST();