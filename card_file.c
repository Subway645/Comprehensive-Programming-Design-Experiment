#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "model.h"
#include "global.h"

// 保存卡信息到文件（追加模式）
int saveCard(const Card* pCard, const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "ab");
	if (fp == NULL) {
		return 0;
	}
	fwrite(pCard, sizeof(Card), 1, fp);
	fclose(fp);
	return 1;
}

// 从文件读取所有卡信息
int readCard(Card* pCard, const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if (fp == NULL) {
		return 0;
	}

	// 计算记录数
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	if (nSize <= 0) {
		fclose(fp);
		return 0;
	}
	long nCount = nSize / sizeof(Card);
	fseek(fp, 0, SEEK_SET);

	// 读取所有记录
	size_t nRead = fread(pCard, sizeof(Card), (size_t)nCount, fp);
	fclose(fp);
	if (nRead != (size_t)nCount) {
		return 0;
	}
	return 1;
}

// 获取卡记录数量
int getCardCount(const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb");
	if (fp == NULL) {
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	long nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return nSize / sizeof(Card);
}

// 更新指定索引的卡信息
int updateCard(const Card* pCard, const char* pPath, int nIndex) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "rb+");
	if (fp == NULL) {
		return 0;
	}
	long lPosition = nIndex * sizeof(Card);
	fseek(fp, lPosition, SEEK_SET);
	fwrite(pCard, sizeof(Card), 1, fp);
	fclose(fp);
	return 1;
}
