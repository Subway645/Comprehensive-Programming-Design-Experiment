#ifndef CARD_SERVICE_H
#define CARD_SERVICE_H

#include "model.h"

extern lpCardNode cardList;

int addCard(Card card);
Card* queryCards(const char* pName, int* pIndex);
Card* queryCard(const char* pName);
int doLogon(const char* aName, const char* aPwd, LogonInfo* pLongInfo);
int getCard(void);
void releaseCardList(void);

#endif
