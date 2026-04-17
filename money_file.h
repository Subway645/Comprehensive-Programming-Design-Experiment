#ifndef MONEY_FILE_H
#define MONEY_FILE_H

#include "model.h"

//保存充值退费信息到文件
int saveMoney(const Money* pMoney, const char* pPath);

#endif
