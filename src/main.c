#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "./utils/utils.h"
#include "create_html.h"
#include "too.h"
#include "thttpd.h"
#include <locale.h>
#include <stdlib.h>
#include <sys/wait.h>


configures *p_globalConfigure = NULL;
static const char *host = "0.0.0.0";
static int port = 8000;
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

int server_sock = -1;
threadpool_t *pool = NULL;

void exit_handle(void) {
  printf("exit..\r\n");
  close(server_sock);
  threadpool_destroy(pool);
  exit(0);
}

int main(int argc, char **argv) {
  targs_deal_args(argc, argv);

  p_globalConfigure = get_configures_point();

  port = atoi(p_globalConfigure->items[CONFIGURE_PORT]);

  if (p_globalConfigure->items[CONFIGURE_THREADS][0] != 'd') {
    thread_num = atoi(p_globalConfigure->items[CONFIGURE_THREADS]);
  } else {
    thread_num = 1;
  }

  if (thread_num == 0)
    thread_num = 1;

  struct sockaddr_in client_name;
  int client_name_len = sizeof(client_name);

  server_sock = startup(&port);
  LOG_SUCCESS("tinyhttpd listening on %s:%d, listenfd=%d, thread_num=%d\n",
              host, port, server_sock, thread_num)

  setlocale(LC_ALL, "en_US.UTF-8");

  pool = threadpool_create(1, thread_num, 20);
  signal(SIGINT, exit_handle);
  while (1) {
    int *client_sock_p = NULL;
    client_sock_p = (int *)malloc(sizeof(int));

    *client_sock_p =
        accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
    if (*client_sock_p == -1) {
      error_die("accept");
    }
    threadpool_add_task(pool, accept_request, client_sock_p);
  }
  close(server_sock);
  return 0;
}
