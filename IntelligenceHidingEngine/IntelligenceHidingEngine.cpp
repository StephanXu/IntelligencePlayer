//IntelligenceHidingEngine.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "IntelligenceHidingEngine.h"
#include "CommonMessages.h"
#include "Engine.h"
#include "B_ShareMemory.h"
#include "http.h"
#include <ShellAPI.h>
#include <ShlObj.h>
#include <fstream>
#include <DbgHelp.h>

#pragma comment(lib,"DbgHelp.lib")

#pragma data_seg("shared")

#pragma data_seg()

#define MAX_LOADSTRING 100


// 全局变量: 
HWND g_hWnd{};
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				SelfDel();
void				WriteLog(string& str);
long CALLBACK		CrashHandler(EXCEPTION_POINTERS *pException);
string				SendLogs(time_t begin_time, int lock_enemy, int lock_winner, int winner, string log, int income, int client_ver, string client_machine_code);
void				CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS * pException);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INTELLIGENCEHIDINGENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	SetUnhandledExceptionFilter(CrashHandler);
	//check web arguments
	char num[0x10]{};
	char send[0x50]{};
	_itoa_s(VER, num, 16);
	sprintf_s(send, "data=%s", num);
	FunData http_ret = sendhttpc(WEB_CHECK, send, true);
	string ret = http_ret.getdata();
	if (ret.compare(num))
	{
		HWND hwnd{};
		SetWindowText(g_hWnd, L"Engine");
		for (int i{}; ; i++)
		{
			hwnd = FindWindow(0, INTELLIGENCE_BUILD_TITLE(i));
			if (!hwnd)
			{
				hwnd = FindWindow(0, L"IntelligenceHidingEngine");
			}
			if (!hwnd)
			{
				break;
			}
			SendMessage(hwnd, WM_INTELLIGENCE_CLOSE_ENGINE, 0, 0);
		}
		SelfDel();
		exit(0);
	}

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INTELLIGENCEHIDINGENGINE));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INTELLIGENCEHIDINGENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_INTELLIGENCEHIDINGENGINE);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   g_hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 100, 100, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   //ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

CTableStatus* g_game{ nullptr };
CMultiThreadMonteCarloTree* g_mct{ nullptr };
_MemorySharer g_ms;
double g_decay{};
stringstream g_log;
stringstream g_sflog;
stringstream g_uplog;
fstream g_flog;

