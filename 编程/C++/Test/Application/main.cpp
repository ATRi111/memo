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
		cout << "Ĭ��" << endl;
	}
	Test(const Test& t)
	{
		a = new int(*(t.a));
		cout << "����" << endl;
	}
	Test(Test&& t) noexcept
	{
		a = t.a;
		t.a = nullptr;
		cout << "�ƶ�" << endl;
	}
	~Test()
	{
		delete a;
		cout << "����" << endl;
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