#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "model.h"
#include "global.h"
#include "time.h"
#include"tool.h"

#define CARDCHARNUM 256

Card praseCard(char* pBuf);

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

int readCard(Card* pCard, const char* pPath) {
	FILE* fp = NULL;
	char aBuf[CARDCHARNUM] = { 0 };
	int i = 0;
	//打开文件
	fp = fopen(pPath, "r");
	if(fp == NULL) {
		return 0;
	}
	//读取文件
	while (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
		if(strlen(aBuf)>0){
			pCard[i] = praseCard(aBuf);
			i++;
		}
	}
	//关闭文件
	fclose(fp);
	return 1;
}

Card praseCard(char* pBuf) {
	Card card;
	const char* delim = "##";//分隔符
	char* buf = NULL;
	char* str = NULL;
	char flag[10][20] = { 0 };//存储分割后的字符串
	int index = 0;
	buf = pBuf;//第一次调用strtok时，buf为解析字符串
	while ((str = strtok(buf, delim)) != NULL) {
		strcpy(flag[index], str);
		index++;
		buf = NULL;//第二次调用strok时，buf为NULL
	}
	strcpy(card.aName, flag[0]);
	strcpy(card.aPwd, flag[1]);
	card.nStatus = atoi(flag[2]);
	card.tStart = stringToTime(flag[3]);
	card.tEnd = stringToTime(flag[4]);
	card.fTotalUse = atof(flag[5]);
	card.tLast = stringToTime(flag[6]);
	card.nUseCount = atoi(flag[7]);
	card.fBalance = atof(flag[8]);
	card.nDel = atoi(flag[9]);

	return card;
}

int getCardCount(const char* pPath) {
	FILE* fp = NULL;
	char aBuf[CARDCHARNUM] = { 0 };
	int nCount = 0;
	//打开文件
	fp = fopen(pPath, "r");
	if (fp == NULL) {
		return 0;
	}
	//读取文件
	while (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
		if (strlen(aBuf) > 0) {
			nCount++;
		}
	}
	//关闭文件
	fclose(fp);
	return nCount;
}
