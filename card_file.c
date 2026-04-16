#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "model.h"
#include "global.h"
#include "time.h"
#include"tool.h"

#define CARDCHARNUM 256

Card praseCard(char* pBuf);

int saveCard(const Card* pcard, const char* pPath) {
	FILE* fp = NULL;
	fp = fopen(pPath, "a");
	if (fp == NULL) {
		return 0;
	}

	char StartTime[20], EndTime[20], LastTime[20];
	timeToString(pcard->tStart,StartTime);
	timeToString(pcard->tEnd, EndTime);
	timeToString(pcard->tLast, LastTime);
	fprintf(fp, "%s##%s##%d##%s##%s##%.1f##%s##%d##%.1f##%d\n",
		pcard->aName, pcard->aPwd, pcard->nStatus, StartTime, EndTime,
		pcard->fTotalUse, LastTime, pcard->nUseCount, pcard->fBalance, pcard->nDel);
	fclose(fp);
	return 1;
}

int readCard(Card* pCard, const char* pPath) {
	FILE* fp = NULL;
	char aBuf[CARDCHARNUM] = { 0 };
	int i = 0;
	fp = fopen(pPath, "r");
	if(fp == NULL) {
		return 0;
	}
	while (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
		if(strlen(aBuf)>0){
			pCard[i] = praseCard(aBuf);
			i++;
		}
	}
	fclose(fp);
	return 1;
}

Card praseCard(char* pBuf) {
	Card card;
	const char* delim = "##";
	char* buf = NULL;
	char* str = NULL;
	char flag[10][20] = { 0 };
	int index = 0;
	buf = pBuf;
	while ((str = strtok(buf, delim)) != NULL) {
		strcpy(flag[index], str);
		index++;
		buf = NULL;
	}
	strcpy(card.aName, flag[0]);
	strcpy(card.aPwd, flag[1]);
	card.nStatus = atoi(flag[2]);
	card.tStart = stringToTime(flag[3]);
	card.tEnd = stringToTime(flag[4]);
	card.fTotalUse = atof(flag[5]);
	card.tLast = stringToTime(flag[6]);
	card.nUseCount = atoi(flag[7]);
	card.fBalance = atof(flag[8]);
	card.nDel = atoi(flag[9]);

	return card;
}

int getCardCount(const char* pPath) {
	FILE* fp = NULL;
	char aBuf[CARDCHARNUM] = { 0 };
	int nCount = 0;
	fp = fopen(pPath, "r");
	if (fp == NULL) {
		return 0;
	}
	while (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
		if (strlen(aBuf) > 0) {
			nCount++;
		}
	}
	fclose(fp);
	return nCount;
}

int updateCard(const Card* pCard, const char* pPath, int nIndex) {
	FILE* fp = NULL;
	int nLine = 0;
	char aBuf[CARDCHARNUM] = { 0 };
	long lPosition = 0;
	fp = fopen(pPath, "r+");
	if (fp == NULL) {
		return 0;
	}
	while (fgets(aBuf, CARDCHARNUM, fp) != NULL) {
		if (nLine == nIndex) {
			lPosition = ftell(fp) - strlen(aBuf);
			break;
		}
		nLine++;
	}
	if (nLine != nIndex) {
		fclose(fp);
		return 0;
	}
	fseek(fp, lPosition, 0);
	char StartTime[20], EndTime[20], LastTime[20];
	timeToString(pCard->tStart, StartTime);
	timeToString(pCard->tEnd, EndTime);
	timeToString(pCard->tLast, LastTime);
	fprintf(fp, "%s##%s##%d##%s##%s##%.1f##%s##%d##%.1f##%d\n",
		pCard->aName, pCard->aPwd, pCard->nStatus, StartTime, EndTime,
		pCard->fTotalUse, LastTime, pCard->nUseCount, pCard->fBalance, pCard->nDel);
	fclose(fp);
	return 1;
}
