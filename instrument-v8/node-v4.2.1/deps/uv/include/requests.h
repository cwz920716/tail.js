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

typedef struct Item {
  void *req;
  uint64_t io;
  uint64_t compute;
  uint64_t wallclock;
  struct Item *next;
} item_t;

typedef struct {
  item_t *head;
  item_t *tail;
  int cnt;
} requests_t;

void pushRQ(requests_t *r, void *req, uint64_t io, uint64_t compute, uint64_t wallclock);

void *popRQ(requests_t *r, FILE *fp);

void initRQ(requests_t *r);

int emptyRQ(requests_t *r);

#endif
