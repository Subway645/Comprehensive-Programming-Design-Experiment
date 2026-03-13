#include"model.h"

Card aCard[50];  //卡信息结构体
int nCount = 0;  //记录卡的数量

int addCard(Card card) {
	if (nCount < 50) {
		aCard[nCount] = card;
		nCount++;
		return 1; //添加成功
	}
	else {
		printf("卡已满，无法添加！\n");
		return 0; //添加失败，卡已满
	}
}

Card* queryCard(const char* aName) {
	int i;
	for (i = 0; i < nCount; i++) {
		if (strcmp(aCard[i].aName, aName) == 0 && aCard[i].nDel == 0) {
			return &aCard[i]; //找到卡，返回指针
		}
	}
	return NULL; //未找到卡
}