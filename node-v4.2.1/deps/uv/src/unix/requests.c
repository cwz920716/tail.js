#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "requests.h"
#include "internal.h"

void init_req_queue(requests_t *r) {
  r->head = r->tail = NULL;
}

int empty_req_queue(requests_t *r) {
  return !r->head && !r->tail;
}

void push_req_queue(requests_t *r, item_t *i) {
  if (empty_req_queue(r)) {
    r->head = r->tail = i;
  } else {
    r->tail->next = i;
    r->tail = i;
  }
}

item_t *pop_req_queue(requests_t *r) {
  item_t *ret;

  if (empty_req_queue(r)) {
    return NULL;
  } else if (r->head == r->tail) {
    ret = r->head;
    r->head = r->tail = NULL;
  } else {
    ret = r->head;
    r->head = ret->next;
  }

  return ret;
}

void pushRQ(requests_t *r, void *req, uint64_t io, uint64_t compute, uint64_t wallclock) {
  if (r->cnt < 400) {
    r->cnt++;
    return;
  }

  item_t *item = (item_t *) uv__malloc(sizeof(item_t));
  if (!item)
    return;

  item->req = req;
  item->io = io;
  item->compute = compute;
  item->wallclock = wallclock;
  item->next = NULL;
  push_req_queue(r, item);
}

void *popRQ(requests_t *r, FILE *fp) {
  item_t *item = pop_req_queue(r);
  void *ret;

  if (!item)
    return NULL;

  ret = item->req;
  fprintf(fp, "%ld\t%ld\t%ld\t%f\t%ld\n", (item->compute + item->io), item->compute, item->io, ((double) item->compute * 1.0 / item->io), item->wallclock );
  uv__free(item);
  return ret;
}

void initRQ(requests_t *r) {
  init_req_queue(r);
}

int emptyRQ(requests_t *r) {
  return empty_req_queue(r);
}
