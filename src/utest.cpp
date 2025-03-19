#include "net/http_server.h"
#include "net/event_loop.h"
#include "log/pr.h"
#include "log/log.h"

int main()
{   
    Logger::get_instance()->init(NULL);

    EventLoop base_loop;
    HttpServer server(&base_loop, "127.0.0.1", 8889);
    server.set_tcp_cn_timeout_ms(8000);
    server.start(4);
    base_loop.loop();
    
    return 0;
}