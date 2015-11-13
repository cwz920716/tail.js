'use strict';

var cstack = [];

function Context(socket, reqId) {
  if (socket._handle === null) {
    console.log('ERROR: _handle is null!');
  }

  this.socket = socket;
  this.reqId = reqId;
}

function createContext(socket, reqId) {
  return new Context(socket, reqId);
}
exports.createContext = createContext;

function enterContext(ctx) {
  cstack.push(ctx);
  if (ctx === null)
    return;
  if (ctx.socket._handle === null) {
    // console.log('SILENCE: _handle is null!');
    return;
  }

  ctx.socket._handle.uv_eventOf(ctx.reqId);
}
exports.enterContext = enterContext;

function exitContext() {
  return cstack.pop();
}
exports.exitContext = exitContext;

function getContext() {
  if (cstack.length == 0) {
    return null;
  }

  return cstack[cstack.length - 1];
}
exports.getContext = getContext;

function wrapMe(cb) {
  if (!cb)
    return cb;

  var ctx = getContext(); // save the current context

  function wrapper() {
    enterContext(ctx); // recover it;
    // console.log('Hi before xxx listener!');

    var out = getContext();
    var i = 0, args, len = arguments.length;
    args = new Array(len);
    for (i = 0; i < len; i++)
      args[i] = arguments[i];
    cb.apply(this, args);
    exitContext();
  }

  return wrapper;
}
exports.wrapMe = wrapMe;
