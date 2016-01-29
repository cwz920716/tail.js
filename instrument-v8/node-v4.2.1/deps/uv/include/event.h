#ifndef UV_EVENT_H_
#define UV_EVENT_H_

typedef struct LoopInfo {
  uint64_t id;
  uint64_t ts;
} LoopInfo_t;

typedef struct EventInfo {
  LoopInfo_t loop;
  uint64_t active;
  uint64_t id;
  uint64_t start_ts, end_ts;
  uint64_t wait_ts, exec_ts;
} EventInfo_t;

extern EventInfo_t currentEvent;

void LoopBarrier(void);
void EventBegins(EventInfo_t *event);
void EventEnds(EventInfo_t *event);

#endif
