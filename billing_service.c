#include"model.h"
#include"global.h"
#include"billing_file.h"
#include"billing_service.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

BillingNode* billingList = NULL;

// 初始化链表
int initBillingList() {
	BillingNode* head = (BillingNode*)malloc(sizeof(BillingNode));
	if (head != NULL) {
		head->next = NULL;
		billingList = head;
		return 1;
	}
	return 0;
}

// 释放链表资源
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

// 添加计费信息（调用 billing_file 中的保存函数）
int addBilling(Billing billing){
	return saveBilling(&billing, BILLINGPATH);
}

// 获取计费信息（调用 billing_file 中的读取函数，将计费信息保存到链表中）
int getBilling() {
	//调用billing_file中函数,将计费信息保存到链表中
	Billing* pBilling = NULL;
	BillingNode* node = NULL;
	BillingNode* cur = NULL;
	if (billingList != NULL) {
		releaseBillingList();
	}
	initBillingList();
	
	int nCount = getBillingCount(BILLINGPATH);
	if(nCount <= 0) return 0;
	
	pBilling = (Billing*)malloc(sizeof(Billing) * nCount);
	if (pBilling == NULL) return 0;
	
	if (readBilling(pBilling, BILLINGPATH) == 0) {
		free(pBilling);
		return 0;
	}
	
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

// 查询计费信息（调用 billing_file 中的查询函数）
Billing* queryBilling(const char* pName, int* pIndex) {
	if (pName == NULL) return NULL;
	if (pIndex) *pIndex = 0;
	
	if (getBilling() == 0) return NULL;
	
	BillingNode* cur = billingList->next;
	Billing* pBilling = NULL;
	int count = 0;
	
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