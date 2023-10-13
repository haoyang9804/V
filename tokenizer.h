#pragma once

typedef enum { TKOP, TKINTEGER, TOKEN_GROUP, TKEOF } TokenKind;

typedef struct Token Token;

struct Token {
  int val;
  char *code;
  Token *next;
  TokenKind kind;
};

/*! \brief tokenize the code, return the head of the token stream */
Token *tokenizer(char *code);

/*! \brief check the token stream*/
void check(Token*);