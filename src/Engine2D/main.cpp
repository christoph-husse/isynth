/*
The MIT License (MIT)

Copyright (c) 2014 Christoph Husse

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "stdafx.h"
#include "Engine2D.hpp"

namespace Engine2D
{
	class MainEntryPoint
	{
	public:
		MainEntryPoint(std::vector<std::string> args)
		{
			Engine2D::Application::RunStatic(args);
		}
	};
}



#ifdef WIN32

	#include <windows.h>
	#include <stdio.h>
	#include <shellapi.h>

	int CALLBACK WinMain(
		_In_  HINSTANCE hInstance,
		_In_  HINSTANCE hPrevInstance,
		_In_  LPSTR lpCmdLine,
		_In_  int nCmdShow
		)
	{
	#ifdef DEBUG
		// enable debug heap
		int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
		// tmpFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
		_CrtSetDbgFlag(tmpFlag);
	#endif

		// convert command line arguments
		std::vector<std::string> args;

		int argc = 0;
		auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);

		for (int i = 0; i < argc; i++)
		{
			int len = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
			std::string str(len - 1, (char)0);
			WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, (LPSTR)str.data(), (int)str.size(), nullptr, nullptr);
			args.push_back(str);
		}

		Engine2D::MainEntryPoint{ args };
	}
#else

	int main(int argc, char** argv)
	{
		std::vector<std::string> args;

		for (int i = 0; i < argc; i++)
		{
			args.emplace_back(argv[i]);
		}

		Engine2D::MainEntryPoint{ args };

		return 0;
	}

#endif
