#define _CRT_SECURE_NO_WARNINGS
#include<time.h>
//将time_t类型的时间转换为字符串格式，格式为"年-月-日 时：分"
void timeToString(time_t t, char* pBur) {
	struct tm* pTimeInfo;
	pTimeInfo = localtime(&t);
	strftime(pBur, 20, "%Y-%m-%d %H:%M", pTimeInfo);
}