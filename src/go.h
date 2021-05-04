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
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "MontecarloTreeSearch.h"
/******************************************************************************
*                    围棋AI Go-AI
-------------------------------------------------------------------------------
*	Example:
#include "../LiGu_Codes/Daiyu-Go/src/go.h"
int main() {
	Go_AI::State board;
	board.player = WHITE;
	int JiePos = -1;
	while (true) {
		Go_AI::run(&board, JiePos);
		printf("AI:%d\n", board.pos);
		for (char x = 0; x < BOARDSIZE; x++) {
			for (char y = 0; y < BOARDSIZE; y++) {
				printf("%c ", board(x ,y) == 0 ? '+' : (board(x ,y) == 1 ? 'x' : 'o'));
			}printf("\n");
		}printf("\n");
	}
}
******************************************************************************/
namespace Go_AI {
#define BOARDSIZE 13
#define BLACK  1
#define WHITE -1
#define EYEPOINT -10
#define NOTPOINT -20
#define JIEPOINT -30
typedef char STONE;
/******************************************************************************
*                    状态
*	[数据]: [1]下子位置		[2]棋手
			[3]棋盘			[4]标记棋盘		[5]气标记
******************************************************************************/
struct State {
	int  pos = -1;
	char player = 0;
	Mat<STONE> board{ BOARDSIZE , BOARDSIZE };
	Mat<int>   mark { BOARDSIZE , BOARDSIZE },
			   qi	{ BOARDSIZE * BOARDSIZE };
	STONE& operator[](int i)		{ return board[i]; }
	STONE& operator()(int x, int y) { return board(x, y); }
};
/******************************************************************************
*                    基础函数
******************************************************************************/
//函数声明
void run			(STONE* board, int& x, int& y, STONE who, int& JiePos);
void run			(State* board, int& JiePos);
bool judgeOut(int x, int y);
bool newStateRand	(State&, State&, bool);
bool newStateRand	(State& state);
bool downStone		(State& state);
char judgeWin		(State& state);
void ComputerQi		(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark);
void judgeEyeAndNot	(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, STONE player);
int  judgeJie		(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, int pos, STONE player);
/*--------------------------------[ 运行 ]--------------------------------*/
void run(STONE* board, int& x, int& y, STONE who, int& JiePos) {
	State* root = new State;
	root->player = -who;
	root->board.getData(board);
	run(root, JiePos);
	x = root->board.i2x(root->pos);
	y = root->board.i2y(root->pos);
}
void run(State* board, int& JiePos) {
	board->qi.  zero();
	board->mark.zero();
	ComputerQi		(board->board, board->qi, board->mark);
	judgeEyeAndNot	(board->board, board->qi, board->mark, -board->player);
	if (JIEPOINT != -1)board->board[JiePos] = JIEPOINT;
	typedef MontecarloTreeSearch<State> AI;
	AI ai(
		newStateRand,
		judgeWin
	);
	*board = *ai.run(board);
	JiePos = -1;
	for (int i = 0; i < board->mark.size(); i++)
		if(board->mark[i] == JIEPOINT) JiePos = i;
}
/*--------------------------------------------------------------------------------
						随机新状态生成 New State Rand
*	[算法]: 基于输入[棋盘]，在可能的动作下，随机选择一个动作
*	[流程]: 
		[1] 确认状态空间数量
			[1.1] 若数量为零, 返回失败
		[2] 随机选择一个状态, 并返回
--------------------------------------------------------------------------------*/
// 用于Montecarlo[2. 拓展]
bool newStateRand(State& state, State& newState, bool isSimulation) {
	if (isSimulation) return newStateRand(state);
	newState.board  = state.board;
	newState.mark   = state.mark;
	newState.player =-state.player;
	for (int i = 0; i < newState.mark.size(); i++)
		if (newState.mark[i] == -40) newState.mark[i] = 0;
	//[1]
	int num = 0;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	if (num == 0) 
		return false;
	//[2]
	int index = rand() % num + 1;
	for (int i = 0; i < state.board.size(); i++) {
		if (state.mark [i] == 0
		&&  state.board[i] == 0) index--;
		if (index == 0) { 
			newState.pos = i; break; 
		}
	}
	downStone(newState);
	state.mark[newState.pos] = -40;
	return true;
}
// 用于Montecarlo[3. 模拟]
bool newStateRand(State& state) {
	state.player = -state.player;
	//[1]
	int num = 0;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	if (num == 0) 
		return false;
	//[2]
	int index = rand() % num + 1;
	for (int i = 0; i < state.board.size(); i++) {
		if (state.mark [i] == 0
		&&  state.board[i] == 0) index--;
		if (index == 0) { 
			state.pos = i; break;
		}
	}
	downStone(state);
	return true;
}
/*--------------------------------[ 判断是否过界 ]--------------------------------*/
bool judgeOut(int x, int y) {
	return (x >= 0 && x < BOARDSIZE 
		 && y >= 0 && y < BOARDSIZE) ? false : true;
}
/******************************************************************************
*                   围棋规则函数
******************************************************************************/
/*--------------------------------[ [1]:落子提子 ]--------------------------------
*	[算法]:
		[RULE 1]:无气提子
		[RULE 3]:劫判定
*	[流程]:
		[1] 劫判定
		[2] 落子
		[3] 棋块数气
		[4] 无气提子
		[5] 眼点禁入点劫点标记 (帮下一回标记)
**-------------------------------------------------------------------------------*/
static bool downStone(State& state) {
	//劫判定
	int JiePos = judgeJie(state.board, state.qi, state.mark, state.pos, state.player);
	//落子
	state[state.pos] = state.player;
	state.mark.zero();
	state.qi  .zero();
	//棋块数气
	ComputerQi(state.board, state.qi, state.mark);
	//无气提子
	for (int i = 0; i < state.board.size(); i++)
		if (state.qi[state.mark[i]] == 0
		&&  state.mark [i] != state.mark[state.pos]) {
			state.board[i] = 0;
			state.mark [i] = 0;
		}
	//眼点禁入点劫点标记 (帮下一回标记)
	judgeEyeAndNot(state.board, state.qi, state.mark, -state.player);
	if (JiePos != -1)state.mark[JiePos] = JIEPOINT;
	return true;
}
/*--------------------------------[ 眼点+禁入点标记 ]--------------------------------
*	*输入: [1] 棋盘board	[3] 气	[4] 棋块
*	*输出: [1] 被标记眼点的棋盘(双方都标记)
*	眼点: 敌方禁入点，且上下左右皆为我					#bug: 算的是 真眼还是假眼
		我方眼点->敌方禁入点，敌方禁入点-/>我方眼点
*	我方禁入点不能走，敌方禁入点兼我必占点(眼点)不必走

*	*眼点判定:   [1]上下左右皆我，且均非一气
*	*禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
------
*	眼点: 敌方禁入点，且上下左右皆为我
		我方眼点->敌方禁入点，敌方禁入点-/>我方眼点
*	*判定: [1] 只一空点	[2] 上下左右同一色，且均非一气
------
*	[RULE 2]:非提禁入
*	禁入点: [1]我无气	[2]非杀他
*	禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
**------------------------------------------------------------------------------*/
static void judgeEyeAndNot(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, STONE player) {
	const static int
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	for (int i = 0; i < board.size(); i++) {
		if (board[i] != 0) continue;
		int  flagEye = 0x7FFFFFFF;
		bool flagNot = 1;
		for (int j = 0; j < 4; j++) {
			int xt = board.i2x(i) + x_step[j],
				yt = board.i2y(i) + y_step[j];
			if (judgeOut(xt, yt)) continue;
			//核心判断
			if (flagEye == 0x7FFFFFFF) flagEye = board(xt, yt);
			if (board(xt, yt) != flagEye|| qi[mark(xt, yt)] == 1) { flagEye = 0; }	//同一色，且均非一气
			if (board(xt, yt) == 0													//上下左右应不为空
			|| (board(xt, yt) == player && qi[mark(xt, yt)] != 1)					//若是我，应只一气
			|| (board(xt, yt) != player && qi[mark(xt, yt)] == 1)){ flagNot = 0; }	//若是敌，应必不只一气
		}
		if (flagEye) mark[i] = EYEPOINT + (flagEye > 0 ? 1 : -1);
		else 
		if (flagNot) mark[i] = NOTPOINT;
	}
}
/*--------------------------------[ 劫判定 ]--------------------------------
*	[输入]: [1] 棋盘		[2] 己方颜色	[3]预期落子点	[4] 气	[5] 棋块
*	[输出]: [1] 是否为劫	[2] 对应劫点
*	[劫]  : 只有一种模式: 黑白眼相交错。
			x o @ x		x o @ x
			o   o @		o @   @
			x o @ x		x o @ x
*	[判定]: [1] 四周全敌 [2] 有且只有一方，为一气一点
**------------------------------------------------------------------------*/
static int judgeJie(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, int pos, STONE player) {
	const static int
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	int  JiePos  =-1;
	bool JieArmy = 0;
	int x0 = board.i2x(pos),
		y0 = board.i2y(pos);
	for (int i = 0; i < 4; i++) {
		int xt = x0 + x_step[i],
			yt = y0 + y_step[i];
		if (judgeOut(xt, yt)) continue;
		if (board(xt, yt) == 0
		||  board(xt, yt) == player) return -1;				//四周非全敌，非劫
		if (qi[mark(xt, yt)] == 1) {						//是敌且只一气
			if  (JieArmy == 1) return -1;					//有两个一气敌，非劫
			else JieArmy =  1;
			for (int j = 0; j < 4; j++) {					//一气敌只有自己
				int xtt = xt + x_step[j],
					ytt = yt + y_step[j];
				if (judgeOut(xtt, ytt)
				|| (ytt == y0 && xtt == x0)) continue;
				if (board(xtt, ytt) != player) return -1;
			}
			JiePos = board.xy2i(xt, yt); 
		}
	}
	return JiePos;
}
/*--------------------------------[ [4]:输赢判定(数子法) ]--------------------------------
*	[算法]:
		[RULE 4]:局势判定(数子法)
*	[注]: 无气杀我非杀他，为禁入
**----------------------------------------------------------------------------------------*/
static char judgeWin(State& state) {	//[RULE 4]:局势判定(数子法)
	int ScoreBlack = 0,
		ScoreWhite = 0;
	for (int i = 0; i < state.board.size(); i++) {
		if		(state.mark [i] == 0|| state.mark[i] == -40)	return 0;
		else if (state.board[i] == BLACK)				ScoreBlack++;
		else if (state.board[i] 
			== WHITE)				ScoreWhite++;
		else if (state.mark [i] == EYEPOINT + BLACK)	ScoreBlack++;
		else if (state.mark [i] == EYEPOINT + WHITE)	ScoreWhite++;
	}
	return ScoreBlack > ScoreWhite ? 1 : -1;		//贴子
}
/*--------------------------------[ 棋块数气 ]--------------------------------
*	[输入]: [1] 棋盘board
*	[输出]: [1] 棋盘上棋块mark [2] 棋块对应的气
*	[棋块]: 上下左右连通的同一色棋的区域.
*	[气]  : 同一棋块上下左右连接的空点总数.
**--------------------------------------------------------------------------*/
static void ComputerQi(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark) {
	//棋块标记
	int markCur = 1;
	int HeadStoneFlag[BOARDSIZE * BOARDSIZE] = { 0 };			//并查集头节点标记
	for (int y = 0; y < BOARDSIZE; y++) {
		for (int x = 0; x < BOARDSIZE; x++) {
			if (board(x, y) == 0) continue;
			bool flag = 1;
			//STONE UP
			if (y > 0 && board(x, y - 1) == board(x, y)) {
				mark(x, y) = mark(x, y - 1);
				flag = 0;
			}
			//STONE LEFT
			if (x > 0 && board(x - 1, y) == board(x, y)) {
				flag = 0;
				if (mark(x, y) == 0)
					mark(x, y) =  mark(x - 1, y);
				else if (
					mark(x, y) != mark(x - 1, y)
				) {
					//并查集 更新
					int    HeadStoneMe   =  HeadStoneFlag[mark(x, y)],
						   HeadStoneLeft =  HeadStoneFlag[mark(x - 1, y)];
					while (HeadStoneMe   != HeadStoneFlag[HeadStoneMe  ])
						   HeadStoneMe   =  HeadStoneFlag[HeadStoneMe  ];	//检索头节点
					while (HeadStoneLeft != HeadStoneFlag[HeadStoneLeft])
						   HeadStoneLeft =  HeadStoneFlag[HeadStoneLeft];	//检索头节点
					if (HeadStoneMe != HeadStoneLeft)
						HeadStoneFlag[HeadStoneLeft] = HeadStoneMe;
				}
			}
			//No STONE like
			if (flag) {
				HeadStoneFlag[markCur] = markCur;
				mark(x, y) = markCur++;
			}
		}
	}
	//完成棋块标记
	for (int i = 0; i < board.size(); i++) {
		int    HeadStone  = HeadStoneFlag[mark[i]];
		while (HeadStone != HeadStoneFlag[HeadStone])
			   HeadStone  = HeadStoneFlag[HeadStone];	//检索头节点
		mark[i] = HeadStone;
	}
	//棋块数气
	const static int
		x_step[] = { 0, 0, 1,-1 },
		y_step[] = { 1,-1, 0, 0 };
	int buf[4], bufcur = 0;
	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
		if (board[i] != 0) continue;
		bufcur = 0;
		for (int j = 0; j < 4; j++) {
			int xt = board.i2x(i) + x_step[j],
				yt = board.i2y(i) + y_step[j];
			if (judgeOut(xt, yt)
				|| board(xt, yt) == 0)continue;
			buf[bufcur++] = mark(xt, yt);
		}
		for (int j = 0; j < bufcur; j++) {
			if (buf[j] == 0) continue;
			for (int k = j + 1; k < bufcur; k++)
				if (buf[j] == buf[k]) buf[k] = 0;
			qi[buf[j]]++;
		}
	}
}
};
#endif