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
#ifndef GO_AI_H
#define GO_AI_H
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../../../LiGu_Math/src/Math/Matrix/Mat.h"
#include "../../src/MontecarloTreeSearch.h"
/******************************************************************************
* 
*                    围棋AI Go-AI
*
******************************************************************************/
namespace GoAI {
#define BOARDSIZE 9
#define BLACK  1
#define WHITE -1
#define BANPOINT 0x7FFF
#define EYEPOINT 0x7FFC
#define JIEPOINT 0x7FF9
#define HADMOVEPOINT 0x7FF0
#define STATELASTNUM 7
typedef char STONE;
/******************************************************************************
*                    状态
*	[数据]: [1]下子位置		[2]棋手
			[3]棋盘			[4]标记棋盘		[5]气标记
******************************************************************************/
struct State {
	short action = -1;
	char  player = 0;
	Mat<STONE> board{ BOARDSIZE , BOARDSIZE };
	Mat<short> mark { BOARDSIZE , BOARDSIZE },
			   qi	{ BOARDSIZE * BOARDSIZE };
	State* StateLast[STATELASTNUM] = { NULL };
	STONE& operator[](int i)		{ return board[i]; }
	STONE& operator()(int x, int y) { return board(x, y); }
	State& operator=(State& x) {
		action	= x.action;
		player	= x.player;
		board	= x.board;
		mark	= x.mark;
		qi		= x.qi;
		for (int i = 0; i < STATELASTNUM; i++)
			StateLast[i] = x.StateLast[i];
		return *this;
	}
};
/******************************************************************************
*                    基础函数
******************************************************************************/
//函数声明
void run			(STONE* board, int& x, int& y, STONE player);
void run			(State* board);
bool newStateRand	(State&, State&, bool);
bool newStateRand	(State& state);
bool downStone		(State& state);
char judgeWin		(State& state);
void ComputerQi		(Mat<STONE>& board, Mat<short>& qi, Mat<short>& mark);
void judgeBanAndEye	(Mat<STONE>& board, Mat<short>& qi, Mat<short>& mark, STONE player);
bool judgeJie		(State& state);
/*--------------------------------[ 运行 ]--------------------------------*/
Mat<STONE>* run(int& x, int& y, STONE player) {
	static State* root = new State;
	if (root->StateLast[0] == NULL) {
		for (int i = 0; i < STATELASTNUM; i++)
			root->StateLast[i] = new State;
	}
	// People
	root->player = player;
	root->action = x * BOARDSIZE + y;
	downStone(*root);

	for (int i = 1; i < STATELASTNUM; i++)
		root->StateLast[i]->board = root->StateLast[i - 1]->board;
	root->StateLast[0]->board = root->board;
	// Ai
	run(root);
	x = root->board.i2x(root->action);
	y = root->board.i2y(root->action);
	return &root->board;
}

void run(STONE* board, int& x, int& y, STONE player) {
	State* root = new State;
	root->player = -player;
	memcpy(root->board.data, board, sizeof(STONE) * BOARDSIZE * BOARDSIZE);
	run(root);
	x = root->board.i2x(root->action);
	y = root->board.i2y(root->action);
	memcpy(board, root->board.data, sizeof(STONE) * BOARDSIZE * BOARDSIZE);
}

void run(State* board) {
	ComputerQi		(board->board, board->qi, board->mark);
	judgeBanAndEye	(board->board, board->qi, board->mark, -board->player);

	typedef MonteCarloTreeSearch<State> AI;
	AI ai(newStateRand, judgeWin, 1E5);

	*board = *ai.run(board);
}

/*--------------------------------------------------------------------------------
						随机新状态生成 New State Rand
*	[算法]: 基于输入[棋盘]，在可能的动作下，随机选择一个动作
*	[流程]: 
		[1] 确认状态空间数量
			[1.1] 若数量为零, 返回失败
		[2] 随机选择一个状态, 并返回
--------------------------------------------------------------------------------*/
// 用于MonteCarlo[2. 拓展]
bool newStateRand(State& state, State& newState, bool isSimulation) {
	if (isSimulation) 
		return newStateRand(state);
	//init
	newState.player = -state.player;
	memcpy(newState.StateLast + 1, state.StateLast, (STATELASTNUM - 1) * sizeof(State*));
	newState.StateLast[0] = &state;
	//[1]
	short num = 0;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	//[2]
	while (num > 0) {
		newState.board = state.board;
		short index = rand() % num + 1;
		for (int i = 0; i < state.board.size(); i++) {
			if (state.mark [i] == 0
			&&  state.board[i] == 0) index--;
			if (index == 0) {
				newState.action = i; break;
			}
		}
		downStone(newState);
		if (judgeJie(newState)) { 
			state.mark[newState.action] = JIEPOINT; 
			num--; 
		}
		else { 
			state.mark[newState.action] = HADMOVEPOINT; 
			return true;
		}
;	}
	newState.action = -1; 
	ComputerQi(state.board, state.qi, state.mark);
	judgeBanAndEye(state.board, state.qi, state.mark, -state.player);
	return false;
}
// 用于MonteCarlo[3. 模拟]
State oldState[STATELASTNUM]; 
short oldStateCur = 0;

bool newStateRand(State& state) {
	oldState[oldStateCur] = state;
	state.player = -state.player;
	memcpy(state.StateLast + 1, oldState[oldStateCur].StateLast, (STATELASTNUM - 1) * sizeof(State*));
	state.StateLast[0] = &oldState[oldStateCur];
	//[1]
	short num = 0;
	for (int i = 0; i < state.board.size(); i++)
		if (state.mark [i] == 0 
		&&  state.board[i] == 0) num++;
	//[2]
	while (num > 0) {
		short index = rand() % num + 1;
		for (int i = 0; i < state.board.size(); i++) {
			if (oldState[oldStateCur].mark [i] == 0
			&&  oldState[oldStateCur].board[i] == 0) index--;
			if (index == 0) {
				state.action = i; break;
			}
		}
		downStone(state);
		if (judgeJie(state)) { 
			oldState[oldStateCur].mark[state.action] = JIEPOINT; 
			state.board = oldState[oldStateCur].board; 
			num--; 
		}
		else {
			oldStateCur = (oldStateCur + 1) % STATELASTNUM;
			return true;
		}
	}
	state.action = -1; 
	ComputerQi(state.board, state.qi, state.mark); 
	judgeBanAndEye(state.board, state.qi, state.mark, -state.player); 
	return false;
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
		[4] 无气提子: 只提对方无气子
		[5] 眼点禁入点劫点标记 (帮下一回标记)
**-------------------------------------------------------------------------------*/
static bool downStone12(State& state) {
	//落子
	if (state[state.action] != 0) 
		return false;
	state[state.action] = state.player;

	//无气提子
	const static int
		x_step[] = { 0, 0, 1,-1 },
		y_step[] = { 1,-1, 0, 0 };

	for (int j = 0; j < 4; j++) {						// 无气提子
		int xt = state.board.i2x(state.action) + x_step[j],
			yt = state.board.i2y(state.action) + y_step[j];

		if (!state.board.isOut(xt, yt) 
			&& state.board(xt, yt) == -state.player 
			&& state.qi[state.mark(xt, yt)] == 1
		) {
			for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
				if (state.mark[i] == state.mark(xt, yt))
					state.board[i] = 0;
			}
		}
	}

	//棋块数气
	ComputerQi(state.board, state.qi, state.mark);

	//眼点禁入点劫点标记 (帮下一回标记)
	judgeBanAndEye(state.board, state.qi, state.mark, -state.player);
	return true;
}

static bool downStone(State& state) {
	//落子
	if (state[state.action] != 0)
		return false;
	state[state.action] = state.player;

	//棋块数气
	ComputerQi(state.board, state.qi, state.mark);

	//无气提子
	for (int i = 0; i < state.board.size(); i++)
		if (state.board[i] == -state.player && state.qi[state.mark[i]] == 0)
			state.board[i] = state.mark[i] = 0;

	//眼点禁入点劫点标记 (帮下一回标记)
	judgeBanAndEye(state.board, state.qi, state.mark, -state.player);
	return true;
}

/*--------------------------------[ 落子 ]--------------------------------*/
bool downStone(Mat<STONE>* board, Mat<STONE>& boardNew, int x, int y, STONE player) {
	boardNew = board[0];
	
	if (boardNew.isOut(x, y) || boardNew(x, y) != 0)	// 是否过界, 是否空子
		return false;

	Mat<short> qi, mark;									// 数气
	ComputerQi(boardNew, qi, mark);
	
	//if (judgeBan(boardNew, qi, mark, x, y, player))		// 禁入点判定
		//return false;

	boardNew(x, y) = player;							// 落子

	const static int
		x_step[] = { 0, 0, 1,-1 },
		y_step[] = { 1,-1, 0, 0 };

	for (int j = 0; j < 4; j++) {						// 无气提子
		int xt = x + x_step[j],
			yt = y + y_step[j];

		if (boardNew(xt, yt) == -player && qi[mark(xt, yt)] == 1) {
			for (int i = 0; i < boardNew.size(); i++) {
				if (mark[i] == mark(xt, yt))
					boardNew[i] = 0;
			}
		}
	}
	
	for (int i = 0; i < STATELASTNUM; i++) {			// 禁同形
		if (board + i != NULL && boardNew == board[i]) {
			boardNew = board[0];
			return false;
		}
	}
	return true;
}

/*--------------------------------[ 禁入点标记 ]--------------------------------
*	*输入: [1] 棋盘board	[3] 气	[4] 棋块
*	*输出: [1] 被标记眼点的棋盘(双方都标记)
*	*禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
*	*眼点判定:   [1]上下左右皆我，且均非一气 (眼点一定是对方禁入点)
**------------------------------------------------------------------------------*/
static void judgeBanAndEye(Mat<STONE>& board, Mat<short>& qi, Mat<short>& mark, STONE player) {
	const static int
		x_step[] = { 0, 0, 1,-1 },
		y_step[] = { 1,-1, 0, 0 };
	for (int i = 0; i < board.size(); i++) {
		if (board[i] != 0) continue;
		char isEye = 0x7F;
		bool isBan = 1;
		for (int j = 0; j < 4; j++) {
			int xt = board.i2x(i) + x_step[j],
				yt = board.i2y(i) + y_step[j];
			if (board.isOut(xt, yt)) continue;
			//核心判断
			if ((board(xt, yt) == 0)) { isEye = isBan = 0; break; }
			if (isEye == 0x7F) isEye = board(xt, yt);
			if  (board(xt, yt) != isEye  || qi[mark(xt, yt)] == 1) isEye = 0; 	//同一色，且均非一气
			if ((board(xt, yt) == player && qi[mark(xt, yt)] != 1)				//若是我，应只一气
			||  (board(xt, yt) != player && qi[mark(xt, yt)] == 1))isBan = 0; 	//若是敌，应必不只一气
		}
		mark[i] = EYEPOINT * isEye;
		if (isBan) mark[i] = BANPOINT * player;
	}
}
/*--------------------------------[ 劫判定 ]--------------------------------
*	[算法]:
		[RULE 4]: 禁止全局同形
			劫争: 双方棋子互围, 一方提子, 另一方不得立即回提, 只能在别处下子, 再回提, 
				  以避免全局同形导致死循环.
**------------------------------------------------------------------------------*/
static bool judgeJie(State& state) {
	if(state.StateLast[1] != NULL && state.board == state.StateLast[1]->board) return true;
	if(state.StateLast[3] != NULL && state.board == state.StateLast[3]->board) return true;
	if(state.StateLast[5] != NULL && state.board == state.StateLast[5]->board) return true;
	return false;
}
/*--------------------------------[ [4]:输赢判定(数子法) ]--------------------------------
*	[算法]:
		[RULE 4]:局势判定(数子法)
*	[注]: 无气杀我非杀他，为禁入
**----------------------------------------------------------------------------------------*/
static char judgeWin(State& state) {	//[RULE 4]:局势判定(数子法)
	short ScoreBlack = 0;
	if (state.action != -1 || (state.StateLast[0] != NULL && state.StateLast[0]->action != -1)) return 0;
	for (int i = 0; i < state.board.size(); i++) {
		if (state.mark [i] == 0 || state.mark[i] == HADMOVEPOINT) return 0;
		if (state.board[i] == BLACK 
		||  state.mark [i] == EYEPOINT * BLACK
		||  state.mark [i] == BANPOINT * WHITE
		) ScoreBlack++;
	}
	return ScoreBlack > BOARDSIZE * BOARDSIZE / 2.0 + 3.75/*184.25*/ ? BLACK : WHITE;		//贴子
}

/*--------------------------------[ 棋块数气 ]--------------------------------
*	[输入]: [1] 棋盘board
*	[输出]: [1] 棋盘上棋块mark [2] 棋块对应的气
*	[棋块]: 上下左右连通的同一色棋的区域.
*	[气]  : 同一棋块上下左右连接的空点总数.
**--------------------------------------------------------------------------*/
static void ComputerQi(Mat<STONE>& board, Mat<short>& qi, Mat<short>& mark) {
	//init. clear
	qi.  zero();
	mark.zero();
	//棋块标记
	short markCur = 1;
	static Mat<short> HeadStoneFlag(board.size());			//并查集头节点标记
	HeadStoneFlag.zero();

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
				else if (mark(x, y) != mark(x - 1, y)) {
					//并查集 更新
					int    HeadStoneMe   =  HeadStoneFlag[mark(x,    y)],
						   HeadStoneLeft =  HeadStoneFlag[mark(x - 1,y)];
					while (HeadStoneMe   != HeadStoneFlag[HeadStoneMe  ])
						   HeadStoneMe   =  HeadStoneFlag[HeadStoneMe  ];	//检索头节点
					while (HeadStoneLeft != HeadStoneFlag[HeadStoneLeft])
						   HeadStoneLeft =  HeadStoneFlag[HeadStoneLeft];	//检索头节点
					if (HeadStoneMe != HeadStoneLeft)
						HeadStoneFlag[HeadStoneLeft] = HeadStoneMe;
				}
			}
			//No STONE like
			if (flag) 
				mark(x, y) = HeadStoneFlag[markCur] = markCur++;
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
	static int buf[4], bufcur;

	for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
		if (board[i] != 0) continue;
		bufcur = 0;
		for (int j = 0; j < 4; j++) {
			int xt = board.i2x(i) + x_step[j],
				yt = board.i2y(i) + y_step[j];
			if (board.isOut(xt, yt) || board(xt, yt) == 0) 
				continue;
			//气+1
			bool isRepeat = 0;
			for (int k = 0; k < bufcur; k++)
				if (buf[k] == mark(xt, yt)) {
					isRepeat = 1; 
					break; 
				};
			if (!isRepeat) {
				qi[mark(xt, yt)]++;
				buf[bufcur++] = mark(xt, yt);
			}
		}
	}
}
};
#endif