#ifndef CARD_FILE_H
#define CARD_FILE_H

int saveCard(const Card* pCard, const char* pPath);  //保存卡信息到文件，参数为卡信息结构体和文件路径，返回值为1表示保存成功，0表示保存失败
int getCardCount(const char* pPath);  //获取文件中卡信息的数量，参数为文件路径，返回值为卡信息的数量
int readCard(Card* pCard, const char* pPath);  //读取文件中的卡信息，参数为保存卡信息的结构体指针和文件路径，返回值为1表示读取成功，0表示读取失败
int updateCard(const Card* pCard, const char* pPath, int nIndex);  //更新文件中的卡信息，参数为卡信息结构体指针、文件路径和要更新的行数，返回值为1表示更新成功，0表示更新失败
#endif
