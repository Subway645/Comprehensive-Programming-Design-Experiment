#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "model.h"
#include "global.h"

// 保存计费记录到文件（追加模式）
int saveBilling(const Billing* pBilling, const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "ab");
	if (fp == NULL) {
		return 0;
	}
	fwrite(pBilling, sizeof(Billing), 1, fp);
	fclose(fp);
	return 1;
}

// 从文件读取所有计费记录
int readBilling(Billing* pBilling, const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if (fp == NULL) {
		return 0;
	}

	// 计算文件中记录数
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	if (nSize <= 0) {
		fclose(fp);
		return 0;
	}
	long nCount = nSize / sizeof(Billing);
	fseek(fp, 0, SEEK_SET);

	// 一次性读取所有记录
	size_t nRead = fread(pBilling, sizeof(Billing), (size_t)nCount, fp);
	fclose(fp);

	if (nRead != (size_t)nCount) {
		return 0;
	}
	return 1;
}

// 获取计费记录数量
int getBillingCount(const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if (fp == NULL) {
		return 0;
	}

	// 获取文件大小
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);

	return nSize / sizeof(Billing);
}

// 更新指定索引的计费记录
int updateBilling(Billing* pBilling, char* pPath, int nIndex) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb+");
	if (fp == NULL) {
		return 0;
	}

	// 定位到指定记录位置
	long lPosition = nIndex * sizeof(Billing);
	fseek(fp, lPosition, SEEK_SET);

	// 写入更新后的数据
	fwrite(pBilling, sizeof(Billing), 1, fp);
	fclose(fp);
	return 1;
}
