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
******************************************************************************/
namespace Go_AI {
#define BOARDSIZE 9
#define BLACK  1
#define WHITE -1
#define EYEPOINT -10
#define NOTPOINT -20
#define JIEPOINT -30
typedef char STONE;
/******************************************************************************
*                    状态
******************************************************************************/
struct State {
	int  pos = -1;
	char player = 0;
	Mat<STONE> board{ BOARDSIZE , BOARDSIZE };
	Mat<int>   mark { BOARDSIZE , BOARDSIZE };
	Mat<int>   qi	{ BOARDSIZE * BOARDSIZE };
	STONE& operator[](int i)		{ return board[i]; }
	STONE& operator()(int i, int j) { return board(i, j); }
};
/******************************************************************************
*                    基础函数
******************************************************************************/
//函数声明
void  run(STONE* board, int& x, int& y, STONE who, int& JiePos);
void  run(State& board, int& pos, int& JiePos);
bool judgeOut(int x, int y);
bool newStateRand(State&, State&, bool);
bool newStateRand(State& state);
bool downStone	(State& state);
void ComputerQi	(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark);
char judgeWin	(State& state);
int  judgeJie	(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, int pos, STONE player);
void judgeEyeAndNot(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, STONE player);
/*--------------------------------[ 下子 ]--------------------------------*/
void run(STONE* board, int& x, int& y, STONE who, int& JiePos) {
	State root;
	root.player = -who;
	root.board.getData(board);
	ComputerQi(root.board, root.qi, root.mark);
	judgeEyeAndNot(root.board, root.qi, root.mark, root.player);
	if (JIEPOINT != -1)root.board[JiePos] = JIEPOINT;
	int pos;
	run(root, pos, JiePos);
	x = root.board.i2x(pos);
	y = root.board.i2y(pos);
}
void run(State& board, int& pos, int& JiePos) {
	typedef MontecarloTreeSearch<State> AI;
	AI ai(
		newStateRand,
		judgeWin
	);
	State* state = ai.run(&board);
	pos    = state->pos;
	JiePos = -1;
	for (int i = 0; i < state->mark.size(); i++)
		if(state->mark[i] == JIEPOINT) JiePos = i;
}
/*--------------------------------[ nextStateRand ]--------------------------------
基于输入[board]棋盘，在可能的动作下，随机选择一个动作
*	* [a,b)随机整数: (rand() % (b-a))+ a;
**----------------------------------------------------------------------------*/
bool newStateRand(State& state, State& newState, bool isSimulation) {
	if (isSimulation) return newStateRand(state);
	newState.board  = state.board;
	newState.mark   = state.mark;
	newState.player =-state.player;
	int num = 0, index;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	if (num == 0) 
		return false;
	index = rand() % num;
	for (int i = 0; i < state.board.size(); i++) {
		if (state.mark [i] == 0
		&&  state.board[i] == 0) index--;
		if (index == 0) { 
			newState.pos = i; break; 
		}
	}
	downStone(newState);
	state.mark[newState.pos] = 40;
	return true;
}
bool newStateRand(State& state) {
	state.player = -state.player;
	int num = 0, index;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	if (num == 0) 
		return false;
	index = rand() % num;
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
[RULE 1]:无气提子
[RULE 3]:劫判定
**-------------------------------------------------------------------------------*/
bool downStone(State& state) {
	//劫判定
	int JiePos = judgeJie(state.board, state.qi, state.mark, state.pos, state.player);
	//落子
	state[state.pos] = state.player;
	state.mark.zero();
	//棋块数气
	ComputerQi(state.board, state.qi, state.mark);
	//无气提子
	for (int i = 0; i < state.board.size(); i++)
		if (state.qi[state.mark[i]] == 0
		&&  state.mark[i] != state.mark[state.pos]) {
			state.board[i] = 0;
			state.mark [i] = 0;
		}
	//禁入点标记 (帮下一回标记)
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
*	*判定: [1] 只一空点	[2] 上下左右同一色	[3]上下左右棋块，均非一气
------
*	[RULE 2]:非提禁入
*	禁入点: [1]我无气	[2]非杀他
*	禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
**------------------------------------------------------------------------------*/
void judgeEyeAndNot(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, STONE player) {
	const static int
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	for (int i = 0; i < board.size(); i++) {
		if (board[i] != 0) continue;
		bool flagEye = 1, 
				flagNot = 1;
		for (int j = 0; j < 4; j++) {
			int xt = board.i2x(i) + x_step[j],
				yt = board.i2y(i) + y_step[j];
			if (judgeOut(xt, yt)) continue;
			//核心判断
			if (board(xt, yt) != player || qi[mark(xt, yt)] == 1) { flagEye = 0; }
			if (board(xt, yt) == 0															//上下左右应不为空
			|| (board(xt, yt) == player && qi[mark(xt, yt)] != 1)		//若是我，应只一气
			|| (board(xt, yt) != player && qi[mark(xt, yt)] == 1)) {	//若是敌，应必不只一气
				flagNot = 0;
			}
		}
		if (flagEye) mark[i] = EYEPOINT + player;
		if (flagNot) mark[i] = NOTPOINT;
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
int judgeJie(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark, int pos, STONE player) {
	const static int
		x_step[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
		y_step[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
	int  JiePos  =-1;
	bool JieFlag = 1, 
		 JieArmy = 0;
	int x0 = board.i2x(pos),
		y0 = board.i2y(pos);
	for (int i = 0; i < 4; i++) {
		int xt = x0 + x_step[i],
			yt = y0 + y_step[i];
		if (judgeOut(xt, yt)) continue;						//过界
		if (board(xt, yt) == 0
		||  board(xt, yt) == player) {				//四周非全敌，非劫
			JieFlag = 0; break;
		}
		if (qi[mark(xt, yt)] == 1) {						//是敌且只1气
			if  (JieArmy == 1) { JieFlag = 0; break; }		//有两个1气敌，非劫
			else JieArmy =  1;
			for (int j = 0; j < 4; j++) {					//1气敌只有自己
				int xtt = xt + x_step[j],
					ytt = yt + y_step[j];
				if (judgeOut(xtt, ytt)
				|| (ytt == y0 && xtt == x0)) continue;
				if (board(xtt, ytt) != player) { 
					JieFlag = 0; 
					JieArmy = 0; 
					break; 
				}
			}
			if (JieArmy) { JiePos = board.xy2i(xt, yt); JieFlag = 1; }
		}
	}
	return JieFlag ? true : JiePos = -1, false;
}
/*--------------------------------[ [4]:输赢判定(数子法) ]--------------------------------
*	[RULE 4]:局势判定(数子法)
*	无气杀我非杀他，为禁入
**----------------------------------------------------------------------------------------*/
char judgeWin(State& state) {	//[RULE 4]:局势判定(数子法)
	double ScoreBlack = 0,
		   ScoreWhite = 0;
	for (int i = 0; i < state.board.size(); i++) {
		if		(state.board[i] == 0)					return 0;
		else if (state.board[i] == BLACK)				ScoreBlack++;
		else if (state.board[i] == WHITE)				ScoreWhite++;
		else if (state.mark [i] == EYEPOINT + BLACK)	ScoreBlack++;
		else if (state.mark [i] == EYEPOINT + WHITE)	ScoreWhite++;
	}
	return ScoreBlack - 2.5 > ScoreWhite ? 1 : -1;		//贴子
}
/*--------------------------------[ 棋块数气 ]--------------------------------
*	*输入: [1] 棋盘board
*	*输出: [1] 棋盘上棋块mark [2] 棋块对应的气
*	棋块: 上下左右连通的同一色棋的区域
*	气:
**--------------------------------------------------------------------------*/
void ComputerQi(Mat<STONE>& board, Mat<int>& qi, Mat<int>& mark) {
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