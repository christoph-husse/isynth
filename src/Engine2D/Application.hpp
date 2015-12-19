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

#include "Engine2D.hpp"

namespace Engine2D
{
	using namespace Framework;


	///////////////////////////////////////////////////////////////////////////////////////
	////////// AppSettings
	///////////////////////////////////////////////////////////////////////////////////////
	struct AppSettings final
	{
		std::string windowTitle = "Sample Application";

		uint32_t windowWidth = 700, windowHeight = 723;

		// amount of times OnFixedUpdate() shall be called per second (is also stable over large time periods
		// and can be used for turn-based scheduling). For instance if PC is blocked for a few seconds,
		// OnFixedUpdate() will be called tons of times to catch up again. This does not happen to OnRender().
		// Note that in case of very large time shifts (hibernate/sleep) an unknown number of frames can 
		// be skipped to not deadlock the application.
		uint32_t fixedFramesPerSecond = 128;

		// defines a desired target framerate for OnRender()  (rate enforced on a frame-by-frame basis)
		uint32_t renderFramesPerSecond = 60;

		// if neither OnFixedUpdate() nor OnRender() needed to be called, the following is the time in milliseconds
		// the app will be put to sleep before checking again if something needs to be done...
		uint32_t delayMillisIfIdle = 3;

		AppSettings& MakeValid()
		{
			fixedFramesPerSecond = std::min(1024u, std::max(1u, fixedFramesPerSecond));
			renderFramesPerSecond = std::min(120u, std::max(1u, renderFramesPerSecond));
			delayMillisIfIdle = std::min(std::min(fixedFramesPerSecond, renderFramesPerSecond), std::max(1u, delayMillisIfIdle));

			return *this;
		}

		AppSettings& SetWindowTitle(std::string title)
		{
			windowTitle = title;
			return *this;
		}
	};


	///////////////////////////////////////////////////////////////////////////////////////
	////////// Application
	///////////////////////////////////////////////////////////////////////////////////////
	class Application : NonCopyable, NonMovable
	{
	private:
		friend class TextureHandle;
		friend class MainEntryPoint;

		bool running = false;
		const AppSettings settings;
		std::shared_ptr<ScreenSurface> screenSurface;
		uint32_t lastFixedFrameTime;
		uint32_t millisPerFixedFrame;
		uint32_t millisPerRenderFrame;
		uint32_t lastFrameTime;
		uint64_t audioTimer = 1;
		std::unordered_map<int, Touch> touches;
		EventObserver observer;
		std::vector<std::string> envArgs;

		struct
		{
			float logicalWidth, logicalHeight;
			int canvasWidth, canvasHeight;
			float transformX, transformY;
			float logicalAspect, canvasAspect, scale;
		}viewport;

		static Application* instance;

		void Init();
		void Loop();
		void Render();
		void Cleanup();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void HandleOnGestureEvent(GestureEvent gesture);
		void HandleOnQuit();
		void HandleAudioCallback(std::vector<int16_t>& pcmData);
		static void HandleAudioCallbackStatic(std::vector<int16_t>& pcmData);

		void RunLoopIteration();
		static void RunLoopIterationStatic();
		void Run(std::vector<std::string> args);
		static void RunStatic(std::vector<std::string> args);

	protected:

		void SetLogicalViewport(float width, float height);
		void SetLogicalViewportToFill(float width, float height);

		template<class T1, class T2>
		void SetLogicalViewport(T1 width, T2 height) { SetLogicalViewport((float)width, (float)height); }

		fRect CoordLogicalToCanvas(fRect r);
		fRect CoordCanvasToLogical(fRect r);

		// will throw if OnStartup() was not yet called.
		void AssertRunning();

		virtual void OnFixedUpdate() {}
		virtual void OnRender() {}
		virtual void OnFillAudioBuffer(uint64_t audioTime, std::vector<int16_t>& pcmData) {}

		// Is called after leaving the main loop. The application is still in a fully
		// functional state. All functionality available in OnRender() will be available 
		// here. But after this method returns its really over...
		virtual void OnShutdown() {}

		// Is called after the app has been fully initialized. All functionality available
		// in OnRender() will be available here. Its basically "the first frame".
		virtual void OnStartup() {}

		virtual void OnTouchStart(Touch touch) { }
		virtual void OnTouchEnd(Touch touch) { }
		virtual void OnTouchDrag(Touch touch) { }

		// Within the constructor the application is still in a volatile state. You must not call
		// ANY of the member functions within subclass-constructors!! Defer all such actions into OnStartup().
		Application(AppSettings settings);
	public:
		virtual ~Application();

		int GetAudioSamplerRate();
		int GetWindowWidth() const { return settings.windowWidth; }
		int GetWindowHeight() const { return settings.windowHeight; }
		std::vector<std::string> GetEnvironmentArgs() const { return envArgs; }

		void ClearScreen(int r, int g, int b);
		void DrawTexture(std::shared_ptr<Surface> texture, fRect dst);
		void DrawTexture(std::shared_ptr<Surface> texture, fRect src, fRect dst);
		void DrawNumber(std::shared_ptr<Surface> numberArray, fCoord position, uint32_t value);

		static uint32_t GetTicks();
	};
}

