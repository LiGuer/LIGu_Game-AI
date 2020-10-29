#include "Go.h"
/******************************************************************************
*                    基础函数
******************************************************************************/
//----------------初始化----------------
void Go::init() {
	Map = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);
	clearMap();
	srand((unsigned int)time(NULL));
}
//----------------清空棋盘----------------
void Go::clearMap() {
	memset(Map, 0, sizeof(CHESS) * MapSize * MapSize);
}
//----------------下子----------------
bool Go::setMap(INT8S x, INT8S y, CHESS chess) {
	if (!judgeOutMap(x, y))return false;
	if (Map[y * MapSize + x] != 0)return false;
	CHESS maptemp[MapSize * MapSize];
	memcpy(maptemp, Map, sizeof(CHESS) * MapSize * MapSize);
	judgeNotPoint(maptemp, chess);
	if (maptemp[y * MapSize + x] == NOTPOINT)return false;
	return downChess(Map, x, y, chess);
}
//----------------判断该点是否过界----------------
bool Go::judgeOutMap(int x, int y) {
	return (x >= 0 && x < MapSize && y >= 0 && y < MapSize) ? true : false;
}
/******************************************************************************
*                   核心算法: 蒙特卡洛树搜索
*蒙特卡洛树搜索 MCTS:
*The process of MCTS is split up into four steps:
*[1]selection,[2]expansion,[3]simulation,[4]backpropagation.
*
******************************************************************************/
/*---------------- MCTS ----------------
*蒙特卡洛树搜索 MCTS:
*The process of MCTS is split up into four steps:
*[1]selection,[2]expansion,[3]simulation,[4]backpropagation.
**--------------------------------*/
MCTSNode* Go::MCTS(MCTSNode* node) {
	for (int i = 0; i < MctsBudget; i++) {
		MCTSNode* expandNode = TreePolicy(node);		//[1][2]
		int reward = Simulation(expandNode->state);			//[3]
		Backpropagation(expandNode, reward);			//[4]
	}
	return Select(node, false);							//Ans
}
/*---------------- [1]Select 选择 ----------------
以[UCB公式]为判别标准，
选择[node]节点下，最优子节点作为输出。
若isExploration关闭，则为Exploitation-Only仅利用模式，只按下目前最优的摇臂。
若isExploration开启，有探索尝试的机会。
**--------------------------------*/
MCTSNode* Go::Select(MCTSNode* node, bool isExploration)
{
	double UcbConst = isExploration == true ? sqrt(2) : 0;
	double maxScore = -0x7fffffff;
	MCTSNode* bestnode = NULL;
	for (int i = 0; i < node->childCur; i++) {
		double scoreTemp = UCB(node->child[i], UcbConst);
		if (maxScore < scoreTemp) {
			maxScore = scoreTemp;
			bestnode = node->child[i];
		}
	}
	return bestnode;
}
/*---------------- UCB ----------------
*Upper Confidence Bound (UCB)上置信界算法:
*                              ------------------------
*             W(node)         /   In( N(parentNode) )
*UCT(node) = --------- + c * / ------------------------
*             N(node)       √         N(node)
**--------------------------------*/
double Go::UCB(MCTSNode* node, double C) {
	double a = (double)node->WinTime / node->VisitTime;
	double b = (double)log(node->parent->VisitTime) / node->VisitTime;
	return a + C * sqrt(b);
}
/*---------------- [2]Expand 拓展 ----------------
在[node]节点上拓展一个新节点，
随机选择下一步Action，注意，需要保证新增的节点与其他节点Action不同。
**--------------------------------*/
MCTSNode* Go::Expand(MCTSNode* node) {
	/*-------- New State --------*/
	State* state = new State;
	state->player = -node->state->player;
	state->map = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);
	memcpy(state->map, node->state->map, sizeof(CHESS) * MapSize * MapSize);
	/*-------- 随机选择动作 --------*/
	for (int i = 0; i < node->childCur; i++)
		state->map[node->child[i]->state->y * MapSize + node->child[i]->state->x] = 2;
	nextStateRand(state->map, state->x, state->y, state->player);
	memcpy(state->map, node->state->map, sizeof(CHESS) * MapSize * MapSize);
	state->map[state->y * MapSize + state->x] = state->player;
	//showMap(state->map);
	/*-------- New Node --------*/
	MCTSNode* newnode = new MCTSNode;
	newnode->state = state;
	newnode->parent = node;
	node->child[node->childCur++] = newnode;
	return newnode;
}
/*---------------- [4]Backpropagation 回溯 ----------------
将[3]模拟的得到的[Reward]奖惩值，
从叶[node]往根回溯，更新每个相关的节点。
**--------------------------------*/
void Go::Backpropagation(MCTSNode* node, int reward)
{
	while (node != NULL) {
		node->VisitTime++;
		if (node->state->player == reward)node->WinTime++;
		else node->WinTime--;
		node = node->parent;
	}
}
/*---------------- TreePolicy ----------------
  蒙特卡罗树搜索的[1.Selection],[2.Expansion]阶段，
  输入当前开始搜索的节点[node]，
  Selection返回最好的需要expend的节点，注意如果节点是叶子结点直接返回。
  基本策略是先找当前未选择过的子节点，
  如果有多个则随机选。
  如果都选择过就找权衡过exploration/exploitation的UCB值最大的，
  如果UCB值相等则随机选。
**--------------------------------*/
MCTSNode* Go::TreePolicy(MCTSNode* node) {
	while (!judgeWin(node->state->map)) {
		if (node->isFullChild())
			node = Select(node, true);
		else return Expand(node);
	}
	return node;
}
/*---------------- [3]Simulation 模拟----------------
对[node]节点的状态[state]，进行模拟，直到胜利结束。
返回该状态模拟的[Reward]奖惩值。
**--------------------------------*/
int Go::Simulation(State* state0) {
	State state;
	memcpy(&state, state0, sizeof(State));
	state.map = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);
	memcpy(state.map, state0->map, sizeof(CHESS) * MapSize * MapSize);
	int reward = judgeWin(state.map);
	while (reward == 0) {
		state.player = -state.player;
		if(nextStateRand(state.map, state.x, state.y, state.player))	//随机选择下一动作
			downChess(state.map, state.x, state.y, state.player);	//落子
		reward = judgeWin(state.map);
	}
	return reward;
}
/*---------------- nextStateRand ----------------
基于输入[map]棋盘，在可能的动作下，随机选择一个动作
*	* [a,b)随机整数: (rand() % (b-a))+ a;
**--------------------------------*/
bool Go::nextStateRand(CHESS* map, INT8S& x0, INT8S& y0, CHESS chess) {
	CHESS mapTemp[MapSize * MapSize];
	memcpy(mapTemp, map, sizeof(CHESS) * MapSize * MapSize);

	/*-------- 棋块数气 --------*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(mapTemp, qi, chBlockMap);

	/*-------- 禁入点标记 --------*/	//无气杀我非杀他，为禁入
	static const INT8S x_step[] = { 1,-1,0,0 }, y_step[] = { 0,0,1,-1 };
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (mapTemp[y * MapSize + x] == 0) {
				bool flagMe = 1, flagArmy = 0;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize)continue;	//过界
					if (mapTemp[yt * MapSize + xt] == 0 || (mapTemp[yt * MapSize + xt] == chess && qi[chBlockMap[yt * MapSize + xt]] != 1)) {
						flagMe = 0; break;										//该子有气，非禁入
					}
					if (qi[chBlockMap[yt * MapSize + xt]] == 1) {				//存在一气棋块
						if (mapTemp[yt * MapSize + xt] == chess)flagMe = 1;		//杀己，禁入
						else flagArmy = 1;										//杀他，皆不禁入
					}
				}
				if (flagMe && !flagArmy) {
					mapTemp[y * MapSize + x] = NOTPOINT;
				}
			}
		}
	}

	/*-------- 己方眼点标记 --------*/
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (mapTemp[y * MapSize + x] == 0) {
				bool flag = 1;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize) continue;			//过界
					else if (mapTemp[yt * MapSize + xt] != chess) { flag = 0; break; }				//无子点	//不是己方
					else if (qi[chBlockMap[yt * MapSize + xt]] == 1) { flag = 0; break; }		//存在1气棋块，则该点要么让其死，要么让其被连接
				}
				if (flag)mapTemp[y * MapSize + x] = NOTPOINT + chess;
			}
		}
	}
	//printf("nextStateRand:%d\n",chess);
	//showMap(mapTemp);
	/*-------- 随机走子 --------*/
	int xMem[MapSize * MapSize] = { 0 };
	int yMem[MapSize * MapSize] = { 0 };
	int cur = 0;
	for (int z = 0; z < MapSize * MapSize; z++) {
		if (mapTemp[z] == 0) {
			xMem[cur] = z % MapSize;
			yMem[cur] = z / MapSize;
			cur++;
		}
	}
	if (cur == 0)return false;
	int curRand = rand() % cur;
	x0 = xMem[curRand];	y0 = yMem[curRand];
	return true;
}
/******************************************************************************
*                   围棋规则函数
******************************************************************************/
/*---------------- [1]:落子提子 ----------------
[RULE 1]:无气提子
**--------------------------------*/
bool Go::downChess(CHESS* map, INT8S x0, INT8S y0, CHESS chess) {
	map[y0 * MapSize + x0] = chess;
	/*-------- Computer Qi 棋块数气 --------*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);
	/*-------- 无气提子 --------*/
	for (int x = 0; x < MapSize * MapSize; x++)
		if (qi[chBlockMap[x]] == 0 && chBlockMap[x] != chBlockMap[y0 * MapSize + x0])map[x] = 0;
	return true;
}
/*---------------- [2]:禁入点标记 ----------------
*	[RULE 2]:非提禁入
*	无气杀我非杀他，为禁入
**--------------------------------*/
void Go::judgeNotPoint(CHESS* map, CHESS chess) {
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);

	static const INT8S x_step[] = { 1,-1,0,0 }, y_step[] = { 0,0,1,-1 };
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0) {
				bool flagMe = 1, flagArmy = 0;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize)continue;	//过界
					if (map[yt * MapSize + xt] == 0 || (map[yt * MapSize + xt] == chess && qi[chBlockMap[yt * MapSize + xt]] != 1)) {
						flagMe = 0; break;										//该子有气，非禁入
					}
					if (qi[chBlockMap[yt * MapSize + xt]] == 1) {				//存在一气棋块
						if (map[yt * MapSize + xt] == chess)flagMe = 1;			//杀己，禁入
						else flagArmy = 1;										//杀他，皆不禁入
					}
				}
				if (flagMe && !flagArmy) {
					map[y * MapSize + x] = NOTPOINT;
				}
			}
		}
	}
}
/*---------------- [4]:输赢判定(数子法) ----------------
*	[RULE 4]:局势判定(数子法)
*	无气杀我非杀他，为禁入
**--------------------------------*/
int Go::judgeWin(CHESS* map) {										//[RULE 4]:局势判定(数子法)
	/*-------- 棋块数气 --------*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);
	/*-------- 标记眼点 --------*/
	CHESS mapTemp[MapSize * MapSize] = { 0 };
	memcpy(mapTemp, map, sizeof(CHESS) * MapSize * MapSize);
	static const INT8S x_step[] = { 1,-1,0,0 }, y_step[] = { 0,0,1,-1 };
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0) {
				bool flag = 1; CHESS who = 0;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize) continue;			//过界
					else if (map[yt * MapSize + xt] == 0) { flag = 0; break; }					//无子点
					else if (map[yt * MapSize + xt] * who < 0) { flag = 0; break; }				//上下左右四点不一同一色
					else if (qi[chBlockMap[yt * MapSize + xt]] == 1) { flag = 0; break; }		//存在1气棋块，则该点要么让其死，要么让其被连接
					who += map[yt * MapSize + xt];
				}
				if (flag)mapTemp[y * MapSize + x] = NOTPOINT + (who > 0 ? 1 : -1);
			}
		}
	}//printf("judgeWin:\n");
	//showMap(mapTemp);
	/*-------- 是否存在可下子点 --------*//*-------- 已经终局，数子法 --------*/
	INT8U ScoreBlack = 0, ScoreWhite = 0;
	for (int x = 0; x < MapSize * MapSize; x++) {
		if (mapTemp[x] == 0)return 0;
		else if (mapTemp[x] == Black)ScoreBlack++;
		else if (mapTemp[x] == White)ScoreWhite++;
		else if (mapTemp[x] == NOTPOINT + White)ScoreBlack++;
		else if (mapTemp[x] == NOTPOINT + White)ScoreWhite++;
	}
	return ScoreBlack > ScoreWhite ? 1 : -1;
}
/*---------------- Computer Qi 棋块数气 ----------------
**--------------------------------*/
void Go::ComputerQi(CHESS* map, INT8U qi[], INT8U chBlockMap[]) {
	INT8S chBlockCur = 1;
	/*-------- 棋块标记 --------*/
	INT8S HeadChessFlag[MapSize * MapSize] = { 0 };			//并查集头节点标记
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0)continue;
			bool flag = 1;
			/*-------- CHESS UP --------*/
			if (y > 0 && map[(y - 1) * MapSize + x] == map[y * MapSize + x]) {
				chBlockMap[y * MapSize + x] = chBlockMap[(y - 1) * MapSize + x];
				flag = 0;
			}
			/*-------- CHESS LEFT --------*/
			if (x > 0 && map[y * MapSize + x - 1] == map[y * MapSize + x]) {
				flag = 0;
				if (chBlockMap[y * MapSize + x] == 0)
					chBlockMap[y * MapSize + x] = chBlockMap[y * MapSize + x - 1];
				else if (chBlockMap[y * MapSize + x] != chBlockMap[y * MapSize + x - 1]) {
					/*-------- 并查集 更新 --------*/
					int HeadChess = HeadChessFlag[chBlockMap[y * MapSize + x - 1]];
					while (HeadChessFlag[HeadChess] != HeadChess)HeadChess = HeadChessFlag[HeadChess];	//检索头节点
					HeadChessFlag[chBlockMap[y * MapSize + x]] = HeadChess;
				}
			}
			/*-------- No CHESS like --------*/
			if (flag) {
				HeadChessFlag[chBlockCur] = chBlockCur;
				chBlockMap[y * MapSize + x] = chBlockCur++;
			}
		}
	}
	/*-------- 完成棋块标记 --------*/
	for (int x = 0; x < MapSize * MapSize; x++) {
		INT8S HeadChess = HeadChessFlag[chBlockMap[x]];
		while (HeadChessFlag[HeadChess] != HeadChess)HeadChess = HeadChessFlag[HeadChess];	//检索头节点
		chBlockMap[x] = HeadChess;
	}
	/*-------- Computer Qi 棋块数气 --------*/
	static const INT8S x_step[] = { 1,-1,0,0 }, y_step[] = { 0,0,1,-1 };
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			bool qiflag[MapSize * MapSize] = { 0 };						//#需更改
			if (map[y * MapSize + x] == 0) {
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize
						|| map[yt * MapSize + xt] == 0
						|| qiflag[chBlockMap[yt * MapSize + xt]] == 1)continue;
					qi[chBlockMap[yt * MapSize + xt]]++;
					qiflag[chBlockMap[yt * MapSize + xt]] = 1;
				}
			}
		}
	}
}
/*---------------- GoAI ----------------
**--------------------------------*/
void Go::GoAI(CHESS* map, INT8S player, INT8S& x0, INT8S& y0) {
	/*-------- root --------*/
	MCTSNode root;
	root.state = new State;
	root.state->player = -player;
	root.state->map = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);
	memcpy(root.state->map, map, sizeof(CHESS) * MapSize * MapSize);
	/*-------- MCTS --------*/
	MCTSNode* ans = MCTS(&root);
	x0 = ans->state->x;
	y0 = ans->state->y;
	setMap(x0, y0, -1);
	showTree(&root);
}
/******************************************************************************
*                   调试函数
******************************************************************************/
void Go::showMap(CHESS* map)
{
	printf("\n");
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 1)printf("@ ");
			else if (map[y * MapSize + x] == -1)printf("o ");
			else if (map[y * MapSize + x] == 0)printf("+ ");
			else printf("x ");
		}printf("\n");
	}
}
void Go::showTree(MCTSNode* root)
{
	for (int i = 0; i < root->childCur; i++) {
		MCTSNode* t = root->child[i];
		printf("(%c,%c) ", t->state->x + 'A', t->state->y + 'A');
		printf("vt:%d,wt:%d,Q:%f", t->VisitTime, t->WinTime, 1.0 * t->WinTime / t->VisitTime);
		printf("\n");
	}
}
