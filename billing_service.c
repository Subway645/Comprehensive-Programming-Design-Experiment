#include "model.h"
#include "global.h"
#include "billing_file.h"
#include "billing_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 计费链表头指针
BillingNode* billingList = NULL;

// 初始化计费链表
int initBillingList() {
	BillingNode* head = (BillingNode*)malloc(sizeof(BillingNode));
	if (head != NULL) {
		head->next = NULL;
		billingList = head;
		return 1;
	}
	return 0;
}

// 释放计费链表内存
void releaseBillingList() {
	BillingNode* cur = billingList;
	BillingNode* next = NULL;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
	billingList = NULL;
}

// 添加计费记录到文件
int addBilling(Billing billing) {
	return saveBilling(&billing, BILLINGPATH);
}

// 从文件加载计费信息到链表
int getBilling() {
	Billing* pBilling = NULL;
	BillingNode* node = NULL;
	BillingNode* cur = NULL;

	// 释放已有链表并重新初始化
	if (billingList != NULL) {
		releaseBillingList();
	}
	initBillingList();

	// 获取文件中的记录数
	int nCount = getBillingCount(BILLINGPATH);
	if (nCount <= 0) return 0;

	// 分配内存并读取文件
	pBilling = (Billing*)malloc(sizeof(Billing) * nCount);
	if (pBilling == NULL) return 0;

	if (readBilling(pBilling, BILLINGPATH) == 0) {
		free(pBilling);
		return 0;
	}

	// 构建链表
	node = billingList;
	for (int i = 0; i < nCount; i++) {
		cur = (BillingNode*)malloc(sizeof(BillingNode));
		if (cur == NULL) {
			releaseBillingList();
			free(pBilling);
			return 0;
		}
		cur->data = pBilling[i];
		cur->next = NULL;
		node->next = cur;
		node = cur;
	}
	free(pBilling);
	return 1;
}

// 查询指定卡的未结算计费记录
Billing* queryBilling(const char* pName, int* pIndex) {
	if (pName == NULL) return NULL;
	if (pIndex) *pIndex = 0;

	if (getBilling() == 0) return NULL;

	BillingNode* cur = billingList->next;
	Billing* pBilling = NULL;
	int count = 0;

	// 查找该卡且状态为未结算的记录
	while (cur != NULL) {
		if (strcmp(cur->data.aName, pName) == 0 && cur->data.nStatus == 0) {
			if (pIndex) *pIndex = count;
			return &cur->data;
		}
		cur = cur->next;
		count++;
	}
	return NULL;
}
