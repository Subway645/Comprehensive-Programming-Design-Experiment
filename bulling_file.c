#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"model.h"
#include"global.h"

int saveBilling(const Billing* pBilling, const char* pPath)
{
	FILE* fp = NULL;
	//打开二进制文件
	fopen_s(&fp, pPath, "ab");
	if(fp == NULL) {
		return 0;
	}

	//将数据写进二进制文件
	fwrite(pBilling, sizeof(Billing), 1, fp);

	//关闭文件
	fclose(fp);

	return 1;
}

int readBilling(Billing* pBilling, const char* pPath)
{
	//读取计费信息文件中所有的计费信息
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if(fp == NULL) {
		return 0;
	}

	// 计算文件中记录数并一次性读取所有记录
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	if (nSize <= 0) {
		fclose(fp);
		return 0;
	}
	long nCount = nSize / sizeof(Billing);
	fseek(fp, 0, SEEK_SET);

	// 读取所有记录到 pBilling 指向的缓冲区
	size_t nRead = fread(pBilling, sizeof(Billing), (size_t)nCount, fp);

	//关闭文件
	fclose(fp);

	if (nRead != (size_t)nCount) {
		return 0;
	}

	return 1;
}

int getBillingCount(const char* pPath) {
	//读取计费信息文件中计费信息的数量
		FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if(fp == NULL) {
		return 0;
	}

	//获取文件大小
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fclose(fp);

	return nSize / sizeof(Billing);
}

int updateBilling(Billing* pBilling, char* pPath, int nIndex) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb+"); 
	if(fp == NULL) {
		return 0;
	}
	//计算要更新的记录在文件中的位置
	long lPosition = nIndex * sizeof(Billing);
	fseek(fp, lPosition, SEEK_SET);
	//将更新后的数据写回文件
	fwrite(pBilling, sizeof(Billing), 1, fp);
	fclose(fp);
	return 1;
}