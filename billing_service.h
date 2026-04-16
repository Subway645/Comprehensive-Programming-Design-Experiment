#pragma once
#include"model.h"
int addBilling(Billing billing);
Billing* queryBilling(const char* pName, int* pIndex);
void releaseBillingList(void);
int updateBilling(Billing* pBilling, char* pPath, int nIndex);