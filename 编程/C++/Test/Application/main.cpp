#include<iostream>
#include<vector>
#include<algorithm>
#include<functional>
#include "Library.h"
using namespace std;

struct Vector2
{
	float x, y;
	float Magnitude()
	{
		return sqrtf(x * x + y * y);
	}
};
struct Vector3
{
	float x, y, z;
	float Magnitude()
	{
		return sqrtf(x * x + y * y + z * z);
	}
};
struct Vector4
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float r, g, b, a;	//取别名
		};
		struct
		{
			Vector3 v3;			//视为Vector3，不访问w
		};
		struct
		{
			Vector2 v2a, v2b;	//视为两个Vector2
		};
	};
};

int main()
{
	Vector4 v4 = { 1.0f,2.0f,2.0f,4.0f };
	cout << v4.v3.Magnitude() << endl;
	v4.z = 3.0f;
	cout << v4.v2a.Magnitude() << endl;
	cout << v4.v2b.Magnitude() << endl;
}
