#include "thttpd.h"
#include "./utils/utils.h"
#include "create_html.h"
#include "thttpd.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <locale.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "tinyBlog/1.2.0"

http_message_t *parse_http_message(int sock);
void create_http_response_statusline(http_message_t *phmsg,
                                     const char *protocol,
                                     const char *statue_code,
                                     const char *status_message);
void set_http_response_body_charbuf(http_message_t *phmsg, const char *charbuf);
void set_http_response_body_file(http_message_t *phmsg, FILE *fp);
void http_reply(http_message_t *phmsg, char *status_code, char *status_message, char *content_type, void *body, int body_type);
void http_send(int client,http_message_t *phmsg);
http_header_t *create_new_header_from_oneline(char *oneline);
http_header_t *create_new_header_from_keyvalue(char *key,char *value);
int append_header(http_headers_t *headers_rootnode,http_header_t *header_newnode);

http_header_t *create_new_header_from_oneline(char *oneline){
	http_header_t*pnewheader = NULL;
	if (strcmp(oneline, "\r\n") == 0) {
		return NULL;
	}
	pnewheader = TMALLOC(http_header_t, 1);
	if (pnewheader == NULL) {
		LOG_ERROR("malloc memory faild in %s",__func__)
		return NULL;
	}
	char *s1,*s2;
	int length = 0;
	s1 = oneline;
	s2 = oneline;
	while(*s1 != ' ' && *s1 !=':'){
		length++;
		s1++;
	}
	pnewheader->key = TMALLOC(char, length + 1);
	pnewheader->key[length] = '\0';
	for (--length; length >= 0; length--) {
		pnewheader->key[length] = s2[length];
	}
	s1 = seek_until(s1, ':');
	s1 = skip_char(s1+1,' ');
	s2 = s1;
	length = 0;
	while (*s1 != '\r' && *s1 != '\0') {
		s1++;
		length++;
	}
	pnewheader->value = TMALLOC(char, length + 1);
	pnewheader->value[length] = '\0';
	for (--length; length >= 0; length--) {
		pnewheader->value[length] = s2[length];
	}

	pnewheader->next = NULL;

	DEBUG_NORMAL("You get %s : %s ",pnewheader->key,pnewheader->value)
	
	return pnewheader;

}

http_header_t *create_new_header_from_keyvalue(char *key,char *value){
	http_header_t *pnewheader = NULL;
	char *p1=NULL,*p2=NULL;
	int length = 0;
	if (key == NULL || value == NULL) {
		LOG_WARN("you pass a null key or value string point in %s",__func__)
		return NULL;
	}
	pnewheader = TMALLOC(http_header_t, 1);
	if (pnewheader == NULL) {
		LOG_ERROR("malloc memory faild in %s",__func__)
		return NULL;
	}
	length = strlen(key);
	p1 = TMALLOC(char, length + 1);
	strcpy(p1, key);
	p1[length] = '\0';
	length = strlen(value);
	p2 = TMALLOC(char, length + 1);
	strcpy(p2, value);
	p2[length] = '\0';

	pnewheader->key = p1;
	pnewheader->value = p2;
	pnewheader->next = NULL;

	DEBUG_NORMAL("You create %s : %s ",pnewheader->key,pnewheader->value)

	return pnewheader;
	
}

int append_header(http_headers_t *headers_rootnode,http_header_t *header_newnode){
	if (headers_rootnode == NULL || header_newnode == NULL) {
		LOG_WARN("you pass a null node in %s ",__func__)
		return 0;	
	}
	if (headers_rootnode->head == NULL && headers_rootnode->tail == NULL) {
		headers_rootnode->head = header_newnode;
		headers_rootnode->tail = header_newnode;
		headers_rootnode->headers_number += 1;
		return headers_rootnode->headers_number;
	}
	headers_rootnode->tail->next = header_newnode;
	headers_rootnode->tail = header_newnode;
	headers_rootnode->headers_number += 1;
	return headers_rootnode->headers_number;

}

void free_headers(http_headers_t *headers_rootnode){
	http_header_t *pheader1 = headers_rootnode->head;
	http_header_t *pheader2 = pheader1;
	if (pheader1 == NULL) {
		return ;
	}
	while (pheader1->next != NULL) {
		pheader2 = pheader1->next;
		free(pheader1);
		pheader1 = pheader2;
	}
	free(pheader1);
	headers_rootnode->head = NULL;
	headers_rootnode->tail = NULL;
}

