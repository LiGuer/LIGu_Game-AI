#ifndef GO_H
#define GO_H

#include "state.h"
#include "updateState.h"

using namespace std;

namespace Go {

	/*
	 *	calculate reach color
	 */
	inline int calculateReachColor(array<Color, BOARDNUM>& board, const int color) {
		auto reachable = 0;
		auto bd = vector<bool>(BOARDNUM, false);
		auto open = queue<int>();

		for (auto i = 0; i < BOARDNUM; i++) {
			if (board[i] == color) {
				reachable++;
				bd[i] = true;
				open.push(i);
			}
		}

		while (!open.empty()) {
			auto v = open.front();
			open.pop();

			for (auto j = 0; j < 4; j++) {
				int xt = v % BOARDSIZE + adj_x[j],
					yt = v / BOARDSIZE + adj_y[j],
					vt = yt * BOARDSIZE + xt;

				if (xt >= 0 && xt < BOARDSIZE &&
					yt >= 0 && yt < BOARDSIZE &&
					!bd[vt] && board[vt] == EMPTY) {
					reachable++;
					bd[vt] = true;
					open.push(vt);
				}
			}
		}
		return reachable;
	}

	/*
	 *  输赢判定
	 */
	inline char computeReward(State& s) {
		float komi = 7.5;

		auto white = calculateReachColor(s.board, WHITE);
		auto black = calculateReachColor(s.board, BLACK);
		return black - white - komi;
	}



	/*
	 *  落子
	 */
	inline State* nextState(State& s, int action) {
		State* s_ = new State();
		*s_ = s;
		s_->parent = &s;
		s_->player = -s.player;
		s_->action = action;

		if(updateState(*s_))
			return s_;
		else {
			s_->parent = NULL;
			delete s_;
			return NULL;
		}
	}
}

#endif