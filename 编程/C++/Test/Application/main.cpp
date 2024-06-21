#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
#include "Library.h"
using namespace std;

class Test
{
public:
	int* a;
	Test()
	{
		a = new int(0);
		cout << "默认" << endl;
	}
	Test(const Test& t)
	{
		a = new int(*(t.a));
		cout << "拷贝" << endl;
	}
	Test(Test&& t) noexcept
	{
		a = t.a;
		t.a = nullptr;
		cout << "移动" << endl;
	}
	~Test()
	{
		delete a;
		cout << "析构" << endl;
	}
};

const int F()
{
	return 1;
}

int main()
{
	{
		Test t1;					//默认
		Visit(move(t1));
	}								//析构
}