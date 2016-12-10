#pragma once

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
	Task nextTask();

	// Do the task, these are what the threads in the thread queue run.
	void DoTask();


protected:
public:
	//Constructor, if no thread number is assigned 
	//Then assign the maximum amount
	ThreadPool(int numThreads);

	// Enqueue the Task we want to complete. 
	void EnqueueTask(const Task& task);

	// Join all the threads
	// NOTE:: WILL BLOCK UNTIL ALL QUEUES ARE COMPLETE. 
	void JoinAll();

	bool isFinished();
};