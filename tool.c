#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdio.h>
#include <string.h>

// 将time_t时间转换为字符串，格式："年-月-日 时:分"
void timeToString(time_t t, char* pBuf) {
	struct tm* pTimeInfo;
	if (pBuf == NULL) {
		return;
	}
	pTimeInfo = localtime(&t);
	if (pTimeInfo == NULL) {
		strncpy(pBuf, "1970-01-01 00:00", 20);
		pBuf[19] = '\0';
		return;
	}
	strftime(pBuf, 20, "%Y-%m-%d %H:%M", pTimeInfo);
}

// 将字符串时间转换为time_t，格式："年-月-日 时:分"
time_t stringToTime(const char* pTime) {
	struct tm tm;
	int y, mo, d, h, mi;
	if (pTime == NULL) {
		return (time_t)0;
	}
	memset(&tm, 0, sizeof(tm));
	if (sscanf(pTime, "%d-%d-%d %d:%d", &y, &mo, &d, &h, &mi) != 5) {
		return (time_t)0;
	}
	tm.tm_year = y - 1900;
	tm.tm_mon = mo - 1;
	tm.tm_mday = d;
	tm.tm_hour = h;
	tm.tm_min = mi;
	tm.tm_sec = 0;
	tm.tm_isdst = -1;
	return mktime(&tm);
}
