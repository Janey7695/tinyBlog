#ifndef _THTTP_H__
#define _THTTP_H__
#include <stdio.h>
#include <sys/types.h>


typedef struct request_header{
	char *key;
	char *value;
	struct request_header *next;
}http_header_t;

typedef struct {
	http_header_t *head;
	http_header_t *tail;
	int headers_number;
}http_headers_t;


typedef struct{
	char method[10];
	char path[100];
	char protocol[15];
	//char headers[1024];
	http_headers_t *headers;
}http_request_t;

typedef struct{
	char protocol[15];
	char status_code[5];
	char status_message[20];
//	char headers[1024];
	http_headers_t *headers;
	char *contentType;
	char *charbuf;
	FILE *fp;
}http_response_t;

typedef struct {
	http_request_t request;
	http_response_t response;
}http_message_t;

void accept_request(void *);
void bad_request(int);
void cat_file(int, FILE *);
void cat_string(int client, char *string, int length);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void not_found(int);
void serve_file(int,http_message_t *, const char *);
int startup(u_short *);
void unimplemented(int);

#endif
