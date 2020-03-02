//示例代码1.0 http://www.cnblogs.com/feng-sc/p/5710724.html
/*
  * 使用weak_ptr解决shared_ptr自引用的问题
   * weak_ptr并不会使得shared_ptr 基数加1，在使用时weak_pre.lock()可以提升为shared_ptr
  */
#include <memory>
#include <iostream>
class TestB;
class TestA
{
public:
	TestA()
	{
		std::cout << "TestA()" << std::endl;
	}
	void ReferTestB(std::shared_ptr<TestB> test_ptr)
	{
		m_TestB_Ptr = test_ptr;
	}
	void TestWork()
	{
		std::cout << "~TestA::TestWork()" << std::endl;
	}
	~TestA()
	{
		std::cout << "~TestA()" << std::endl;
	}
private:
	std::weak_ptr<TestB> m_TestB_Ptr;
};

class TestB
{
public:
	TestB()
	{
		std::cout << "TestB()" << std::endl;
	}

	void ReferTestB(std::shared_ptr<TestA> test_ptr)
	{
		m_TestA_Ptr = test_ptr;
	}
	void TestWork()
	{
		std::cout << "~TestB::TestWork()" << std::endl;
	}
	~TestB()
	{
		std::shared_ptr<TestA> tmp = m_TestA_Ptr.lock();
		tmp->TestWork();
		std::cout << "2 ref a:" << tmp.use_count() << std::endl;
		std::cout << "~TestB()" << std::endl;
	}
	std::weak_ptr<TestA> m_TestA_Ptr;
};


int main()
{
	std::shared_ptr<TestA> ptr_a = std::make_shared<TestA>();
	std::shared_ptr<TestB> ptr_b = std::make_shared<TestB>();
	ptr_a->ReferTestB(ptr_b);
	ptr_b->ReferTestB(ptr_a);
	std::cout << "1 ref a:" << ptr_a.use_count() << std::endl; // 1
	std::cout << "1 ref b:" << ptr_a.use_count() << std::endl; // 1
	return 0;
}