#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "model.h"

// 保存充值退费记录到文件（追加模式）
int saveMoney(const Money* pMoney, const char* pPath) {
	FILE* fp = NULL;
	fopen_s(&fp, pPath, "ab");
	if (fp == NULL) {
		return 0;
	}
	fwrite(pMoney, sizeof(Money), 1, fp);
	fclose(fp);
	return 1;
}
