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

#include "Framework.hpp"

namespace Framework
{
	struct Array2DIndex final
	{
		int x = 0, y = 0;

		Array2DIndex() {}
		Array2DIndex(int x, int y) : x(x), y(y) { }

		bool operator ==(Array2DIndex other)
		{
			return (x == other.x) && (y == other.y);
		}

		bool operator !=(Array2DIndex other)
		{
			return (x != other.x) || (y != other.y);
		}

		Array2DIndex operator+ (Array2DIndex other)
		{
			return{ (x + other.x), (y + other.y) };
		}

		Array2DIndex operator- ()
		{
			return{ -x, -y };
		}

		Array2DIndex operator- (Array2DIndex other)
		{
			return{ (x - other.x), (y - other.y) };
		}

		Array2DIndex operator* (int other)
		{
			return { (x * other), (y * other) };
		}
	};

	template<class T>
	class Array2D final
	{
	private:
		std::vector<T> entries;
		int width, height;

	public:

		Array2D(int width, int height) : width(std::max(1, (int)width)), height(std::max(1, (int)height))
		{
			entries.resize(width * height);
		}

		T operator()(Array2DIndex index) const
		{
			return entries[index.x + index.y * width];
		}

		T& operator()(Array2DIndex index)
		{
			return entries[index.x + index.y * width];
		}

		template<class TCallback_void_Array2DIndex>
		void operator()(Array2DIndex index, TCallback_void_Array2DIndex callback)
		{
			callback((*this)(index));
		}

		template<class TCallback_void_T>
		void Iterate(TCallback_void_T callback)
		{
			for (T& ref : entries)
				callback(ref);
		}

		template<class TCallback_void_T>
		void Iterate(TCallback_void_T callback) const
		{
			for (const T& ref : entries)
				callback(ref);
		}
	};
}
