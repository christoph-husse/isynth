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

#include "SDL.h"

#ifdef IS_EMSCRIPTEN
	#include "SDL_image.h"
#else
	#include "stb_image.h"
#endif

using namespace Framework;

namespace Engine2D
{
#ifdef IS_EMSCRIPTEN
	
	Surface::Surface(std::string path)
	{
		surface = IMG_Load(path.c_str());
		if (surface == nullptr)
			THROW GraphicException("Unable to load image \"", path, "\": ", IMG_GetError());
	}

#else

	Surface::Surface(std::string path)
	{
		int width, height, channels;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
		if (data == nullptr)
			THROW GraphicException("Unable to load image \"", path, "\"!");

		finally data_releaser([&]() { stbi_image_free(data);  });

		surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
		if (surface == nullptr)
			THROW GraphicException("Unable to load image \"", path, "\"! SDL Error: ", SDL_GetError());

		SDL_LockSurface(surface);
		auto pixels = (Uint32*)surface->pixels;

		for (int y = 0, iSrc = 0, iDst = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Uint32 p = 0;
				p |= ((Uint32)data[iSrc++]) << 16;
				p |= ((Uint32)data[iSrc++]) << 8;
				p |= ((Uint32)data[iSrc++]) << 0;
				p |= ((Uint32)data[iSrc++]) << 24;

				pixels[iDst++] = p;
			}
		}

		SDL_UnlockSurface(surface);
	}
#endif

	int Surface::GetWidth() { return surface->w; }
	int Surface::GetHeight() { return surface->h; }

	Surface::~Surface()
	{
		try
		{
			if (surface != nullptr)
				SDL_FreeSurface(surface);
		}
		catch (...)
		{
		}

		surface = nullptr;
	}
}