#include<iostream>
#include<vector>
#include<string>
#include<functional>
#include "Library.h"
using namespace std;

template<typename R, typename... Args >
class Function
{
public:
	R(*Func)(Args...);
	Function(R(*F)(Args...))
	{
		Func = F;
	}
};

int Transform(float f)
{
	return (int)f;
}

int main()
{
	Function<int,float> F1 = Transform;
}