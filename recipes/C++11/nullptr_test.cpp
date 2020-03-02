//示例代码1.0 http://www.cnblogs.com/feng-sc/p/5710724.html
/*
 *  NULL 表示空指针，而是表示0；
 *  nullptr 表示的是空指针；
*/

#include<iostream>
class Test
{
public:
	void TestWork(int index)
	{
		std::cout << "TestWork 1" << std::endl;
	}
	void TestWork(int * index)
	{
		std::cout << "TestWork 2" << std::endl;
	}
};

int main()
{
	Test test;
	test.TestWork(NULL); // TestWork 1
	test.TestWork(nullptr); // TestWork 2
	return 0;
}