#ifndef __LOCK_QUEUE__
#define __LOCK_QUEUE__

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
//_LOCK_QUEUE_STD
namespace entities {
template <typename T>
class LockQueue {
 public:
  T front() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    return (queue_.front());
  }

  T pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void push(const T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }

  bool empty() { return queue_.empty(); }
  LockQueue() = default;
  LockQueue(const LockQueue&) = delete;
  LockQueue& operator=(const LockQueue&) = delete;

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};
}  // namespace entities
#endif  //__LOCK_QUEUE__