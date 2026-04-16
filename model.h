#ifndef MODEL_H
#define MODEL_H

#include <time.h>

//卡信息结构体
typedef struct Card
{
	char aName[10];   //卡号
	char aPwd[8];     //密码
	int nStatus;      //卡状态（0-未上机；1-正在上机；2-已注销；3-失效）
	time_t tStart;    //开卡时间
	time_t tEnd;      //卡的截止时间
	float fTotalUse;  //累计金额
	time_t tLast;   //最后使用时间
	int nUseCount;    //使用次数
	float fBalance;   //余额
	int nDel;		  //删除标志（0-未删除；1-已删除）
}Card;

//链表节点结构体
typedef struct CardNode {
	Card data;
	struct CardNode* next;
}CardNode,*lpCardNode;


//消费记录结构体
typedef struct Billing {
	char aName[18];   //卡号
	time_t tStart;    //上机时间
	time_t tEnd;      //下机时间
	float fAmount;   //本次使用金额
	int nStatus;      //消费状态（0-未结算；1-已结算）
	int nDel;		  //删除标志（0-未删除；1-已删除）
}Billing;

//上机信息结构体
typedef struct LogonInfo {
	char aName[18];   //卡号
	time_t tLogon;    //上机时间
	float fBalance;   //余额
}LogonInfo;

//下机信息结构体
typedef struct BillingNode {
	Billing data;
	struct BillingNode* next;
} BillingNode;

//结算信息结构体
typedef struct SettleInfo {
	char aName[18];   //卡号
	time_t tStart;    //上机时间
	time_t tEnd;   //下机时间
	float fAmount;    //本次使用金额
	float fBalance;   //余额
}SettleInfo;
#endif