http_message_t *parse_http_message(int sock) {
  char buf[1024];
  int numberChars = 0;
  int i = 0, j = 0;
  http_message_t *phmsg = TMALLOC(http_message_t, 1);

  if (phmsg == NULL) {
    LOG_ERROR("alloc memory faild in %s", __func__)
    return NULL;
  }
  numberChars = get_line(sock, buf, sizeof(buf));
  DEBUG_NORMAL("get %d chars", numberChars)
  while (buf[i] != ' ') {
    phmsg->request.method[i] = buf[i];
    i++;
  }
  phmsg->request.method[i] = '\0';
  while (buf[i] != '/') {
    i++;
  }
  while (buf[i] != ' ') {
    phmsg->request.path[j++] = buf[i++];
  }
  phmsg->request.path[j] = '\0';
  while (buf[i] == ' ') {
    i++;
  }
  j = 0;
  while (buf[i] != ' ' && buf[i] != '\0') {
    phmsg->request.protocol[j++] = buf[i++];
  }
  phmsg->request.protocol[j] = '\0';

  phmsg->request.headers = TMALLOC(http_headers_t, 1);
  if (phmsg->request.headers == NULL) {
  	LOG_ERROR("alloc memory faild in %s",__func__)
	return NULL;
  }
  phmsg->request.headers->headers_number = 0;
  phmsg->request.headers->head = NULL;
  phmsg->request.headers->tail = NULL;
  while ((numberChars > 0) && strcmp("\n", buf)) {
    numberChars = get_line(sock, buf, sizeof(buf));
	DEBUG_NORMAL("You may get %s ",buf);
	append_header(phmsg->request.headers, create_new_header_from_oneline(buf));
  }

  phmsg->response.fp = NULL;
  phmsg->response.charbuf = NULL;
  phmsg->response.headers = TMALLOC(http_headers_t, 1);
  if (phmsg->response.headers == NULL) {
  	LOG_ERROR("alloc memory faild in %s",__func__)
	return NULL;
  }
  phmsg->response.headers->headers_number = 0;
  phmsg->response.headers->head = NULL;
  phmsg->response.headers->tail = NULL;

  append_header(phmsg->response.headers, create_new_header_from_keyvalue("Server",SERVER_STRING));

  return phmsg;
}

void free_http_message(http_message_t *phmsg){
	if (phmsg->request.headers != NULL) {
		free_headers(phmsg->request.headers);
	}	
	if (phmsg->response.headers != NULL) {
		free_headers(phmsg->response.headers);
	}
	free(phmsg);
}

void create_http_response_statusline(http_message_t *phmsg,
                                     const char *protocol,
                                     const char *statue_code,
                                     const char *status_message) {
  int length = 0;
  length = strlen(protocol);
  strcpy(phmsg->response.protocol, protocol);
  phmsg->response.protocol[length] = '\0';
  length = strlen(statue_code);
  strcpy(phmsg->response.status_code, statue_code);
  phmsg->response.status_code[length] = '\0';
  length = strlen(status_message);
  strcpy(phmsg->response.status_message, status_message);
  phmsg->response.status_message[length] = '\0';
}

void set_http_response_body_charbuf(http_message_t *phmsg, const char *charbuf)
{
	phmsg->response.charbuf = charbuf;
}
void set_http_response_body_file(http_message_t *phmsg, FILE *fp)
{
	phmsg->response.fp =fp;
}

void http_reply(http_message_t *phmsg, char *status_code, char *status_message, char *content_type, void *body, int body_type)
{
	create_http_response_statusline(phmsg, "HTTP/1.0", status_code, status_message);
	phmsg->response.contentType = content_type;
	append_header(phmsg->response.headers, create_new_header_from_keyvalue("Content-Type", content_type));
	if(body_type == 0){
		phmsg->response.charbuf = (char *)body;
	}else {
		phmsg->response.fp = (FILE *)body;
	}
}

