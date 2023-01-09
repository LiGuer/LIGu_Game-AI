#ifndef GO_H
#define GO_H

#include <stdio.h>
#include <vector>
#include <functional>
#include <algorithm>
#include "C:/Users/29753/Desktop/Library/Math/src/Matrix/Mat.h"

using namespace std;

namespace Go {
#define BOARDSIZE 9
#define BLACK  1
#define WHITE -1
#define BANPOINT 0x7FFF
#define EYEPOINT 0x7FFC

	typedef char Stone;

	struct State {
		short action = -1;
		Stone player = -1;
		Mat<Stone> board{ BOARDSIZE , BOARDSIZE };
		Mat<short> 
			mark{ BOARDSIZE , BOARDSIZE },
			qi  { BOARDSIZE * BOARDSIZE };

		State* parent = NULL;

		State() {
			board.zero(BOARDSIZE,  BOARDSIZE);
			mark .zero(BOARDSIZE,  BOARDSIZE);
			qi   .zero(BOARDSIZE * BOARDSIZE);

			for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
				mark[i] = -1;
		}

		State& operator=(State& x) {
			action = x.action;
			player = x.player;
			board  = x.board;
			mark   = x.mark;
			qi     = x.qi;
			parent = x.parent;
			return *this;
		}
	};

	const static int
		x_step[] = { 0, 0, 1,-1 },
		y_step[] = { 1,-1, 0, 0 };

	/*
	 *  棋块识别, Union-Find Set
	 */
	static short find(Mat<short>& mark, short a) {
		if (mark[a] != a) {
			mark[a] = find(mark, mark[a]);
		}
		return mark[a];
	};

	static void detectBlock(Mat<Stone>& board, Mat<short>& mark) {
		for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
			mark[i] = i;
		}
		/* static function<short(short)> find = [&](short a) {
			if (mark[a] != a) {
				mark[a] = find(mark[a]);
			}
			return mark[a];
		};*/

		for (int y = 0; y < BOARDSIZE; y++) {
			for (int x = 0; x < BOARDSIZE; x++) {
				if (board(x, y) == 0) {
					mark(x, y) = -1;
					continue;
				}

				//Stone UP
				if (y > 0 && board(x, y - 1) == board(x, y)) {
					mark[find(mark, y + x * BOARDSIZE)] = find(mark, (y - 1) + x * BOARDSIZE);
				}
				//Stone LEFT
				if (x > 0 && board(x - 1, y) == board(x, y)) {
					mark[find(mark, y + x * BOARDSIZE)] = find(mark, y + (x - 1) * BOARDSIZE);
				}
			}
		}
		
