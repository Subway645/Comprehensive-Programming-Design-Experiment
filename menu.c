#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "model.h"
#include "service.h"
#include "tool.h"

int getSize(const char* pInfo);

void outputmenu() {
	printf("欢迎使用计费管理系统\n\
----------菜单----------\n\
1.添加卡\n\
2.查询卡\n\
3.上机\n\
4.下机\n\
5.充值\n\
6.退费\n\
7.查询统计\n\
8.注销卡\n\
0.退出\n\
请选择菜单项编号（0~8）:");
}

/*1.添加卡*/
void add() {
	printf("----------添加卡----------\n");
	Card card;
	char aName[32] = { '\0' };
	char aPwd[20] = { '\0' };
	printf("请输入卡号：");
	
	//判断卡号长度
	
	while(1) {
		scanf("%s", aName);
		int nNameSize = getSize(aName);
		if (nNameSize > 0 && nNameSize < 19) {
			strcpy(card.aName, aName);
			break;
		}
		else {
			printf("卡号长度应为1~18之间，请重新输入：");
		}
	}
	printf("请输入密码：");
	
	//判断密码长度
	
	while(1) {
		scanf("%s", aPwd);
		int nPwdSize = getSize(aPwd);
		if (nPwdSize > 0 && nPwdSize < 9) {
			strcpy(card.aPwd, aPwd);
			break;
		}
		else {
			printf("密码长度应为1~8之间，请重新输入：");
		}
	}

	printf("请输入开卡金额：");
	scanf("%f", &card.fBalance);
	card.fTotalUse = 0;
	card.nUseCount = 0;
	card.nStatus = 0;
	card.nDel = 0;
	card.tStart = card.tEnd=card.tLast=time(NULL);

	//根据开卡时间，计算截止时间，每张卡的有效期为一年
	struct tm* startTime;
	struct tm* endTime;
	startTime = localtime(&card.tStart);
	endTime = localtime(&card.tEnd);
	endTime->tm_year = startTime->tm_year + 1; //到期时间延后1年
	card.tEnd = mktime(endTime); //将结束时间转换为time_t类型

	if(addCardInfo(card)==0) {
		printf("添加卡失败！\n");
		return;
	}
	else {
		printf("-----添加的卡信息如下-----\n");
		printf("卡号\t密码\t状态\t余额\n");
		printf("%s\t%s\t%d\t%.1f\n", card.aName, card.aPwd, card.nStatus, card.fBalance);
	}
}
int getSize(const char* pInfo) {
	int nSize = 0;
	while(*pInfo++ != '\0') {
		nSize++;
	}
	return nSize;
} 

/*2.查询卡*/ 
void query() {
	printf("----------查询卡----------\n");
	char aName[18] = { 0 };
	char aTime[20] = { 0 };
	Card* pCard = NULL;
	int nIndex = 0;
	int i = 0;
	printf("请输入要查询的卡号：");
	scanf("%s", aName);
	pCard = queryCardsInfo(aName, &nIndex);
	if (pCard == NULL || nIndex == 0) { printf("未找到卡号为%s的卡！\n", aName); return; }

	if (pCard != NULL) {
		printf("-----查询到的卡信息如下-----\n");
		for (i = 0;i < nIndex;i++) {
			timeToString(pCard[i].tLast, aTime);
			printf("卡号\t密码\t卡状态\t余额\t累计使用\t使用次数\t上次使用时间\n");
			printf("%s\t%s\t%d\t%.1f\t%.2f\t\t%d\t\t%s\n", pCard[i].aName, pCard[i].aPwd, pCard[i].nStatus, pCard[i].fBalance, pCard[i].fTotalUse, pCard[i].nUseCount, aTime);
		}	
	}
	else {
		printf("未找到卡号为%s的卡！\n", aName);
	}
}

/*3.上机*/ 
void logon() {
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	LogonInfo logonInfo;
	int nResult = 0;
	//接收用户输入的卡号和密码
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);
	//进行上机
	nResult = doLogonInfo(aName, aPwd, &logonInfo);
	//显示上机结果
	switch (nResult) {
		case 0:
		printf("上机失败\n");
		break;
		case 1:
		printf("上机成功\n");
		printf("-----上机信息如下-----\n");
		printf("卡号\t状态\t余额\n");
		printf("%s\t%d\t%.1f\n", logonInfo.aName, 1, logonInfo.fBalance);
		break;
		case 2:
		printf("该卡无法使用！\n");
		break;
		case 3:
		printf("余额不足，无法上机！\n");
		break;
	}
}

/*4.下机*/ 
void settle() {
	printf("----------下机----------\n");
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	//提示用户输入卡号，密码
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);
	//下机
	SettleInfo info;
	int nResult = doSettle(aName, aPwd, &info);
	//显示下机结果
	char startTime[20] = { 0 };
	char endTime[20] = { 0 };
	
	switch (nResult) {
		case 0:
		printf("下机失败\n");
		break;
		case 1:
		timeToString(info.tStart, startTime);
		timeToString(info.tEnd, endTime);
		printf("下机成功\n");
		printf("-----下机信息如下-----\n");
		printf("卡号\t状态\t余额\t应付金额\t开始时间\t结束时间\n");
		printf("%s\t%d\t%.1f\t%.1f\t%-20s\t%-20s\n", info.aName, 0, info.fBalance, info.fAmount, startTime, endTime);
		break;
		case 2:
		printf("该卡未上机！\n");
		break;
		case 3:
		printf("余额不足，无法下机！\n");
		break;
	}
}