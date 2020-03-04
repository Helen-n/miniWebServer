//#include <condition_variable>
//#include <iostream>
//#include <mutex>
//#include <thread>
//#include <vector>
//
//class BoundedBuffer {
//public:
//	BoundedBuffer(const BoundedBuffer& rhs) = delete;
//	BoundedBuffer& operator=(const BoundedBuffer& rhs) = delete;
//
//	BoundedBuffer(std::size_t size)
//		: begin_(0), end_(0), buffered_(0), circular_buffer_(size) {
//	}
//
//	void Produce(int n) 
//	{
//		{   
//			std::unique_lock<std::mutex> lock(mutex_);
//			// 等待缓冲不为满。
//			not_full_cv_.wait(lock, [=] { return buffered_ < circular_buffer_.size(); });
//
//			// 插入新的元素，更新下标。
//			circular_buffer_[end_] = n;
//			end_ = (end_ + 1) % circular_buffer_.size();
//
//			++buffered_;
//		}  // 通知前，自动解锁。解锁方式1
//
//		   // 通知消费者。
//		not_empty_cv_.notify_one();
//	}
//
//	int Consume() {
//		std::unique_lock<std::mutex> lock(mutex_);
//		// 等待缓冲不为空。
//		not_empty_cv_.wait(lock, [=] { return buffered_ > 0; });
//
//		// 移除一个元素。
//		int n = circular_buffer_[begin_];
//		begin_ = (begin_ + 1) % circular_buffer_.size();
//
//		--buffered_;
//
//		// 通知前，手动解锁。解锁方式2
//		lock.unlock();
//		// 通知生产者。
//		not_full_cv_.notify_one();
//		return n;
//	}
//
//private:
//	std::size_t begin_;
//	std::size_t end_;
//	std::size_t buffered_;
//	std::vector<int> circular_buffer_;
//	std::condition_variable not_full_cv_;
//	std::condition_variable not_empty_cv_;
//	std::mutex mutex_;
//};
//
//BoundedBuffer g_buffer(4);
//std::mutex g_io_mutex;
//
//void Producer() {
//	std::thread::id thread_id = std::this_thread::get_id();
//	int n = 0;
//	while (n < 100000) {
//		g_buffer.Produce(n);
//		if ((n % 10000) == 0) {
//			std::unique_lock<std::mutex> lock(g_io_mutex);
//			std::cout << "Produce: " << n << " (threadid: " << thread_id << ")" << std::endl;
//		}
//		++n;
//	}
//	// 往缓冲里再放一个 -1，这样生产者才能结束。
//	g_buffer.Produce(-1);
//}
//
//void Consumer() {
//	std::thread::id thread_id = std::this_thread::get_id();
//
//	int n = 0;
//	do {
//		n = g_buffer.Consume();
//		if ((n % 10000) == 0) {
//			std::unique_lock<std::mutex> lock(g_io_mutex);
//			std::cout << "Consume: " << n << " (threadid: " << thread_id << ")" << std::endl;
//		}
//	} while (n != -1);  // -1 表示缓冲已达末尾。
//
//						// 往缓冲里再放一个 -1，这样其他消费者才能结束。
//	g_buffer.Produce(-1);
//}
//
//int main() {
//	std::vector<std::thread> threads;
//
//	threads.push_back(std::thread(&Producer));
//	threads.push_back(std::thread(&Consumer));
//	threads.push_back(std::thread(&Consumer));
//	threads.push_back(std::thread(&Consumer));
//
//	for (auto& t : threads) {
//		t.join();
//	}
//
//	return 0;
//}

// exampe2
#include <thread>
#include <mutex>
#include <condition_variable>
#include<iostream>
#include<queue>

using namespace std;

class BoundedBuffer {
private:
	deque<int> buffer;
	int size = 10;
	mutex m;
	condition_variable cv;
public:
	BoundedBuffer(int size_):size(size_) {};
	void produce(int num) {
		
		unique_lock<mutex> lock(m);
		std::thread::id thread_id = std::this_thread::get_id();
		cv.wait(lock, [this]() {return buffer.size()<size; });
		buffer.push_back(num);
		cout << "produce " << num << " threadid: " << thread_id << '\n';
		cv.notify_all();
	}
	int consume() {
		
		unique_lock<mutex> lock(m);
		std::thread::id thread_id = std::this_thread::get_id();
		cv.wait(lock, [this]() {return buffer.size()>0; });
		int res = buffer.front(); buffer.pop_front();
		cout << "consume " << res << " threadid: " << thread_id  << '\n';
		cv.notify_all();
		return res;
	}
};

int main() {
	BoundedBuffer bf(4);
	auto producer = [&]() {
		for (int i = 0; i<10; ++i) bf.produce(i);
	};
	auto consumer = [&]() {
		for (int i = 0; i<10; ++i) bf.consume();
	};
	auto consumer1 = [&]() {
		for (int i = 0; i<10; ++i) bf.consume();
	};
	thread producer_thread(producer);
	thread consumer_thread(consumer);

	producer_thread.join();
	consumer_thread.join();
	return 0;
}