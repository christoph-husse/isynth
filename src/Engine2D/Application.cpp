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

#ifdef IS_EMSCRIPTEN
	#include "emscripten/emscripten.h"
#endif

#include "SDL.h"

using namespace Framework;

namespace Engine2D
{
	Application* Application::instance = nullptr;
	static AtomicBool isAudioThreadRunning;

	fRect::operator SDL_Rect()
	{
		SDL_Rect r;
		r.x = (int)x;
		r.y = (int)y;
		r.w = (int)w;
		r.h = (int)h;
		return r;
	}

	Application::Application(AppSettings settings) : 
		settings(settings.MakeValid()), 
		screenSurface(std::make_shared<ScreenSurface>(settings))
	{
		observer.RegisterOnGestureEvent([=](const GestureEvent& gesture) { HandleOnGestureEvent(gesture); });
		observer.RegisterOnQuit([=]() { HandleOnQuit(); });

		if (instance != nullptr)
			THROW GraphicException("There is already an instance of an application running.");

		instance = this;
	}

	uint32_t Application::GetTicks()
	{
		return SDL_GetTicks();
	}

	Application::~Application()
	{
	}

	void Application::Init()
	{
		running = true;

		OnStartup();
	}

	void Application::AssertRunning()
	{
		if (!running)
			THROW ArgumentException("This method must not be called until OnStartup() is being invoked.");
	}

	void Application::ClearScreen(int r, int g, int b)
	{
		screenSurface->Clear(r, g, b);
	}

	void Application::Render()
	{
		ClearScreen(0, 0, 0);
		
		OnRender();

		screenSurface->Present();
	}

	void Application::Cleanup()
	{
		running = false;
		instance = nullptr;
		
		while (isAudioThreadRunning) 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		OnShutdown();
	}

	void Application::RunLoopIterationStatic()
	{
		instance->RunLoopIteration();
	}

	void Application::HandleOnQuit()
	{
		running = false;
	}

	void Application::RunLoopIteration()
	{
		// call OnFixedUpdate() to catch up with time
		uint32_t currentFixedTime = GetTicks();
		int i = 0;

		while (lastFixedFrameTime < currentFixedTime)
		{
			if (i++ > 1000)
			{
				// in case of PC sleep/hibernate, this loop could really hurt
				lastFixedFrameTime = GetTicks();

				// we can't get here more than once a second, so no further "spam" protection needed
				std::cerr << "[WARNING]: OnFixedUpdate() frames were lost." << std::endl;

				break;
			}

			OnFixedUpdate();
			lastFixedFrameTime += millisPerFixedFrame;
		}

		Render();

		EventMapper::ProcessEvents(&observer);
	}

	void Application::RunStatic(std::vector<std::string> args)
	{
#ifndef DEBUG
		try
		{
#endif
			if (instance != nullptr)
				instance->Run(args);
#ifndef DEBUG
		}
		catch (Exception& e)
		{
			// error message was already forwarded to stderr
		}
		catch (std::exception& e)
		{
			std::cerr << std::endl << "[FATAL ERROR]: Enexpected exception. Terminating program." << std::endl;
		}
#endif
	}

	void Application::Run(std::vector<std::string> args)
	{
		envArgs = args;

		Init();

		lastFixedFrameTime = GetTicks();
		millisPerFixedFrame = std::max(1u, (uint32_t)(1000.0f / std::max(1u, settings.fixedFramesPerSecond)));
		millisPerRenderFrame = std::max(1u, (uint32_t)(1000.0f / std::max(1u, settings.renderFramesPerSecond)));
		lastFrameTime = 0;

		PcmPlayback::SetCallback(HandleAudioCallbackStatic);
		SetLogicalViewport(settings.windowWidth, settings.windowHeight);

#ifdef IS_EMSCRIPTEN
		emscripten_set_main_loop(RunLoopIterationStatic, 30, 1);
#else
		while (running)
		{
			if (millisPerRenderFrame < GetTicks() - lastFrameTime)
			{
				lastFrameTime = GetTicks();
				RunLoopIterationStatic();
			}
			else
				SDL_Delay(std::max(1u, std::min(100u, settings.delayMillisIfIdle)));
		}

		Cleanup();
#endif
	}

	void Application::HandleAudioCallbackStatic(std::vector<int16_t>& pcmData)
	{
		if (instance != nullptr)
		{
			isAudioThreadRunning = true;

			instance->HandleAudioCallback(pcmData);
		}
		else
		{
			// notify that callback won't keep calling the member method from now on
			isAudioThreadRunning = false;
		}
	}

	int Application::GetAudioSamplerRate()
	{
		return 44100;
	}

	void Application::HandleAudioCallback(std::vector<int16_t>& pcmData)
	{
		if (!running)
			return;

		OnFillAudioBuffer(audioTimer, pcmData);
		audioTimer += pcmData.size();
	}

