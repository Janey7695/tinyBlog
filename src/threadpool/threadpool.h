#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#define THREADPOOL_MAX_NUM 16
#define THREADPOOL_AUTO_GROW

using namespace std;

class Threadpool
{
public:
    typedef function<void()> Task;
    inline Threadpool(unsigned short size = 4)
    {
        assert(size <= THREADPOOL_MAX_NUM);
        tp_running = true;
        add_thread(size);
    }

    inline ~Threadpool()
    {
        tp_running = false;
        tp_task_cv.notify_all();
        for (auto &t : tp_pool)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    template <typename F, typename... Args>
    decltype(auto) post_task(F &&f, Args &&...args)
    {
        if (!tp_running)
            throw runtime_error("post_task on  ThreadPool has been stopped due to "
                                "the tp_running flag.");

        // 接下来要还原F和Args为一个可调用对象
        using resType = typename std::result_of_t<F(Args...)>;
        auto task = make_shared<packaged_task<resType()>>(
            bind(forward<F>(f), forward<Args>(args)...));

        // 使用future来从线程任务中获取返回值
        future<resType> res = task->get_future();

        {
            lock_guard<mutex> lock{tp_lock};
            tp_task_queue.emplace([task]()
                                  { (*task)(); });
        }

#ifdef THREADPOOL_AUTO_GROW
        if (tp_idl_num < 1 && tp_pool.size() < THREADPOOL_MAX_NUM)
        {
            add_thread(1);
        }
#endif
        tp_task_cv.notify_one();
        return res;
    }

private:
    void add_thread(unsigned short size)
    {
        for (; size > 0 && tp_pool.size() <= THREADPOOL_MAX_NUM; --size)
        {
            tp_pool.emplace_back([this]
                                 {
        Task task;
        {
          unique_lock<mutex> lock{tp_lock};
          tp_task_cv.wait(
              lock, [this] { return !tp_running || !tp_task_queue.empty(); });
          if (!tp_running && tp_task_queue.empty()) {
            return;
          }
          task = move(tp_task_queue.front());
          tp_task_queue.pop();
        }
        tp_idl_num--;
        task();
        tp_idl_num++; });
        tp_idl_num++;
        }
    }
    vector<thread> tp_pool;        // 线程vector
    queue<Task> tp_task_queue;     // 任务队列
    mutex tp_lock;                 // 互斥锁
    condition_variable tp_task_cv; // 条件变量

    atomic<int> tp_idl_num{0};      // 记录空闲线程数量
    atomic<bool> tp_running{false}; // 记录线程池允许状态
};

#endif