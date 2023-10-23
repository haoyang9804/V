#include "tokenizer.h"
#include "error.h"
#include "vcc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *code;
Token *token;
Token *token_;

static Token *_generate_token(Token *cur_token, TokenKind kind) {
  Token *new_token = malloc(sizeof(Token));
  new_token->kind = kind;
  cur_token->next = new_token;
  return new_token;
}

void tokenizer(char *_code) {
  Token head = {};
  Token *cur_token = &head;
  code = _code;
  while (*_code) {
    if (isspace(*_code)) {
      _code++;
      continue;
    } else if (ispunct(*_code)) {
      /*
      TODO: Currently, only supports token whose length is 1
      */
      cur_token = _generate_token(cur_token, TK_PUNC);
      cur_token->code = _code;
      cur_token->len = 1;
      _code++;
    } else if (isdigit(*_code)) {
      cur_token = _generate_token(cur_token, TK_INTEGER);
      cur_token->code = _code;
      char *pre_code = _code;
      cur_token->val = strtol(_code, &_code, 10);
      cur_token->len = _code - pre_code;
    } else {
      error_at(_code, "Cannot tokenize this code: %c\n", *_code);
    }
  }
  cur_token = _generate_token(cur_token, TK_EOF);
  token_ = token = head.next;
}