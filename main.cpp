#include <iostream>
#include <string>

#include <event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include <event2/http_struct.h>
#include <event2/buffer_compat.h>

using namespace std;

void httpdNotification(struct evhttp_request *req, void *arg)
{
  struct evbuffer *evb = evbuffer_new();
  evbuffer_add_printf(evb, "{\"err_no\":0,\"err_msg\":\"notify success\"}");
  evhttp_send_reply(req, HTTP_OK, "OK", evb);
  evbuffer_free(evb);

  const string postData = string((char *)EVBUFFER_DATA(req->input_buffer), EVBUFFER_LENGTH(req->input_buffer));
  cout << "makeRawGwMsg for notify:" << postData << endl;

}

void runHttpd( string host, int port )
{
   cout << "-----runHttpd------" <<endl;

  struct evhttp_bound_socket *handle;
  struct evhttp *httpd;
  struct event_base *base_;

  base_ = event_base_new();
  httpd = evhttp_new(base_);

  evhttp_set_allowed_methods(httpd, EVHTTP_REQ_GET | EVHTTP_REQ_POST | EVHTTP_REQ_HEAD);
  evhttp_set_timeout(httpd, 5 /* timeout in seconds */);

  evhttp_set_cb(httpd, "/notify",  httpdNotification, NULL);

  handle = evhttp_bind_socket_with_handle(httpd, host.c_str(), port);
  if (!handle) {
    cout << "couldn't bind to port: " << port << ", host: " << host << ", exiting." << endl;
    return;
  }
  event_base_dispatch(base_);
}

int main(void)
{
    cout << "-----run main------" <<endl;
    runHttpd("0.0.0.0", 3344);
}
