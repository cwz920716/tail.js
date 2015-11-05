'use strict';

var context = null;
function setContext(request) {
  context = request;
}
exports.setContext = setContext;

function resetContext() {
  context = null;
}
exports.resetContext = resetContext;

function getContext() {
  return context;
}
exports.getContext = getContext;

function wrapMe(cb, recovery) {
  if (!cb)
    return cb;

  var ctx = getContext(); // save the current context

  function wrapper() {
    if (!recovery) {
      ctx = getContext();
    }
    
    setContext(ctx); // recover it;
    console.log('Hi before xxx listener!');

    var out = getContext();
    var i = 0, args, len = arguments.length;
    args = new Array(len);
    for (i = 0; i < len; i++)
      args[i] = arguments[i];
    cb.apply(this, args);
    console.log('------');
    resetContext();
  }

  return wrapper;
}
exports.wrapMe = wrapMe;
