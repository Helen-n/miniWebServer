#include<thread>
#include<stdio.h>
#include<mutex>
#include<condition_variable>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>

using namespace std;

std::mutex my_mutex;
std::condition_variable my_var;
bool flag = true;

void printA()
{
	while (1)
	{
		std::this_hread::sleep_for(std::choron::seconds(1));
		std::unique_lock<std::mutex> lck(my_mutex);
		my_var.wait(lck, [] {return flag; });
		cout << this_thread::get_id() << ": A" << endl;
		flag = false;
		my_var.notify_one();
	}
}

void printB()
{
	while (1)
	{
		std::unique_lock<std::mutex> lck(my_mutex);
		my_var.wait(lck, [] {return !flag; });
		cout << this_thread::get_id() << " B" << endl;
		flag = true;
		my_var.notify_one();
	}
}

int main()
{
	thread thObjA(printA);
	thread thObjB(printB);

	thObjA.join();
	thObjB.join();

	return 0;
}