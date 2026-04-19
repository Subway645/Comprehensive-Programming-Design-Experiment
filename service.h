#pragma once

#include "model.h"

int addCardInfo(Card card);
int checkCardExists(const char* pName);
Card* queryCardsInfo(const char* pName, int* pIndex);
int doLogonInfo(const char* pName, const char* pPwd, LogonInfo* pLogonInfo);
void releaseList(void);
int doSettle(const char* pName, const char* pPwd, SettleInfo* pInfo);
int doAddMoney(const char* pName, const char* pPwd, MoneyInfo* pMoneyInfo);
int doRefundMoney(const char* pName, const char* pPwd, MoneyInfo* pMoneyInfo);
int doAnnulCard(const char* pName, const char* pPwd, AnnulInfo* pAnnulInfo);
int doQueryCardConsumption(const char* pName, const char* pPwd, const char* pStartDate, const char* pEndDate, ConsumptionInfo* pInfo);
float doQueryTurnover(const char* pStartDate, const char* pEndDate);
