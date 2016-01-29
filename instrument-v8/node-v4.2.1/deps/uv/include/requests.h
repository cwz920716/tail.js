#ifndef UV_REQUESTS_H_
#define UV_REQUESTS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#define ALEN (40)

typedef struct ArrayType {
  uint64_t index;
  uint64_t a[ALEN];
  struct ArrayType *next;
} array_t;

typedef struct ArrayList {
  array_t head;
  array_t *tail_pos, *head_pos;
  uint64_t len;
  uint64_t max_len;
} ArrayList_t;

array_t *newArray();

void appendArrayList(ArrayList_t *alist, uint64_t v);

int inArrayList(ArrayList_t *alist, uint64_t v);

void initArrayList(ArrayList_t *alist);

void destroyArrayList(ArrayList_t *alist);

void printArrayList(ArrayList_t *alist, FILE *fp);

typedef struct Item {
  void *req;
  uint64_t io;
  uint64_t compute;
  uint64_t wallclock;
  uint64_t wait;
  ArrayList_t events;
  struct Item *next;
} item_t;

typedef struct {
  item_t *head;
  item_t *tail;
  int cnt;
} requests_t;

void pushRQ(requests_t *r, void *req, uint64_t io, uint64_t compute, uint64_t wallclock, uint64_t wait, ArrayList_t *e);

void *popRQ(requests_t *r, FILE *fp, FILE *fp2);

void initRQ(requests_t *r);

int emptyRQ(requests_t *r);

#endif
