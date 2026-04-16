#ifndef CARD_FILE_H
#define CARD_FILE_H

int saveCard(const Card* pCard, const char* pPath);  //保存卡信息到文件
int getCardCount(const char* pPath);  //获取文件中卡信息的数量
int readCard(Card* pCard, const char* pPath);  //读取文件中的卡信息
int updateCard(const Card* pCard, const char* pPath, int nIndex);  //更新第nIndex条卡信息

#endif
