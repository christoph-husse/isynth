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

namespace Engine2D
{
	class Surface;
	class Application;
	struct AppSettings;
	class TextureHandle;
	class ScreenSurface;
	class GestureEvent;
	class MainEntryPoint;
}

// Engine2D headers would pollute us to death, so only predeclare what we really need and use
// the real headers only in the CPP files that actually abstract from Engine2D.
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Rect;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
typedef struct _Mix_Music Mix_Music;


#include "Framework.hpp"
#include "Coords.hpp"
#include "Surface.hpp"
#include "SAL.hpp"
#include "Exceptions.hpp"
#include "Application.hpp"
#include "WizardApplication.hpp"
#include "Midi.hpp"