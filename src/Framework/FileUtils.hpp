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
	class FileUtils final
	{
	public:
		static std::shared_ptr<std::vector<unsigned char> > ReadAllBytes(std::string fileName)
		{
			std::ifstream file;
			file.open(fileName.c_str(), std::ios::binary | std::ios::in);
			if (!file.is_open())
				THROW FileNotFoundException("Could not open audio file: \"", fileName, "\".");

			file.seekg(0, file.end);
			auto length = file.tellg();
			file.seekg(0, file.beg);

			auto buffer = std::make_shared<std::vector<unsigned char>>((size_t)length);
			file.read((char*)buffer->data(), buffer->size());

			return buffer;
		}
	};
}