#pragma once

#include "rgb24Buffer.h"
#include "listsOfLRImages.h"
#include <deque>

double diffFunc(RGB24Buffer *startImage, std::deque<RGB24Buffer*> imageCollection, std::deque<LRImage> LRImages);
