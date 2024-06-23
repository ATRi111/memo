#include<iostream>
#include<vector>
#include<string>
#include "Library.h"
using namespace std;


int main()
{
	string s1 = "Hello";
	string s2 = "World";
	cout << &s2 << endl;
	s2 += s1;
	cout << &s2 << endl;
}