#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//Wrapper around a standard Queue. 
template <typename T> 
class ConcurrentQueue {
public: 

  // Pop from the queue, use a condition variable 
  // To ensure that we block when we're empty.
  T pop() {

    unique_lock<mutex> lk(mMutex);

    while (mQueue.empty()) {
      mCondVar.wait(lk);
    }
    //Get the item from the front of the queue.
    T res = mQueue.front();
    //Then pop it. 
    mQueue.pop();

    return res; 
  }

  // Return the size of the queue, lock 
  // for thread safety before accessing the queue.
  size_t size() {
    size_t res = 0;
    unique_lock<mutex> lk(mMutex);
    res = mQueue.size();
    lk.unlock();
    return res;
  }

  // Push an item to the queue
  // Block before pushing to the queue.
  // Then notify the Queue that we've pushed.
  void push(const T& item) {
    unique_lock<mutex> lk(mMutex);
    mQueue.push(item);
    lk.unlock();
    mCondVar.notify_all();
  }


  // Clear the queue.
  void clear() {
    unique_lock<mutex> lk(mMutex);
    mQueue.clear();
    lk.unlock();
  }

private: 
  // The inner queue
  queue<T> mQueue; 

  // The mutex we lock against/
  mutex mMutex; 

  // The coniditon variable to wait for.
  condition_variable mCondVar;

};