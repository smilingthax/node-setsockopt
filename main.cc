#include <nan.h>
#include <tcp_wrap.h>
#include <udp_wrap.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#define setsocksize(a,b,c,d,e) setsocksize(a,b,c,(const char*)d,e)
#define errno uv_translate_sys_error(WSAGetLastError())

#else
#include <sys/socket.h>
#include <errno.h>

#endif

enum SockType { BAD_TYPE=-1, kTCP, kUDP };

void setSockOpt(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
  if (info.Length() != 5) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  Nan::MaybeLocal<v8::Object> arg0 = Nan::To<v8::Object>(info[0]);
  if (arg0.IsEmpty()) {
    Nan::ThrowTypeError("Expected object as first argument");
    return;
  }

  v8::Local<v8::Object> handle = arg0.ToLocalChecked();

  uv_os_sock_t sock;
  if (type == kTCP) {
    // Nan::ObjectWrap::Unwrap   vs.  node::ObjectWrap::Unwrap ?
    uv_tcp_t *tcp = node::ObjectWrap::Unwrap<node::TCPWrap>(handle)->UVHandle();
#ifdef _WIN32
    // cf. uv/uv-private/uv-win.h
    sock = tcp->socket;
#else
    // cf. uv/uv-private/uv-unix.h
    sock = tcp->io_watcher.fd;
#endif
  } else if (type == kUDP) {
    uv_udp_t *udp = node::ObjectWrap::Unwrap<node::UDPWrap>(handle)->UVHandle();
#ifdef _WIN32
    sock = udp->socket;
#else
    sock = udp->io_watcher.fd;
#endif
  } else {
    Nan::ThrowTypeError("Unknown type");
    return;
  }

  if ( (!info[1]->IsInt32())||(!info[2]->IsInt32()) ) {
    Nan::ThrowTypeError("Expected integer as second/third argument");
    return;
  }
  int level = info[1]->Int32Value();
  int optname = info[2]->Int32Value();

// TODO... other types than int...
  if (!info[3]->IsInt32()) {
    Nan::ThrowTypeError("Currently supports only integer as fourth argument");
    return;
  }
  int val = info[3]->Int32Value();

  int res = setsockopt(sock, level, optname, &val, sizeof(val));

  if (res) {
    res = errno;
  }
  info.GetReturnValue().Set(Nan::New(res));   // ? .toLocalChecked() ?
}

void init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
{
  v8::Local<v8::FunctionTemplate> tmpl = Nan::New<v8::FunctionTemplate>(setSockOpt);

#define SetIntConst(target, nameval) \
  Nan::SetTemplate(target, #nameval, Nan::New<v8::Integer>(nameval))

  SetIntConst(tmpl, SOL_SOCKET);
  SetIntConst(tmpl, SO_SNDBUF);
  SetIntConst(tmpl, SO_RCVBUF);

  SetIntConst(tmpl, kTCP);
  SetIntConst(tmpl, kUDP);
#undef SetIntConst

  v8::Local<v8::Function> fn = Nan::GetFunction(tmpl).ToLocalChecked();
  fn->SetName(Nan::New("setsockopt").ToLocalChecked());

  Nan::Set(module,
           Nan::New<v8::String>("exports").ToLocalChecked(),
           fn);
}

NODE_MODULE(setsockopt, init)

