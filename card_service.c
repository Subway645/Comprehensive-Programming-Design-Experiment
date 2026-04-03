#include<string.h>
 #include<stdlib.h>
#include"model.h"
#include"global.h"
#include"card_file.h"
int getCard();
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