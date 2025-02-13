// IntelligenceExport.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "IntelligenceExport.h"
#include "B_ShareMemory.h"
#include "..\IntelligenceHidingEngine\CommonMessages.h"
#include "Instance.h"


using namespace std;

CIntelligenceControl g_InsCon;
char Num2CardName(int num);
int CardName2Num(char card_name);

int __stdcall Ver()
{
	return VER;
}

int __stdcall SetupInstance()
{
	return g_InsCon.SetupInstance(0);
}

int __stdcall InitializeInstance(int id, double explore, double decay, int search_ability, int lockenemy, int lockwinner, double benefitgain, double avoidrisk, double complex_prop, double base_prop)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->InitializeInstance(explore, decay, search_ability, lockenemy, lockwinner, benefitgain, avoidrisk, complex_prop, base_prop);
}

int __stdcall SetStatus(
	int id,
	char* player0_card,
	char* player1_card,
	char* player2_card,
	char* player0_last,
	char* player1_last,
	char* player2_last,
	int current_player)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->SetStatus(player0_card,
		player1_card,
		player2_card,
		player0_last,
		player1_last,
		player2_last,
		current_player);
}

int __stdcall PutCards(int id, char* put, int put_num)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	int put_card[21]{};
	for (int i{}; i < put_num; i++)
	{
		put_card[i] = CardName2Num(put[i]);
	}
	return pIns->PutCards(put_card);
}

int __stdcall GetBestChoice(int id, char* dest, int* cardnum)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	int put_card[21]{};
	int card_num{};
	int ret{};
	ret = pIns->GetBestChoice(put_card, &card_num);
	for (int i{}; i < card_num; i++)
	{
		dest[i] = Num2CardName(put_card[i]);
	}
	*cardnum = card_num;
	return ret;
}

int __stdcall BeginCalc(int id)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->BeginCalc();
}

int __stdcall FreeHidingEngine(int id)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->FreeHidingEngine();
}

int __stdcall GetLog(int id, char* buf, int buflen)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->GetLog(buf, buflen);
}

int __stdcall ResetArgu(int id, double explore, double decay, int search_ability, int lockenemy, int lockwinner, double benefitgain, double avoidrisk, double complex_prop, double base_prop)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->ResetArgu(explore, decay, search_ability, lockenemy, lockwinner, benefitgain, avoidrisk, complex_prop, base_prop);
}

int __stdcall IsStillExsist(int id)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->IsStillExsist();
}

int __stdcall SubmitResult(int id, int winner, int income)
{
	CInstance* pIns = g_InsCon.GetInstance(id);
	if (!pIns)
		return -1;
	return pIns->SubmitResult(winner, income);
}

char Num2CardName(int num)
{
	if (num == 3)
		return '3';
	if (num == 4)
		return '4';
	if (num == 5)
		return '5';
	if (num == 6)
		return '6';
	if (num == 7)
		return '7';
	if (num == 8)
		return '8';
	if (num == 9)
		return '9';
	if (num == 10)
		return 'X';
	if (num == 11)
		return 'J';
	if (num == 12)
		return 'Q';
	if (num == 13)
		return 'K';
	if (num == 14)
		return 'A';
	if (num == 15)
		return '2';
	if (num == 16)
		return 'b';
	if (num == 17)
		return 'B';
	return 0;
}

int CardName2Num(char card_name)
{
	if (card_name == '3')
		return 3;
	if (card_name == '4')
		return 4;
	if (card_name == '5')
		return 5;
	if (card_name == '6')
		return 6;
	if (card_name == '7')
		return 7;
	if (card_name == '8')
		return 8;
	if (card_name == '9')
		return 9;
	if (card_name == 'X')
		return 10;
	if (card_name == 'J')
		return 11;
	if (card_name == 'Q')
		return 12;
	if (card_name == 'K')
		return 13;
	if (card_name == 'A')
		return 14;
	if (card_name == '2')
		return 15;
	if (card_name == 'b')
		return 16;
	if (card_name == 'B')
		return 17;

	return 0;
}
