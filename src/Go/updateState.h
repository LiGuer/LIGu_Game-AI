#ifndef GO_UPDATE_STATE_H
#define GO_UPDATE_STATE_H

#include "include.h"
#include "action.h"

using namespace std;

namespace Go {
	/*
	 *  remove stone block
	 */
	inline void removeBlock(State& s, int id) {
		int i = id;

		int buf[4], bufcur = 0;

		do {
			s.board[i] = 0;
			s.mark [i] = -1;

			bufcur = 0;

			for (int j = 0; j < 4; j++) {
				int xt = i % BOARDSIZE + adj_x[j],
					yt = i / BOARDSIZE + adj_y[j],
					vt = yt * BOARDSIZE + xt;

				if (xt >= 0 && xt < BOARDSIZE &&
					yt >= 0 && yt < BOARDSIZE && s.board[vt] != EMPTY)
					buf[bufcur++] = s.mark[vt];
			}

			if (bufcur != 0) {
				sort(buf, buf + bufcur);

				for (int j = 0; j < bufcur; j++) {
					if (j == 0)
						s.qi[buf[j]] ++;
					else if (buf[j] != buf[j - 1])
						s.qi[buf[j]] ++;
				}
			}
		} while ((i = s.next[i]) != id);

		return;
	}

	/*
	 *  merge stone block
	 */
	inline void mergeBlock(State& s, int id1, int id2) {
		if (id1 < id2)
			swap(id1, id2);

		int i = id1;

		do {
			for (int j = 0; j < 4; j++) {
				int xt = i % BOARDSIZE + adj_x[j],
					yt = i / BOARDSIZE + adj_y[j],
					vt = yt * BOARDSIZE + xt,
					found = 0;;

				if (xt < 0 || xt >= BOARDSIZE ||
					yt < 0 || yt >= BOARDSIZE || s.board[vt] != EMPTY)
					continue;

				for (int k = 0; k < 4; k++) {
					int xt_ = xt + adj_x[k],
						yt_ = yt + adj_y[k],
						vt_ = yt_ * BOARDSIZE + xt_;

					if (xt_ >= 0 && xt_ < BOARDSIZE &&
						yt_ >= 0 && yt_ < BOARDSIZE && s.mark[vt_] == id2) {
						found = 1;
						break;
					}			
				}
				if (!found)
					s.qi[id2] ++;
			}
			// update mark and avoid repeated search 
			s.mark[i] = id2; 
		} while ((i = s.next[i]) != id1);

		swap(s.next[id1], s.next[id2]);

		return;
	}

	/*
	 *  update state with move stone
	 */
	inline bool updateState(State& s) {
		// Pass
		if (s.action == PASS)
			return true;

		// is legal move
		if (s.board[s.action] != EMPTY || isSuicide(s))
			return false;

		// play move
		s.board[s.action] = s.player;
		s.mark [s.action] = s.action;
		s.next [s.action] = s.action;
		s.qi   [s.action] = 0;

		for (int j = 0; j < 4; j++) {
			int xt = s.action % BOARDSIZE + adj_x[j],
				yt = s.action / BOARDSIZE + adj_y[j],
				vt = yt * BOARDSIZE + xt;

			if (xt >= 0 && xt < BOARDSIZE &&
				yt >= 0 && yt < BOARDSIZE && s.board[vt] == EMPTY)
				s.qi[s.action] ++;
		}

		// detect neighborhood
		int buf[4], bufcur = 0;

		for (int j = 0; j < 4; j++) {
			int xt = s.action % BOARDSIZE + adj_x[j],
				yt = s.action / BOARDSIZE + adj_y[j],
				vt = yt * BOARDSIZE + xt;

			if (xt < 0 || xt >= BOARDSIZE ||
				yt < 0 || yt >= BOARDSIZE || s.board[vt] == EMPTY)
				continue;

			buf[bufcur++] = s.mark[vt];
		}

		// clear the opponent colors have no qi and merge stone blocks
		if (bufcur != 0) {
			sort(buf, buf + bufcur);

			for (int j = 0; j < bufcur; j++) {
				if (j == 0 || buf[j] != buf[j - 1]) {
					s.qi[buf[j]] --;

					if (s.board[buf[j]] == -s.player && s.qi[buf[j]] == 0) 
						removeBlock(s, buf[j]);
					else if(s.board[buf[j]] == s.player)
						mergeBlock(s, s.mark[s.action], buf[j]);
				}
			}
		}

		// disable global isomorphism
		if (judgeJie(s))
			return false;

		return true;
	}


}
#endif
