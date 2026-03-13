#pragma once
int addCard(Card card);//添加卡函数，参数为卡信息结构体，返回值为1表示添加成功，0表示添加失败
Card* queryCard(const char* aName);//查询卡函数，参数为卡号，返回值为指向卡信息结构体的指针，如果未找到则返回NULL

