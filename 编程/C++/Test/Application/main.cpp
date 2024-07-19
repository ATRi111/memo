#include<iostream>
#include<vector>
#include<string>
#include<functional>
#include "Library.h"
using namespace std;

void Print(int a, int b, int c)
{
    cout << a << b << c << endl;
}

int main()
{
    function<void(int, int)> F = bind(&Print, placeholders::_2, 4, placeholders::_1);
    F(2, 3);    //342
    return 0;
}