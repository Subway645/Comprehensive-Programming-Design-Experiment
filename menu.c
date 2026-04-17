#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "model.h"
#include "service.h"
#include "tool.h"
#include "global.h"
#include "menu.h"

int getSize(const char* pInfo);

// 输出主菜单
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

// 获取字符串长度
int getSize(const char* pInfo) {
	int nSize = 0;
	while(*pInfo++ != '\0') {
		nSize++;
	}
	return nSize;
}

/*1.添加卡*/
void add() {
	printf("----------添加卡----------\n");
	Card card;
	char aName[32] = { '\0' };
	char aPwd[20] = { '\0' };
	printf("请输入卡号：");

	// 判断卡号长度（1~18位）
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

	// 判断密码长度（1~8位）
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
	card.tStart = card.tEnd = card.tLast = time(NULL);

	// 根据开卡时间计算截止时间，卡的有效期为一年
	struct tm* startTime;
	struct tm* endTime;
	startTime = localtime(&card.tStart);
	endTime = localtime(&card.tEnd);
	endTime->tm_year = startTime->tm_year + 1;
	card.tEnd = mktime(endTime);

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
	if (pCard == NULL || nIndex == 0) {
		printf("未找到卡号为%s的卡！\n", aName);
		return;
	}

	printf("-----查询到的卡信息如下-----\n");
	for (i = 0; i < nIndex; i++) {
		timeToString(pCard[i].tLast, aTime);
		printf("卡号\t密码\t卡状态\t余额\t累计使用\t使用次数\t上次使用时间\n");
		printf("%s\t%s\t%d\t%.1f\t%.2f\t\t%d\t\t%s\n",
			pCard[i].aName, pCard[i].aPwd, pCard[i].nStatus,
			pCard[i].fBalance, pCard[i].fTotalUse, pCard[i].nUseCount, aTime);
	}
}

/*3.上机*/
void logon() {
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	LogonInfo logonInfo;
	int nResult = 0;

	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);
	nResult = doLogonInfo(aName, aPwd, &logonInfo);

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
		case INVALID:
			printf("该卡无法使用！\n");
			break;
		case ENOUGHMONEY:
			printf("余额不足，无法上机！\n");
			break;
		case ONSTATUS:
			printf("该卡已上机！\n");
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
	}
}

/*4.下机*/
void settle() {
	printf("----------下机----------\n");
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);

	SettleInfo info;
	int nResult = doSettle(aName, aPwd, &info);
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
			printf("%s\t%d\t%.1f\t%.1f\t%-20s\t%-20s\n",
				info.aName, 0, info.fBalance, info.fAmount, startTime, endTime);
			break;
		case OUTSTATUS:
			printf("该卡未上机！\n");
			break;
		case ENOUGHMONEY:
			printf("余额不足，无法下机！\n");
			break;
		case INVALID:
			printf("该卡无法使用！\n");
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
	}
}

/*5.充值*/
void addMoney() {
	printf("----------充值----------\n");
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	float fMoney = 0;
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);
	printf("请输入充值金额：");
	scanf("%f", &fMoney);

	MoneyInfo moneyInfo;
	moneyInfo.fMoney = fMoney;
	int nResult = doAddMoney(aName, aPwd, &moneyInfo);

	char aTime[20] = { 0 };
	timeToString(moneyInfo.tTime, aTime);

	switch (nResult) {
		case 0:
			printf("充值失败\n");
			break;
		case 1:
			printf("充值成功\n");
			printf("-----充值信息如下-----\n");
			printf("卡号\t充值金额\t余额\t时间\n");
			printf("%s\t%.1f\t\t%.1f\t%s\n",
				moneyInfo.aName, moneyInfo.fMoney, moneyInfo.fBalance, aTime);
			break;
		case INVALID:
			printf("该卡无法使用！\n");
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
	}
}

