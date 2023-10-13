#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

char *_tokenizer_code_head;

Token *_generate_token(Token *cur_token, TokenKind kind) {
  Token *new_token = malloc(sizeof(Token));
  new_token->kind = kind;
  cur_token->next = new_token;
  return new_token;
}

void _error_at(char *cur_code, char *fmt, ...) {
  va_list ag;
  va_start(ag, fmt);
  fprintf(stderr, "%s\n", _tokenizer_code_head);
  fprintf(stderr, "%*s^ ", (int)(cur_code - _tokenizer_code_head), "");
  vfprintf(stderr, fmt, ag);
  fprintf(stderr, "\n");
  va_end(ag);
  exit(1);
}

Token *tokenizer(char *code) {
  Token head = {};
  Token *cur_token = &head;
  _tokenizer_code_head = code;
  while (*code) {
    if (isspace(*code)) {
      code++;
      continue;
    } else if (*code == '+' || *code == '-') {
      cur_token = _generate_token(cur_token, TKOP);
      cur_token->code = code;
      code++;
    } else if (isdigit(*code)) {
      cur_token = _generate_token(cur_token, TKINTEGER);
      cur_token->code = code; 
      cur_token->val = strtol(code, &code, 10);
    } else {
      _error_at(code, "Cannot tokenize this code: %c\n", *code);
    }
  }
  cur_token = _generate_token(cur_token, TKEOF);
  return head.next;
}

bool _at_end(Token *token) {
  return token->kind == TKEOF;
}

bool _next_is_op(Token* token, char op) {
  if (token->next->kind == TKOP && token->next->code[0] == op) {
    return true;
  }
  return false;
}

bool _next_is_integer(Token* token) {
  if (token->next->kind == TKINTEGER) {
    return true;
  }
  return false;
}

bool _is_op(Token* token) {
  return token->kind == TKOP;
}

bool _is_integer(Token* token) {
  return token->kind == TKINTEGER;
}

void _eat(Token** token) {
  *token = (*token)->next;
}

void _eat_op(Token **token) {
  if (!_is_op(*token)) {
    _error_at((*token)->code, "Expect an operator");
  }
  _eat(token);
}

void _eat_integer(Token **token) {
  if (!_is_integer(*token)) {
    _error_at((*token)->code, "Expect an integer");
  }
  _eat(token);
}

void check(Token *token) {
  _eat_integer(&token); 
  while(!_at_end(token)) {
    _eat_op(&token);
    if (_at_end(token)) {
      _error_at(token->code, "Expect an integer");
    }
    else {
      _eat_integer(&token);
    }
  }
}