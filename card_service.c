#define _CRT_SECURE_NO_WARNINGS
#include<string.h>
#include<stdlib.h>
#include"model.h"
#include"global.h"
#include"card_file.h"
#include"time.h"
#include"billing_file.h"
#include"billing_service.h"
int getCard(void);
Card aCard[50];  //卡信息结构体
int nCount = 0;  //记录卡的数量

lpCardNode cardList = NULL; //链表头指针

//初始化链表
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
	return saveCard(&card, CARDPATH);
}

Card* queryCard(const char* aName) {
	if (aName == NULL || cardList == NULL) {
		return NULL;
	}

	lpCardNode cur = cardList->next;
	while (cur != NULL) {
		// 匹配卡号且该卡未被删除 (nDel == 0)
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

	// 第一遍遍历：统计符合要求的卡的数量
	while (cur != NULL) {
		if (strstr(cur->data.aName, pName) != NULL && cur->data.nDel == 0) {
			count++;
		}
		cur = cur->next;
	}

	if (count == 0) {
		return NULL;
	}

	// 第二遍遍历：分配精确的内存并将卡信息保存到数组	
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

// 释放链表
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

//将文件中的卡信息保存到链表中
int getCard() {
	Card* pCard = NULL;
	lpCardNode node = NULL;
	lpCardNode cur = NULL;
	if(cardList != NULL) {
		releaseCardList();
	}
	initCardList();
	//获取卡信息数量
	int nCount = getCardCount(CARDPATH);

	//动态分配内存保存卡信息
	pCard = (Card*)malloc(nCount * sizeof(Card));
	if(pCard == NULL) {
		return 0;
	}
	//读取卡信息
	if(readCard(pCard, CARDPATH) == 0) {
		pCard = NULL;
		return 0;
	}
	node = cardList;
	//将卡信息添加到链表中
for(int i = 0; i < nCount; i++) {
		//为节点分配内存
		cur = (lpCardNode)malloc(sizeof(CardNode));

		//如果分配内存失败，释放已分配的内存并返回0
		if(cur == NULL) {
			releaseCardList();
			return 0;
		}

		//初始化新的空间
		memset(cur, 0, sizeof(CardNode));

		//将卡信息保存到节点中
		cur->data = pCard[i];
		cur->next = NULL;

		//将节点添加到链表尾部
		node->next = cur;
		node = cur;
	}
	free(pCard);
	pCard = NULL;
	return 1;
}

// 上机处理：按 card_service.h 中声明的 doLogon 接口实现
int doLogon(const char* aName, const char* aPwd, LogonInfo* pLongInfo) {
	lpCardNode cardNode = NULL;
	int nIndex = 0;
	Billing billing;
	//获取文件中卡信息
	if(getCard() == 0) {
		return 0;
	}

	cardNode = cardList->next;
	//遍历链表判断能否上机
	while(cardNode != NULL) {
		if(strcmp(cardNode->data.aName, aName) == 0 && strcmp(cardNode->data.aPwd, aPwd) == 0) {
			if(cardNode->data.nStatus == 1) {
				//如果卡状态为正在上机，返回0表示不能上机
				return 0;
			}
			if(cardNode->data.fBalance <= 0) {
				//如果余额不足，返回3表示余额不足
				return 3;
			}
			if(cardNode->data.nStatus == 2 || cardNode->data.nStatus == 3 || cardNode->data.nDel == 1) {
				//如果卡状态为已注销或失效，返回2表示不可用
				return 2;
			}
			// 找到匹配并且状态有效的卡，直接跳出循环
			break;
		}
		cardNode = cardNode->next;
		nIndex++;
	}

	if (cardNode == NULL) {
		return 0; // 卡不存在或密码错误
	}

	cardNode->data.nStatus = 1; //更新卡状态为正在上机
	cardNode->data.tLast = time(NULL); //更新最后使用时间
	cardNode->data.nUseCount++; //使用次数加1
	//如果能上机，更新卡信息
	if (updateCard(&cardNode->data, CARDPATH, nIndex)==1) {
		strcpy_s(billing.aName, sizeof(billing.aName), aName);
		billing.tStart = time(NULL);
		billing.nStatus = 0;
		billing.nDel = 0;
		billing.fAmount = 0;
		billing.tEnd = 0;
		if(addBilling(billing) == 1) {
			if(pLongInfo != NULL) {
				strcpy(pLongInfo->aName, aName);						
	pLongInfo->fBalance = cardNode->data.fBalance;
			}
			return 1;
		}
	}

	return 0;
}