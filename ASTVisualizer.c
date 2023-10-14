#include "ASTVisualizer.h"
#include "vcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp;
static int nodeID = 0;

static void _write_a_line(char *str) { fprintf(fp, "%s\n", str); }

static void _write_a_seg(char *str) { fprintf(fp, "%s ", str); }

static void _write(char *str) { fprintf(fp, "%s", str); }

static char *_get_node_content(Node *node) {
  if (node->kind == ND_NUM) {
    // support integer now, so the length is 10
    char *snum = calloc(sizeof(char), 10);
    sprintf(snum, "%d", node->content.val);
    return snum;
  }
  char *snum = calloc(sizeof(char), 10);
  strcat(snum, "\"");
  strcat(snum, node->content.op);
  strcat(snum, "\"");
  return snum;
}

static char *_node_var_name(Node *node) {
  // Suppose the number of AST node will not be over 1e10-1;
  char *node_str = calloc(sizeof(char), 16);
  strcpy(node_str, "node");
  char nodeID_str[10];
  sprintf(nodeID_str, "%d", nodeID++);
  strncpy(node_str + 4, nodeID_str, strlen(nodeID_str) + 1);
  return node_str;
}

static char *_graphviz_arrow_code(char *name_from, char *name_to) {
  _write_a_seg(name_from);
  _write_a_seg("->");
  _write(name_to);
  _write_a_line(";");
}

static char *_graphviz_declare_code(Node *node, char *name_str) {
  _write_a_seg(name_str);
  _write("[label=");
  _write(_get_node_content(node));
  _write_a_line("];");
}

static void _traverse_AST(Node *node, char *node_name) {
  _graphviz_declare_code(node, node_name);
  if (node->left) {
    char *left_node_name = _node_var_name(node->left);
    _graphviz_declare_code(node->left, left_node_name);
    _graphviz_arrow_code(node_name, left_node_name);
    _traverse_AST(node->left, left_node_name);
  }
  if (node->right) {
    char *right_node_name = _node_var_name(node->right);
    _graphviz_declare_code(node->right, right_node_name);
    _graphviz_arrow_code(node_name, right_node_name);
    _traverse_AST(node->right, right_node_name);
  }
}

void visualize() {
  fp = fopen("AST.dot", "w");
  _write_a_line("digraph G {");
  Node *node = root;
  _traverse_AST(node, _node_var_name(node));
  _write_a_line("}");
  fclose(fp);
  system("dot -Tsvg AST.dot > AST.svg");
}