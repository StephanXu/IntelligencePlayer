#include "stdafx.h"
#include "Instance.h"

int CInstance::InitializeInstance(double explore, double decay, int maxthread, int lockenemy, int lockwinner, double benefitgain, double avoidrisk, double complex_prop, double base_prop)
{
	CInitStruct* pS = (CInitStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CInitStruct));
	ZeroMemory(pS, sizeof(CInitStruct));
	if (!pS)
		return -1;
	pS->m_Explore = explore;
	pS->m_DecayFactor = decay;
	pS->m_MaxSubThreadNum = maxthread;
	pS->m_LockEnemy = lockenemy;
	pS->m_LockWinner = lockwinner;
	pS->m_BenefitGain = benefitgain;
	pS->m_AvoidRisk = avoidrisk;
	pS->m_Complex_prop = complex_prop;
	pS->m_Base_prop = base_prop;
	return SendMessage(m_hWnd, WM_INTELLIGENCE_INIT_INSTANCE, m_Id, VER);
}

int CInstance::SetStatus(char * player0_card, char * player1_card, char * player2_card, char * player0_last, char * player1_last, char * player2_last, int current_player)
{
	CSetStatusStruct* pS = (CSetStatusStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CSetStatusStruct));
	ZeroMemory(pS, sizeof(CSetStatusStruct));
	memcpy_s(pS->m_Player_cards[0], 21, player0_card, 21);
	memcpy_s(pS->m_Player_cards[1], 21, player1_card, 21);
	memcpy_s(pS->m_Player_cards[2], 21, player2_card, 21);
	memcpy_s(pS->m_Player_lastcards[0], 21, player0_last, 21);
	memcpy_s(pS->m_Player_lastcards[1], 21, player1_last, 21);
	memcpy_s(pS->m_Player_lastcards[2], 21, player2_last, 21);
	pS->m_CurrentPlayer = current_player;

	return SendMessage(m_hWnd, WM_INTELLIGENCE_SET_STATUS, m_Id, 0);
}

int CInstance::PutCards(int * put)
{
	CPutCardsStruct* pS = (CPutCardsStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CPutCardsStruct));
	ZeroMemory(pS, sizeof(CPutCardsStruct));
	memcpy_s(pS->m_Put, sizeof(int) * 21, put, sizeof(int) * 21);
	pS->m_Player = AUTO_PLAYER;
	return SendMessage(m_hWnd, WM_INTELLIGENCE_PUT_CARD, m_Id, 0);
}

int CInstance::GetBestChoice(int * dest, int * cardnum)
{
	CPutCardsStruct* pS = (CPutCardsStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CPutCardsStruct));
	ZeroMemory(pS, sizeof(CPutCardsStruct));
	SendMessage(m_hWnd, WM_INTELLIGENCE_END_CALC, m_Id, 0);
	SendMessage(m_hWnd, WM_INTELLIGENCE_BEGIN_CALC, m_Id, 0);
	ZeroMemory(dest, sizeof(int) * 21);
	memcpy_s(dest, sizeof(int) * 21, pS->m_Put, sizeof(int) * 21);
	int ret{ pS->m_Player };
	if (cardnum) *cardnum = pS->m_PutCount;
	UnmapViewOfFile(pS);
	return ret;
}

int CInstance::BeginCalc()
{
	return SendMessage(m_hWnd, WM_INTELLIGENCE_BEGIN_CALC, 0, 0);
}

int CInstance::FreeHidingEngine()
{
	return SendMessage(m_hWnd, WM_INTELLIGENCE_CLOSE_ENGINE, 0, 0);
}

int CInstance::GetLog(char * buf, int buflen)
{
	int len = (int)SendMessage(m_hWnd, WM_INTELLIGENCE_GET_LOG, m_Id, 0);
	char* pBuf = (char*)m_MemShare.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id));
	ZeroMemory(buf, buflen);
	memcpy_s(buf, buflen, pBuf, len);
	UnmapViewOfFile(pBuf);
	return len;
}

int CInstance::ResetArgu(double explore, double decay, int maxthread, int lockenemy, int lockwinner, double benefitgain, double avoidrisk, double complex_prop, double base_prop)
{
	CInitStruct* pS = (CInitStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CInitStruct));
	ZeroMemory(pS, sizeof(CInitStruct));
	if (!pS)
		return -1;
	pS->m_Explore = explore;
	pS->m_DecayFactor = decay;
	pS->m_MaxSubThreadNum = maxthread;
	pS->m_LockEnemy = lockenemy;
	pS->m_LockWinner = lockwinner;
	pS->m_BenefitGain = benefitgain;
	pS->m_AvoidRisk = avoidrisk;
	pS->m_Complex_prop = complex_prop;
	pS->m_Base_prop = base_prop;
	return SendMessage(m_hWnd, WM_INTELLIGENCE_RESET_ARGU, m_Id, VER);
}

int CInstance::IsStillExsist()
{
	return IsWindow(m_hWnd);
}

int CInstance::SubmitResult(int winner, int income)
{
	CSubmitResultStruct* pS = (CSubmitResultStruct*)m_MemShare.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(m_Id), sizeof(CSubmitResultStruct));
	ZeroMemory(pS, sizeof(CSubmitResultStruct));
	if (!pS)
		return -1;
	pS->m_Winner = winner;
	pS->m_Income = income;
	return SendMessage(m_hWnd, WM_INTELLIGENCE_SUBMIT_RESULT, m_Id, VER);
}

int CIntelligenceControl::SetupInstance(CInstance ** dest_new_instance)
{
	CInstance ins;
	ins.m_Id = (unsigned int)m_instances.size();
	HWND hwnd = FindWindow(NULL, L"IntelligenceHidingEngine");
	if (hwnd)
		return -1;
	WinExec(ENGINE_NAME, SW_SHOWNORMAL);
	int start_time{ clock() };
	while (!(hwnd = FindWindow(NULL, L"IntelligenceHidingEngine")))
	{
		if (clock() - start_time > WAIT_TIME_OUT)
			return -2;
	}
	ins.m_hWnd = hwnd;
	SetWindowText(ins.m_hWnd, INTELLIGENCE_BUILD_TITLE(ins.m_Id));
	m_instances.push_back(ins);
	if (dest_new_instance != 0)
		*dest_new_instance = &m_instances[m_instances.size() - 1];
	return ins.m_Id;
}

CInstance * CIntelligenceControl::GetInstance(int id)
{
	if ((unsigned int)id >= m_instances.size())
		return 0;
	return &m_instances[id];
}
