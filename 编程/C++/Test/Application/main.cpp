#include<iostream>
#include<string>
#include"library.h"
#include"glfw3.h"
using namespace std;

#define DEBUGMODE 0

#if DEBUGMODE == 1
    #define LOG(x) cout << x << endl;
#else
    #define LOG(x)
#endif

int main()
{
 	int a = 1;
    LOG(a);
}