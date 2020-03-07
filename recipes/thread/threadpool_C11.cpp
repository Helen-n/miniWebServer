#include <functional>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <queue>
#include <vector>
#include<iostream>

class threadpool
{
	using Task = std::function<void()>;

public:
	threadpool(size_t size = 4)
		: _stop(false)
	{
		size = size < 1 ? 1 : size;

		for (size_t i = 0; i < size; ++i)
		{
			_pool.emplace_back(&threadpool::schedual, this);
		}
	}

	~threadpool()
	{
		shutdown();
	}

	// 提交一个任务
	template<class F, class... Args>
	auto commit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
	{
		using ResType = decltype(f(args...));// 函数f的返回值类型

		auto task = std::make_shared<std::packaged_task<ResType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		{
			// 添加任务到队列
			std::lock_guard<std::mutex> lock(_taskMutex);

			_tasks.emplace([task]()
			{
				(*task)();
			});
		}

		_taskCV.notify_all(); //唤醒线程执行

		std::future<ResType> future = task->get_future();
		return future;
	}

private:
	// 获取一个待执行的task
	Task get_one_task()
	{
		std::unique_lock<std::mutex> lock(_taskMutex);

		_taskCV.wait(lock, [this]() { return !_tasks.empty() || _stop.load(); }); // wait直到有task

		if (_stop.load())
		{
			return nullptr;
		}

		Task task{ std::move(_tasks.front()) }; // 取一个task
		_tasks.pop();
		return task;
	}

	// 任务调度
	void schedual()
	{
		while (!_stop.load())
		{
			if (Task task = get_one_task())
			{
				task();
			}
		}
	}

	// 关闭线程池，并等待结束
	void shutdown()
	{
		this->_stop.store(true);
		_taskCV.notify_all();

		for (std::thread &thrd : _pool)
		{
			thrd.join();// 等待任务结束， 前提：线程一定会执行完
		}
	}

private:
	// 线程池
	std::vector<std::thread> _pool;

	// 任务队列
	std::queue<Task> _tasks;

	// 同步
	std::mutex _taskMutex;
	std::condition_variable _taskCV;

	// 是否关闭提交
	std::atomic<bool> _stop;
};

void func1()
{
	std::cout << "hello, f !" << std::endl;
}

struct struct1
{
	int operator()()
	{
		std::cout << "hello, g !" << std::endl;
		return 42;
	}
};

class class1
{
public:
	double class_func(double in)
	{
		return in;
	}
};

int main(int argc, char *argv[])
{
	threadpool executor(10);

	std::future<void> ret_func1 = executor.commit(func1);
	std::future<int> ret_struct1 = executor.commit(struct1{});
	std::future<std::string> ret_lambda1 = executor.commit([]()->std::string { std::cout << "hello, h !" << std::endl; return "hello,fh !"; });

	class1 class1_;
	std::future<double> ret_float1 = executor.commit(std::bind(&class1::class_func, &class1_, 0.9));

	std::cout << "ret_struct1:" << ret_struct1.get()
		<< "\nret_lambda1:" << ret_lambda1.get().c_str()
		<< "\nret_float1:" << ret_float1.get() << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::future<int> ret_struct2 = executor.commit(struct1{});
	int ret_struct2_int = ret_struct2.get();

	std::cout << "end..." << std::endl;

	return 0;
}