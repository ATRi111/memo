#include<iostream>
#include"library.h"

using namespace std;

int Library::s = 1;
void Library::LogC()
{
	cout << Library::c << endl;
}

void Library::LogS()
{
	cout << Library::s << endl;
}

void Library::LogA() const
{
	cout << Library::a << endl;
}

void Library::Log(int num) const
{
    static int prev = 0;
    cout << prev << endl;
    prev = num;
}