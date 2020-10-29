#ifndef GOBANG_H
#define GOBANG_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
typedef signed char INT8S;
typedef unsigned char INT8U;
typedef INT8S CHESS;
/******************************************************************************
*                    状态
******************************************************************************/
class State
{
public:
	~State() {
		free(map);
	}
	INT8S x, y;
	CHESS* map = NULL;
	INT8S player = 0;
};
/******************************************************************************
*                    蒙特卡洛树搜索 节点
*The 1th number: the number of wins in that subtree
*The 2th number: the total number of simulations performed in that subtree
*The ratio of these two numbers provides us with the estimated value of each node
******************************************************************************/
struct MCTSNode {
	~MCTSNode() {
		delete state;
		for (int i = 0; i < childCur; i++)delete child[i];
	}
	int WinTime = 0, VisitTime = 0;
	State* state = NULL;
	MCTSNode* child[9 * 9] = { NULL };
	MCTSNode* parent = NULL;
	int childCur = 0;
	bool isFullChildFlag = false;
	bool isFullChild() {
	}
};
/******************************************************************************
*                    围棋 Go
******************************************************************************/
class Go {
public:
	/*---------------- 基础参数 ----------------*/
	const static INT8S MapSize = 9;									//棋盘尺寸
	const static INT8S  Black = 1, White = -1;
	CHESS* Map;														//棋盘
	const int MctsBudget = 100000;
	const INT8S NOTPOINT = 10;										//禁入点标记//眼点标记 = 禁入点标记+棋子标记
	/*---------------- 基础函数 ----------------*/
	void init();													//初始化
	void clearMap();												//清盘
	bool setMap(INT8S x, INT8S y, CHESS chess);						//下子
	bool judgeOutMap(int x, int y);									//判断是否过界
	/*---------------- [MCTS]核心函数 ----------------*/
	MCTSNode* MCTS(MCTSNode* node);									//[MCTS]蒙特卡洛树搜索
	MCTSNode* Select(MCTSNode* node, bool isExploration);			//[1]选择最优节点
	MCTSNode* Expand(MCTSNode* node);								//[2]拓展新节点
	int Simulation(State* state0);									//[3]模拟,快速走子
	void Backpropagation(MCTSNode* node, int reward);				//[4]回溯
	double UCB(MCTSNode* node, double C);							//UCB公式
	MCTSNode* TreePolicy(MCTSNode* node);							//
	bool nextStateRand(CHESS* map, INT8S& x0, INT8S& y0, CHESS chess);//随机走子
	/*---------------- 围棋规则函数 ----------------*/
	bool downChess(CHESS* map, INT8S x0, INT8S y0, CHESS chess);	//落子//[RULE 1]:无气提子
	void judgeNotPoint(CHESS* map, CHESS chess);					//[RULE 2]:非提禁入
																	//[RULE 3]:劫停一手
	int judgeWin(CHESS* map);										//[RULE 4]:局势判定(数子法)
	void ComputerQi(CHESS* map, INT8U qi[], INT8U chBlockMap[]);	//棋块数气
	void GoAI(CHESS* map, INT8S player, INT8S& x0, INT8S& y0);
	/*---------------- 调试函数 ----------------*/
	void showMap(CHESS* map);
	void showTree(MCTSNode* root);
};
#endif // MAINWINDOW_H

