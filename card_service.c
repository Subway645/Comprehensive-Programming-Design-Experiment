#define _CRT_SECURE_NO_WARNINGS
#include<string.h>
#include<stdlib.h>
#include"model.h"
#include"global.h"
#include"card_file.h"
#include<time.h>
#include"billing_file.h"
#include"billing_service.h"

int getCard(void);
Card aCard[50];
int nCount = 0;

lpCardNode cardList = NULL;

int initCardList() {
	lpCardNode head = NULL;
	head = (lpCardNode)malloc(sizeof(CardNode));
	if (head != NULL) {
		head->next = NULL;
		cardList = head;
		return 1;
	}
	return 0;
}

int addCard(Card card) {
	if (saveCard(&card, CARDPATH) == 1) {
		lpCardNode newNode = (lpCardNode)malloc(sizeof(CardNode));
		if (newNode == NULL) return 1;
		memset(newNode, 0, sizeof(CardNode));
		newNode->data = card;
		newNode->next = NULL;
		lpCardNode tail = cardList;
		while (tail->next != NULL) tail = tail->next;
		tail->next = newNode;
		return 1;
	}
	return 0;
}

Card* queryCard(const char* aName) {
	if (aName == NULL || cardList == NULL) {
		return NULL;
	}
	lpCardNode cur = cardList->next;
	while (cur != NULL) {
		if (strcmp(cur->data.aName, aName) == 0 && cur->data.nDel == 0) {
			return &cur->data;
		}
		cur = cur->next;
	}
	return NULL;
}

Card* queryCards(const char* pName, int* pIndex) {
	if (pIndex) *pIndex = 0;
	if (pName == NULL || cardList == NULL) {
		return NULL;
	}

	lpCardNode cur = cardList->next;
	Card* pCard = NULL;
	int count = 0;

	while (cur != NULL) {
		if (strstr(cur->data.aName, pName) != NULL && cur->data.nDel == 0) {
			count++;
		}
		cur = cur->next;
	}

	if (count == 0) {
		return NULL;
	}

	pCard = (Card*)malloc(count * sizeof(Card));
	if (pCard == NULL) return NULL;

	int i = 0;
	cur = cardList->next;
	while (cur != NULL) {
		if (strstr(cur->data.aName, pName) != NULL && cur->data.nDel == 0) {
			pCard[i++] = cur->data;
		}
		cur = cur->next;
	}

	if (pIndex) *pIndex = count;
	return pCard;
}

void releaseCardList() {
	lpCardNode cur = cardList;
	lpCardNode next = NULL;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
	cardList = NULL;
}

int getCard() {
	Card* pCard = NULL;
	lpCardNode node = NULL;
	lpCardNode cur = NULL;
	if (cardList != NULL) {
		releaseCardList();
	}
	initCardList();
	int nCount = getCardCount(CARDPATH);

	pCard = (Card*)malloc(nCount * sizeof(Card));
	if (pCard == NULL) {
		return 0;
	}
	if (readCard(pCard, CARDPATH) == 0) {
		free(pCard);
		return 0;
	}
	node = cardList;
	for (int i = 0; i < nCount; i++) {
		cur = (lpCardNode)malloc(sizeof(CardNode));
		if (cur == NULL) {
			releaseCardList();
			free(pCard);
			return 0;
		}
		memset(cur, 0, sizeof(CardNode));
		cur->data = pCard[i];
		cur->next = NULL;
		node->next = cur;
		node = cur;
	}
	free(pCard);
	pCard = NULL;
	return 1;
}

int doLogon(const char* aName, const char* aPwd, LogonInfo* pLongInfo) {
	lpCardNode cardNode = NULL;
	int nIndex = 0;
	Billing billing;

	if (getCard() == 0) {
		return 0;
	}

	cardNode = cardList->next;
	while (cardNode != NULL) {
		if (strcmp(cardNode->data.aName, aName) == 0 && strcmp(cardNode->data.aPwd, aPwd) == 0) {
			if (cardNode->data.nStatus == 1) {
				return 0;
			}
			if (cardNode->data.fBalance <= 0) {
				return 3;
			}
			if (cardNode->data.nStatus == 2 || cardNode->data.nStatus == 3 || cardNode->data.nDel == 1) {
				return 2;
			}
			break;
		}
		cardNode = cardNode->next;
		nIndex++;
	}

	if (cardNode == NULL) {
		return 0;
	}

	cardNode->data.nStatus = 1;
	cardNode->data.tLast = time(NULL);
	cardNode->data.nUseCount++;

	if (updateCard(&cardNode->data, CARDPATH, nIndex) == 1) {
		strcpy_s(billing.aName, sizeof(billing.aName), aName);
		billing.tStart = time(NULL);
		billing.nStatus = 0;
		billing.nDel = 0;
		billing.fAmount = 0;
		billing.tEnd = 0;
		if (addBilling(billing) == 1) {
			if (pLongInfo != NULL) {
				strcpy(pLongInfo->aName, aName);
				pLongInfo->fBalance = cardNode->data.fBalance;
			}
			return 1;
		}
	}

	return 0;
}