time_t g_begintime{};
int g_winner{};
int g_income{};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
    switch (message)
	{
	case WM_INTELLIGENCE_INIT_INSTANCE:
	{
		srand((unsigned int)time(NULL));
		if (lParam != VER)return -1;
		CInitStruct* ins = (CInitStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins)
			return -1;
		g_game = new CTableStatus;
		g_mct = new CMultiThreadMonteCarloTree;
		g_mct->SetMaxSubThreadNum(ins->m_MaxSubThreadNum);
		g_mct->InitializeSimulationObject<CRandomGame>();
		CRandomGame::Explore = ins->m_Explore;
		CRandomGame::LockEnemy = ins->m_LockEnemy;
		CRandomGame::LockWinner = ins->m_LockWinner;
		CRandomGame::BenefitGain = ins->m_BenefitGain;
		CRandomGame::AvoidRisk = ins->m_AvoidRisk;
		CCardsJudge::m_complex_prop = ins->m_Complex_prop;
		CCardsJudge::m_base_prop = ins->m_Base_prop;
		g_mct->InitializeRootNode();
		g_decay = ins->m_DecayFactor;
		g_begintime = time(0);
		//log
		g_sflog << "Initialize Suc:" << endl;
		g_sflog
			<< "\t" << "[Exp]:" << CRandomGame::Explore << endl
			<< "\t" << "[Dec]:" << g_decay << endl
			<< "\t" << "[Seb]:" << ins->m_MaxSubThreadNum << endl
			<< "\t" << "[Lem]:" << CRandomGame::LockEnemy << endl
			<< "\t" << "[Lwr]:" << CRandomGame::LockWinner << endl
			<< "\t" << "[Bfg]:" << CRandomGame::BenefitGain << endl
			<< "\t" << "[Aer]:" << CRandomGame::AvoidRisk << endl
			<< "\t" << "[Cpr]:" << CCardsJudge::m_complex_prop << endl
			<< "\t" << "[Bpr]:" << CCardsJudge::m_base_prop << endl;
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		UnmapViewOfFile(g_ms.m_hMapFile);
		return 1;
	}
	case WM_INTELLIGENCE_SET_STATUS:
	{
		CSetStatusStruct* ins = (CSetStatusStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins || !g_game || !g_mct)
			return -1;
		CTableStatus status;
		status.m_CurrentPlayer = ins->m_CurrentPlayer;
		vector<int> last_put_vec;
		for (int p{}; p < 3; ++p)
		{
			last_put_vec.clear();
			for (int i{}; i < 21; ++i)
			{
				if (ins->m_Player_cards[p][i])
				{
					int n = CCardDivider::CardName2Num(ins->m_Player_cards[p][i]);
					if (n)
						status.m_player[p].m_Cards.insert(n);
				}
				if (ins->m_Player_lastcards[p][i])
					last_put_vec.push_back(CCardDivider::CardName2Num(ins->m_Player_lastcards[p][i]));
			}
			if (last_put_vec.size() > 0)
				status.m_player[p].m_LastPut.m_cards.AnalyseCardsSet(last_put_vec);
		}
		*g_game = status;

		//log
		g_sflog << "SetStatus Suc:" << endl;
		g_sflog << "\tArguments:" << endl;
		for (int p{}; p < 3; ++p)
		{
			stringstream cardstream, lastcardstream;
			for (int i{}; i < 21; ++i)
			{
				cardstream << CCardDivider::CardName2Num(ins->m_Player_cards[p][i]) << ",";
				lastcardstream << CCardDivider::CardName2Num(ins->m_Player_lastcards[p][i]) << ",";
			}
			g_sflog << "\t\t[Player" << p << "]Card:" << cardstream.str() << endl;
			g_sflog << "\t\t[Player" << p << "]Last:" << lastcardstream.str() << endl;
		}
		g_sflog << "\tStatus:" << endl;
		for (int i = 0; i < 3; i++)
		{
			g_sflog << "\t\tPlayer[" << i << "]:";
			for (auto it = g_game->m_player[i].m_Cards.begin(); it != g_game->m_player[i].m_Cards.end(); ++it)
			{
				g_sflog << CCardDivider::Num2CardName(*it) << ',';
			}
			g_sflog << endl;
		}
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		UnmapViewOfFile(g_ms.m_hMapFile);
		return 1;
	}
	case WM_INTELLIGENCE_PUT_CARD:
	{
		CPutCardsStruct* ins = (CPutCardsStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins || !g_game || !g_mct)
			return -1;
		vector<int> put_cards;
		CPutCard cp;
		cp.m_IsPass = true;
		for (int i{}; i < 21; ++i)
		{
			if (ins->m_Put[i])
				put_cards.push_back(ins->m_Put[i]);
			else
				break;
		}
		if (put_cards.size() > 0)
		{
			cp.m_IsPass = false;
			cp.m_cards.AnalyseCardsSet(put_cards);
		}
		if (ins->m_Player != AUTO_PLAYER)
		{
			return -1;
		}
		g_game->PutCard(cp);
		g_log.str("");
		g_mct->EndSearchThread(&g_log);
		CRandomGame::ConvergenceUCTScopeFactor_Static(g_decay);
		CNode* pNewRoot = g_mct->ChangeNodeToNext(cp);
		if (!pNewRoot)
		{
			g_sflog << "---------!<PutCard rebuild the tree>!---------" << endl;
			g_mct->InitializeRootNode();
		}
		g_mct->InitializeRootStatus(*g_game);
		{
			//log
			g_sflog << "PutCards Suc:" << endl;
			g_sflog << "\tArguments:" << endl;
			g_sflog << "\t\t[Player" << ((g_game->m_CurrentPlayer == 0) ? 2 : g_game->m_CurrentPlayer - 1) << "]:";
			for (int i{}; i < 21; ++i)
			{
				g_sflog << (ins->m_Put[i]) << ",";
			}
			g_sflog << endl;
			g_sflog << "\tAnalyseResult:" << endl;
			{
				vector<int> analyseresult;
				cp.m_cards.CombineCards(analyseresult);
				g_sflog << "\t\t[Cards]:";
				for (auto it = analyseresult.begin(); it != analyseresult.end(); ++it)
				{
					g_sflog << *it<<",";
				}
				g_sflog << endl;
			}
			g_sflog << "\tRootAddress:" << endl;
			g_sflog << "\t\t" << pNewRoot << endl;
			g_sflog << "\tSimulationResult:" << endl;
			g_sflog << "--------------------------------" << endl;
			g_sflog << g_log.str();
			g_sflog << "--------------------------------" << endl;
			g_sflog << "\tStatus:" << endl;
			for (int i = 0; i < 3; i++)
			{
				g_sflog << "\t\tPlayer[" << i << "]:";
				for (auto it = g_game->m_player[i].m_Cards.begin(); it != g_game->m_player[i].m_Cards.end(); ++it)
				{
					g_sflog << CCardDivider::Num2CardName(*it) << ',';
				}
				g_sflog << endl;
			}
			WriteLog(g_sflog.str());
			g_uplog << g_sflog.str() << endl;
			g_sflog.str("");
			g_log.str("");
		}
		g_mct->BeginSearchThread();
		UnmapViewOfFile(g_ms.m_hMapFile);
		return 1;
	}
	case WM_INTELLIGENCE_BEGIN_CALC:
	{
		if (!g_game || !g_mct)
			return -1;
		g_mct->InitializeRootStatus(*g_game);
		g_mct->BeginSearchThread();
		//log
		g_sflog << "BeginCalc Suc";
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");
		return 1;
	}
	case WM_INTELLIGENCE_END_CALC:
	{
		CPutCardsStruct* ins = (CPutCardsStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins || !g_game || !g_mct)
			return -1;
		CNode* best = g_mct->EndSearchThread(&g_log);
		vector<int> put;
		best->m_Action.m_cards.CombineCards(put);
		if (!best->m_Action.m_IsPass)
		{
			for (unsigned int i{}; i<21; ++i)
			{
				if (i < put.size())
					ins->m_Put[i] = put[i];
				else
					ins->m_Put[i] = 0;
			}
		}
		ins->m_Player = g_game->m_CurrentPlayer;
		ins->m_PutCount = (unsigned int)put.size();
		g_game->PutCard(best->m_Action);
		CRandomGame::ConvergenceUCTScopeFactor_Static(g_decay);
		CNode* pNewRoot = g_mct->ChangeNodeToNext(best->m_Action);
		if (!pNewRoot)
		{
			g_sflog << "---------!<EndCalc rebuild the tree>!---------" << endl;
			g_mct->InitializeRootNode();
		}
		//log
		g_sflog << "EndCalc Suc:" << endl;
		g_sflog << g_log.str();
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		return 1;
	}
	case WM_INTELLIGENCE_CLOSE_ENGINE:
	{
		if (g_mct)
		{
			g_mct->EndSearchThread(false);
		}
		//log
		g_sflog << "Close Engine Suc:";
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		//submit data
		string re = SendLogs(
			g_begintime,
			CRandomGame::LockEnemy,
			CRandomGame::LockWinner,
			g_winner,
			g_uplog.str(),
			g_income,
			VER,
			""
		);
		if (re.compare("1"))
		{
			g_sflog << "-----![Submit data fail]!-----" << endl;
			g_sflog << re << endl;
			WriteLog(g_sflog.str());
			g_sflog.str("");
		}
		else
			WriteLog(string("Submit data suc"));

		exit(0);
		return 1;
	}
	case WM_INTELLIGENCE_GET_LOG:
	{
		string log;
		log = g_log.str();
		//g_log.clear();
		g_log.str("");
		char* pRet = (char*)g_ms.CreateMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam), (unsigned int)log.length() + 1);
		ZeroMemory(pRet, log.length() + 1);
		memcpy_s(pRet, log.length() + 1, log.c_str(), log.length());
		//log
		g_sflog << "Get Log Suc:";
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		return log.length();
	}
	case WM_INTELLIGENCE_RESET_ARGU:
	{
		CInitStruct* ins = (CInitStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins || !g_game || !g_mct)
			return -1;
		g_mct->EndSearchThread(false);
		g_mct->SetMaxSubThreadNum(ins->m_MaxSubThreadNum);
		g_mct->InitializeSimulationObject<CRandomGame>();
		CRandomGame::Explore = ins->m_Explore;
		CRandomGame::LockEnemy = ins->m_LockEnemy;
		CRandomGame::LockWinner = ins->m_LockWinner;
		CRandomGame::BenefitGain = ins->m_BenefitGain;
		CRandomGame::AvoidRisk = ins->m_AvoidRisk;
		CCardsJudge::m_complex_prop = ins->m_Complex_prop;
		CCardsJudge::m_base_prop = ins->m_Base_prop;
		g_mct->InitializeRootNode();
		g_decay = ins->m_DecayFactor;
		g_mct->BeginSearchThread();
		//log
		g_sflog << "ResetArgu Suc:" << endl;
		g_sflog
			<< "\t" << "[Exp]:" << CRandomGame::Explore << endl
			<< "\t" << "[Dec]:" << g_decay << endl
			<< "\t" << "[Seb]:" << ins->m_MaxSubThreadNum << endl
			<< "\t" << "[Lem]:" << CRandomGame::LockEnemy << endl
			<< "\t" << "[Lwr]:" << CRandomGame::LockWinner << endl
			<< "\t" << "[Bfg]:" << CRandomGame::BenefitGain << endl
			<< "\t" << "[Aer]:" << CRandomGame::AvoidRisk << endl
			<< "\t" << "[Cpr]:" << CCardsJudge::m_complex_prop << endl
			<< "\t" << "[Bpr]:" << CCardsJudge::m_base_prop << endl;
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		UnmapViewOfFile(g_ms.m_hMapFile);
		return 1;
	}
	case WM_INTELLIGENCE_SUBMIT_RESULT:
	{
		CSubmitResultStruct* ins = (CSubmitResultStruct*)g_ms.GetMemory(INTELLIGENCE_BUILD_TUBE_NAME(wParam));
		if (!ins || !g_game || !g_mct)
			return -1;
		g_winner = ins->m_Winner;
		g_income = ins->m_Income;
		//log
		g_sflog << "Submit_result_Suc:" << endl;
		g_sflog
			<< "\t" << "[Wnr]:" << g_winner << endl
			<< "\t" << "[Ine]:" << g_income << endl;
		WriteLog(g_sflog.str());
		g_uplog << g_sflog.str() << endl;
		g_sflog.str("");

		UnmapViewOfFile(g_ms.m_hMapFile);
		return 1;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL SelfDel()
{
	SHELLEXECUTEINFO sei;
	TCHAR szModule[MAX_PATH], szComspec[MAX_PATH], szParams[MAX_PATH];//字符串数组  

																	  // 获得文件名.  
	if ((GetModuleFileName(0, szModule, MAX_PATH) != 0) &&
		(GetShortPathName(szModule, szModule, MAX_PATH) != 0) &&
		(GetEnvironmentVariable(L"COMSPEC", szComspec, MAX_PATH) != 0))//获取szComspec=cmd.exe  
	{
		// 设置命令参数.  
		lstrcpy(szParams, L"/c del ");
		lstrcat(szParams, szModule);
		lstrcat(szParams, L" > nul");

		// 设置结构成员.  
		sei.cbSize = sizeof(sei);
		sei.hwnd = 0;
		sei.lpVerb = L"Open";
		sei.lpFile = szComspec;//C:\Windows\system32\cmd.exe  
		sei.lpParameters = szParams;//  /c del E:\adb\datasafe\Debug\datasafe.exe > nul  
		sei.lpDirectory = 0;
		sei.nShow = SW_HIDE;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;

		// 执行shell命令.  
		if (ShellExecuteEx(&sei))
		{
			// 设置命令行进程的执行级别为空闲执行,使本程序有足够的时间从内存中退出.   
			SetPriorityClass(sei.hProcess, IDLE_PRIORITY_CLASS);
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

			// 通知Windows资源浏览器,本程序文件已经被删除.  
			SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, szModule, 0);
			return TRUE;
		}
	}
	return FALSE;
}

