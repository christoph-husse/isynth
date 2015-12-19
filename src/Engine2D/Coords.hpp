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

#pragma once

#include "Engine2D.hpp"

namespace Engine2D
{
	struct fCoord
	{
		float x, y;

		fCoord() : x(0), y(0) { }

		fCoord(float x, float y) : x(x), y(y)
		{

		}

		fCoord(int x, int y) : x((float)x), y((float)y)
		{

		}
	};

	struct fRect
	{
		float x, y, w, h;

		fRect() : x(0), y(0), w(0), h(0) { }

		fRect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h)
		{

		}

		template<class T1, class T2, class T3, class T4>
		fRect(T1 x, T2 y, T3 w, T4 h) : x((float)x), y((float)y), w((float)w), h((float)h)
		{

		}

		fRect(int x, int y, int w, int h) : x((float)x), y((float)y), w((float)w), h((float)h)
		{

		}

		fRect(fCoord pos, fCoord size) : x(pos.x), y(pos.y), w(size.x), h(size.y)
		{

		}

		operator SDL_Rect();

		operator fCoord()
		{
			return{ x, y };
		}
	};
}