void accept_request(void *arg) {
  int client = -1;
  char buf[1024];
  int numchars;
  char method[255];
  char url[255];
  char path[512];
  size_t i, j;
  struct stat st;
  int cgi = 0; /* becomes true if server decides this is a CGI
                * program */
  char *query_string = NULL;
  client = *(int *)arg;
  http_message_t *phttp_message = parse_http_message(client);
  if (phttp_message == NULL) {
    LOG_ERROR("couldn't parse http message .")
    return;
  }
  DEBUG_NORMAL("Method : %s", phttp_message->request.method);
  DEBUG_NORMAL("Path : %s", phttp_message->request.path);
  DEBUG_NORMAL("protocol : %s", phttp_message->request.protocol);
  if (strcmp(phttp_message->request.method, "GET") == 0) {
    if (strcmp(phttp_message->request.path, "/article-list") == 0) {
      char *contentNav, *contentArticlesList, *contentWrapped;
      int length = 0;
      contentArticlesList = parse_articlesList_to_htmlBytesStream(
          get_configures_point()->items[CONFIGURE_MKDPATH], &length);
      DEBUG_WARN("size is %d", strlen(contentArticlesList))
      if (contentArticlesList == NULL) {
        LOG_WARN("couldn't parse articles list,will return null text.")
        not_found(client);
        close(client);
        free((int *)arg);
		free_http_message(phttp_message);
        return;
        // return 404 page
      } else {
        contentWrapped = wrap_with_html_heads(contentArticlesList, &length,
                                              PAGE_TYPE_LISTPAGE);
        if (contentWrapped == NULL) {
          LOG_WARN("couldn't wrap link list html tags,will return null text.")
          // return 404 page
          free(contentArticlesList);
          not_found(client);
          close(client);
          free((int *)arg);
		  free_http_message(phttp_message);
          return;
        }
        // return content page
        // http_reply(conn, 200, "OK", TEXT_HTML, contentWrapped, length);
		http_reply(phttp_message, "200", "OK", "text/html",contentWrapped,  0);
		DEBUG_WARN("wrapped content is %s ",contentWrapped)
		http_send(client, phttp_message);
        //headers(client, "article-list.html");
        //cat_string(client, contentWrapped, length);
        DEBUG_WARN("size is %d", strlen(contentWrapped))
        free(contentArticlesList);
        free(contentWrapped);
      }
      close(client);
      free((int *)arg);
	  free_http_message(phttp_message);
      return;
    } else if (strncmp(phttp_message->request.path, "/articles", 9) == 0) {
      char *contentNav, *contentMd, *contentWrapped;
      int length = 0, len = 0;
      char filePath[256] = "";
      memset(filePath, 0x0, 256);
      urldecode(phttp_message->request.path + 9);
      sprintf(filePath, "%s%s.md",
              get_configures_point()->items[CONFIGURE_MKDPATH],
              phttp_message->request.path + 9);
      LOG_NORMAL("a user want to read file : %s", filePath)
      contentMd = parse_md_to_htmlBytesStream(filePath, &length);
      len += length;
      if (contentMd == NULL) {
        LOG_WARN("couldn't parse markdown file,will return null text.")
        not_found(client);
        close(client);
        free((int *)arg);
        free(phttp_message);
		free_http_message(phttp_message);
        return;
        // return 404 page
      } else {
        contentNav = create_nav_htmlBytesStream(&length);
        len += length;
        char *contentNavMd = (char *)malloc(sizeof(char) * (len + 1));
        length = sprintf(contentNavMd, "%s%s", contentNav, contentMd);
        contentNavMd[length] = '\0';
        DEBUG_WARN("size is %d", strlen(contentNavMd))
        contentWrapped =
            wrap_with_html_heads(contentNavMd, &length, PAGE_TYPE_MARKDOWN);
        if (contentWrapped == NULL) {
          LOG_WARN("couldn't wrap markdown html tags,will return null text.")
          // return 404 page
          not_found(client);
          free(contentMd);
          free(contentNav);
          free(contentNavMd);
          close(client);
          free((int *)arg);
          free_http_message(phttp_message);
          return;
        }
        // return conntent page
        // http_reply(conn, 200, "OK", TEXT_HTML, contentWrapped, length);
		http_reply(phttp_message, "200", "OK", "text/html", contentWrapped, 0);
		http_send(client, phttp_message);

        //headers(client, "article.html");
        //cat_string(client, contentWrapped, length);
        DEBUG_WARN("size is %d", strlen(contentWrapped))
        free(contentMd);
        free(contentWrapped);
        free(contentNav);
        free(contentNavMd);
      }
      close(client);
      free((int *)arg);
      free_http_message(phttp_message);
      return;
    } else {
      // TODO: Add handler for your path
    }
    // return http_serve_file(conn);
  }
  int len1 = sprintf(path, ".%s", phttp_message->request.path);

  // ?????? path ???????????????????????????????????????????????????????????? /
  // ?????????????????????????????????"index.html"??????????????????????????????
  if (path[strlen(path) - 1] == '/')
    len1 = sprintf(path, "./themes/%s/index.html",
                   get_configures_point()->items[CONFIGURE_THEME]);

  path[len1] = '\0';

  // ??????????????????????????????????????????
  if (stat(path, &st) == -1) {
    // ?????????????????????????????? http ????????????????????????(head ??? body)?????????????????????
    DEBUG_ERROR("file don't exsit %s", path)
    // while ((numchars > 0) && strcmp("\n", buf)) /* read & discard headers */
    // {

    //   numchars = get_line(client, buf, sizeof(buf));
    //   DEBUG_WARN("http request : %s", buf)
    // }
    // ???????????????????????????????????? response ????????????
    not_found(client);
  } else {
    // ??????????????????????????????S_IFMT????????????????????????????????????????????????????????????????????????
    // S_IFMT?????????TLPI???P281?????????????????????????????????????????????<sys/stat.h>
    if ((st.st_mode & S_IFMT) == S_IFDIR)
      // ??????????????????????????????????????????????????? path
      // ??????????????????"/index.html"????????????
      strcat(path, "/index.html");

    // S_IXUSR, S_IXGRP, S_IXOTH?????????????????????TLPI???P295
    if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) ||
        (st.st_mode & S_IXOTH))
      // ??????????????????????????????????????????????????????????????????/???/?????????????????????????????????
      // cgi ??????????????????
      cgi = 1;

    if (!cgi)
      // ??????????????? cgi ???????????????
      serve_file(client,phttp_message ,path);
    else
      cgi = 0;
    // ?????????????????????
    // execute_cgi(client, path, method, query_string);
  }

  close(client);
  free((int *)arg);
  free_http_message(phttp_message);
}

