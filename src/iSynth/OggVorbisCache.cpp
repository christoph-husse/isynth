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
#include "OggVorbisCache.hpp"

using namespace Engine2D;

namespace iSynth
{
	std::unordered_map<std::string, OggVorbisCache::CacheEntry> OggVorbisCache::cachedFiles;

	std::shared_ptr<OggVorbis::OggVorbisFile> OggVorbisCache::LoadFile(std::string fileName)
	{
		auto it = cachedFiles.find(fileName);
		if (it != cachedFiles.end())
		{
			it->second.lastUse = Application::GetTicks();
			return it->second.file;
		}

		auto result = std::make_shared<OggVorbis::OggVorbisFile>(FileUtils::ReadAllBytes(fileName));

		if (result->GetSampleRate() != 44100)
			THROW Framework::ArgumentException("Only files with 44100 Hz sample rate are supported!");

		if (result->GetChannelCount() != 2)
			THROW Framework::ArgumentException("Only files with two channels are supported!");

		cachedFiles[fileName].file = result;
		cachedFiles[fileName].lastUse = Application::GetTicks();

		return result;
	}
}