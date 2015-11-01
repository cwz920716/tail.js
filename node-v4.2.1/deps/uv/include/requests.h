#ifndef UV_REQUESTS_H_
#define UV_REQUESTS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct Item {
  void *req;
  struct Item *next;
} item_t;

typedef struct {
  item_t *head;
  item_t *tail;
} requests_t;

void pushRQ(requests_t *r, void *req);

void *popRQ(requests_t *r);

void initRQ(requests_t *r);

#endif