void bad_request(int client) {
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "<P>Your browser sent a bad request, ");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "such as a POST without a Content-Length.\r\n");
  send(client, buf, sizeof(buf), 0);
}

/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat_file(int client, FILE *resource) {
  char buf[1024];

  // ?????????????????????????????????????????????
  fgets(buf, sizeof(buf), resource);
  while (!feof(resource)) {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
  }
}

void cat_string(int client, char *string, int length) {
  send(client, string, length, 0);
}

/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client) {
  char buf[1024];

  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
  send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc) {
  // ?????????<stdio.h>,??????????????? errno
  // ????????????????????????????????????????????????????????????TLPI???P49
  perror(sc);
  exit(1);
}

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path, const char *method,
                 const char *query_string) {
	// TODO
  }

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line(int sock, char *buf, int size) {
  int i = 0;
  char c = '\0';
  int n;

  while ((i < size - 1) && (c != '\n')) {
    // recv()?????????<sys/socket.h>,?????????TLPI???P1259,
    // ????????????????????????????????? c ???
    n = recv(sock, &c, 1, 0);
    /* DEBUG printf("%02X\n", c); */
    if (n > 0) {
      if (c == '\r') {
        //
        n = recv(sock, &c, 1, MSG_PEEK);
        /* DEBUG printf("%02X\n", c); */
        if ((n > 0) && (c == '\n'))
          recv(sock, &c, 1, 0);
        else
          c = '\n';
      }
      buf[i] = c;
      i++;
    } else
      c = '\n';
  }
  buf[i] = '\0';

  return (i);
}

