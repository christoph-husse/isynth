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
	///////////////////////////////////////////////////////////////////////////////////////
	////////// WizardPage
	///////////////////////////////////////////////////////////////////////////////////////
	class WizardPage : NonCopyable, NonMovable
	{
	private:
		Application& app;

		WizardPage(const WizardPage&) = delete;
		WizardPage& operator=(const WizardPage&) = delete;

	protected:
		friend class WizardApplication;

		virtual void OnFixedUpdate() {}
		virtual void OnRender() {}

		virtual void OnShutdown() {}
		virtual void OnStartup() {}

		virtual void OnShow() {}
		virtual void OnHide() {}

		virtual void OnTouchStart(Touch touch) {}
		virtual void OnTouchEnd(Touch touch) {}
		virtual void OnTouchDrag(Touch touch) {}

	public:

		void DrawTexture(std::shared_ptr<Surface> texture, fRect dst);
		void DrawTexture(std::shared_ptr<Surface> texture, fRect src, fRect dst);
		void DrawNumber(std::shared_ptr<Surface> numberArray, fCoord position, uint32_t value);

		virtual ~WizardPage() { }
		WizardPage(Application& app) : app(app) {}
	};

	///////////////////////////////////////////////////////////////////////////////////////
	////////// WizardApplication
	///////////////////////////////////////////////////////////////////////////////////////
	class WizardApplication : public Application
	{
	private:
		std::vector<std::shared_ptr<WizardPage>> pages;
		int pageIndex = 0;

		WizardApplication(const WizardApplication&) = delete;
		WizardApplication& operator=(const WizardApplication&) = delete;

	protected:
		void AddPage(std::shared_ptr<WizardPage> page);

		template<class TPage, class... TPageConstructorArgs>
		void AddPage(TPageConstructorArgs&&... args)
		{
			AddPage(std::make_shared<TPage>(std::forward<TPageConstructorArgs>(args)...));
		}

		void OnFixedUpdate() override;
		void OnRender() override;
		void OnShutdown() override;
		void OnStartup() override;
		void OnTouchStart(Touch touch) override;
		void OnTouchEnd(Touch touch) override;
		void OnTouchDrag(Touch touch) override;

	public:
		WizardApplication(AppSettings settings) : Application(settings) { }

		void SetPage(int index);
		int GetPage();
	};
}