#pragma once

class Library
{
public:
	const static int c = 5;
	static int s;
	int a;

	Library(int a)
	{
		this->a = a;
	}

	static void LogC();
	static void LogS();
	void LogA() const; 
	void Log(int num) const;
};