// DbgGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <sstream>

int main()
{
	OutputDebugString(L"DbgGenerator Starting Up");
	OutputDebugString(L"Counting to 10");

	for (size_t i = 0; i < 10; i++)
	{
		Sleep(1000);
		std::wstringstream out;
		out << i;
		OutputDebugString(out.str().c_str());
	}


	OutputDebugString(L"DbgGenerator Shutting Down");
    return 0;
}

