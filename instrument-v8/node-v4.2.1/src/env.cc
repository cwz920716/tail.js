#include "env.h"
#include "env-inl.h"
#include "v8.h"
#include <stdio.h>

#include <iostream>
#include <fstream>

namespace node {

using v8::HandleScope;
using v8::Local;
using v8::Message;
using v8::StackFrame;
using v8::StackTrace;
using v8::TryCatch;

namespace instrument {

stack<Context> ContextStack::cstack;
using namespace std;
// ofstream out("/tmp/pedg.dot");

void createFunHandler(FunctionWrap *wrap) {
  bool in = ContextStack::inContext();
  Context ctx = ContextStack::getContext();
  if (in) {
    wrap->setBoolFields(0, true);
    wrap->setPointerFields(0, ctx._handle);
    wrap->setIntFields(0, ctx._reqId);
    wrap->setUInt64Fields(0, uv_eventId());
  } else
    wrap->setBoolFields(0, false);
}

void beforeCallHandler(FunctionWrap *wrap) {
  if (wrap->getBoolFields(0)) {
    uv_stream_t * handle = reinterpret_cast<uv_stream_t *> (wrap->getPointerFields(0));
    int reqId = wrap->getIntFields(0);
    Context ctx = {handle, reqId};
    ContextStack::enterContext(ctx);

    uint64_t cur = uv_eventId();
    uint64_t create = wrap->getUInt64Fields(0);
    if (cur != create) {
      // out << create << "->" << cur << ";" << endl;
    }
  }
}

void afterCallHandler(FunctionWrap *wrap) {
  if (wrap->getBoolFields(0)) {
    ContextStack::exitContext();
  }
}

} // instrument

void Environment::PrintSyncTrace() const {
  if (!trace_sync_io_)
    return;

  HandleScope handle_scope(isolate());
  Local<v8::StackTrace> stack =
      StackTrace::CurrentStackTrace(isolate(), 10, StackTrace::kDetailed);

  fprintf(stderr, "WARNING: Detected use of sync API\n");

  for (int i = 0; i < stack->GetFrameCount() - 1; i++) {
    Local<StackFrame> stack_frame = stack->GetFrame(i);
    node::Utf8Value fn_name_s(isolate(), stack_frame->GetFunctionName());
    node::Utf8Value script_name(isolate(), stack_frame->GetScriptName());
    const int line_number = stack_frame->GetLineNumber();
    const int column = stack_frame->GetColumn();

    if (stack_frame->IsEval()) {
      if (stack_frame->GetScriptId() == Message::kNoScriptIdInfo) {
        fprintf(stderr, "    at [eval]:%i:%i\n", line_number, column);
      } else {
        fprintf(stderr,
                "    at [eval] (%s:%i:%i)\n",
                *script_name,
                line_number,
                column);
      }
      break;
    }

    if (fn_name_s.length() == 0) {
      fprintf(stderr, "    at %s:%i:%i\n", *script_name, line_number, column);
    } else {
      fprintf(stderr,
              "    at %s (%s:%i:%i)\n",
              *fn_name_s,
              *script_name,
              line_number,
              column);
    }
  }
  fflush(stderr);
}


bool Environment::KickNextTick() {
  TickInfo* info = tick_info();

  if (info->in_tick()) {
    return true;
  }

  if (info->length() == 0) {
    isolate()->RunMicrotasks();
  }

  if (info->length() == 0) {
    info->set_index(0);
    return true;
  }

  info->set_in_tick(true);

  // process nextTicks after call
  TryCatch try_catch;
  try_catch.SetVerbose(true);
  tick_callback_function()->Call(process_object(), 0, nullptr);

  info->set_in_tick(false);

  if (try_catch.HasCaught()) {
    info->set_last_threw(true);
    return false;
  }

  return true;
}

}  // namespace node
