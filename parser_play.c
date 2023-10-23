#include "parser_play.h"
#include "error.h"
#include "vcc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Node_ *root_;

static bool _at_end() { return token_->kind == TK_EOF; }

static int _at_end_k(int k) {
  Token *token_copy = token_;
  while (k--) {
    token_copy = token_copy->next;
    if (_at_end(token_copy))
      return true;
  }
  return false;
}

// check the current token_

static bool _is_op() { return token_->kind == TK_PUNC; }

static bool _is_plus() {
  return _is_op() && token_->code[0] == '+';
}

static bool _is_minus() {
  return _is_op() && token_->code[0] == '-';
}

static bool _is_integer() { return token_->kind == TK_INTEGER; }

static bool _is_open_bracket() {
  return _is_op(token_) && token_->code[0] == '(';
}

static bool _is_closed_bracket() {
  return _is_op(token_) && token_->code[0] == ')';
}

static void _expect_closed_bracket() {
  if (!_is_closed_bracket(token_)) {
    error_at(token_->code, "Expect )");
  }
}

static void _expect_number() {
  if (!_is_integer(token_)) {
    error_at(token_->code, "Expect a number");
  }
}

static void _expect_not_at_end() {
  if (_at_end(token_)) {
    report("Should not be at end_");
  }
}

// consumes a token_

static void _eat() {
  // Token *token_tmp = token_;
  // free(token_tmp);
  token_ = token_->next; 
}

static void _eat_op() {
  if (!_is_op()) {
    error_at(token_->code, "Expect an operator");
  }
  _eat();
}

static void _eat_integer() {
  if (!_is_integer()) {
    error_at(token_->code, "Expect an integer");
  }
  _eat();
}

/*! \brief look ahead the next k tokens, check if the tokens can be concatenated
 * into c*/

static bool lookahead(int k, char *c) {
  int siz = 0;
  Token *tmp_token = token_;
  for (int i = 0; i < k; i++) {
    siz += tmp_token->len;
    tmp_token = tmp_token->next;
    if (_at_end(token_)) return false;
  }
  char text[siz + 1];
  int len = 0;
  tmp_token = token_;
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

static Node_ *_new_number_node(int val) {
  Node_ *node = malloc(sizeof(Node_));
  node->content.val = val;
  node->kind = ND_NUM_;
  node->left = NULL;
  node->right = NULL;
  node_num++;
  return node;
}

static Node_ *_new_binary(NodeKind_ kind, Node_ *left, Node_ *right) {
  Node_ *root_ = malloc(sizeof(Node_));
  root_->kind = kind;
  switch (kind) {
  case ND_ADD_:
    root_->content.op = "+";
    break;
  case ND_SUB_:
    root_->content.op = "-";
    break;
  case ND_DIV_:
    root_->content.op = "/";
    break;
  case ND_MUL_:
    root_->content.op = "*";
    break;
  default:
    break;
  }
  root_->left = left;
  root_->right = right;
  node_num++;
  return root_;
}


// Automata
static Node_ *_add_or_sub_(Node_ *left);
static Node_ *_add_or_sub();
static Node_ *_num_or_bracket();
static Node_ *_mul_or_div_(Node_ *left);
static Node_ *_mul_or_div();
static Node_ *_unary();

static Node_ *_num_or_bracket() {
  Node_ *node;
  if (_is_open_bracket()) {
    _eat(); // eat the open bracket
    node = _add_or_sub();
    // check for the existence of the closed bracket
    _expect_closed_bracket();
    _eat(); // eat the closed bracket
  } else {
    _expect_number();
    node = _new_number_node(token_->val);
    _eat(); // eat the number token_
  }
  return node;
}



static Node_ *_unary() {
  if (_is_plus()) {
    _eat(); // eat the +
    return _unary();
  }
  if (_is_minus()) {
    _eat(); // eat the -
    return _new_binary(ND_SUB_, _new_number_node(0), _unary());
  }
  return _num_or_bracket();
  
}

static Node_ *_mul_or_div() {
  return _mul_or_div_(_unary());
}

static Node_ *_mul_or_div_(Node_ *left) {
  if (lookahead(1, "*")) {
    return _mul_or_div_(_new_binary(ND_MUL_, left, _unary()));
  } 
  if (lookahead(1, "/")) {
    return _mul_or_div_(_new_binary(ND_DIV_, left, _unary()));
  } 
  return left;
}

static Node_ *_add_or_sub_(Node_ *left) {
  if (lookahead(1, "+")) {
    return _add_or_sub_(_new_binary(ND_ADD_, left, _mul_or_div()));
  }
  if (lookahead(1, "-")) {
    return _add_or_sub_(_new_binary(ND_SUB_, left, _mul_or_div()));
  } 
  return left;
}

static Node_ *_add_or_sub() {
  return _add_or_sub_(_mul_or_div());
}

void AST2() { root_ = _add_or_sub(); }