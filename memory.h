#pragma once


#define BLOCK_SIZE 24

typedef struct block *block_ptr;

struct block {
  size_t size; // the size of data, not including BLOCK_SIZE
  int8_t free;
  block_ptr next;
  block_ptr pre;
  char data[1];
};

void vfree(void *p);

void *valloc(size_t size);

