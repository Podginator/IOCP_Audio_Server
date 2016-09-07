#include "stdadfx.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include "ConcurrentQueue.h"

// A Task in this instance is just a function that Returns void and takes void.
// This could later be extended to include templates to include params
// and return an std::future to ensure that we can return parameters.
typedef function<void(void)> Task; 

// A simple way of pooling threads to avoid unnecessary thread creation and
// To ensure that we don't create more pools than is strictly necessary 
// (which can be a performance hindrance)  
class ThreadPool {
private:

  // A Threadsafe counter for how many jobs we have remaining
  atomic_int mJobsRemaining;

  // A threadsafe flag to say whether we're finished
  atomic_bool mFinished;

  // A pool of threads
  vector<thread> mThreads;

  // A queue of threads.
  // NOTE: This handles any issues with concurrency. 
  ConcurrentQueue<Task> mTasks;

  // Get the Next Task in the queue.
  Task nextTask() {
    Task res;

    if (!isFinished()) {
      res = mTasks.pop();
    }

    return res; 
  }

  // Do the task, these are what the threads in the thread queue run.
  void DoTask() {
    while (isFinished()) {
      // Get the next task (This will block)
      Task task = nextTask();
      
      // Run the task.
      task();

      // Decrement the remaining Task.
      mJobsRemaining -= 1; 
      // Then Notify the ???
      mWait.notify_one();
    }
  }


protected:
public:
  //Constructor, if no thread number is assigned 
  //Then assign the maximum amount
  ThreadPool(int numThreads = -1) {
    if (numThreads < 0) {
      numThreads = std::thread::hardware_concurrency();
    }

    mThreads.reserve(numThreads);

    for (int i = 0; i < numThreads; i++) {
      mThreads.push_back(thread([this, i] {this->DoTask(); }));
    }
  }


  // Enqueue the Task we want to complete. 
  void EnqueueTask(const Task& task) {
    mTasks.push(task);
  }

  // Join all the threads
  // NOTE:: WILL BLOCK UNTIL ALL QUEUES ARE COMPLETE. 
  void JoinAll() {
    if (!isFinished()) {
      // Notify all the queues to drain the threads. 
      mJobAvailable.notify_all();

      for (auto &x : mThreads) {
        if (x.joinable()) {
          x.join();
        }
      }

      mFinished.store(true);
    }
  }

  bool isFinished() {
    return mFinished.load();
  }  
};