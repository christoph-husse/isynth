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
#include "OggVorbis.hpp"

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

struct ogg_file
{
	char* curPtr;
	char* filePtr;
	size_t fileSize;
};

size_t AR_readOgg(void* dst, size_t size1, size_t size2, void* fh)
{
	ogg_file* of = reinterpret_cast<ogg_file*>(fh);
	size_t len = size1 * size2;
	if (of->curPtr + len > of->filePtr + of->fileSize)
	{
		len = of->filePtr + of->fileSize - of->curPtr;
	}
	memcpy(dst, of->curPtr, len);
	of->curPtr += len;
	return len;
}

int AR_seekOgg(void *fh, ogg_int64_t to, int type) {
	ogg_file* of = reinterpret_cast<ogg_file*>(fh);

	switch (type) {
	case SEEK_CUR:
		of->curPtr += to;
		break;
	case SEEK_END:
		of->curPtr = of->filePtr + of->fileSize - to;
		break;
	case SEEK_SET:
		of->curPtr = of->filePtr + to;
		break;
	default:
		return -1;
	}
	if (of->curPtr < of->filePtr) {
		of->curPtr = of->filePtr;
		return -1;
	}
	if (of->curPtr > of->filePtr + of->fileSize) {
		of->curPtr = of->filePtr + of->fileSize;
		return -1;
	}
	return 0;
}

int AR_closeOgg(void* fh)
{
	return 0;
}

long AR_tellOgg(void *fh)
{
	ogg_file* of = reinterpret_cast<ogg_file*>(fh);
	return (of->curPtr - of->filePtr);
}

namespace OggVorbis
{
	struct OggVorbisPimpl
	{
		ov_callbacks callbacks;
		ogg_file file;
		OggVorbis_File ov;
	};

	OggVorbisFile::~OggVorbisFile()
	{
		ClosePimpl();
	}

	void OggVorbisFile::ClosePimpl()
	{
		if (pimpl != nullptr)
			ov_clear(&pimpl->ov);

		delete pimpl;
		pimpl = nullptr;
	}

	OggVorbisFile::OggVorbisFile(std::shared_ptr<std::vector<unsigned char>> oggFileBytes) : fileBytes(oggFileBytes)
	{
		pimpl = new OggVorbisPimpl();
		memset(pimpl, 0, sizeof(OggVorbisPimpl));

		pimpl->file.curPtr = pimpl->file.filePtr = (char*)oggFileBytes->data();
		pimpl->file.fileSize = oggFileBytes->size();

		pimpl->callbacks.read_func = AR_readOgg;
		pimpl->callbacks.seek_func = AR_seekOgg;
		pimpl->callbacks.close_func = AR_closeOgg;
		pimpl->callbacks.tell_func = AR_tellOgg;

		if (ov_open_callbacks((void *)&pimpl->file, &pimpl->ov, nullptr, -1, pimpl->callbacks) != 0)
			THROW Framework::ArgumentException("Could not open OGG file.");

		auto vi = ov_info(&pimpl->ov, -1);

		sampleRate = vi->rate;
		channelCount = vi->channels;
		bitRate = vi->bitrate_nominal;
		sampleCount = ov_pcm_total(&pimpl->ov, -1) * channelCount;

		cache.reserve(sampleCount);

#if defined(IS_EMSCRIPTEN) || defined(DEBUG)
		// javascript/DEBUG is too slow for realtime decoding of a huge amount of
		// OGG tracks simultaneously, so we do it in advance for smooth playback.
		Prefetch(sampleCount);
#else
		// this will initialize the decoder and prevent smaller lagspikes during playback.
		Prefetch(1);
#endif
	}

	void OggVorbisFile::Prefetch(int sampleCountFromBeginning)
	{
		if (sampleCountFromBeginning < 0) 
			THROW Framework::ArgumentException();

		if (pimpl != nullptr)
		{
			if (sampleCountFromBeginning > (int)cache.size())
			{
				// try to read more samples from source
				std::array<int16_t, 1024> tmp;
				int n = 1;

				while ((n > 0) && (sampleCountFromBeginning > (int)cache.size()))
				{
					n = ov_read(&pimpl->ov, (char*)tmp.data(), tmp.size() * sizeof(tmp[0]), 0, sizeof(tmp[0]), 1, nullptr) / sizeof(tmp[0]);

					cache.reserve(cache.size() + n);
					for (int i = 0; i < n; i++)
						cache.push_back(tmp[i]);
				}
			}
		}
	}
}