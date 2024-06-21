#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
#include "Library.h"
using namespace std;

class Parent
{
public:
	int age;
	Parent()
	{
		age = 0;
	}
	Parent(int age)
	{
		this->age = age;
	}
	virtual ~Parent()
	{

	}
	void F()
	{
		cout << "Parent" << endl;
	}
};
class Child : public Parent
{
public:
	Child()
	{

	}
	Child(int age) :Parent(age)
	{

	}
	~Child()
	{

	}
	void F()
	{
		Parent::F();
		cout << "Child" << endl;
	}
};

int main()
{

}