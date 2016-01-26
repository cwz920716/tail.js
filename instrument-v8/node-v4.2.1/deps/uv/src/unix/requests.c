#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "requests.h"
#include "internal.h"

array_t *newArray() {
  array_t *v = (array_t *) malloc(sizeof(array_t));
  if (v == NULL)
    return NULL;

  v->next = NULL;
  return v;
}

void appendArrayList(ArrayList_t *alist, uint64_t v) {
  array_t *newa = NULL;

  if (alist->len >= alist->max_len) {
    // newa = newArray();
    if (newa == NULL)
      return;
    newa->index = alist->tail_pos->index + 1;
    alist->tail_pos->next = newa;
    alist->tail_pos = newa;
    alist->max_len += ALEN;
  }
  if (alist->len < alist->max_len) {
    // printf("append %d\n", v);
    alist->tail_pos->a[alist->len % ALEN] = v;
    alist->len++;
  }
}

int inArrayList(ArrayList_t *alist, uint64_t v) {
  uint64_t max_v = 0;
  if (alist->len == 0)
    return 0;
  max_v = alist->tail_pos->a[(alist->len - 1) % ALEN];
  return (v <= max_v);
}

void initArrayList(ArrayList_t *alist) {
  alist->len = 0;
  alist->max_len = ALEN;
  alist->tail_pos = alist->head_pos = &alist->head;
  alist->head_pos->next = NULL;
  alist->head_pos->index = 0;
}

void destroyArrayList(ArrayList_t *alist) {
  array_t *cur, *next;
  cur = alist->head_pos->next;
  while (cur != NULL) {
    next = cur->next;
    free(cur);
    cur = next;
  }
  initArrayList(alist);
}

void cloneArrayList(ArrayList_t *src, ArrayList_t *dst) {
  initArrayList(dst);
  dst->max_len = src->max_len;
  dst->len = src->len;
  dst->head = src->head;
}

char *colors[] = {"blue", "red", "black", "purple", "orange"};
int color_len = 5, next_color = 0;

void printArrayList(ArrayList_t *alist, FILE *fp) {
  uint64_t next = 0;
  array_t *cur = alist->head_pos;

  // if (alist->len <= 1) return;

  while (next < alist->len && cur != NULL) {
    fprintf(fp, "%ld", cur->a[next % ALEN]);
    next++;
    if (next < alist->len)
      fprintf(fp, "->");
    if ((next / 20) > cur->index) {
      cur = cur->next;
    }
  }
  fprintf(fp, "[color=%s, penwidth=2]\n", colors[next_color++ % color_len]);
  return;
}

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

void pushRQ(requests_t *r, void *req, uint64_t io, uint64_t compute, uint64_t wallclock, uint64_t wait, ArrayList_t *e) {
  if (r->cnt < 0) {
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
  item->wait = wait;
  if (e != NULL) {
    cloneArrayList(e, &item->events);
  } else
    initArrayList(&item->events);
  item->next = NULL;
  push_req_queue(r, item);
}

double NZDivison(double a, double b) {
  if (b != 0)
    return (a / b);
  else
    return 0;
}

void *popRQ(requests_t *r, FILE *fp, FILE *fp2) {
  item_t *item = pop_req_queue(r);
  void *ret;

  if (!item)
    return NULL;

  ret = item->req;
  if (fp)
    fprintf(fp, "%ld\t%ld\t%ld\t%f\t%f\t%ld\n", (item->compute + item->io), item->compute, item->io, 
           NZDivison(item->compute * 1.0, item->io * 1.0), NZDivison(item->wait * 1.0, item->compute * 1.0), item->wait );
  if (fp2) {
    // printArrayList(&item->events, stdout);
    printArrayList(&item->events, fp2);
  }
  destroyArrayList(&item->events);
  uv__free(item);
  return ret;
}

void initRQ(requests_t *r) {
  init_req_queue(r);
}

int emptyRQ(requests_t *r) {
  return empty_req_queue(r);
}
