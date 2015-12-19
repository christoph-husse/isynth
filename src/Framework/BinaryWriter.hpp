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
	class BinaryWriter final : NonCopyable, NonMovable
	{
	private:
		std::ostream& stream;

		template<class T>
		void WriteInteger(T value)
		{
			for (int i = 0; i < sizeof(T); i++)
			{
				WriteByte((unsigned char)((value >> (i * 8)) & 0xFF));
			}
		}

	public:
		BinaryWriter(std::ostream& stream) : stream(stream)
		{
		}

		void WriteByte(unsigned char value)
		{
			stream.write((char*)&value, 1);
		}

		void WriteBoolean(bool value) { WriteByte(value ? 1 : 0); }

		void WriteString(std::string value)
		{
			auto len = (uint16_t)std::min(value.size(), (size_t)std::numeric_limits<uint16_t>::max());

			WriteUInt16(len);
			
			for (int i = 0; i < len; i++)
			{
				WriteByte((unsigned char)value[i]);
			}
		}

		void WriteFloat(float value)
		{
			WriteUInt32(*((uint32_t*)&value));
		}

		void WriteDouble(double value)
		{
			WriteUInt64(*((uint64_t*)&value));
		}

		void WriteInt16(int16_t value) { WriteInteger<int16_t>(value); }
		void WriteUInt16(uint16_t value) { WriteInteger<uint16_t>(value); }
		void WriteInt32(int32_t value) { WriteInteger<int32_t>(value); }
		void WriteUInt32(uint32_t value) { WriteInteger<uint32_t>(value); }
		void WriteInt64(int64_t value) { WriteInteger<int64_t>(value); }
		void WriteUInt64(uint64_t value) { WriteInteger<uint64_t>(value); }
	};
}