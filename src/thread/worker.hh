#ifndef __WORKER_H__
#define __WORKER_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class WorkerThread {
  public:
    WorkerThread() {
        running = true;
        worker = std::thread([this] { this->loop(); });
    }

    ~WorkerThread() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            running = false;
        }
        cv.notify_all();
        if (worker.joinable()) {
            worker.join();
        }
    }

    WorkerThread(const WorkerThread &) = delete;
    WorkerThread &operator=(const WorkerThread &) = delete;
    WorkerThread(WorkerThread &&) = delete;
    WorkerThread &operator=(WorkerThread &&) = delete;

    void async(std::function<void()> fn) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(std::move(fn));
        }
        cv.notify_one();
    }

  private:
    std::thread worker;
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    std::atomic<bool> running = false;

    void loop() {
        while (running) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&] { return !tasks.empty() || !running; });
                if (!running) {
                    break;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }
};

#endif  // __WORKER_H__