void http_send(int client,http_message_t *phmsg)
{
	char *protocol,*status_code,*status_message,*headers;
	char *statuslinebuf = NULL;
	char headersbuf[1024];
	http_header_t *pheader;
	protocol = phmsg->response.protocol;
	status_code = phmsg->response.status_code;
	status_message = phmsg->response.status_message;
	headers = phmsg->response.headers;
	int l1 = strlen(protocol) + strlen(status_code) + strlen(status_message) + strlen("\r\n") + 3;
	statuslinebuf = TMALLOC(char, l1);
	l1 = sprintf(statuslinebuf, "%s %s %s\r\n",protocol,status_code,status_message);
	statuslinebuf[l1] = '\0';

	int l2 = 0;
	pheader = phmsg->response.headers->head;
	while (pheader != NULL) {
		l2 += sprintf(headersbuf+l2, "%s: %s\r\n",pheader->key,pheader->value);
		pheader = pheader->next;
	}

	l2 += sprintf(headersbuf+l2, "\r\n");
	headersbuf[l2] = '\0';
	DEBUG_NORMAL("rspon header : %s",headersbuf)
	
	send(client, statuslinebuf, l1, 0);
	send(client, headersbuf, l2, 0);
	if (phmsg->response.charbuf!=NULL) {
		send(client, phmsg->response.charbuf, strlen(phmsg->response.charbuf), 0);
	}else {
		cat_file(client, phmsg->response.fp);
	}

	free(statuslinebuf);

}

/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename) {
  char buf[1024];
  char *suffix_p;
  (void)filename; /* could use filename to determine file type */

  strcpy(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);
  strcpy(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  suffix_p = get_suffix(filename);
  if (suffix_p == NULL) {
    sprintf(buf, "Content-Type: text/plain\r\n");
    send(client, buf, strlen(buf), 0);
  } else if (strcmp(suffix_p, "css") == 0) {
    sprintf(buf, "Content-Type: text/css\r\n");
    send(client, buf, strlen(buf), 0);
  } else {
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
  }
  strcpy(buf, "\r\n");
  send(client, buf, strlen(buf), 0);

  free(suffix_p);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client) {
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client,http_message_t *phmsg, const char *filename) {
  FILE *resource = NULL;
  int numchars = 1;
  char buf[1024];

  // ?????? buf ???????????????????????????????????? while ??????
  buf[0] = 'A';
  buf[1] = '\0';
  // ??????????????????????????????????????? http ???????????????????????????
  // while ((numchars > 0) && strcmp("\n", buf)) /* read & discard headers */
  //   numchars = get_line(client, buf, sizeof(buf));
  DEBUG_WARN("start to serve file.")
  // ???????????????????????????????????????????????????
  resource = fopen(filename, "r");
  if (resource == NULL) {
    DEBUG_ERROR("couldn't open %s", filename)
    not_found(client);
  } else {
    // ????????????????????????????????????????????????????????? response ?????????(header)?????????
	char *p_suffix = get_suffix(filename);
	if (strcmp(p_suffix, "css")==0) {
		http_reply(phmsg, "200", "OK", "text/css", resource, 1);
	}
	else {
		http_reply(phmsg, "200", "OK", "text/html", resource, 1);
	}
	http_send(client, phmsg);
	free(p_suffix);
  }

  fclose(resource);
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short *port) {
  int httpd = 0;
  // sockaddr_in ??? IPV4????????????????????????????????????<netinet/in.h>,?????????TLPI???P1202
  struct sockaddr_in name;

  // socket()???????????????????????? socket
  // ??????????????????????????????<sys/socket.h>????????????TLPI???P1153 ?????????PF_INET????????????
  // AF_INET??????????????????????????????TLPI???P946
  httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");

  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  // htons()???ntohs() ??? htonl()?????????<arpa/inet.h>, ?????????TLPI???P1199
  // ???*port ????????????????????????????????????16?????????
  name.sin_port = htons(*port);
  // INADDR_ANY????????? IPV4?????????????????????????????????<netinet/in.h>
  // ?????????????????????????????????0.0.0.0 ?????????TLPI???P1187
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind()????????????????????? socket????????????TLPI???P1153
  // ??????????????????sockaddr???????????? sin_port
  // ?????????0????????????????????????????????????????????????
  if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    error_die("bind");

  // ???????????? bind ?????????????????????0??????????????????getsockname()???????????????
  if (*port == 0) /* if dynamically allocating a port */
  {
    int namelen = sizeof(name);
    // getsockname()?????????<sys/socker.h>???????????????TLPI???P1263
    // ??????getsockname()??????????????? httpd ?????? socket ????????????????????????
    if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
      error_die("getsockname");
    *port = ntohs(name.sin_port);
  }

  // ????????? BSD socket ????????????backlog ????????????5.?????????TLPI???P1156
  if (listen(httpd, 5) < 0)
    error_die("listen");
  return (httpd);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client) {
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}
