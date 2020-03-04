
/*
一个生活中的例子：
张三：站在北京，等李四
李四：站在深圳，等张三
在c++中，
如果说我又两把锁（死锁这个问题，是由至少有两个互斥量才能产生）金锁，银锁
1.线程A执行的时候，这个线程先锁金锁，把金锁lock()成功，然后他去lock()银锁。。。
假如这个时候出现了上下文切换
2.线程B执行了，这个线程先锁银锁，因为银锁线程A还没执行，所以B银锁会成功，然后线程B要去Lock金锁。
此时死锁就产生了.
*/

#include<thread>
#include<iostream>
#include<mutex>
#include<list>

using namespace std;

class A {

public:
	//把收到的消息（玩家命令）送到一个队列中
	void inMsgRecvQueue() {
		for (int i = 0; i < 100000; i++)
		{
			cout << "inMsgRecvQueue()执行，插入一个元素" << endl;
			my_mutex1.lock();//实际工程中可能是这两个锁头不一定挨着
							 //.....................
			my_mutex2.lock();
			msgRecvQueue.push_back(i);//假设这个数字就是我收到的命令，我直接弄到消息队列里面去
			my_mutex2.unlock();
			my_mutex1.unlock();
		}
		return;
	}

	bool outMgLULProc(int& command) {


		my_mutex1.lock();
		my_mutex2.lock();

		std::lock(my_mutex1, my_mutex2);
		if (!msgRecvQueue.empty())
		{
			int command = msgRecvQueue.front();//返回第一个元素，
			msgRecvQueue.pop_front();//移除第一个元素，但不返回
									 //这里就考虑处理数据
			my_mutex1.unlock();
			my_mutex2.unlock();
			return true;
		}
		my_mutex1.unlock();
		my_mutex2.unlock();
		return false;
	}



	//把数据从消息队列中取出线程：
	void outMsgRecvQueue() {
		int command = 0;
		for (int i = 0; i < 100000; i++)
		{
			bool result = outMgLULProc(command);
			if (result == true)
			{
				cout << "outMsgRecvQueue()执行，取出一个元素";
			}
		}
		cout << "end" << endl;
	}
private:
	//这个就是共享数据
	std::list<int> msgRecvQueue;//容器，专门用于代表玩家给我们发送来的命令
	std::mutex my_mutex1;//创建一个互斥量
	std::mutex my_mutex2;//
};

int main()
{
	A a;
	thread threadObj1(&A::inMsgRecvQueue, &a);
	thread threadObj2(&A::outMgLULProc, &a);
	threadObj1.join();
	threadObj2.join();
	return 0;
}