	void Application::HandleOnGestureEvent(GestureEvent gesture)
	{
		Touch touch;
		fCoord coord = CoordCanvasToLogical({ gesture.x, gesture.y, 0, 0 });

		switch(gesture.type)
		{
		case GestureType::Touch:

			touch = touches[gesture.fingerId];
			touch.Start(coord);
			touches[gesture.fingerId] = touch;
			OnTouchStart(touch);
			break;

		case GestureType::Release:
			touch = touches[gesture.fingerId];
			touch.Release(coord);
			touches.erase(touches.find(gesture.fingerId));
			OnTouchEnd(touch);
			break;

		case GestureType::Drag:
			touch = touches[gesture.fingerId];
			touch.Drag(coord);
			OnTouchDrag(touch);
			break;
		}
	}

	void Application::SetLogicalViewportToFill(float width, float height)
	{
		auto& v = viewport;

		v.logicalHeight = height;
		v.logicalWidth = width;
		v.canvasHeight = Environment::GetCanvasHeight();
		v.canvasWidth = Environment::GetCanvasWidth();
		v.logicalAspect = width / height;
		v.canvasAspect = v.canvasWidth / (float)v.canvasHeight;

		float widthWeight = (float)v.canvasWidth / v.logicalWidth;
		float heightWeight = (float)v.canvasHeight / v.logicalHeight;

		if (widthWeight < heightWeight)
		{
			// scale by height weight to fill canvas
			v.scale = 1 / heightWeight;
			float scaledWidth = v.logicalWidth / v.scale;

			v.transformX = (v.canvasWidth + scaledWidth) / 2 - scaledWidth;
			v.transformY = 0;
		}
		else
		{
			// scale by width weight to fill canvas
			v.scale = 1 / widthWeight;
			float scaledHeight = v.logicalHeight / v.scale;

			v.transformX = 0;
			v.transformY = (v.canvasHeight + scaledHeight) / 2 - scaledHeight;
		}
	}

	void Application::SetLogicalViewport(float width, float height)
	{
		auto& v = viewport;

		v.logicalHeight = height;
		v.logicalWidth = width;
		v.canvasHeight = Environment::GetCanvasHeight();
		v.canvasWidth = Environment::GetCanvasWidth();
		v.logicalAspect = width / height;
		v.canvasAspect = v.canvasWidth / (float)v.canvasHeight;

		if (v.logicalAspect < v.canvasAspect)
		{
			// logical needs to be scaled down vertically
			v.scale = v.logicalHeight / v.canvasHeight;
			float scaledWidth = v.logicalWidth / v.scale;

			v.transformX = (v.canvasWidth + scaledWidth) / 2 - scaledWidth;
			v.transformY = 0;
		}
		else
		{
			// logical needs to be scaled down horizontally
			v.scale = v.logicalWidth / v.canvasWidth;
			float scaledHeight = v.logicalHeight / v.scale;

			v.transformX = 0;
			v.transformY = (v.canvasHeight + scaledHeight) / 2 - scaledHeight;
		}
	}

	fRect Application::CoordLogicalToCanvas(fRect r)
	{
		auto v = viewport;
		auto res = r;

		res.x /= v.scale;
		res.y /= v.scale;
		res.w /= v.scale;
		res.h /= v.scale;

		res.x += v.transformX;
		res.y += v.transformY;

		return res;
	}

	fRect Application::CoordCanvasToLogical(fRect r)
	{
		auto v = viewport;
		auto res = r;

		res.x -= v.transformX;
		res.y -= v.transformY;

		res.x *= v.scale;
		res.y *= v.scale;
		res.w *= v.scale;
		res.h *= v.scale;

		return res;
	}

	void Application::DrawTexture(std::shared_ptr<Surface> texture, fRect dst)
	{
		AssertRunning();

		screenSurface->DrawTexture(texture->GetHandle(), CoordLogicalToCanvas(dst));
	}

	void Application::DrawTexture(std::shared_ptr<Surface> texture, fRect src, fRect dst)
	{
		AssertRunning();

		screenSurface->DrawTexture(texture->GetHandle(), src, CoordLogicalToCanvas(dst));
	}

	void Application::DrawNumber(std::shared_ptr<Surface> numberArray, fCoord position, uint32_t value)
	{
		AssertRunning();

		/*
			Assumes that numbers in texture are equi-distant and positioned from left to right
			starting from zero over one to nine... Then it will just decompose the value into 
			digits and render each digit individually by indexing into the provided texture.
		*/
		float stride = (float)numberArray->GetWidth() / 10;
		int h = numberArray->GetHeight();
		int i = 0, j = 0;
		std::array<int, 16> digits;

		while ((value > 0) || (i == 0))
		{
			digits[i++] = (value % 10);
			value /= 10;
		}

		while (i > 0)
		{
			i--;
			DrawTexture(
				numberArray, 
				{ (stride * digits[i] + 1), 0, (stride - 2), h }, 
				{ position.x + stride * (j++), position.y, stride, h });
		}
	}
}