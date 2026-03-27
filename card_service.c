#include<string.h>
#include<stdlib.h>
#include"model.h"
#include"global.h"

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
	lpCardNode cur = NULL;

	if (cardList == NULL) {
		if (!initCardList()) return 0;
	}
	//将数据保存到结点中
	cur = (lpCardNode)malloc(sizeof(CardNode));
	if (cur != NULL) {
		cur->data = card;
		cur->next = NULL;
		//遍历链表找到最后一个结点（使用临时指针，避免修改全局头指针）
		lpCardNode p = cardList;
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = cur;
		return 1;
	}
	return 0;
}

Card* queryCard(const char* aName) {
	lpCardNode cur = NULL;
	if (cardList != NULL) {
		cur = cardList->next;
		while (cur != NULL) {
			if (strcmp(cur->data.aName, aName) == 0) {
				return &cur->data;
			}
			cur = cur->next;
		}
	}
	return NULL;
}

Card* queryCards(const char* pName, int* pIndex) {
	lpCardNode cur = NULL;
	Card* pCard = NULL;
	int count = 0;

	if (pIndex) *pIndex = 0;

	if (cardList != NULL) {
		cur = cardList->next;
		pCard = (Card*)malloc(sizeof(Card));
		if (pCard == NULL) return NULL;
		while (cur != NULL) {
			if (strstr(cur->data.aName, pName) != NULL) {
				pCard[count] = cur->data;
				count++;
				if (count > 0) {
					Card* tmp = (Card*)realloc(pCard, (count + 1) * sizeof(Card));
					if (tmp == NULL) {
						free(pCard);
						if (pIndex) *pIndex = 0;
						return NULL;
					}
					pCard = tmp;
				}
			}
			cur = cur->next;
		}
	}

	if (count == 0) {
		if (pCard) free(pCard);
		if (pIndex) *pIndex = 0;
		return NULL;
	}
	if (pIndex) *pIndex = count;
	return pCard;
}