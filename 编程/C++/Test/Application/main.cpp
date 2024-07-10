#include<iostream>
#include<vector>
#include<string>
#include "Library.h"
using namespace std;

struct A
{
	int num;
	A(int num)
		:num(num)
	{

	}
};

const A a0;
struct B
{
	const static int a1 = 1;
};

int main()
{
	B b;
	cout << b.a1.num << endl;
}