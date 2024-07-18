#include<iostream>
#include<vector>
#include<string>
#include<functional>
#include "Library.h"
using namespace std;

int sum(int a, int b) 
{
    return a + b;
}

int main() 
{
    auto sum_five = bind(sum, 5, placeholders::_1);   //将sum函数的第一个参数固定为5，以此生成一个新的可调用对象
    sum_five(10);
    return 0;
}