#ifndef GO_ACTION_H
#define GO_ACTION_H

#include "include.h"
#include "state.h"

using namespace std;

namespace Go {
	/*
	 *  Disable global isomorphism
	 */
	inline bool judgeJie(State& state) {
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
	 *  禁入点标记, not allow suicide
	 */
	inline void judgeBan(array<Color, BOARDNUM>& board,
		array<unsigned char, BOARDNUM>& qi,
		array<short, BOARDNUM>& mark, Color player) {

		for (int i = 0; i < board.size(); i++) {
			if (board[i] != 0) continue;

			bool isBan = 1;

			for (int j = 0; j < 4; j++) {
				int xt = i % BOARDSIZE + adj_x[j],
					yt = i / BOARDSIZE + adj_y[j],
					vt = yt * BOARDSIZE + xt;

				if (xt < 0 || xt >= BOARDSIZE ||
					yt < 0 || yt >= BOARDSIZE)
					continue;

				//核心判断
				if ((board[vt] == 0)) {
					isBan = 0;
					break;
				}

				if ((board[vt] == player && qi[mark[vt]] != 1) || 	//若是我，应只一气
					(board[vt] != player && qi[mark[vt]] == 1)) 	//若是敌，应必不只一气
					isBan = 0;
			}

			if (isBan)
				mark[i] = BANPOINT * player;
		}
	}

	/*
	 *  禁入点、眼点标记, not allow suicide
	 */
	inline void judgeBanAndEye(array<Color, BOARDNUM>& board,
		array<unsigned char, BOARDNUM>& qi,
		array<short, BOARDNUM>& mark, Color player) {

		for (int i = 0; i < board.size(); i++) {
			if (board[i] != 0) continue;

			char isEye = 0x7F;
			bool isBan = 1;

			for (int j = 0; j < 4; j++) {
				int xt = i % BOARDSIZE + adj_x[j],
					yt = i / BOARDSIZE + adj_y[j],
					vt = yt * BOARDSIZE + xt;

				if (xt < 0 || xt >= BOARDSIZE ||
					yt < 0 || yt >= BOARDSIZE)
					continue;

				//核心判断
				if ((board[vt] == 0)) {
					isEye = isBan = 0;
					break;
				}
				if (isEye == 0x7F)
					isEye = board[vt];

				if (board[vt] != isEye || qi[mark[vt]] == 1)
					isEye = 0; 	//同一色，且均非一气

				if ((board[vt] == player && qi[mark[vt]] != 1) ||	//若是我，应只一气
					(board[vt] != player && qi[mark[vt]] == 1)) 	//若是敌，应必不只一气
					isBan = 0;
			}

			if (isEye != 0)
				mark[i] = EYEPOINT * isEye;
			if (isBan)
				mark[i] = BANPOINT * player;
		}
	}

}
#endif
