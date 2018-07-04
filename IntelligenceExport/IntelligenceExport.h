#pragma once
#include "Instance.h"
#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif

//	概述：
//		术语：
//			计算引擎(Calc engine)：	承担所有的计算任务
//			操作管道(Com tube)：		承担所有的数据传递，中转操作
//			实例(Instance)：			一个实例中含有一个计算引擎和一个操作管道
//		相关流程：
//			输入流程：
//				调用方->实例->操作管道->计算引擎
//			输出流程：
//				调用方->实例->操作管道->计算引擎->操作管道->实例->调用方
//	对于参数的说明：
//		参数方向：
//			带[in]字样的参数均为传入参数，即从调用方传入到实例中使用
//				实例中将不进行改动
//			带[out]字样的参数均为传出参数，即调用方给定地址，由实例进
//				行操作，随后调用方可以在函数返回后在自己的逻辑上下文中
//				对数据进行操作
//			对于易语言，[in]参数不需要在“传址”上打勾，但[out]参数需要
//		参数大小：
//			对于字节集(char)类型的参数，可能需要调用方自行初始化长度
//				例如：打出={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
//				对于这一点，笔者暂未做出测试，需要调用方自行测试
//			参数中要求固定长度的参数，需要严格按照长度建立参数，且需要
//				先初始化为“0”后再进行操作（尤其是传出参数）
//	流程
//		创建实例
//		初始化实例
//		设置初始牌局
//		开始计算
//		牌局（PutCards或GetBestChoice)
//		释放
//		备注：没有加入判断牌局是否结束，如果牌局结束了仍然调用PutCards或GetBestChoice可能会导致引擎崩溃



//	函数名：	Ver
//	说明：	返回版本号
//	参数：	无
//	返回值：	整数(int)	版本号
EXPORT int __stdcall Ver();

//	函数名：	SetupInstance
//	说明：	建立一个实例（一个实例操作一个计算引擎）
//	参数：	无
//	返回值：	整数(int)	-1	已经存在未初始化完成的引擎
//						-2	开启引擎失败/超时
//						1	执行成功
//	备注：	一个计算引擎仅仅对应一个实例，如果实例当中的计算引擎
//			被释放，则需要重新创建一个实例。
EXPORT int __stdcall SetupInstance();

//	函数名：	InitializeInstance
//	说明：	初始化计算引擎
//	参数：	[in]	整数(int)	操作实例ID
//			[in]	小数(double)	探索系数（越大搜索广度越广，越小搜索深度越深）
//			[in]	小数(double)	衰减系数（在每一次出牌后对探索系数进行衰减）
//			[in]	整数(int)	搜索系数（越大占用资源越多，越小搜索速度越慢）
//			[in]	整数(int)	锁定敌方编号
//			[in]	整数(int)	锁定赢家编号（不锁定为-1)
//			[in]	小数(double)	获利增益系数（越大，越倾向于获利多的方案）
//			[in]	小数(double)	风险系数（0~1）最好不大于0.4，越大，则越倾向于风险更小（最大最小胜率跨度）的选择
//			[in]	小数(double)	牌分复杂度影响系数（0~1）
//			[in]	小数(double)	牌分基础分数影响系数（0~1）
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
//	备注：	玩家0为地主
EXPORT int __stdcall InitializeInstance(
	int id, 
	double explore, 
	double decay, 
	int search_ability, 
	int lockenemy, 
	int lockwinner, 
	double benefitgain, 
	double avoidrisk, 
	double complex_prop, 
	double base_prop);

//	函数名：	SetStatus
//	说明：	设置初始牌局状态
//	参数：	[in]	整数(int)	操作实例ID
//			[in]	字节集(char)	玩家0手牌
//								玩家1手牌
//								玩家2手牌
//								玩家0上一次出牌（默认全0）
//								玩家1上一次出牌（默认全0）
//								玩家2上一次出牌（默认全0）
//			[in]	整数(int)	当前玩家
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
//	备注：	玩家0为地主，如上的字节集参数，长度均为21字节
EXPORT int __stdcall SetStatus(
	int id,
	char* player0_card,
	char* player1_card,
	char* player2_card,
	char* player0_last,
	char* player1_last,
	char* player2_last,
	int current_player);

//	函数名：	PutCards
//	说明：	输入一次手动出牌（通常为玩家出牌）
//	参数：	[in]	整数(int)	操作实例ID
//			[in]	字节集(char)	打出的牌
//			[in]	整数(int)	打出的牌数量
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
//	备注：	打出的牌参数长度为21字节
EXPORT int __stdcall PutCards(int id, char* put, int put_num);

//	函数名：	GetBestChoice
//	说明：	获得一次出牌方案（以当前玩家为准）
//	参数：	[in]	整数(int)	操作实例ID
//			[out]	字节集(char)	打出的牌
//			[out]	整数(int)	打出的牌数量
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						其他	出牌玩家编号
//	备注：	打出的牌参数长度为21字节，且传入前需归零
EXPORT int __stdcall GetBestChoice(int id, char* dest, int* cardnum);

//	函数名：	BeginCalc
//	说明：	开始计算（通常在初始化全部完毕后开始计算）
//	参数：	[in]	整数(int)	操作实例ID
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
//	备注：	玩家0为地主
EXPORT int __stdcall BeginCalc(int id);

//	函数名：	FreeHidingEngine
//	说明：	释放计算引擎（若不释放引擎将持续占用CPU资源，通常在本局结束后释放）
//	参数：	[in]	整数(int)	操作实例ID
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1或0	执行成功
EXPORT int __stdcall FreeHidingEngine(int id);

//	函数名：	GetLog
//	说明：	获得运行日志
//	参数：	[in]	整数(int)	操作实例ID
//			[out]	字节集(char)	日志存放的缓冲区
//			[in]	整数型(int)	日志存放的缓冲区大小
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						其他	返回的日志长度
EXPORT int __stdcall GetLog(int id, char* buf, int buflen);

//	函数名：	ResetArgu
//	说明：	重设计算引擎计算参数
//	参数：	[in]	整数(int)	操作实例ID
//			[in]	小数(double)	探索系数（越大搜索广度越广，越小搜索深度越深）
//			[in]	小数(double)	衰减系数（在每一次出牌后对探索系数进行衰减）
//			[in]	整数(int)	搜索系数（越大占用资源越多，越小搜索速度越慢）
//			[in]	整数(int)	锁定敌方编号
//			[in]	整数(int)	锁定赢家编号（不锁定为-1）
//			[in]	小数(double)	获利增益系数（越大，越倾向于获利多的方案）
//			[in]	小数(double)	风险系数（0~1）最好不大于0.4，越大，则越倾向于风险更小（最大最小胜率跨度）的选择
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
EXPORT int __stdcall ResetArgu(
	int id,
	double explore, 
	double decay, 
	int search_ability, 
	int lockenemy, 
	int lockwinner, 
	double benefitgain,
	double avoidrisk, 
	double complex_prop, 
	double base_prop);

//	函数名：	IsStillExsist
//	说明：	判断引擎是否正在运行
//	参数：	[in]	整数(int)	操作实例ID
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						非零	引擎仍存在
//						0	引擎已不存在
EXPORT int __stdcall IsStillExsist(int id);

//	函数名：	SubmitResult
//	说明：	提交最终情况
//	参数：	[in]	整数(int)	操作实例ID
//			[in]	整数(int)	赢家id
//			[in]	整数(int)	收入
//	返回值：	整数(int)	-1	实例ID不存在/实例未初始化
//						1	执行成功
EXPORT int __stdcall SubmitResult(int id, int winner, int income);