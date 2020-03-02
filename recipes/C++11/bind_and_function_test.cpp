/*
 * function类似函数指针，但是比函数指针安全，而且可以绑定成员函数
 * bind 主要用于绑定生成目标函数，一般用于生成的回调函数
*/
#include <iostream>
#include <functional>
using namespace std;
class Func
{
public:
	static void  func6(int numa, int numb, int numc)
	{
		cout << numa << " " << numb << " " << numc << endl;
	}
	void func2(int numa, int numb, int numc, std::string name)
	{
		cout << numa << " " << numb << " " << numc << " " << name.c_str() << endl;
	}
};

void callFunc(std::function<void(int a, int b)> call)
{
	call(1, 2);
}
void func1(int numa, int numb, int numc)
{
	std::cout << numa << " " << numb << " " << numc << endl;
}
int main()
{
	callFunc(std::bind(func1, std::placeholders::_1, std::placeholders::_2, 3));      // 绑定全局函数
	callFunc(std::bind(func1, std::placeholders::_2, std::placeholders::_1, 3));
	callFunc(std::bind(func1, std::placeholders::_2, 3, std::placeholders::_1));

	callFunc(std::bind(Func::func6, std::placeholders::_1, std::placeholders::_2, 3)); // 绑定static函数

	Func func;
	callFunc(std::bind(&Func::func2,func, std::placeholders::_1, std::placeholders::_2, 3, "name")); // 绑定成员函数
	return 0;
}