		for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
			if (mark[i] == -1) {
				continue;
			}
			find(mark, i);
		}
	}

	/*
	 *  棋块数气 
	 */
	static void countQi(Mat<Stone>& board, Mat<short>& qi, Mat<short>& mark) {
		qi.zero();

		static int buf[4], bufcur;

		for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
			if (board[i] != 0) continue;

			bufcur = 0;

			for (int j = 0; j < 4; j++) {
				int xt = i / BOARDSIZE + x_step[j],
					yt = i % BOARDSIZE + y_step[j];
				if (board.isOut(xt, yt) || board(xt, yt) == 0)
					continue;
				else
					buf[bufcur++] = mark[xt * BOARDSIZE + yt];
			}

			sort(buf, buf + bufcur);

			for (int j = 0; j < bufcur; j++) {
				if (j == 0) 
					qi[buf[j]] ++;
				else if (buf[j] != buf[j - 1]) 
					qi[buf[j]] ++;
			}
		}
	}

	/*
	 *  禁止全局同形
	 */
	static bool judgeJie(State& state) {
		State* s = &state;

		for (int i = 0; i <= 2; i++) {
			if (s->parent != NULL && s->parent->parent != NULL) {
				s = s->parent->parent;

				if (state.board == s->board)
					return true;
			}
			else break;
		}
		return false;
	}

	/*
	 *  输赢判定
	 */
	static char judgeWin(State& s) {	//[RULE 4]:局势判定(数子法)
		short ScoreBlack = 0;
		/*for (int i = 0; i < BOARDSIZE; i++) {
			for (int j = 0; j < BOARDSIZE; j++) {
				if (s.board[i * BOARDSIZE + j] == 1)
					printf(" x");
				if (s.board[i * BOARDSIZE + j] == 0)
					printf(" .");
				if (s.board[i * BOARDSIZE + j] == -1)
					printf(" o");
			}printf("\n");
		}printf("\n");
		for (int i = 0; i < BOARDSIZE; i++) {
			for (int j = 0; j < BOARDSIZE; j++) {
				printf("%6d ", s.mark[i * BOARDSIZE + j]);
			}printf("\n\n");
		}printf("\n");
		
		if (state.action != -1 || 
		   (s.parent != NULL &&
			s.parent->action != -1)
		) return 0;*/

		for (int i = 0; i < s.board.size(); i++) {
			if (s.mark[i] == -1) 
				return 0;

			if (s.board[i] == BLACK || 
				s.mark[i]  == BANPOINT * WHITE ||
				s.mark [i] == EYEPOINT * BLACK
				) ScoreBlack++;
		}
		return ScoreBlack >= BOARDSIZE * BOARDSIZE / 2.0 + 3.75 ? BLACK : WHITE;		//贴子 184
	}

	/*
	 *  禁入点标记
	 */
	static void judgeBan(Mat<Stone>& board, Mat<short>& qi, Mat<short>& mark, Stone player) {
		for (int i = 0; i < board.size(); i++) {
			if (board[i] != 0) continue;

			bool isBan = 1;

			for (int j = 0; j < 4; j++) {
				int xt = i / BOARDSIZE + x_step[j],
					yt = i % BOARDSIZE + y_step[j];

				if (xt < 0 || xt >= BOARDSIZE || 
					yt < 0 || yt >= BOARDSIZE)
					continue;

				//核心判断
				if ((board(xt, yt) == 0)) { 
					isBan = 0; 
					break; 
				}

				if ((board(xt, yt) == player && qi[mark(xt, yt)] != 1) || 	//若是我，应只一气
					(board(xt, yt) != player && qi[mark(xt, yt)] == 1)) 	//若是敌，应必不只一气
					isBan = 0;
			}

			if (isBan)
				mark[i] = BANPOINT * player;
		}
	}

	static void judgeBanAndEye(Mat<Stone>& board, Mat<short>& qi, Mat<short>& mark, Stone player) {
		for (int i = 0; i < board.size(); i++) {
			if (board[i] != 0) continue;

			char isEye = 0x7F;
			bool isBan = 1;

			for (int j = 0; j < 4; j++) {
				int xt = board.i2x(i) + x_step[j],
					yt = board.i2y(i) + y_step[j];

				if (board.isOut(xt, yt)) continue;

				//核心判断
				if ((board(xt, yt) == 0)) { 
					isEye = isBan = 0;
					break;
				}
				if (isEye == 0x7F) 
					isEye = board(xt, yt);

				if (board(xt, yt) != isEye || qi[mark(xt, yt)] == 1) 
					isEye = 0; 	//同一色，且均非一气

				if ((board(xt, yt) == player && qi[mark(xt, yt)] != 1) ||	//若是我，应只一气
					(board(xt, yt) != player && qi[mark(xt, yt)] == 1)) 	//若是敌，应必不只一气
					isBan = 0;
			}

			if(isEye != 0)
				mark[i] = EYEPOINT * isEye;
			if (isBan) 
				mark[i] = BANPOINT * player;
		}
	}

	/*
	 *  无气提子
	 */
	static bool extractStone(Mat<Stone>& board, Mat<short>& qi, Mat<short>& mark, Stone player, short action) {
		bool fg = 0;

		for (int j = 0; j < 4; j++) {
			int xt = action / BOARDSIZE + x_step[j],
				yt = action % BOARDSIZE + y_step[j];

			if (xt < 0 || xt >= BOARDSIZE ||
				yt < 0 || yt >= BOARDSIZE ||
				board(xt, yt) != -player)
				continue;

			if (qi[mark(xt, yt)] == 1) {
				fg = 1;
				int mt = mark(xt, yt);

				for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
					if (mark[i] == mt)
						board[i] = 0;
				}

			}
		}

		return fg;
	}

	/*
	 *  落子
	 */
	static bool downStone(State& s) {
		//落子
		if (s.board[s.action] != 0 || 
		    (s.mark[s.action] != -1 && s.mark[s.action] != EYEPOINT && s.mark[s.action] != -EYEPOINT))
			return false;

		s.board[s.action] = s.player;

		bool fg = extractStone(s.board, s.qi, s.mark, s.player, s.action);

		if (1) {
			detectBlock(s.board, s.mark); 
			countQi(s.board, s.qi, s.mark);
		}
		else {
			static int buf[4], bufcur;
			
			bufcur = 0;
			s.mark[s.action] = s.action;

			for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
				if (s.board[i] == 0)
					s.mark[i] = -1;
			}

			for (int j = 0; j < 4; j++) {
				int xt = s.action / BOARDSIZE + x_step[j],
					yt = s.action % BOARDSIZE + y_step[j];

				if (xt < 0 || xt >= BOARDSIZE ||
					yt < 0 || yt >= BOARDSIZE ||
					s.board(xt, yt) != s.player)
					continue;

				buf[bufcur++] = s.mark[xt * BOARDSIZE + yt];
			}

			if (bufcur != 0) {
				sort(buf, buf + bufcur);
				s.mark[s.action] = buf[0];

				if (bufcur > 1) {
					for (int j = 1; j < bufcur; j++) {
						if (buf[j] != buf[j - 1]) {
							for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
								if (s.mark[i] == buf[j])
									s.mark[i] = buf[0];
							}
						}
					}
				}
			}

			countQi(s.board, s.qi, s.mark);
		}

		if (judgeJie(s))
			return false;

		//眼点禁入点劫点标记 (帮下一回标记)
		judgeBanAndEye(s.board, s.qi, s.mark, -s.player);
		return true;
	}

	/*
	 *  落子
	 */
	static State* nextState(State& s, int x, int y) {
		State* s_ = new State();
		*s_ = s;
		s_->parent = &s;
		s_->player = -s.player;
		s_->action = x * BOARDSIZE + y;

		if(downStone(*s_))
			return s_;
		else {
			s_->parent = NULL;
			delete s_;
			return NULL;
		}
	}
}

#endif