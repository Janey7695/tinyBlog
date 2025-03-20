#ifndef __FILE_MONITOR_H__
#define __FILE_MONITOR_H__

#include "../timer/timer.h"
#include "../log/log.h"
#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>
class DirectoryMonitor {
public:
  using Callback = std::function<void(const std::filesystem::path &)>;
  DirectoryMonitor(const std::filesystem::path &root_dir, Callback callback)
      : root_dir(root_dir), callback(callback) {
    update_file_list();
    tm.run();
    tm.run_after(5000,true,[this](){
        this->check_for_changes();
    });
  }

private:
  void update_file_list() {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(root_dir)) {
      if (entry.is_regular_file()) {
        file_modify_time_map[entry.path()] =
            std::filesystem::last_write_time(entry.path());
      }
    }
  }
  void check_for_changes() {
    // 获取当前目录下的所有文件
    std::set<std::filesystem ::path> current_files;
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(root_dir)) {
      if (entry.is_regular_file()) {
        current_files.insert(entry.path());
      }
    }

    // 检查新文件
    for (const auto &file_path : current_files) {
      if (file_modify_time_map.find(file_path) == file_modify_time_map.end()) {
        // 新文件
        std::cout << "New file detected: " << file_path << std::endl;
        callback(file_path);
        file_modify_time_map[file_path] =
            std::filesystem::last_write_time(file_path);
      }
    }

    // 检查文件修改
    for (auto &[file_path, last_modified] : file_modify_time_map) {
      if (current_files.find(file_path) == current_files.end()) {
        // 文件被删除
        file_modify_time_map.erase(file_path);
        continue;
      }

      auto current_modified = std::filesystem::last_write_time(file_path);
      if (current_modified != last_modified) {
        // 文件被修改
        std::cout << "File modified: " << file_path << std::endl;
        callback(file_path);
        last_modified = current_modified;
      }
    }
  }
  Timer tm;
  Callback callback;
  std::filesystem::path root_dir = "./";
  std::unordered_map<std::filesystem::path, std::filesystem::file_time_type>
      file_modify_time_map;
};

#endif
