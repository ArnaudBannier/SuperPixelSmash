/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "box2d.h"

#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <algorithm>

#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>

#include <iostream>
#include <iomanip>
#include <string>

#include <cassert>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }

#define DEFAULT_UI_LAYER 1024
#define MAX_TIME_MS (Uint64)0xFFFFFFFFFFFFFFFF
