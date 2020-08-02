#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <vector>
#include <atomic>
#include <thread>
#include <future>


// With priority level
class WplThreadingPool {
public:
	typedef std::function<void()> task_type;
private:
	static const size_t maxThreads = 160;
	

	std::queue<task_type> tasks;
	std::vector<std::thread> pool;
	std::mutex mlock;
	std::condition_variable cond;
	std::atomic<size_t> freeThreadsNum;
	std::atomic<bool> terminate;

public:
	WplThreadingPool(WplThreadingPool&&) = delete;
	WplThreadingPool(const WplThreadingPool&) = delete;
	WplThreadingPool& operator=(const WplThreadingPool&) = delete;
	WplThreadingPool& operator=(WplThreadingPool&&) = delete;

public:
	// create  and destroy threads pool
	WplThreadingPool(size_t size): terminate(false) {
		size = size < 1 ? 1 : size;
		size = size < maxThreads ? size : maxThreads;
		freeThreadsNum = size;
		for (size_t ind = 0; ind < size; ind++) {
			pool.emplace_back([this](){
				while (true) {
					task_type task = NULL;
					{
						std::unique_lock<std::mutex> guard_lock(this->mlock);
						this->cond.wait(guard_lock, [this] {
							return this->terminate.load() || !(this->tasks.empty());
						});
						if (this->terminate.load()) break;
						task = std::move(tasks.front());
						this->tasks.pop();
					}
					freeThreadsNum--;
					task();
					freeThreadsNum++;
				}
			});
		}
	}

	~WplThreadingPool() {
		terminate.store(true);
		cond.notify_all();
		for (std::thread& thr : pool) {
			if (thr.joinable())
				thr.join();
		}
	}

	// 
	int freeNums() { return freeThreadsNum; }

	template<class Func, class... Args>
	auto add_task(Func&& fun, Args&&... args) -> std::future<decltype(fun(args...))> {
		using Rettype = decltype(fun(args...));
		auto ptr = std::make_shared<std::packaged_task<Rettype()>>(
			std::bind(std::forward<Func>(fun), std::forward<Args>(args)...));
		std::future<Rettype> res = ptr->get_future();
		{
			std::lock_guard<std::mutex> _lock(mlock);
			tasks.emplace([ptr]() {
				(*ptr)();
			});
		}
		cond.notify_one();

		return res;
	}
};

#endif
