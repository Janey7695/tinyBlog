#include "log/log.h"
#include "log/pr.h"
#include "net/event_loop.h"
#include "net/http_server.h"
#include "timer/timer.h"
#include "mdconvert/file_monitor.h"

void on_file_change(const std::filesystem::path& file_path) {
  std::cout << "Callback triggered for file: " << file_path << std::endl;
}

int main() {
  Logger::get_instance()->init(NULL);
  DirectoryMonitor dm("./",on_file_change);

  EventLoop base_loop;
  HttpServer server(&base_loop, "127.0.0.1", 8889);
  server.set_tcp_cn_timeout_ms(8000);
  server.start(4);
  base_loop.loop();

  return 0;
}
