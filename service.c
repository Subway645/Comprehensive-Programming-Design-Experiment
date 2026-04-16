#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <string.h>
#include"model.h"
#include"card_service.h"
#include"global.h"
#include "billing_service.h"
#include "card_file.h"

extern lpCardNode cardList;

int addCardInfo(Card card) {
	return addCard(card);
}

Card* queryCardsInfo(const char* pName,int* pIndex) {
	return queryCards(pName, pIndex);
}

int doLogonInfo(const char* pName, const char* pPwd, LogonInfo* pLogonInfo) {
	return doLogon(pName, pPwd, pLogonInfo);
}

void releaseList() {
	releaseCardList();
	releaseBillingList();
}

double getAmount(time_t t);

int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo) {
	Card* pCard = NULL;
	Billing* pBilling = NULL;
	double dbAmount = 0.0;
	float fBalance = 0.0;

	if (getCard() == 0) return 0;
	pCard = queryCard(pName);
	if (pCard == NULL) return 0;
	if (strcmp(pCard->aPwd, pPwd) != 0) return 0;
	if (pCard->nStatus != 1) return 2;

	int nBillingPosition = 0;
	pBilling = queryBilling(pName, &nBillingPosition);
	if (pBilling == NULL) return 0;

	dbAmount = getAmount(pBilling->tStart);
	if (pCard->fBalance < dbAmount) return 3;

	fBalance = pCard->fBalance - (float)dbAmount;
	pCard->fBalance = fBalance;

	strcpy(pInfo->aName, pName);
	pInfo->fAmount = dbAmount;
	pInfo->fBalance = fBalance;
	pInfo->tStart = pBilling->tStart;
	pInfo->tEnd = time(NULL);

	pCard->nStatus = 0;
	pCard->fTotalUse += (float)dbAmount;
	pCard->tLast = time(NULL);

	pBilling->fAmount = (float)dbAmount;
	pBilling->nStatus = 1;
	pBilling->tEnd = time(NULL);

	int nCardIndex = 0;
	lpCardNode node = cardList->next;
	while (node != NULL && &node->data != pCard) {
		nCardIndex++;
		node = node->next;
	}
	updateCard(pCard, CARDPATH, nCardIndex);

	return 1;
}

double getAmount(time_t t) {
	int nSec;
	int nMin;
	int nCount;
	time_t tEnd = time(NULL);
	double dbAmount;
	nSec = (int)(tEnd - t);
	nMin = nSec / 60;

	if (nMin % UNIT == 0) {
		nCount = nMin / UNIT;
	} else {
		nCount = nMin / UNIT + 1;
	}

	dbAmount = nCount * CHARGE;
	return dbAmount;
}
