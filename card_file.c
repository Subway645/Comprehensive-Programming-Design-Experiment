#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include "model.h"
#include "global.h"
#include "time.h"
int saveCard(const Card* pcard, const char* pPath) {
	FILE* fp = NULL;
	//打开文件
	if ((fp = fopen(pPath,"a"))==NULL) {
		fp = fopen(pPath, "w");
		if (fp == NULL) {
			return 0;
		}
	}
	
	//将时间转换为字符串格式
	char StartTime[20], EndTime[20], LastTime[20];
	timeToString(pcard->tStart,StartTime);
	timeToString(pcard->tEnd, EndTime);
	timeToString(pcard->tLast, LastTime);
	//将数据写进文件
	//格式为：卡号##密码##状态##开卡时间##截止时间##累计金额##最后使用时间##使用次数##当前余额##删除标识
	fprintf(fp, "%s##%s##%d##%s##%s##%.1f##%s##%d##%.1f##%d\n",
		pcard->aName, pcard->aPwd, pcard->nStatus, StartTime, EndTime,
		pcard->fTotalUse, LastTime, pcard->nUseCount, pcard->fBalance, pcard->nDel);
	//关闭文件
	fclose(fp);
	return 1;
}