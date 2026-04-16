#pragma once

#include "model.h"

int addCardInfo(Card card);
Card* queryCardsInfo(const char* pName, int* pIndex);
int doLogonInfo(const char* pName, const char* pPwd, LogonInfo* pLogonInfo);
void releaseList(void);
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);
