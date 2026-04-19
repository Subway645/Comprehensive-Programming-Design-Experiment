#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "model.h"
#include "card_service.h"
#include "global.h"
#include "billing_service.h"
#include "card_file.h"
#include "money_file.h"

#define MONEYPATH "data\\money.ams"

// 声明外部变量
extern lpCardNode cardList;
extern BillingNode* billingList;

// 费用计算函数声明
double getAmount(time_t t);

// 添加卡信息
int addCardInfo(Card card) {
	return addCard(card);
}

// 检查卡号是否已存在
int checkCardExists(const char* pName) {
	if (getCard() == 0) return 0;
	return (queryCard(pName) != NULL) ? 1 : 0;
}

// 查询卡信息
Card* queryCardsInfo(const char* pName, int* pIndex) {
	return queryCards(pName, pIndex);
}

// 上机信息处理
int doLogonInfo(const char* pName, const char* pPwd, LogonInfo* pLogonInfo) {
	return doLogon(pName, pPwd, pLogonInfo);
}

// 释放所有链表资源
void releaseList() {
	releaseCardList();
	releaseBillingList();
}

// 计算费用：根据上机时长按计费单元计算
double getAmount(time_t t) {
	int nSec = (int)(time(NULL) - t);  // 上机秒数
	int nMin = nSec / 60;               // 转换为分钟
	int nCount;

	// 不足一个计费单元按一个单元计算
	if (nMin % UNIT == 0) {
		nCount = nMin / UNIT;
	} else {
		nCount = nMin / UNIT + 1;
	}

	return nCount * CHARGE;
}

// 下机结算
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo) {
	Card* pCard = NULL;
	Billing* pBilling = NULL;
	double dbAmount = 0.0;
	float fBalance = 0.0;

	// 加载卡信息并验证
	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return WRONGCARD;
	if (strcmp(pCard->aPwd, pPwd) != 0) return WRONGPWD;
	if (pCard->nStatus == 2 || pCard->nStatus == 3 || pCard->nDel == 1) return INVALID;
	if (pCard->nStatus == 0) return OUTSTATUS;

	// 查询该卡的计费记录
	int nBillingPosition = 0;
	pBilling = queryBilling(pName, &nBillingPosition);
	if (pBilling == NULL) return 0;

	// 计算费用
	dbAmount = getAmount(pBilling->tStart);
	if (pCard->fBalance < dbAmount) return ENOUGHMONEY;

	// 更新余额
	fBalance = pCard->fBalance - (float)dbAmount;
	pCard->fBalance = fBalance;

	// 填充结算信息
	strcpy(pInfo->aName, pName);
	pInfo->fAmount = dbAmount;
	pInfo->fBalance = fBalance;
	pInfo->tStart = pBilling->tStart;
	pInfo->tEnd = time(NULL);

	// 更新卡信息
	pCard->nStatus = 0;
	pCard->fTotalUse += (float)dbAmount;
	pCard->tLast = time(NULL);

	// 更新计费记录
	pBilling->fAmount = (float)dbAmount;
	pBilling->nStatus = 1;
	pBilling->tEnd = time(NULL);

	// 查找卡在链表中的索引并更新文件
	int nCardIndex = 0;
	lpCardNode node = cardList->next;
	while (node != NULL && &node->data != pCard) {
		nCardIndex++;
		node = node->next;
	}
	updateCard(pCard, CARDPATH, nCardIndex);

	// 查找计费记录在链表中的索引并更新文件
	int nBillingIndex = 0;
	BillingNode* billNode = billingList->next;
	while (billNode != NULL && &billNode->data != pBilling) {
		nBillingIndex++;
		billNode = billNode->next;
	}
	updateBilling(pBilling, BILLINGPATH, nBillingIndex);

	return 1;
}

// 充值
int doAddMoney(const char* pName, const char* pPwd, MoneyInfo* pMoneyInfo) {
	Card* pCard = NULL;
	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return WRONGCARD;
	if (strcmp(pCard->aPwd, pPwd) != 0) return WRONGPWD;
	if (pCard->nStatus == 2 || pCard->nStatus == 3 || pCard->nDel == 1) return INVALID;

	// 增加余额
	pCard->fBalance += pMoneyInfo->fMoney;

	// 查找卡在链表中的索引
	int nCardIndex = 0;
	lpCardNode node = cardList->next;
	while (node != NULL) {
		if (strcmp(node->data.aName, pName) == 0) {
			break;
		}
		nCardIndex++;
		node = node->next;
	}

	// 更新卡文件
	if (updateCard(pCard, CARDPATH, nCardIndex) == 0) return 0;

	// 保存充值记录
	Money money;
	strcpy(money.aName, pName);
	money.fMoney = pMoneyInfo->fMoney;
	money.tTime = time(NULL);
	money.nStatus = 0; 
	money.nDel = 0;
	saveMoney(&money, MONEYPATH);

	// 返回充值信息
	strcpy(pMoneyInfo->aName, pName);
	pMoneyInfo->fBalance = pCard->fBalance;
	pMoneyInfo->tTime = money.tTime;

	return 1;
}

