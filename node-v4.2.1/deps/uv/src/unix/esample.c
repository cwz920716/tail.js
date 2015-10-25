#include "internal.h"
#include "esample.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int ratio(uint64_t iq, uint64_t ex) {
  int r = ((double )iq / ex) * PRECISION;
  if (r >= BUCKETS)
    r = BUCKETS;
  return r;
}

/*
sample_ptr sample_new(uint64_t iq, uint64_t ex) {
  void *ptr = uv__malloc(sizeof(sample_t));
  if (!ptr) {
    printf("ERROR: newSample alloc failed!\n");
    abort();
  }

  ptr->inqueue = iq;
  ptr->exec = ex;
  ptr->w = ratio(iq, ex);

  QUEUE_INIT(q);
}
*/

static void sample_set_min(sample_ptr sp) {
  sp->inqueue = 0;
  sp->exec = 0;
  sp->w = 0;
}

static void sample_set_max(sample_ptr sp) {
  sp->inqueue = -1;
  sp->exec = -1;
  sp->w = -1;
}

static sample_table_t st;
static sample_table_ptr stp = &st;

void st_init(void) {
  int i = 0;
  stp->total_iq = stp->total_ex = stp->n = 0;

  for (i = 0; i < N_BUCKETS; i++) {
    /* QUEUE_INIT(stp->buckets[i]); */
    stp->n_[i] = 0;
    stp->total_iq_[i] = 0;
    stp->total_ex_[i] = 0;
    sample_set_max(&stp->max_[i]);
    sample_set_min(&stp->min_[i]);
  }
}

void st_add_sample(uint64_t iq, uint64_t ex) {
  sample_t s;
  int i;

  s.inqueue = iq;
  s.exec = ex;
  i = s.w = ratio(iq, ex);

  stp->n++;
  stp->total_iq += iq;
  stp->total_ex += ex;

  stp->n_[i]++;
  stp->total_iq_[i] += iq;
  stp->total_ex_[i] += ex;

  if (s.w > stp->max_[i].w) {
    stp->max_[i] = s;
  }

  if (s.w < stp->min_[i].w) {
    stp->min_[i] = s;
  }
}

void st_dump(void) {
  int i;

  printf("n=%ld\n", stp->n);
  for (i = 0; i < N_BUCKETS; i++) {
    printf("%d/%d-th bucket: n_=%ld\n",
             i, PRECISION, stp->n_[i]
          );
  }
}

void st_get_percentile(uint64_t permile) {
  uint64_t k = (uint64_t) (stp->n * (double) (permile) / 1000);

  uint64_t prev = 0, left, right;
  int i;

  if (k < 1) k = 1;
  for (i = 0; i < N_BUCKETS; i++) {
    left = prev + 1;
    right = prev + stp->n_[i];
    if (k >= left && k <= right) {
      printf("%ld/1000 -th percentile: inprecise iq/ex=%d/%d, approx_iq=%ld, approx_ex=%ld, n_=%ld\n",
             permile, i, PRECISION, (stp->total_iq_[i] / stp->n_[i]), (stp->total_ex_[i] / stp->n_[i]), stp->n_[i]
            );
      return;
    }
    prev = right;
  }

  printf("ERROR: get percentile have boundary bugs!\n");  
}
