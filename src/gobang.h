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
#ifndef GOBANG_H
#define GOBANG_H
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MiniMax.h"
#include "../../LiGu_AlgorithmLib/Mat.h"
/******************************************************************************
*                    GoBang AI
******************************************************************************/
namespace GoBang_AI {
	typedef char CHESS;
#define BOARD_SIZE 15
#define BLACK  1
#define WHITE -1
	struct State {
		char me, player;
		int pos = -1;
		Mat<CHESS>* board;
		CHESS& operator[](int i)		{ return (*board)[i]; }
		CHESS& operator()(int i, int j) { return (*board)(i, j); }
	};
	/*---------------- 函数声明 ----------------*/
	void run(CHESS* board, int& x, int& y, char who);
	void run(State& board, int& x, int& y);
	int  evaluate(State& board);
	bool newStateFunc(State& board, State& newboard);
	char judgeWin(State& board);
	char judgeLineChess(char x, char y, char dx, char dy, State& board, char& flag);
	inline static bool judgeOut(int x, int y);
	/*---------------- AI计算结果 ----------------*/
	void run(CHESS* board, int& x, int& y, char who) {
		State boardState;
		Mat<CHESS> boardMat(15, 15);
		boardState.board = &boardMat.getData(board);
		boardState.player = -who;
		boardState.me	  =  who;
		run(boardState, x, y);
	}
	void run(State& board, int& x, int& y) {
		MiniMax<State> AI(evaluate, newStateFunc, judgeWin, [](State& x) { x[x.pos] = 0; });
		int maxScore = AI.Policy(0, board, -0x7fffffff, 0x7fffffff);
		x = AI.maxScoreState.pos / BOARD_SIZE;
		y = AI.maxScoreState.pos % BOARD_SIZE;
	}
	/*---------------- 棋局分数评价函数 ----------------*/
    int evaluate(State& board) {
		int score = 0;
		const static char 
			step_x[4] = { 0, 1, 1, 1 },
			step_y[4] = { 1, 0, 1,-1 };
		for (char y = 0; y < BOARD_SIZE; y++) {
			for (char x = 0; x < BOARD_SIZE; x++) {
				if (board(x, y) == 0) continue;
				char mark = board(x, y) == board.me ? 1 : -1, flag, num;
				for (char k = 0; k < 4; k++) {
					num = judgeLineChess(
						x, y, 
						step_x[k], 
						step_y[k], 
						board, flag
					);
					if (num  == 0) continue;
					if (num  >= 5){ score += mark * pow(10, 5); continue; }
					if (flag == 2) continue;
					else score += mark * pow(10, num - flag);
				}
			}
		}return score;
	}
	/*---------------- 生成新状态 ----------------*/
	//默认五子棋连通，不考虑"飞子"
	bool newStateFunc(State& board, State& newboard) {
		const static char 
			step_x[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
			step_y[] = { 1,-1, 0, 0, 1,-1,-1, 1 };
		if  (newboard.pos == -1) { newboard = board; newboard.pos = -1; }
		else newboard[newboard.pos] = 0;
		for (int i =  newboard.pos + 1; i < BOARD_SIZE * BOARD_SIZE; i++) {
			if (board[i] != 0) continue;
			for (int k = 0; k < 8; k++) {
				int xt = i / BOARD_SIZE + step_x[k],
					yt = i % BOARD_SIZE + step_y[k];
				if (judgeOut(xt, yt) && board(xt, yt) != 0) {
					newboard[i]  = newboard.player = -board.player;
					newboard.pos = i;
					return true;
				}
			}
		}
		return false;
	}
	//----------------判断棋子连线----------------
	char judgeLineChess(char x, char y, char dx, char dy, State& board, char& flag) {
		char num = 1;
		flag = 0;
		//前子
		int xt = x - dx,
			yt = y - dy;
		if  (judgeOut(xt, yt)) {
			if (board(xt, yt) == board(x, y)) return 0;
			if (board(xt, yt) != 0) flag = 1;
		}
		else flag = 1; 
		//连线
		xt = x + num * dx,
		yt = y + num * dy;
		while (
			judgeOut(xt,yt)
			&& board(xt,yt) 
			== board(x, y)
		) {
			num++; 
			xt += dx;
			yt += dy;
		}
		//后子
		if (judgeOut(xt, yt)
			|| board(xt, yt) != 0) flag++;
		return num;
	}
	//----------------判断输赢----------------
	//算连子，连五判赢
	char judgeWin(State& board) {
		const static char 
			step_x[4] = { 0, 1, 1, 1 }, 
			step_y[4] = { 1, 0, 1,-1 };
		for (char y = 0; y < BOARD_SIZE; y++) {
			for (char x = 0; x < BOARD_SIZE; x++) {
				if (board(x, y) == 0) continue;
				for (char k = 0; k < 4; k++) {
					int xt = x - step_x[k],
						yt = y - step_y[k];
					if (judgeOut(xt,yt)
						&& board(xt,yt) 
						== board(x, y)
					) continue;
					int num = 1;
					xt = x + step_x[k],
					yt = y + step_y[k];
					while (
						judgeOut(xt,yt)
						&& board(xt,yt) 
						== board(x, y)
					) {
						num++; 
						xt += step_x[k]; 
						yt += step_y[k];
					}
					if (num >= 5) return board(x, y);
				}
			}
		}return 0;
	}
	//----------------判断过界----------------
	inline static bool judgeOut(int x, int y) {
		return x < 0 || x >= BOARD_SIZE
			|| y < 0 || y >= BOARD_SIZE
			? false : true;
	}
};
#endif