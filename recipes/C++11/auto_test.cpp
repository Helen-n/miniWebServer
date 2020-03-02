//示例代码1.0 http://www.cnblogs.com/feng-sc/p/5710724.html
/*
 *  A、auto关键字能做什么？
 *    只是使用auto的时候，编译器根据上下文情况，确定auto变量的真正类型。
 *  B、auto不能做什么？
 *    auto作为函数返回值时，只能用于定义函数，不能用于声明函数。
*/

#include <iostream>
auto AddTest(int a, int b)
{
	return a + b;
}
int main()
{
	auto index = 10;
	auto str = "abc";
	auto ret = AddTest(1, 2);
	std::cout << "index:" << index << std::endl;
	std::cout << "str:" << str << std::endl;
	std::cout << "res:" << ret << std::endl;
}