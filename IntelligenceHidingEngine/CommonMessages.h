#pragma once


#define WM_INTELLIGENCE_INIT_INSTANCE		0x5001
#define WM_INTELLIGENCE_SET_STATUS			0x5004
#define WM_INTELLIGENCE_PUT_CARD			0x5005
#define WM_INTELLIGENCE_BEGIN_CALC			0x5006
#define WM_INTELLIGENCE_END_CALC			0x5007
#define WM_INTELLIGENCE_CLOSE_ENGINE		0x5008
#define WM_INTELLIGENCE_GET_LOG				0x5009
#define WM_INTELLIGENCE_RESET_ARGU			0x500a
#define WM_INTELLIGENCE_SUBMIT_RESULT		0x500b

#define INTELLIGENCE_BUILD_TUBE_NAME(num)	_bstr_t(L"Intelligence:") + _bstr_t(num)
#define ENGINE_NAME "IntelligenceHidingEngine.exe"
#define WAIT_TIME_OUT 1000
#define INTELLIGENCE_BUILD_TITLE(num) _bstr_t(L"IntelligenceWnd")

#define VER 0x20171110
#define WEB_CHECK "http://106.14.176.131:8468/intelligence.php"
#define WEB_STATISTIC "http://106.14.176.131:8468/add_statistic.php"

class CIntelligenceMessage
{
public:
};

class CInitStruct:public CIntelligenceMessage
{
public:
	int m_MaxSubThreadNum;
	double m_DecayFactor;
	double m_Explore;
	int m_LockEnemy;
	int m_LockWinner;
	double m_BenefitGain;
	double m_AvoidRisk;
	double m_Complex_prop;
	double m_Base_prop;
};


class CSetStatusStruct:public CIntelligenceMessage
{
public:
	int m_CurrentPlayer;
	char m_Player_cards[3][21];
	char m_Player_lastcards[3][21];

};

class CPutCardsStruct:public CIntelligenceMessage
{
public:
	#define AUTO_PLAYER -1
	int m_Put[21];
	int m_Player;
	int m_PutCount;
};

class CSubmitResultStruct :public CIntelligenceMessage
{
public:
	int m_Winner;
	int m_Income;
};