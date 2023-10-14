#pragma once

#include "tokenizer.h"

typedef enum { ND_NUM, ND_ADD, ND_SUB, ND_MUL, ND_DIV } NodeKind;

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