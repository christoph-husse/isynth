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

	class Exception : public std::exception
	{
	protected:
		std::string message;
	public:

		template<class... StringConcat>
		Exception(std::string exceptionName, StringConcat&... args) : std::exception()
		{
			try
			{
				message = strconcat("[", exceptionName, "] ", args...);
			}
			catch (...)
			{
			}
		}

		char const* what() const throw()
		{
			if (message.empty())
				return "[Unknown exception cause.]";
			else
				return message.c_str();
		}
	};

#define DEFINE_EXCEPTION(name)																					\
	class name : public Framework::Exception																	\
	{																											\
	public:																										\
		template<class... StringConcat>																			\
		name(const StringConcat&... args) : Framework::Exception(#name, args...) {}								\
		name AddTrace(std::string file, std::string func, int line) {											\
			message = Framework::strconcat(																		\
				"###########################################\n",												\
				"EXCEPTION: ", message, "\n",																	\
				"   > File: \"", file, "\"\n",																	\
				"   > Function: \"", func, "\"\n",																\
				"   > Line: ", line, "\n",																		\
				"###########################################\n");												\
			std::cerr << std::endl << message << std::endl;														\
			return *this;																						\
		}																										\
	};
	
	class __exception_info
	{ 
	private:
		std::string file;
		std::string func;
		int line;
	public:
		__exception_info(std::string file, std::string func, int line) : file(file), func(func), line(line)
		{

		}

		template<class T>
		typename std::enable_if<std::is_base_of<Exception, T>::value && std::is_same<Exception, T>::value, typename std::remove_reference<T>::type>::type
		operator <<(T&& exception)
		{
			return exception.AddTrace(file, func, line);
		}

		template<class T>
		typename std::enable_if<!std::is_base_of<Exception, T>::value || !std::is_same<Exception, T>::value, typename std::remove_reference<T>::type>::type
		operator <<(T&& exception)
		{
			return std::move(exception);
		}
	};

	DEFINE_EXCEPTION(FileNotFoundException);
	DEFINE_EXCEPTION(IOException);
	DEFINE_EXCEPTION(InvalidDataException);
	DEFINE_EXCEPTION(ApplicationException);
	DEFINE_EXCEPTION(ArgumentException);
	DEFINE_EXCEPTION(ArgumentNullException);
	DEFINE_EXCEPTION(ArgumentOutOfRangeException);
	DEFINE_EXCEPTION(AssertionFailedException);

	#undef assert

	static inline void __assert_internal(std::string condStr, std::string file, std::string func, int line)
	{
		throw AssertionFailedException(
			" > Condition: \"", condStr, "\"\n", 
			"   > File: \"", file, "\"\n",
			"   > Function: \"", func, "\"\n",
			"   > Line: ", line, "\n");
	}

	#define assert(condition) \
		if (!(condition)) \
			__assert_internal(#condition, __FILE__, __FUNCTION__, __LINE__);

	// Fancy magic to enrich exception messages with source code information.
	#define THROW throw Framework::__exception_info(__FILE__, __FUNCTION__, __LINE__) <<

	template<class T>
	void RequireNonNull(const T& value)
	{
		if (value == nullptr)
			THROW ArgumentNullException();
	}
}