// 退费
int doRefundMoney(const char* pName, const char* pPwd, MoneyInfo* pMoneyInfo) {
	Card* pCard = NULL;
	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return WRONGCARD;
	if (strcmp(pCard->aPwd, pPwd) != 0) return WRONGPWD;
	if (pCard->nStatus == 2 || pCard->nStatus == 3 || pCard->nDel == 1) return INVALID;
	if (pCard->nStatus != 0 || pCard->fBalance <= 0) return 0;

	// 记录退费金额
	float fRefund = pCard->fBalance;
	pMoneyInfo->fMoney = fRefund;

	// 余额清零
	pCard->fBalance = 0;

	// 查找卡在链表中的索引
	int nCardIndex = 0;
	lpCardNode node = cardList->next;
	while (node != NULL) {
		if (strcmp(node->data.aName, pName) == 0) {
			break;
		}
		nCardIndex++;
		node = node->next;
	}

	// 更新卡文件
	if (updateCard(pCard, CARDPATH, nCardIndex) == 0) return 0;

	// 保存退费记录
	Money money;
	strcpy(money.aName, pName);
	money.fMoney = fRefund;
	money.tTime = time(NULL);
	money.nStatus = 1;  // 1-退费
	money.nDel = 0;
	saveMoney(&money, MONEYPATH);

	// 返回退费信息
	strcpy(pMoneyInfo->aName, pName);
	pMoneyInfo->fBalance = 0;
	pMoneyInfo->tTime = money.tTime;

	return 1;
}

// 注销卡
int doAnnulCard(const char* pName, const char* pPwd, AnnulInfo* pAnnulInfo) {
	Card* pCard = NULL;
	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return WRONGCARD;
	if (strcmp(pCard->aPwd, pPwd) != 0) return WRONGPWD;
	if (pCard->nStatus == 2 || pCard->nStatus == 3 || pCard->nDel == 1) return INVALID;
	if (pCard->nStatus != 0) return ONSTATUS;

	// 记录退款金额
	float fRefund = pCard->fBalance;

	// 查找卡在链表中的索引
	int nCardIndex = 0;
	lpCardNode node = cardList->next;
	while (node != NULL) {
		if (strcmp(node->data.aName, pName) == 0) {
			break;
		}
		nCardIndex++;
		node = node->next;
	}

	// 更新卡状态为已注销(2)，余额清零
	pCard->nStatus = 2;
	pCard->fBalance = 0;

	// 更新卡文件
	if (updateCard(pCard, CARDPATH, nCardIndex) == 0) return 0;

	// 返回注销信息
	strcpy(pAnnulInfo->aName, pName);
	pAnnulInfo->fBalance = fRefund;

	return 1;
}

// 查询指定卡的消费情况
int doQueryCardConsumption(const char* pName, const char* pPwd,
	const char* pStartDate, const char* pEndDate, ConsumptionInfo* pInfo) {
	Card* pCard = NULL;
	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return WRONGCARD;
	if (strcmp(pCard->aPwd, pPwd) != 0) return WRONGPWD;

	// 解析日期字符串，转换为time_t时间戳
	struct tm startTm = { 0 }, endTm = { 0 };
	sscanf(pStartDate, "%d-%d-%d", &startTm.tm_year, &startTm.tm_mon, &startTm.tm_mday);
	sscanf(pEndDate, "%d-%d-%d", &endTm.tm_year, &endTm.tm_mon, &endTm.tm_mday);
	startTm.tm_year -= 1900;
	endTm.tm_year -= 1900;
	startTm.tm_mon -= 1;
	endTm.tm_mon -= 1;
	startTm.tm_hour = 0;
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	endTm.tm_hour = 23;
	endTm.tm_min = 59;
	endTm.tm_sec = 59;
	time_t startTime = mktime(&startTm);
	time_t endTime = mktime(&endTm);

	// 加载计费记录并统计消费
	if (getBilling() == 0) return 0;

	float fTotal = 0;
	BillingNode* cur = billingList->next;
	while (cur != NULL) {
		// 筛选该卡且已结算的记录
		if (strcmp(cur->data.aName, pName) == 0 && cur->data.nStatus == 1) {
			if (cur->data.tEnd >= startTime && cur->data.tEnd <= endTime) {
				fTotal += cur->data.fAmount;
			}
		}
		cur = cur->next;
	}

	// 返回查询结果
	strcpy(pInfo->aName, pName);
	strcpy(pInfo->startDate, pStartDate);
	strcpy(pInfo->endDate, pEndDate);
	pInfo->fTotalConsumption = fTotal;

	return 1;
}

// 查询营业额
float doQueryTurnover(const char* pStartDate, const char* pEndDate) {
	// 解析日期字符串，转换为time_t时间戳
	struct tm startTm = { 0 }, endTm = { 0 };
	sscanf(pStartDate, "%d-%d-%d", &startTm.tm_year, &startTm.tm_mon, &startTm.tm_mday);
	sscanf(pEndDate, "%d-%d-%d", &endTm.tm_year, &endTm.tm_mon, &endTm.tm_mday);
	startTm.tm_year -= 1900;
	endTm.tm_year -= 1900;
	startTm.tm_mon -= 1;
	endTm.tm_mon -= 1;
	startTm.tm_hour = 0;
	startTm.tm_min = 0;
	startTm.tm_sec = 0;
	endTm.tm_hour = 23;
	endTm.tm_min = 59;
	endTm.tm_sec = 59;
	time_t startTime = mktime(&startTm);
	time_t endTime = mktime(&endTm);

	// 加载计费记录并统计营业额
	if (getBilling() == 0) return 0;

	float fTotal = 0;
	BillingNode* cur = billingList->next;
	while (cur != NULL) {
		// 筛选已结算的记录
		if (cur->data.nStatus == 1) {
			if (cur->data.tEnd >= startTime && cur->data.tEnd <= endTime) {
				fTotal += cur->data.fAmount;
			}
		}
		cur = cur->next;
	}

	return fTotal;
}
