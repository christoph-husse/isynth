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

#ifndef IS_EMSCRIPTEN

#include "SDL.h"

namespace Engine2D
{
	SDL_Window* ScreenSurface::sdl_2_0_surface = nullptr;
	PcmAudioCallback* PcmPlayback::sdl_AudioCallback = nullptr;

	void PcmPlayback::FillAudioBuffer(void* udata, uint8_t* stream, int len)
	{
		static std::vector<int16_t> buffer(1024 * 2);

		assert((len % sizeof(buffer[0])) == 0);

		buffer.resize(len / sizeof(buffer[0]));

		if (PcmPlayback::sdl_AudioCallback == nullptr)
			memset(stream, 0, len);
		else
		{
			memset(buffer.data(), 0, len);

			PcmPlayback::sdl_AudioCallback(buffer);

			memcpy(stream, buffer.data(), std::min(buffer.size() * sizeof(buffer[0]), (size_t)len));
		}
	}

	ScreenSurface::ScreenSurface(AppSettings settings)
	{
		if (sdl_2_0_surface != nullptr)
			THROW GraphicException("Unable to Init Engine2D: Already initialized!");

		auto flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

		if (SDL_Init(flags) < 0)
			THROW GraphicException("Unable to Init Engine2D: ", SDL_GetError());

		SDL_AudioSpec wanted = {};

		wanted.freq = 44100;
		wanted.format = AUDIO_S16;
		wanted.channels = 2; 
		wanted.samples = 1024; 
		wanted.callback = PcmPlayback::FillAudioBuffer;
		wanted.userdata = nullptr;

		if (SDL_OpenAudio(&wanted, nullptr) < 0) 
			THROW GraphicException("Couldn't open audio: ", SDL_GetError());

		SDL_PauseAudio(0);

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

		sdl_2_0_surface = SDL_CreateWindow(
			settings.windowTitle.c_str(),
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			settings.windowWidth, settings.windowHeight,
			SDL_WINDOW_RESIZABLE);

		sdl_2_0_renderer = SDL_CreateRenderer(sdl_2_0_surface, -1, 0);
	}

	void ScreenSurface::Present()
	{
		SDL_RenderPresent(sdl_2_0_renderer);
	}

	void ScreenSurface::Clear(int r, int g, int b)
	{
		SDL_SetRenderDrawColor(sdl_2_0_renderer, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)255);
		SDL_RenderClear(sdl_2_0_renderer);
	}

	SDL_Texture* ScreenSurface::ImportTexture(SDL_Surface* surface)
	{
		auto it = textures.find(surface);
		if (it != textures.end())
			return it->second;

		return textures[surface] = SDL_CreateTextureFromSurface(sdl_2_0_renderer, surface);
	}

	void ScreenSurface::DrawTexture(SDL_Surface* texture, fRect src, fRect dst)
	{
		SDL_Rect _src = src, _dst = dst;
		SDL_RenderCopy(sdl_2_0_renderer, ImportTexture(texture), &_src, &_dst);
	}

	void ScreenSurface::DrawTexture(SDL_Surface* texture, fRect dst)
	{
		SDL_Rect _dst = dst;
		SDL_RenderCopy(sdl_2_0_renderer, ImportTexture(texture), nullptr, &_dst);
	}

	ScreenSurface::~ScreenSurface()
	{
		SDL_CloseAudio();

		for (auto e : textures)
			SDL_DestroyTexture(e.second);
		textures.clear();

		if (sdl_2_0_renderer != nullptr)
			SDL_DestroyRenderer(sdl_2_0_renderer);

		if (sdl_2_0_surface != nullptr)
			SDL_DestroyWindow(sdl_2_0_surface);

		sdl_2_0_renderer = nullptr;
		sdl_2_0_surface = nullptr;
		sdl_1_2_surface = nullptr;
	}

	void EventMapper::ProcessEvents(EventObserver* observer)
	{
		static int lastMouseButtonPressed = -1;
		SDL_Event event;


		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: 
				observer->OnQuit();
				return;
			}

			GestureEvent gesture;

			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN: 
			case SDL_MOUSEBUTTONUP:
				{
					gesture.type = (event.type == SDL_MOUSEBUTTONDOWN) ? GestureType::Touch : GestureType::Release;
					gesture.x = event.button.x;
					gesture.y = event.button.y;
					gesture.fingerId = 10 + event.button.button;

					if (event.type == SDL_MOUSEBUTTONDOWN)
						lastMouseButtonPressed = gesture.fingerId;
					else
						lastMouseButtonPressed = -1;

					observer->OnGestureEvent(gesture);
				}break;
			case SDL_MOUSEMOTION: 
				{
					if (lastMouseButtonPressed < 0)
						continue;

					gesture.type = GestureType::Drag;
					gesture.x = event.motion.x;
					gesture.y = event.motion.y;
					gesture.fingerId = lastMouseButtonPressed;

					observer->OnGestureEvent(gesture);
				}break;
			default:
				continue;
			}
		}
	}

	int Environment::GetCanvasWidth()
	{
		if (ScreenSurface::sdl_2_0_surface == nullptr)
			return 0;

		int result = 0;
		SDL_GetWindowSize(ScreenSurface::sdl_2_0_surface, &result, nullptr);
		return result;
	}

	int Environment::GetCanvasHeight()
	{
		if (ScreenSurface::sdl_2_0_surface == nullptr)
			return 0;

		int result = 0;
		SDL_GetWindowSize(ScreenSurface::sdl_2_0_surface, nullptr, &result);
		return result;
	}

	void PcmPlayback::SetCallback(PcmAudioCallback* handler)
	{
		sdl_AudioCallback = handler;
	}
}

#endif