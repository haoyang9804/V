#pragma once

typedef enum { TK_PUNC, TK_INTEGER, TK_EOF } TokenKind;

typedef struct Token Token;

struct Token {
  int val;
  int len;
  char *code;
  Token *next;
  TokenKind kind;
};

/*! \brief tokenize the code, return the head of the token stream */
void tokenizer(char *);