#ifndef BILLING_FILE_H
#define BILLING_FILE_H
#include "model.h"
int saveBilling(const Billing* pBilling, const char* pPath);
int readBilling(Billing* pBilling, const char* pPath);
int getBillingCount(const char* pPath);
int updateBilling(Billing* pBilling, char* pPath, int nIndex);
#endif

