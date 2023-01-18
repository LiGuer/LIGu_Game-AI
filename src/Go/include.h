#ifndef GO_INCLUDE_H
#define GO_INCLUDE_H

#include <stdio.h>
#include <vector>
#include <array>
#include <queue>
#include <functional>
#include <algorithm>

using namespace std;

namespace Go {
#define BOARDSIZE 13
#define BOARDNUM 13 * 13
#define BLACK  1
#define WHITE -1
#define EMPTY  0
#define PASS  -1
#define BANPOINT 0x7FFF
#define EYEPOINT 0x7FFC

	const static int
		adj_x[] = { 0, 0, 1,-1 },
		adj_y[] = { 1,-1, 0, 0 };

}
#endif