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

#include "stdafx.h"
#include "Engine2D.hpp"

struct SDL_Window;
struct SDL_Rect;
struct SDL_Renderer;

namespace Engine2D
{
	class ScreenSurface final
	{
	private:
		friend class Environment;

		std::unordered_map<SDL_Surface*, SDL_Texture*> textures;
		SDL_Surface* sdl_1_2_surface = nullptr;
		static SDL_Window* sdl_2_0_surface;
		SDL_Renderer* sdl_2_0_renderer = nullptr;

		SDL_Texture* ImportTexture(SDL_Surface* surface);

	public:

		void Clear(int r, int g, int b);
		void Present();
		void DrawTexture(SDL_Surface* texture, fRect src, fRect dst);
		void DrawTexture(SDL_Surface* texture, fRect dst);

		ScreenSurface(AppSettings settings);

		~ScreenSurface();
	};

	enum class GestureType
	{
		Unknown = 0,
		Touch = 1,
		Release = 2,
		Drag = 3,
	};

	class GestureEvent final
	{
	private:

	public:
		GestureType type;
		int x, y, fingerId;
	};

	class Touch
	{
	public:
		fCoord origin;
		fCoord current;

		void Start(fCoord coords);
		void Release(fCoord coords);
		void Drag(fCoord coords);
	};

	class EventObserver
	{
	private:
		std::vector<std::function<void(const GestureEvent&)>> handlers_OnGestureEvent;
		std::vector<std::function<void()>> handlers_OnQuit;
	public:
		virtual ~EventObserver() { }

		void RegisterOnGestureEvent(std::function<void(const GestureEvent&)> handler) { handlers_OnGestureEvent.push_back(handler); }
		void RegisterOnQuit(std::function<void()> handler) { handlers_OnQuit.push_back(handler); }

		virtual void OnQuit() { for (auto& handler : handlers_OnQuit) handler(); }
		virtual void OnGestureEvent(const GestureEvent& gesture) { for (auto& handler : handlers_OnGestureEvent) handler(gesture); }
	};

	class EventMapper final
	{
	public:
		static void ProcessEvents(EventObserver* observer);
	};

	class Environment final
	{
	public:
		static int GetCanvasWidth();
		static int GetCanvasHeight();
	};

	typedef void PcmAudioCallback(std::vector<int16_t>&);

	class PcmPlayback final
	{
	private:
		friend class ScreenSurface;

		static PcmAudioCallback* sdl_AudioCallback;
		static void FillAudioBuffer(void* udata, uint8_t* stream, int len);
	public:

		static void SetCallback(PcmAudioCallback* handler);
	};
}