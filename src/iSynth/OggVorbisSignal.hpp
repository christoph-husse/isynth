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

#include "iSynth.hpp"

namespace iSynth
{
	class OggVorbisSignal : public Signal
	{
	private:
		std::shared_ptr<OggVorbis::OggVorbisFile> file;
		uint64_t lastSignalTime = 0, startTime = 0;
	public:
		virtual void NextSample(uint64_t c2i44100, std::vector<int16_t>& pcmBuffer) override
		{
			lastSignalTime = c2i44100;
			if (startTime == 0)
				startTime = c2i44100;

			int sampleIndex = (int)(c2i44100 - startTime);
			int sampleCount = (int)pcmBuffer.size();

			file->Prefetch(sampleIndex + sampleCount);
			sampleCount = std::min(sampleCount, (int)(file->GetSampleCount() - sampleIndex));

			if (sampleCount <= 0)
				sampleCount = 0;
			else
				std::copy(file->cbegin() + sampleIndex, file->cbegin() + sampleIndex + sampleCount, pcmBuffer.begin());

			std::fill_n(pcmBuffer.begin() + sampleCount, pcmBuffer.size() - sampleCount, 0);
		}

		bool HasFinished() const
		{
			if (file == nullptr)
				return true;

			return lastSignalTime > startTime + file->GetSampleCount();
		}

		OggVorbisSignal()
		{

		}

		OggVorbisSignal(std::shared_ptr<OggVorbis::OggVorbisFile> file) : file(file)
		{
		}
	};
}