void WriteLog(string& str)
{
	g_flog.open("logs.txt", ios::out|ios::app);
	g_flog.seekp(0, ios::end);
	g_flog << "[Log:" << clock() << "]:" << endl;
	g_flog << str << endl;
	g_flog << "========================================" << endl;
	g_flog.close();
}

long CALLBACK CrashHandler(EXCEPTION_POINTERS *pException)
{
	g_sflog << "---!<Engine Unhandled Exception>!---" << endl;
	g_sflog << "\tStatus:" << endl;
	for (int i = 0; i < 3; i++)
	{
		g_sflog << "\t\tPlayer[" << i << "]:";
		for (auto it = g_game->m_player[i].m_Cards.begin(); it != g_game->m_player[i].m_Cards.end(); ++it)
		{
			g_sflog << CCardDivider::Num2CardName(*it) << ',';
		}
		g_sflog << "|" << endl;
	}
	WriteLog(g_sflog.str());
	g_sflog.str("");
	g_log.str("");

	string re = SendLogs(
		g_begintime,
		CRandomGame::LockEnemy,
		CRandomGame::LockWinner,
		-1,
		g_uplog.str(),
		-1,
		VER,
		""
	);

	if (re.compare("1"))
	{
		g_sflog << "-----![Submit data fail]!-----" << endl;
		g_sflog << re << endl;
		WriteLog(g_sflog.str());
		g_sflog.str("");
	}
	else
		WriteLog(string("Submit data suc"));


	time_t timep;
	time(&timep);
	wchar_t tmp[64];
	tm tm_time;
	localtime_s(&tm_time, &timep);
	wcsftime(tmp, sizeof(tmp), L"%Y-%m-%d_%H%M%S", &tm_time);
	CreateDumpFile((wstring(L"CrashInfo-") + wstring(tmp) + wstring(L".dmp")).c_str(), pException);

	return EXCEPTION_EXECUTE_HANDLER;
}

