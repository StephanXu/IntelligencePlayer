#pragma once
#include "B_ShareMemory.h"
#include <Windows.h>
#include "..\IntelligenceHidingEngine\CommonMessages.h"
#include <vector>
#include <ctime>

using namespace std;

class CInstance
{
public:
	_MemorySharer m_MemShare;
	int m_Id;
	HWND m_hWnd;

	int InitializeInstance(
		double explore, 
		double decay, 
		int maxthread,
		int lockenemy, 
		int lockwinner, 
		double benefitgain, 
		double avoidrisk, 
		double complex_prop,
		double base_prop);

	int SetStatus(
		char* player0_card,
		char* player1_card,
		char* player2_card,
		char* player0_last,
		char* player1_last,
		char* player2_last,
		int current_player);

	int PutCards(int* put);

	int GetBestChoice(int* dest, int* cardnum);

	int BeginCalc();

	int FreeHidingEngine();

	int GetLog(char* buf, int buflen);

	int ResetArgu(
		double explore, 
		double decay, 
		int maxthread, 
		int lockenemy, 
		int lockwinner, 
		double benefitgain, 
		double avoidrisk, 
		double complex_prop, 
		double base_prop);

	int IsStillExsist();

	int SubmitResult(int winner, int income);
};

class CIntelligenceControl
{
public:
	int SetupInstance(CInstance** dest_new_instance);

	CInstance* GetInstance(int id);


private:
	vector<CInstance> m_instances;
};
