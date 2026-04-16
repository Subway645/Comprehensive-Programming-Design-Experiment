#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <string.h>
#include"model.h"
#include"card_service.h"
#include"global.h"
#include "billing_service.h"

// 添加卡（调用 card_service 的保存函数)
int addCardInfo(Card card) {
	return addCard(card);
}

// 查询卡（调用 card_service 的查询函数）
Card* queryCardsInfo(const char* pName,int* pIndex) {
	return queryCards(pName, pIndex);
}

// 上机（调用 card_service 的 doLogon）
int doLogonInfo(const char* pName, const char* pPwd, LogonInfo* pLogonInfo) {
	return doLogon(pName, pPwd, pLogonInfo);
}

// 释放链表资源（service 层封装）
void releaseList() {
	releaseCardList();   //释放卡信息链表内存
	releaseBillingList(); //释放消费记录链表内存
}

// 提前声明 getAmount，避免在使用前隐式声明为 int 导致重定义错误
double getAmount(time_t t);

//下机
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo) {
	Card* pCard = NULL;
	Billing* pBilling = NULL;
	int nPosition = 0;   //消费记录在链表中的位置索引
	double dbAmount = 0.0;  //消费金额
	float fBalance = 0.0;   //卡余额
	
	//获取卡信息
	pCard = queryCard(pName);
	if(pCard == NULL) {
		return 0; 
	}
	//密码比对
	if (strcmp(pCard->aPwd, pPwd) != 0) {
		return 0; 
	}

	if(pCard->nStatus != 1){
		return 2; //2表示该卡未上机
	}
	//根据卡号查询计费信息
	pBilling = queryBilling(pName, &nPosition);
	if(pBilling == NULL) {
	    return 0;
	}
	//计算消费金额
	dbAmount = getAmount(pBilling->tStart);
	//判断余额是否足够支付
	if(pCard->fBalance < dbAmount) {
		return 3; //3表示余额不足
	}
	
	//更新卡余额
	fBalance = pCard->fBalance - dbAmount;
	pCard->fBalance = fBalance;
	
	strcpy(pInfo->aName, pName);
	pInfo->fAmount = dbAmount;
	pInfo->fBalance = fBalance;
	pInfo->tStart = pBilling->tStart;
	pInfo->tEnd = time(NULL);
	

	//更新卡信息：余额、状态、上次使用时间
	pCard->fBalance = fBalance;
	pCard->nStatus = 0;
	pCard->fTotalUse += dbAmount;
	pCard->tLast = time(NULL);

    //更新计费信息：消费金额、结束时间、状态
	pBilling->fAmount = dbAmount;
	pBilling->nStatus = 1;
	pBilling->tEnd = time(NULL);
	updateCard(pCard, CARDPATH, nPosition);

	return 1; //1表示下机成功
}

//根据上机时间计算消费金额
double getAmount(time_t t) {
	int nSec;
	int nMin;
	int nCount;
	time_t tEnd = time(NULL);
	double dbAmount;
	nSec = tEnd - t;    //秒数
	nMin = nSec / 60;   //分钟数
	
	//按照计费单位计算费用
	if(nMin% UNIT == 0) {
		nCount = nMin / UNIT;
	} else {
		nCount = nMin / UNIT + 1;
	}
	
	//计算消费金额
	dbAmount = nCount * CHARGE;
	return dbAmount;
}