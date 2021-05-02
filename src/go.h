/*
Copyright 2020,2021 LiGuer. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef GO_H
#define GO_H
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MiniMax.h"
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "MontecarloTreeSearch.h"
/******************************************************************************
*                    围棋AI Go-AI
******************************************************************************/
namespace Go_AI {
#define BOARDSIZE 9
#define BLACK  1
#define WHITE -1
typedef char STONE;
/******************************************************************************
*                    状态
******************************************************************************/
struct State {
	char pos = -1;
	char player = 0;
	State* preState = NULL;
	Mat<STONE>* board;
	Mat<int>  * qi, * chBlock;
	STONE& operator[](int i)		{ return (*board)[i]; }
	STONE& operator()(int i, int j) { return (*board)(j, i); }
};
/******************************************************************************
*                    基础函数
******************************************************************************/
//函数声明
bool judgeOut(int x, int y);
bool newStateRand(State& state, State& newState);
/*--------------------------------[ 下子 ]--------------------------------*/
int run(State& board, int& x, int& y, STONE who) {
	typedef MontecarloTreeSearch<State> AI;
	AI ai;
	AI::TreeNode* ans = ai.run(&board);
	x = ans->state.pos / BOARDSIZE;
	y = ans->state.pos % BOARDSIZE;
}
/*--------------------------------[ nextStateRand ]--------------------------------
基于输入[board]棋盘，在可能的动作下，随机选择一个动作
*	* [a,b)随机整数: (rand() % (b-a))+ a;
**----------------------------------------------------------------------------*/
bool newStateRand(State& state, State& newState) {
	Mat<bool> markBoard(BOARDSIZE, BOARDSIZE);
	// 眼点&禁入点标记			
	// 已经走过子
	// 劫点标记 
	// 随机走子
	int num = 0, index;
	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
		if (markBoard[i] == 0) num++;
	if (num == 0) return false;
	else index = rand() % num;
	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
		if (markBoard[i] == 0) index--;
		if (index == 0) { newState.pos = i; break; }
	}
	return true;
}
/*--------------------------------[ 判断是否过界 ]--------------------------------*/
bool judgeOut(int x, int y) {
	return (x >= 0 && x < BOARDSIZE && y >= 0 && y < BOARDSIZE) ? false : true;
}
/******************************************************************************
*                   围棋规则函数
******************************************************************************/
/*--------------------------------[ [1]:落子提子 ]--------------------------------
[RULE 1]:无气提子
[RULE 3]:劫判定
**-------------------------------------------------------------------------------*/
bool downStone(State& state) {
	//劫判定
	judgeJie(state);
	//落子
	state[state.pos] = state.player;
	//棋块数气
	state.qi     ->zero();
	state.chBlock->zero();
	ComputerQi(*state.board, *state.qi, *state.chBlock);
	//无气提子
	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
		if ((*state.qi)[(*state.chBlock)[i]] == 0
		&&  (*state.chBlock)[i] != (*state.chBlock)[state.pos])
			state[i] = 0;
	return true;
}
/*--------------------------------[ [2]:禁入点标记 ]--------------------------------
*	*输入: [1] 棋盘board	[2] 己方颜色	[3] 气	[4] 棋块
*	*输出: [1] 被标记己方禁入点的棋盘
*	[RULE 2]:非提禁入
*	禁入点: [1]我无气	[2]非杀他
*	*禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
**-------------------------------------------------------------------------------*/
void judgeNotPoint(State& board) {
	const static char
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			if (board(y, x) != 0) continue;
			bool flagMe = 1;
			for (int i = 0; i < 4; i++) {
				int xt = x + x_step[i],
					yt = y + y_step[i];
				if (judgeOut(xt, yt)) continue;
				/*---- 核心判断 ----*/
				if (board(xt, yt) == 0															//上下左右应不为空
				|| (board(xt, yt) == board.player && (*board.qi)[(*board.chBlock)(xt, yt)] != 1)	//若是我，应只一气
				|| (board(xt, yt) != board.player && (*board.qi)[(*board.chBlock)(xt, yt)] == 1)) {	//若是敌，应必不只一气
					flagMe = 0;
				}
			}
			if (flagMe)board(x, y) = NOTPOINT;
		}
	}
}
/*--------------------------------[ 眼点标记 ]--------------------------------
*	*输入: [1] 棋盘board	[3] 气	[4] 棋块
*	*输出: [1] 被标记眼点的棋盘 (双方都标记)
*	眼点: 敌方禁入点，且上下左右皆为我
		我方眼点->敌方禁入点，敌方禁入点-/>我方眼点
*	*判定: [1] 只一空点	[2] 上下左右同一色	[3]上下左右棋块，均非一气
**------------------------------------------------------------------------------*/
void judgeEye(State& board) {
	const static char
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			if (board(y, x) != 0) continue;
			bool flag = 1; STONE who = 0;
			for (int i = 0; i < 4; i++) {
				int xt = x + x_step[i],
					yt = y + y_step[i];
				if (judgeOut(xt, yt)) continue;
				who += board(xt, yt);
				if (board(xt, yt) * who <= 0 || (*board.qi)[(*board.chBlock)(xt, yt)] == 1) {
					flag = 0; break;
				}
			}
			if (flag) board(x, y) = NOTPOINT + (who > 0 ? 1 : -1);
		}
	}
}
/*--------------------------------[ 眼点+禁入点标记 ]--------------------------------
*	*输入: [1] 棋盘board	[3] 气	[4] 棋块
*	*输出: [1] 被标记眼点的棋盘(双方都标记)
*	眼点: 敌方禁入点，且上下左右皆为我					#bug: 算的是 真眼还是假眼
		我方眼点->敌方禁入点，敌方禁入点-/>我方眼点
*	我方禁入点不能走，敌方禁入点兼我必占点(眼点)不必走

*	*眼点判定: [1]上下左右皆我，且均非一气
*	*禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
**------------------------------------------------------------------------------*/
void judgeEyeAndNot(State& board) {
	const static char
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			if (board(y, x) != 0) continue;
			bool flagEye = 1, flagMe = 1;
			for (int i = 0; i < 4; i++) {
				int xt = x + x_step[i],
					yt = y + y_step[i];
				if (judgeOut(xt, yt)) continue;			//过界
				/*---- 核心判断 ----*/
				if (board(xt, yt) != stone || qi[chBlock(xt, yt)] == 1) { flagEye = 0; }
				if (board(xt, yt) == 0															//上下左右应不为空
					|| (board(xt, yt) == stone && qi[chBlock(xt, yt)] != 1)		//若是我，应只一气
					|| (board(xt, yt) != stone && qi[chBlock(xt, yt)] == 1)) {	//若是敌，应必不只一气
					flagMe = 0;
				}
			}
			if (flagEye || flagMe)board(x, y) = NOTPOINT;
		}
	}
}
/*--------------------------------[ 劫判定 ]--------------------------------
*	*输入: [1] 棋盘board	[2] 己方颜色	[3]预期落子点	[4] 气	[5] 棋块
*	*输出: [1] 是否为劫	[2]对应劫点
*	劫: 只有一种模式: 黑白眼相交错。
			x o @ x		x o @ x
			o   o @		o @   @
			x o @ x		x o @ x
*	*判定: [1] 四周全敌 [2] 有且只有一方，只一气，且只一点
**------------------------------------------------------------------------*/
bool judgeJie(State& board) {
	const static char
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	bool Jieflag = 1, JieArmy = 0;
	for (int i = 0; i < 4; i++) {
		int xt = x0 + x_step[i],
			yt = y0 + y_step[i];
		if (judgeOut(xt, yt)) continue;			//过界
		if (board(xt, yt) == 0
			|| board(xt, yt) == stone) {			//四周非全敌，非劫
			Jieflag = 0; break;
		}
		if (qi[chBlock(xt, yt)] == 1) { 		//是敌且只1气
			if (JieArmy == 1) { Jieflag = 0; break; }		//有两个1气敌，非劫
			else JieArmy = 1;
			for (int j = 0; j < 4; j++) {					//1气敌只有自己
				int xtt = xt + x_step[j],
					ytt = yt + y_step[j];
				if (judgeOut(xtt, ytt) || (ytt == y0 && xtt == x0)) continue;			//过界
				if (board(xtt, ytt) != stone) { Jieflag = 0; JieArmy = 0; break; }
			}
			if (JieArmy) { Jie0[0] = xt; Jie0[1] = yt; Jieflag = 1; }
		}
	}
	if (Jieflag) return true;
	Jie0[0] = -1; Jie0[1] = -1; return false;
}
/*--------------------------------[ [4]:输赢判定(数子法) ]--------------------------------
*	[RULE 4]:局势判定(数子法)
*	无气杀我非杀他，为禁入
**----------------------------------------------------------------------------------------*/
int judgeWin(State& state) {	//[RULE 4]:局势判定(数子法)
	judgeEye(board);
	double ScoreBlack = 0,
		ScoreWhite = 0;
	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
		if (board[i] == 0)return 0;
		else if (board[i] == BLACK)				ScoreBlack++;
		else if (board[i] == WHITE)				ScoreWhite++;
		else if (board[i] == NOTPOINT + BLACK)	ScoreBlack++;
		else if (board[i] == NOTPOINT + WHITE)	ScoreWhite++;
	}
	return ScoreBlack - 2.5 > ScoreWhite ? 1 : -1;		//贴子
}
/*--------------------------------[ 棋块数气 ]--------------------------------
*	*输入: [1] 棋盘board
*	*输出: [1] 棋盘上棋块chBlock [2] 棋块对应的气
*	棋块: 上下左右连通的同一色棋的区域
*	气:
**--------------------------------------------------------------------------*/
void ComputerQi(Mat<STONE>& board, Mat<int>& qi, Mat<int>& chBlock) {
	const static char
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	char chBlockCur = 1;
	/*---- 棋块标记 ----*/
	char HeadStoneFlag[BOARDSIZE * BOARDSIZE] = { 0 };			//并查集头节点标记
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			if (board(x, y) == 0)continue;
			bool flag = 1;
			/*---- STONE UP ----*/
			if (y > 0 && board(x, y - 1) == board(x, y)) {
				chBlock(x, y) = chBlock(x, y - 1);
				flag = 0;
			}
			/*---- STONE LEFT ----*/
			if (x > 0 && board(x - 1, y) == board(x, y)) {
				flag = 0;
				if (chBlock(x, y) == 0)
					chBlock(x, y) = chBlock(x - 1, y);
				else if (chBlock(x, y) != chBlock(x - 1, y)) {
					/*---- 并查集 更新 ----*/
					int HeadStoneMe = HeadStoneFlag[chBlock(x, y)];
					while (HeadStoneFlag[HeadStoneMe] != HeadStoneMe)HeadStoneMe = HeadStoneFlag[HeadStoneMe];	//检索头节点
					int HeadStoneLeft = HeadStoneFlag[chBlock(x - 1, y)];
					while (HeadStoneFlag[HeadStoneLeft] != HeadStoneLeft)HeadStoneLeft = HeadStoneFlag[HeadStoneLeft];	//检索头节点
					if (HeadStoneMe != HeadStoneLeft)
						HeadStoneFlag[HeadStoneLeft] = HeadStoneMe;
				}
			}
			/*---- No STONE like ----*/
			if (flag) {
				HeadStoneFlag[chBlockCur] = chBlockCur;
				chBlock(x, y) = chBlockCur++;
			}
		}
	}
	/*---- 完成棋块标记 ----*/
	for (int x = 0; x < BOARDSIZE * BOARDSIZE; x++) {
		char HeadStone = HeadStoneFlag[chBlock[x]];
		while (HeadStoneFlag[HeadStone] != HeadStone)HeadStone = HeadStoneFlag[HeadStone];	//检索头节点
		chBlock[x] = HeadStone;
	}
	/*---- 棋块数气 ----*/
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			bool qiflag[BOARDSIZE * BOARDSIZE] = { 0 };						//#需更改
			if (board(x, y) == 0) {
				for (int i = 0; i < 4; i++) {
					char xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= BOARDSIZE || xt >= BOARDSIZE
						|| board(xt, yt) == 0
						|| qiflag[chBlock(xt, yt)] == 1)continue;
					qi[chBlock(xt, yt)]++;
					qiflag[chBlock(xt, yt)] = 1;
				}
			}
		}
	}
}
};
#endif // MAINWINDOW_H

