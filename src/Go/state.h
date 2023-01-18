#ifndef GO_STATE_H
#define GO_STATE_H

#include "include.h"

using namespace std;

namespace Go {

	typedef char Color;

	/*
	 *  State & Action
	 */
	struct State {
		// action (x, y, c)
		short action = -2;
		Color player = -1;

		array<Color, BOARDNUM> board;
		array<short, BOARDNUM> mark, next;
		array<unsigned char, BOARDNUM> qi;

		State* parent = NULL;

		State() {
			fill(board.begin(),  board.end(), 0);
			fill(mark.begin(),   mark.end(), -1);
			fill(next.begin(),   next.end(), -1);
			fill(qi.begin(),     qi.end(),    0);
		}
	};

	/*
	 *  judge whether a state is terminated
	 */
	inline bool isTermination(State& s) {
		if (s.action == PASS &&
			s.parent != NULL &&
			s.parent->action == PASS)
			return true;
		return false;
	}

}
#endif
