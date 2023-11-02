#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "memory.h"

static block_ptr first_block = NULL, last_block = NULL;

// size: size of data
block_ptr find_block(size_t size) {
  block_ptr p = first_block;
  while (p) {
    if (!(p->free && p->size >= size)) {
      p = p->next;
    }
  }
  return p;
}

block_ptr expand_heap(size_t size) {
  block_ptr boundary = sbrk(0);
  if (sbrk(BLOCK_SIZE + size) == (void*)-1)
    return NULL;
  boundary->size = size;
  boundary->next = NULL;
  boundary->pre = NULL;
  boundary->free = 0;
  if (last_block) {
    last_block->next = boundary;
    boundary->pre = last_block;
  }
  last_block = boundary;
  return boundary;
}

void split(block_ptr p, size_t size) {
  assert(size + BLOCK_SIZE < p->size);
  block_ptr newp = (block_ptr)(p->data + size);
  newp->size = p->size - size - BLOCK_SIZE;
  newp->next = p->next;
  newp->pre = p;
  newp->next->pre = p;
  p->next = newp;
  newp->free = 1;
}

size_t align8(size_t size) {
  if (size & 0x07 == 0) return size;
  return ((size >> 3) + 1) << 3;
}

block_ptr get_block(void *p) {
  char *pcp = p;
  pcp -= BLOCK_SIZE;
  p = pcp;
  return p;
}

int8_t valid(void *p) {
  if (first_block && p > (void*)first_block && p < sbrk(0))
    return p == (get_block(p)->data);
  return 0;
}

block_ptr fuse(block_ptr p);

void vfree(void *p) {
  if (valid(p)) {
    block_ptr bp = get_block(p);
    bp->free = 1;
    if (bp->pre && bp->pre->free) bp = fuse(bp->pre);
    if (bp->next) {
      if (bp->next->free) bp = fuse(bp);
    }
    else {
      brk(bp);
    }
  }
}

// fuse p and p->next
block_ptr fuse(block_ptr p) {
  assert(p->free);
  assert(p->next);
  assert(p->next->free);
  block_ptr nextp = p->next;
  p->next = nextp->next;
  p->size += nextp->size + BLOCK_SIZE;
  vfree(nextp);
  return p;
}

void *valloc(size_t size) {
  size_t aligned_size = align8(size);
  block_ptr p = find_block(aligned_size);
  if (!p) {
    block_ptr newp = expand_heap(aligned_size);
    if (!newp) return NULL;
    return newp->data;
  }
  
  if (p->size > aligned_size + BLOCK_SIZE) split(p, size);
  return p->data;
}
