#ifndef UV_ESAMPLE_H_
#define UV_ESAMPLE_H_

#include "internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PRECISION (100)
#define BUCKETS (1000 * PRECISION)
#define N_BUCKETS (BUCKETS + 1)

struct Sample {
  uint64_t inqueue;
  uint64_t exec;
  uint64_t w;
  /* QUEUE q; */
};
typedef struct Sample sample_t;
typedef sample_t *sample_ptr;

struct SampleTable {
  uint64_t total_iq;
  uint64_t total_ex;
  uint64_t n;
  /* QUEUE buckets[N_BUCKETS]; */
  uint64_t n_[N_BUCKETS];
  sample_t max_[N_BUCKETS];
  sample_t min_[N_BUCKETS];
  uint64_t total_iq_[N_BUCKETS];
  uint64_t total_ex_[N_BUCKETS];
};
typedef struct SampleTable sample_table_t;
typedef sample_table_t *sample_table_ptr;

void st_init(void);

void st_add_sample(uint64_t iq, uint64_t ex);

void st_dump(void);

void st_get_percentile(uint64_t permile);

#endif
