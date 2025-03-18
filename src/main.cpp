#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
extern "C"
{
#include "./utils/utils.h"
#include "create_html.h"
#include "thttpd.h"
}
#include <locale.h>
#include <sys/wait.h>
#include "threadpool/threadpool.h"
#include "log/log.h"
#include "net/epoll.h"
#include <iostream>

configures *p_globalConfigure = NULL;
static const char *host = "0.0.0.0";
static u_short port = 8000;
static int thread_num = 1;

#define HTTP_KEEPALIVE_TIMEOUT 60000 // ms
#define HTTP_MAX_URL_LENGTH 256
#define HTTP_MAX_HEAD_LENGTH 1024

#define HTML_TAG_BEGIN "<html><body><center><h1>"
#define HTML_TAG_END "</h1></center></body></html>"

// status_message
#define HTTP_OK "OK"
#define NOT_FOUND "Not Found"
#define NOT_IMPLEMENTED "Not Implemented"

// Content-Type
#define TEXT_PLAIN "text/plain"
#define TEXT_HTML "text/html"
#define TEXT_CSS "text/css"

/**********************************************************************/

int server_sock_g = -1;

void exit_handle(int)
{
  PR_INFO("exit .. \r\n");
  close(server_sock_g);
  exit(0);
}

int main(int argc, char **argv)
{
  int& server_sock = server_sock_g;
  Logger::get_instance()->init(nullptr, 10);

  targs_deal_args(argc, argv);

  p_globalConfigure = get_configures_point();
  port = atoi(p_globalConfigure->items[CONFIGURE_PORT]);

  if (p_globalConfigure->items[CONFIGURE_THREADS][0] != 'd')
  {
    thread_num = atoi(p_globalConfigure->items[CONFIGURE_THREADS]);
  }
  else
  {
    thread_num = 1;
  }

  if (thread_num == 0)
    thread_num = 1;

  ThreadPool hs_tp{thread_num};

  struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);

  server_sock = startup(&port);
  if (server_sock != -1)
    PR_INFO("tinyhttpd listening on %s:%d, listenfd=%d, thread_num=%d\n",
            host, port, server_sock, thread_num);
  else
    PR_ERROR("create socket faild!\r\n");
  Epoll ep;

  setlocale(LC_ALL, "en_US.UTF-8");

  signal(SIGINT, exit_handle);

  ep.epoll_add(server_sock, EPOLLIN, [&hs_tp,server_sock]
               {
      struct sockaddr_in client_name;
  socklen_t client_name_len = sizeof(client_name);
      int *client_sock_p = NULL;
      client_sock_p = (int *)malloc(sizeof(int));
      *client_sock_p =
        accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (*client_sock_p == -1)
        {
          error_die("accept");
        }
        hs_tp.post_task(accept_request,client_sock_p); });
  ep.poll();

  // while (1)
  // {
  //   int *client_sock_p = NULL;
  //   client_sock_p = (int *)malloc(sizeof(int));

  //   *client_sock_p =
  //       accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
  //   if (*client_sock_p == -1)
  //   {
  //     error_die("accept");
  //   }
  //   hs_tp.post_task(accept_request, client_sock_p);
  // }
  close(server_sock);
  return 0;
}
