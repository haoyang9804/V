// A parser for Non-left-recursive grammar 1
#pragma once

#include "tokenizer.h"

typedef enum { ND_NUM_, ND_ADD_, ND_SUB_, ND_MUL_, ND_DIV_ } NodeKind_;

typedef struct Node_ Node_;

struct Node_ {
  union {
    int val;
    char *op;
  } content;
  Node_ *left;
  Node_ *right;
  NodeKind_ kind;
};

/*! \brief check the token stream*/
void AST2();