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

namespace Engine2D
{
	void WizardApplication::OnFixedUpdate()
	{
		pages[pageIndex]->OnFixedUpdate();
	}

	void WizardApplication::OnRender()
	{
		pages[pageIndex]->OnRender();
	}

	void WizardApplication::OnShutdown()
	{
		for (auto& page : pages) page->OnShutdown();
	}

	void WizardApplication::OnStartup()
	{
		for (auto& page : pages) page->OnStartup();
	}

	void WizardApplication::OnTouchStart(Touch touch)
	{
		pages[pageIndex]->OnTouchStart(touch);
	}

	void WizardApplication::OnTouchEnd(Touch touch)
	{
		pages[pageIndex]->OnTouchEnd(touch);
	}

	void WizardApplication::OnTouchDrag(Touch touch)
	{
		pages[pageIndex]->OnTouchDrag(touch);
	}

	void WizardApplication::AddPage(std::shared_ptr<WizardPage> page)
	{
		if (std::find(pages.begin(), pages.end(), page) != pages.end())
			THROW ArgumentException("Page was already added.");

		pages.push_back(page);

		if (pages.size() == 1)
			pages[0]->OnShow();
	}

	void WizardApplication::SetPage(int index)
	{
		if ((index < 0) || (index >= (int)pages.size()))
			THROW ArgumentException("Page index is out of range.");

		if (index == pageIndex)
			return;

		pages[pageIndex]->OnHide();
		pageIndex = index;
		pages[pageIndex]->OnShow();

	}

	int WizardApplication::GetPage()
	{
		return pageIndex;
	}
}