string SendLogs(
	time_t begin_time,
	int lock_enemy,
	int lock_winner,
	int winner,
	string log,
	int income,
	int client_ver,
	string client_machine_code
)
{
	//$begin_time = $_POST["_begin_time"];
	//$time_consumed = $_POST["_time_consumed"];
	//$lock_enemy = $_POST["_lock_enemy"];
	//$lock_winner = $_POST["_lock_winner"];
	//$winner = $_POST["_winner"];
	//$log_id = $_POST["_log_id"];
	//$income = $_POST["_income"];
	//$client_ver = $_POST["_client_ver"];
	//$client_machine_code = $_POST["_client_machine_code"];
	stringstream ss;
	time_t time_consumed{ time(0) - begin_time };
	ss
		<< "_begin_time=" << begin_time << "&"
		<< "_time_consumed=" << time_consumed << "&"
		<< "_lock_enemy=" << lock_enemy << "&"
		<< "_lock_winner=" << lock_winner << "&"
		<< "_winner=" << winner << "&"
		<< "_log_id=" << log << "&"
		<< "_income=" << income << "&"
		<< "_client_ver=" << std::hex << client_ver << "&"
		<< "_client_machine_code=" << client_machine_code;
	FunData http_ret = sendhttpc(WEB_STATISTIC, ss.str().c_str(), true);
	string ret = http_ret.getdata();
	return ret;
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{            
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	//dump information
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	//fill dump file contents
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}