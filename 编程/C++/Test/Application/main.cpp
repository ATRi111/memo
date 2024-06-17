#include<iostream>
#include<string>
#include"library.h"
#include"..\Dependencies\glfw3.h"
using namespace std;

class Test
{
public:
	int* a;
	Test()
	{
		a = new int(0);
		cout << "Ä¬ÈÏ" << endl;
	}
	Test(const Test& t)
	{
		a = new int(*(t.a));
		cout << "¿½±´" << endl;
	}
	Test(Test&& t) noexcept
	{
		a = t.a;
		t.a = nullptr;
		cout << "ÒÆ¶¯" << endl;
	}
	~Test()
	{
		delete a;
		cout << "Îö¹¹" << endl;
	}
};

Test F(Test& t)
{
	cout << 2 << endl;
	cout << 3 << endl;
	return t;
}

int main()
{
	Test t0;
	{
		cout << 1 << endl;
		Test t = F(t0);

		cout << 4 << endl;
	}
	cout << 5 << endl;
}