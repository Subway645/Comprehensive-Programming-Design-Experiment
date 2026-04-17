#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include "model.h"
#include "global.h"
#include "card_file.h"
#include <time.h>
#include "billing_file.h"
#include "billing_service.h"

// 声明外部变量和函数
int getCard(void);
Card aCard[50];
int nCount = 0;
lpCardNode cardList = NULL;

// 初始化卡链表
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

// 添加卡到链表和文件
int addCard(Card card) {
	if (saveCard(&card, CARDPATH) == 1) {
		lpCardNode newNode = (lpCardNode)malloc(sizeof(CardNode));
		if (newNode == NULL) return 1;
		memset(newNode, 0, sizeof(CardNode));
		newNode->data = card;
		newNode->next = NULL;

		// 找到链表尾部并插入
		lpCardNode tail = cardList;
		while (tail->next != NULL) tail = tail->next;
		tail->next = newNode;
		return 1;
	}
	return 0;
}

// 根据卡号精确查询卡
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

// 根据卡号模糊查询所有匹配的卡
Card* queryCards(const char* pName, int* pIndex) {
	if (pIndex) *pIndex = 0;
	if (pName == NULL || cardList == NULL) {
		return NULL;
	}

	// 第一遍：统计匹配数量
	lpCardNode cur = cardList->next;
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

	// 分配内存
	Card* pCard = (Card*)malloc(count * sizeof(Card));
	if (pCard == NULL) return NULL;

	// 第二遍：复制匹配的数据
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

// 释放卡链表内存
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

// 从文件加载卡信息到链表
int getCard() {
	Card* pCard = NULL;
	lpCardNode node = NULL;
	lpCardNode cur = NULL;

	// 先释放已有链表
	if (cardList != NULL) {
		releaseCardList();
	}
	initCardList();

	// 读取文件中的卡数量
	int nCount = getCardCount(CARDPATH);
	if (nCount <= 0) return 0;

	// 分配内存并读取文件
	pCard = (Card*)malloc(nCount * sizeof(Card));
	if (pCard == NULL) {
		return 0;
	}
	if (readCard(pCard, CARDPATH) == 0) {
		free(pCard);
		return 0;
	}

	// 构建链表
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

// 上机处理
int doLogon(const char* aName, const char* aPwd, LogonInfo* pLongInfo) {
	lpCardNode cardNode = NULL;
	int nIndex = 0;
	Billing billing;

	if (getCard() == 0) {
		return 0;
	}

	// 查找卡并验证
	cardNode = cardList->next;
	while (cardNode != NULL) {
		// 检查密码是否正确
		if (strcmp(cardNode->data.aName, aName) == 0 && strcmp(cardNode->data.aPwd, aPwd) != 0) {
			return WRONGPWD;
		}
		// 检查卡是否存在且可用的各项条件
		if (strcmp(cardNode->data.aName, aName) == 0 && strcmp(cardNode->data.aPwd, aPwd) == 0) {
			if (cardNode->data.nStatus == 2 || cardNode->data.nStatus == 3 || cardNode->data.nDel == 1) {
				return INVALID;
			}
			if (cardNode->data.nStatus == 1) {
				return ONSTATUS;
			}
			if (cardNode->data.fBalance <= 0) {
				return ENOUGHMONEY;
			}
			break;
		}
		cardNode = cardNode->next;
		nIndex++;
	}

	if (cardNode == NULL) {
		return WRONGCARD;
	}

	// 更新卡状态为上机
	cardNode->data.nStatus = 1;
	cardNode->data.tLast = time(NULL);
	cardNode->data.nUseCount++;

	// 保存卡更新并创建计费记录
	if (updateCard(&cardNode->data, CARDPATH, nIndex) == 1) {
		strcpy_s(billing.aName, sizeof(billing.aName), aName);
		billing.tStart = time(NULL);
		billing.nStatus = 0;    // 未结算
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
