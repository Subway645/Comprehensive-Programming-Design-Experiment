#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include "model.h"
#include "card_service.h"
#include "tool.h"
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
			printf("卡号长度必须在1~18之间，请重新输入：");
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
			printf("密码长度必须在1~8之间，请重新输入：");
		}
	}

	printf("请输入开卡金额：");
	scanf("%f", &card.fBalance);
	card.fTotalUse = card.fBalance;
	card.nUseCount = 0;
	card.nStatus = 0;
	card.nDel = 0;
	card.tStart = card.tEnd=card.tLast=time(NULL);

	addCard(card); //调用添加卡函数将卡信息添加到数组中
	printf("-----添加的卡信息如下-----\n");
	printf("卡号\t密码\t卡状态\t余额\n"); 
	printf("%s\t%s\t%d\t%.1f\n", card.aName, card.aPwd, card.nStatus, card.fBalance);
}
int getSize(const char* pInfo) {
	int nSize = 0;
	while(*pInfo++ != '\0') {
		nSize++;
	}
	return nSize;
} 

void query() {
	printf("----------查询卡----------\n");
	char aName[18] = { 0 };
	char aTime[20] = { 0 };
	Card* pCard = NULL;
	int nIndex = 0;
	int i = 0;
	printf("请输入要查询的卡号：");
	scanf("%s", aName);
	pCard = queryCards(aName, &nIndex);
	if (pCard == NULL || nIndex == 0) { printf("未找到卡号为%s的卡！\n", aName); return; }
	timeToString(pCard[0].tLast, aTime);

	if (pCard != NULL) {
		printf("-----查询到的卡信息如下-----\n");
		for (i = 0;i < nIndex;i++) {
			printf("卡号\t密码\t卡状态\t余额\t累计使用\t使用次数\t上次使用时间\n");
			printf("%s\t%s\t%d\t%.1f\t%.2f\t\t%d\t\t%s\n", pCard[i].aName, pCard[i].aPwd, pCard[i].nStatus, pCard[i].fBalance, pCard[i].fTotalUse, pCard[i].nUseCount, aTime);
		}	
	}
	else {
		printf("未找到卡号为%s的卡！\n", aName);
	}
}