/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "tree.h"
#include "internal.h"
#include "heap-inl.h"
#include "esample.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <cpufreq.h>

static void uv_pthread_setaffinity (void) {
  int s, j = 3;
  cpu_set_t cpuset; 
  pthread_t thread;

  thread = pthread_self();
  CPU_ZERO(&cpuset);
  CPU_SET(j, &cpuset);
  s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  if (s != 0) {
    printf("ERROR: uv_pthread_setaffinity failed!\n");
  }

  return;
}

static void uv_sched_setaffinity (void) {
  int s, j = 0;
  cpu_set_t cpuset; 

  CPU_ZERO(&cpuset);
  CPU_SET(j, &cpuset);
  s = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
  if (s != 0) {
    printf("ERROR: uv_sched_setaffinity failed!\n");
  }

  return;
}

extern uint64_t prog_start;
extern uint64_t flex_mode, freq, threshold;
void uv_set_cpufreq(void) {
  int s = 0, cpu = 0, skip = 0;
  uint64_t cpu_cur = 0;
  long cpu_togo = 0;
  char *freq_str = NULL;

  freq_str = getenv("NODE_CPU0_FREQ");
  /* printf("%s ", freq_str); */
  if (freq_str) {
    cpu_togo = atol(freq_str);
    flex_mode = 0;
    if (cpu_togo == 0) {
      cpu_togo = 1200000;
      flex_mode = 1;
      freq = 0;
      threshold = atoi(getenv("NODE_QUEUE_THRESHOLD"));
      /* printf("FLEX MODE: threshold = %lu\n", threshold); */
    } else if (cpu_togo < 0) {
      skip = 1;
      /* printf("SKIP MODE "); */
    }
  } else
    cpu_togo = 3200000;

  if (!skip)
    s = cpufreq_set_frequency(cpu, cpu_togo);
  if (s != 0) {
    printf("ERROR: uv_set_cpufreq failed!\n");
  }
  cpu_cur = cpufreq_get_freq_kernel(cpu);
  prog_start = uv__cputime();
  printf("Current CPU Frequency: %luKHz\n", cpu_cur);
  
}

int uv_loop_init(uv_loop_t* loop) {
  int err;

  uv__signal_global_once_init();

  memset(loop, 0, sizeof(*loop));
  heap_init((struct heap*) &loop->timer_heap);
  QUEUE_INIT(&loop->wq);
  QUEUE_INIT(&loop->active_reqs);
  QUEUE_INIT(&loop->idle_handles);
  QUEUE_INIT(&loop->async_handles);
  QUEUE_INIT(&loop->check_handles);
  QUEUE_INIT(&loop->prepare_handles);
  QUEUE_INIT(&loop->handle_queue);

  loop->nfds = 0;
  loop->watchers = NULL;
  loop->nwatchers = 0;
  QUEUE_INIT(&loop->pending_queue);
  QUEUE_INIT(&loop->watcher_queue);

  loop->closing_handles = NULL;
  uv__update_time(loop);
  uv__async_init(&loop->async_watcher);
  loop->signal_pipefd[0] = -1;
  loop->signal_pipefd[1] = -1;
  loop->backend_fd = -1;
  loop->emfile_fd = -1;

  loop->timer_counter = 0;
  loop->stop_flag = 0;

  err = uv__platform_loop_init(loop);
  if (err)
    return err;

  err = uv_signal_init(loop, &loop->child_watcher);
  if (err)
    goto fail_signal_init;

  uv__handle_unref(&loop->child_watcher);
  loop->child_watcher.flags |= UV__HANDLE_INTERNAL;
  QUEUE_INIT(&loop->process_handles);

  err = uv_rwlock_init(&loop->cloexec_lock);
  if (err)
    goto fail_rwlock_init;

  err = uv_mutex_init(&loop->wq_mutex);
  if (err)
    goto fail_mutex_init;

  err = uv_async_init(loop, &loop->wq_async, uv__work_done);
  if (err)
    goto fail_async_init;

  uv__handle_unref(&loop->wq_async);
  loop->wq_async.flags |= UV__HANDLE_INTERNAL;

  st_init();
  uv_pthread_setaffinity();
  /* uv_set_cpufreq(); */
  return 0;

fail_async_init:
  uv_mutex_destroy(&loop->wq_mutex);

fail_mutex_init:
  uv_rwlock_destroy(&loop->cloexec_lock);

fail_rwlock_init:
  uv__signal_loop_cleanup(loop);

fail_signal_init:
  uv__platform_loop_delete(loop);

  return err;
}


void uv__loop_close(uv_loop_t* loop) {
  uv__signal_loop_cleanup(loop);
  uv__platform_loop_delete(loop);
  uv__async_stop(loop, &loop->async_watcher);

  if (loop->emfile_fd != -1) {
    uv__close(loop->emfile_fd);
    loop->emfile_fd = -1;
  }

  if (loop->backend_fd != -1) {
    uv__close(loop->backend_fd);
    loop->backend_fd = -1;
  }

  uv_mutex_lock(&loop->wq_mutex);
  assert(QUEUE_EMPTY(&loop->wq) && "thread pool work queue not empty!");
  assert(!uv__has_active_reqs(loop));
  uv_mutex_unlock(&loop->wq_mutex);
  uv_mutex_destroy(&loop->wq_mutex);

  /*
   * Note that all thread pool stuff is finished at this point and
   * it is safe to just destroy rw lock
   */
  uv_rwlock_destroy(&loop->cloexec_lock);

#if 0
  assert(QUEUE_EMPTY(&loop->pending_queue));
  assert(QUEUE_EMPTY(&loop->watcher_queue));
  assert(loop->nfds == 0);
#endif

  uv__free(loop->watchers);
  loop->watchers = NULL;
  loop->nwatchers = 0;
}


int uv__loop_configure(uv_loop_t* loop, uv_loop_option option, va_list ap) {
  if (option != UV_LOOP_BLOCK_SIGNAL)
    return UV_ENOSYS;

  if (va_arg(ap, int) != SIGPROF)
    return UV_EINVAL;

  loop->flags |= UV_LOOP_BLOCK_SIGPROF;
  return 0;
}
