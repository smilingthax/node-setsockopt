#!/usr/bin/nodejs

var setsockopt=require('./index.js');

//console.log(setsockopt);

var sock=require('net').connect(80); // ,'127.0.0.1');
sock.on('error', console.log.bind(console));

sock.on('connect', function() {
  //console.log(sock);
  console.log(setsockopt(sock,setsockopt.SOL_SOCKET,setsockopt.SO_RCVBUF,500));
  sock.end();
});

