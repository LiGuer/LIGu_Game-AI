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
	 *  not allow suicide
	 */
	inline bool isSuicide(State& s) {
		for (int j = 0; j < 4; j++) {
			int xt = s.action % BOARDSIZE + adj_x[j],
				yt = s.action / BOARDSIZE + adj_y[j],
				vt = yt * BOARDSIZE + xt;

			if (xt < 0 || xt >= BOARDSIZE ||
				yt < 0 || yt >= BOARDSIZE)
				continue;

			//核心判断
			if (s.board[vt] == EMPTY)
				return false;

			if ((s.board[vt] == s.player && s.qi[s.mark[vt]] != 1) || 	//若是我，应只一气
				(s.board[vt] != s.player && s.qi[s.mark[vt]] == 1)) 	//若是敌，应必不只一气
				return false;
		}

		return true;
	}
}
#endif