/*6.退费*/
void refundMoney() {
	printf("----------退费----------\n");
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);

	MoneyInfo moneyInfo;
	int nResult = doRefundMoney(aName, aPwd, &moneyInfo);

	char aTime[20] = { 0 };
	timeToString(moneyInfo.tTime, aTime);

	switch (nResult) {
		case 0:
			printf("退费失败！该卡可能正在使用或余额为0\n");
			break;
		case 1:
			printf("退费成功\n");
			printf("-----退费信息如下-----\n");
			printf("卡号\t退费金额\t余额\t时间\n");
			printf("%s\t%.1f\t\t%.1f\t%s\n",
				moneyInfo.aName, moneyInfo.fMoney, moneyInfo.fBalance, aTime);
			break;
		case INVALID:
			printf("该卡无法使用！\n");
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
	}
}

/*7.查询统计*/
void statistics() {
	printf("----------查询统计----------\n");
	printf("1.查询卡消费情况\n");
	printf("2.查询营业额\n");
	printf("0.返回\n");
	printf("请选择：");

	int subChoice;
	scanf("%d", &subChoice);

	switch (subChoice) {
		case 1:
			queryCardConsumption();
			break;
		case 2:
			queryTurnover();
			break;
		case 0:
			return;
		default:
			printf("输入错误！\n");
	}
}

/*7.1 查询卡消费情况*/
void queryCardConsumption() {
	printf("----------查询卡消费情况----------\n");
	char aName[18] = { 0 };
	char aPwd[8] = { 0 };
	char startDate[20] = { 0 };
	char endDate[20] = { 0 };

	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);
	printf("请输入开始时间（年-月-日）：");
	scanf("%s", startDate);
	printf("请输入结束时间（年-月-日）：");
	scanf("%s", endDate);

	ConsumptionInfo info;
	int nResult = doQueryCardConsumption(aName, aPwd, startDate, endDate, &info);

	switch (nResult) {
		case 0:
			printf("查询失败！\n");
			break;
		case 1:
			printf("查询成功\n");
			printf("-----卡消费情况如下-----\n");
			printf("卡号\t开始时间\t结束时间\t总消费额\n");
			printf("%s\t%s\t%s\t%.1f\n",
				info.aName, info.startDate, info.endDate, info.fTotalConsumption);
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
	}
}

/*7.2 查询营业额*/
void queryTurnover() {
	printf("----------查询营业额----------\n");
	char startDate[20] = { 0 };
	char endDate[20] = { 0 };

	printf("请输入开始时间（年-月-日）：");
	scanf("%s", startDate);
	printf("请输入结束时间（年-月-日）：");
	scanf("%s", endDate);

	float fTurnover = doQueryTurnover(startDate, endDate);
	printf("-----营业额情况如下-----\n");
	printf("开始时间\t结束时间\t营业额\n");
	printf("%s\t%s\t%.1f\n", startDate, endDate, fTurnover);
}

/*8.注销卡*/
void annul() {
	printf("----------注销卡----------\n");
	char aName[20] = { 0 };
	char aPwd[8] = { 0 };
	printf("请输入卡号：");
	scanf("%s", aName);
	printf("请输入密码：");
	scanf("%s", aPwd);

	AnnulInfo annulInfo;
	int nResult = doAnnulCard(aName, aPwd, &annulInfo);

	switch (nResult) {
		case 0:
			printf("注销卡失败！\n");
			break;
		case 1:
			printf("注销成功\n");
			printf("-----注销信息如下-----\n");
			printf("卡号\t退款金额\n");
			printf("%s\t%.1f\n", annulInfo.aName, annulInfo.fBalance);
			break;
		case WRONGCARD:
			printf("卡号错误！\n");
			break;
		case WRONGPWD:
			printf("密码错误！\n");
			break;
		case ONSTATUS:
			printf("该卡正在使用，无法注销！\n");
			break;
		case INVALID:
			printf("该卡无法使用！\n");
			break;
	}
}
