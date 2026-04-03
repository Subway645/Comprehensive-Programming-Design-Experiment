#pragma once
#include <time.h>

void timeToString(time_t t, char* pBuf);
time_t stringToTime(const char* pTime);