#include "ThreadPool.h"

typedef function<void(void)> Task;

// Do the task, these are what the threads in the thread queue run.
void ThreadPool::DoTask() {
	while (!isFinished()) {
		// Get the next task (This will block)
		Task task = nextTask();

		// Run the task.
		task();

		// Decrement the remaining Task.
		mJobsRemaining -= 1;
	}
}

Task ThreadPool::nextTask() {
	Task res;

	if (!isFinished()) {
		res = mTasks.pop();
	}

	return res;
}

//Constructor, if no thread number is assigned 
//Then assign the maximum amount
ThreadPool::ThreadPool(int numThreads) {
	if (numThreads < 0) {
		numThreads = std::thread::hardware_concurrency();
	}

	mThreads.reserve(numThreads);

	for (int i = 0; i < numThreads; i++) {
		mThreads.push_back(thread([this, i] {this->DoTask(); }));
	}
}


// Enqueue the Task we want to complete. 
void ThreadPool::EnqueueTask(const Task& task) {
	mTasks.push(task);
}

// Join all the threads
// NOTE:: WILL BLOCK UNTIL ALL QUEUES ARE COMPLETE. 
void ThreadPool::JoinAll() {
	if (!isFinished()) {

		for (auto &x : mThreads) {
			if (x.joinable()) {
				x.join();
			}
		}

		mFinished.store(true);
	}
}

bool ThreadPool::isFinished() {
	return mFinished.load();
}
