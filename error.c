#include "error.h"
#include "tokenizer.h"
#include "vcc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void error_at(char *cur_code, char *fmt, ...) {
  va_list ag;
  va_start(ag, fmt);
  fprintf(stderr, "%s\n", code);
  fprintf(stderr, "%*s^ ", (int)(cur_code - code), "");
  vfprintf(stderr, fmt, ag);
  fprintf(stderr, "\n");
  va_end(ag);
  exit(1);
}

void report(char *fmt, ...) {
  va_list ag;
  va_start(ag, fmt);
  vfprintf(stderr, fmt, ag);
  fprintf(stderr, "\n");
  va_end(ag);
  exit(1);
}