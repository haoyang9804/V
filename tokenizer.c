#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TKOP, TKINTEGER, TKEOF } TokenKind;

typedef struct Token Token;

struct Token {
  int val;
  char op;
  Token *next;
  TokenKind kind;
};

Token *generate_token(Token *cur_token, TokenKind kind) {
  Token *new_token = malloc(sizeof(Token));
  new_token->kind = kind;
  cur_token->next = new_token;
  return new_token;
}

void error(char *fmt, ...) {
  va_list ag;
  va_start(ag, fmt);
  vfprintf(stderr, fmt, ag);
  fprintf(stderr, "\n");
  va_end(ag);
  exit(1);
}

Token *tokenizer(char *code) {
  Token head = {};
  Token *cur_token = &head;
  while (*code) {
    if (isspace(*code)) {
      code++;
      continue;
    } else if (*code == '+' || *code == '-') {
      cur_token = generate_token(cur_token, TKOP);
      cur_token->op = *code;
      code++;
    } else if (isdigit(*code)) {
      cur_token = generate_token(cur_token, TKINTEGER);
      cur_token->val = strtol(code, &code, 10);
    } else
      error("Cannot decode this code: %c\n", *code);
  }
  cur_token = generate_token(cur_token, TKEOF);
  return head.next;
}

bool at_eof(Token *token) { return token->kind == TKEOF; }

char *stringtify(Token *head) {
  char str[1000] = ""; // can only handle 1000 characters
  while (!at_eof(head)) {
    printf("%d\n", head->kind);
    switch (head->kind) {
    case TKINTEGER:
      strcat(str, "[INTEGER]");
      char *int_str =
          malloc(sizeof(char) * 10); // can only handle 10-digit integer
      sprintf(int_str, "%d", head->val);
      strcat(str, int_str);
      break;
    case TKOP:
      strcat(str, "[OP]");
      strncat(str, &head->op, 1);
      break;
    default:
      error("WTF!");
    }
    head = head->next;
  }
  char *ret = str;
  return ret;
}

int main(int argc, char **argv) {
  if (argc != 2)
    error("Oh no..., there are/is %d argument(s)", argc);
  Token *token_head = tokenizer(argv[1]);
  printf("Token stream: %s\n", stringtify(token_head));
}