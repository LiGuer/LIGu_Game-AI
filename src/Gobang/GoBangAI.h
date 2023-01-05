/*
Copyright 2020-2022 LiGuer. All Rights Reserved.
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

#ifndef GOBANG_AI_H
#define GOBANG_AI_H

#include "GoBang.h"
#include "../../src/MiniMax.h"

using namespace GoBang;

/******************************************************************************
* 
*                    GoBang AI
* 
******************************************************************************/
namespace GoBang_AI {

	struct State {
		CHESS me, player;
		int action = -1;
		Mat<CHESS>* board;

		CHESS& operator[](int i) { return (*board)[i]; }
		CHESS& operator()(int i, int j) { return (*board)(i, j); }
	};

	/*---------------- 函数声明 ----------------*/
	static int evaluate(State& board);
	static bool newStateFunc(State& board, State& newboard);
	static CHESS judgeWin(State& board) {
		return GoBang::judgeWin(*(board.board));
	};

	/*---------------- AI计算结果 ----------------*/
	static void run(State& board, int& x, int& y) {
		MiniMax<State> AI(evaluate, newStateFunc, judgeWin,
			[](State& x) { x[x.action] = 0; }, 4
		);
		int maxReward = AI.Policy(board);

		x = board.board->i2x(AI.maxRewardState.action);
		y = board.board->i2y(AI.maxRewardState.action);
	}

	static void run(Mat<CHESS>& board, int& x, int& y, char who) {
		State boardState;

		boardState.board  = &board;
		boardState.player = -who;
		boardState.me	  =  who;
		run(boardState, x, y);
	}

	/*---------------- 棋局分数评价函数 ----------------*/
	static int evaluate(State& board) {
		int reward = 0;
		const static char 
			step_x[4] = { 0, 1, 1, 1 },
			step_y[4] = { 1, 0, 1,-1 };

		for (char y = 0; y < BOARD_SIZE; y++) {
			for (char x = 0; x < BOARD_SIZE; x++) {
				if (board(x, y) == 0)
					continue;

				char mark = board(x, y) == board.me ? 1 : -1, flag, num;

				for (char k = 0; k < 4; k++) {
					num = judgeLineChess(
						*board.board,
						x, y, 
						step_x[k], 
						step_y[k], 
						flag
					);
					if (num  == 0) 
						continue;
					if (num  >= 5){ 
						reward += mark * pow(10, 5);
						continue;
					}
					if (flag == 2) 
						continue;
					else
						reward += mark * pow(10, num - flag);
				}
			}
		}
		return reward;
	}

	/*---------------- 生成新状态 ----------------*/
	//默认五子棋连通，不考虑"飞子"
	static bool newStateFunc(State& board, State& newboard) {
		const static char 
			step_x[] = { 0, 0, 1,-1, 1,-1, 1,-1 },
			step_y[] = { 1,-1, 0, 0, 1,-1,-1, 1 };

		if (newboard.action == -1) { 
			newboard = board; 
			newboard.action = -1; 
		}
		else 
			newboard[newboard.action] = 0;

		for (int i = newboard.action + 1; i < board.board->size(); i++) {
			if (board[i] != 0) 
				continue;

			for (int k = 0; k < 8; k++) {
				int xt = board.board->i2x(i) + step_x[k],
					yt = board.board->i2y(i) + step_y[k];
				if (!board.board->isOut(xt, yt) && board(xt, yt) != 0) {
					newboard[i] = newboard.player = -board.player;
					newboard.action = i;
					return true;
				}
			}
		}
		return false;
	}
};

#endif