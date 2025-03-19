#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "tcp_server.h"
#include "event_loop.h"
#include <unordered_map>
#include <string>
class HttpMessage
{
public:
    HttpMessage() {}
    void print_message() const;
    void parse_request_message(string &msg);
    void build_response_message(string protocol ,string status_code ,string status_message,string &content);
    typedef struct
    {
        string method;
        string path;
        string protocol;
        unordered_map<string, string> headers;
    } http_request_t;

    typedef struct
    {
        string protocol;
        string status_code;
        string status_message;
        unordered_map<string, string> headers;
        string contentType;
        string contentLength;
        string databuf;
        string response_seq;
        FILE *fp;
    } http_response_t;

    http_response_t response;
    http_request_t request;

private:
    
};

class HttpServer
{
public:
    HttpServer(EventLoop *loop, const char *ip, uint16_t port) : hs_loop(loop), hs_server(loop, ip, port)
    {
        hs_server.set_connected_cb([this](const TcpConnSP &conn)
                                   { this->echo_conneted_cb(conn); });
        hs_server.set_message_cb([this](const TcpConnSP &conn, InputBuffer *ibuf)
                                 { this->echo_message_cb(conn, ibuf); });
        hs_server.set_close_cb([this]()
                               { this->echo_close_cb(); });
    }

    ~HttpServer() {};

    void start(int thread_num)
    {
        hs_server.set_thread_num(thread_num);
        hs_server.start();
    }
    void set_tcp_cn_timeout_ms(int ms) { hs_server.set_tcp_conn_timeout_ms(ms); }

private:
    void echo_conneted_cb(const TcpConnSP &conn)
    {
        PR_INFO("one connected! peer addr is %s, local socket fd is %d\n", conn->get_peer_addr(), conn->get_fd());
    }
    void echo_message_cb(const TcpConnSP &conn, InputBuffer *ibuf);
    void echo_close_cb() { PR_INFO("one connection closed in echo server!\n"); }
    void deal_method(const TcpConnSP &conn,HttpMessage &hmsg);
    TcpServer hs_server;
    EventLoop *hs_loop;
};

#endif