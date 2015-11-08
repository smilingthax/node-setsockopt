var UDP=process.binding('udp_wrap').UDP,
    TCP=process.binding('tcp_wrap').TCP;

var sso=require('./build/Release/setsockopt.node');

// cf. child_process.js, getHandleWrapType()
module.exports=function(sock, level, optname, optval) {
  var handle=sock._handle, type;
  if (handle instanceof TCP) {
    type=sso.kTCP;
  } else if (handle instanceof UDP) {
    type=sso.kUDP;
  } else {
    throw new TypeError('Unrecognized socket type');
  }
  return sso(handle, level, optname, optval, type);
};

for (var k in sso) {
  if (k[0]==='k') continue;
  module.exports[k]=sso[k];
}

