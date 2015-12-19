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
	class BinaryReader final : NonCopyable, NonMovable
	{
	private:
		std::istream& stream;

		template<class T>
		T ReadInteger()
		{
			T res = 0;
			for (int i = 0; i < sizeof(T); i++)
			{
				res |= ((T)ReadByte()) << (i * 8);
			}
			return res;
		}

	public:
		BinaryReader(std::istream& stream) : stream(stream)
		{
		}

		unsigned char ReadByte()
		{
			unsigned char c;
			stream.read((char*)&c, 1);
			return c;
		}

		bool ReadBoolean() { return ReadByte() != 0; }

		std::string ReadString() 
		{ 
			auto len = ReadUInt16();
			auto res = std::string(len, (char)0);
			for (int i = 0; i < len; i++)
			{
				res[i] = (char)ReadByte();
			}
			return res;
		}
		
		float ReadFloat() 
		{ 
			auto v = ReadUInt32();
			return *((float*)(&v)); 
		}

		double ReadDouble()
		{
			auto v = ReadUInt64();
			return *((double*)(&v));
		}

		int16_t ReadInt16() { return ReadInteger<int16_t>(); }
		uint16_t ReadUInt16() { return ReadInteger<uint16_t>(); }
		int32_t ReadInt32() { return ReadInteger<int32_t>(); }
		uint32_t ReadUInt32() { return ReadInteger<uint32_t>(); }
		int64_t ReadInt64() { return ReadInteger<int64_t>(); }
		uint64_t ReadUInt64() { return ReadInteger<uint64_t>(); }
	};
}