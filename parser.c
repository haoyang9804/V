#include "parser.h"
#include "error.h"
#include "vcc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Node *root;
int node_num = 0;

static bool _at_end() { return token->kind == TK_EOF; }

static int _at_end_k(int k) {
  Token *token_copy = token;
  while (k--) {
    token_copy = token_copy->next;
    if (_at_end(token_copy))
      return true;
  }
  return false;
}

// check the current token

static bool _is_op() { return token->kind == TK_PUNC; }

static bool _is_plus() {
  return _is_op() && token->code[0] == '+';
}

static bool _is_minus() {
  return _is_op() && token->code[0] == '-';
}

static bool _is_integer() { return token->kind == TK_INTEGER; }

static bool _is_open_bracket() {
  return _is_op(token) && token->code[0] == '(';
}

static bool _is_closed_bracket() {
  return _is_op(token) && token->code[0] == ')';
}

static void _expect_closed_bracket() {
  if (!_is_closed_bracket(token)) {
    error_at(token->code, "Expect )");
  }
}

static void _expect_number() {
  if (!_is_integer(token)) {
    error_at(token->code, "Expect a number");
  }
}

static void _expect_not_at_end() {
  if (_at_end(token)) {
    report("Should not be at end");
  }
}

// consumes a token

static void _eat() { 
  Token *token_tmp = token;
  token = token->next; 
  free(token_tmp);
}

/*! \brief look ahead the next k tokens, check if the tokens can be concatenated
 * into c*/

static bool lookahead(int k, char *c) {
  int siz = 0;
  Token *tmp_token = token;
  for (int i = 0; i < k; i++) {
    siz += tmp_token->len;
    tmp_token = tmp_token->next;
    if (_at_end(token)) return false;
  }
  char text[siz + 1];
  int len = 0;
  tmp_token = token;
  for (int i = 0; i < k; i++) {
    strncpy(text + len, tmp_token->code, tmp_token->len);
    len += tmp_token->len;
    tmp_token = tmp_token->next;
  }
  text[siz] = '\0';
  if (!_at_end_k(k) && strncmp(text, c, siz) == 0) {
    while (k--)
      _eat();
    return true;
  }
  return false;
}

// AST creation

static Node *_new_number_node(int val) {
  Node *node = malloc(sizeof(Node));
  node->content.val = val;
  node->kind = ND_NUM;
  node->left = NULL;
  node->right = NULL;
  node_num++;
  return node;
}

static Node *_new_binary(NodeKind kind, Node *left, Node *right) {
  Node *root = malloc(sizeof(Node));
  root->kind = kind;
  switch (kind) {
  case ND_ADD:
    root->content.op = "+";
    break;
  case ND_SUB:
    root->content.op = "-";
    break;
  case ND_DIV:
    root->content.op = "/";
    break;
  case ND_MUL:
    root->content.op = "*";
    break;
  case ND_EQ:
    root->content.op = "==";
    break;
  case ND_NE:
    root->content.op = "!=";
    break;
  case ND_GE:
    root->content.op = ">=";
    break;
  case ND_GT:
    root->content.op = ">";
    break;
  case ND_LE:
    root->content.op = "<=";
    break;
  case ND_LT:
    root->content.op = "<";
    break;
  default:
    break;
  }
  root->left = left;
  root->right = right;
  node_num++;
  return root;
}


// Automata

static Node *_ADD_SUB();
static Node *_NUMBER_BRACKET();
static Node *_MUL_DIV();
static Node *_UNARY();
static Node *_COMPARE1();
static Node *_COMPARE2();

static Node *_NUMBER_BRACKET() {
  Node *node;
  if (_is_open_bracket()) {
    _eat(); // eat the open bracket
    node = _ADD_SUB();
    // check for the existence of the closed bracket
    _expect_closed_bracket();
    _eat(); // eat the closed bracket
  } else {
    _expect_number();
    node = _new_number_node(token->val);
    _eat(); // eat the number token
  }
  return node;
}



static Node *_UNARY() {
  if (_is_plus()) {
    _eat(); // eat the +
    return _UNARY();
  }
  if (_is_minus()) {
    _eat(); // eat the -
    return _new_binary(ND_SUB, _new_number_node(0), _UNARY());
  }
  return _NUMBER_BRACKET();
  
}

static Node *_MUL_DIV() {
  Node *node = _UNARY();
  while (!_at_end()) {
    if (lookahead(1, "*")) {
      node = _new_binary(ND_MUL, node, _UNARY());
    } else if (lookahead(1, "/")) {
      node = _new_binary(ND_DIV, node, _UNARY());
    } else {
      break;
    }
  }
  return node;
}

static Node *_ADD_SUB() {
  Node *node = _MUL_DIV();
  while (!_at_end()) {
    if (lookahead(1, "+")) {
      node = _new_binary(ND_ADD, node, _MUL_DIV());
    } else if (lookahead(1, "-")) {
      node = _new_binary(ND_SUB, node, _MUL_DIV());
    } else {
      break;
    }
  }
  return node;
}



static Node *_COMPARE2() {
  Node *node = _ADD_SUB();
  while (!_at_end()) {
    if (lookahead(2, "<=")) {
      node = _new_binary(ND_LE, node, _ADD_SUB());
    }
    else if (lookahead(2, ">=")) {
      node = _new_binary(ND_GE, node, _ADD_SUB());
    }
    else if (lookahead(1, "<")) {
      node = _new_binary(ND_LT, node, _ADD_SUB());
    }
    else if (lookahead(1, ">")) {
      node = _new_binary(ND_GT, node, _ADD_SUB());
    }
    else {
      break;
    }
  }
  return node;
}

static Node *_COMPARE1() {
  Node *node = _COMPARE2();
  while (!_at_end()) {
    if (lookahead(2, "==")) {
      node = _new_binary(ND_EQ, node, _COMPARE2());
    }
    else if (lookahead(2, "!=")) {
      node = _new_binary(ND_NE, node, _COMPARE2());
    }
    else {
      break;
    }
  }
  return node;
}

void AST() { root = _COMPARE1(); }