#pragma once

#ifdef _MSC_VER
	#pragma warning (disable: 4100) // unreferenced formal parameter
	#pragma warning (disable: 4505) // unreferenced local function was removed

	#ifdef DEBUG
		#pragma warning (disable: 4189) // unreferenced local variable
		#include <crtdbg.h>
	#endif
#endif

#include <memory.h>
#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <fstream>
#include <atomic>
#include <mutex>
#include <random>
#include <thread>
