#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include"model.h"
#include"card_service.h"
#include"menu.h"
int main() {
	getCard(); //程序启动时加载卡数据到链表
	int choice;
	do {
		outputmenu();  //循环输出菜单
		scanf("%d", &choice);
		switch (choice)
		{
		case 1:
			add();
			pause();
			break;
		case 2:
			query();
			pause();
			break;
		case 3:
			logon();
			pause();
			break;
		case 4:
			settle();
			pause();
			break;
		case 5:
			addMoney();
			pause();
			break;
		case 6:
			refundMoney();
			pause();
			break;
		case 7:
			statistics();
			pause();
			break;
		case 8:
			annul();
			pause();
			break;
		case 0:
			printf("----------退出----------\n");
			break;
		default:
			printf("输入错误，请重新输入！\n");
		}
	} while (choice != 0);
		return 0;
}