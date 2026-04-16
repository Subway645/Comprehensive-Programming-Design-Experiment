#ifndef CARD_SERVICE_H
#define CARD_SERVICE_H

#include "model.h"
int addCard(Card card);//添加卡函数，参数为卡信息结构体，返回值为1表示添加成功，0表示添加失败
Card* queryCards(const char* pName, int* pIndex); //查询卡函数，参数为卡号，返回值为指向卡信息结构体的指针，如果未找到则返回NULL
Card* queryCard(const char* pName); //查询卡函数，参数为卡号，返回值为指向卡信息结构体的指针，如果未找到则返回NULL
int doLogon(const char* aName, const char* aPwd, LogonInfo* pLongInfo); //上机函数
int getCard(void);
void releaseCardList(void); //释放卡链表内存

#endif 
