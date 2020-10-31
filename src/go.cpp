#include "Go.h"
/******************************************************************************
*                    基础函数
******************************************************************************/
/*--------------------------------[ 初始化 ]--------------------------------*/
void Go::init() {
	Map = (STONE*)malloc(sizeof(STONE) * MapSize * MapSize);
	clearMap();
	srand((unsigned int)time(NULL));
}
/*--------------------------------[ 清空棋盘 ]--------------------------------*/
void Go::clearMap() {
	memset(Map, 0, sizeof(STONE) * MapSize * MapSize);
}
/*--------------------------------[ 下子 ]--------------------------------*/
bool Go::setMap(INT8S x, INT8S y, STONE stone) {
	if (!judgeOutMap(x, y))return false;							//过界
	if (Map[y * MapSize + x] != 0)return false;						//非空

	STONE maptemp[MapSize * MapSize];
	memcpy(maptemp, Map, sizeof(STONE) * MapSize * MapSize);
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(maptemp, qi, chBlockMap);
	judgeNotPoint(maptemp, stone, qi, chBlockMap);

	if (maptemp[y * MapSize + x] == NOTPOINT)return false;			//禁入点
	return downStone(Map, x, y, stone);								//落子//无气提子
}
/*--------------------------------[ 判断是否过界 ]--------------------------------*/
bool Go::judgeOutMap(int x, int y) {
	return (x >= 0 && x < MapSize && y >= 0 && y < MapSize) ? true : false;
}
/*--------------------------------[ GoAI ]--------------------------------
**-----------------------------------------------------------------------*/
void Go::GoAI(STONE* map, INT8S player, INT8S& x0, INT8S& y0) {
	/*---- root ----*/
	MCTSNode root;
	root.state = new State;
	root.state->player = -player;
	root.state->map = (STONE*)malloc(sizeof(STONE) * MapSize * MapSize);
	memcpy(root.state->map, map, sizeof(STONE) * MapSize * MapSize);
	/*---- MCTS ----*/
	MCTSNode* ans = MCTS(&root);
	x0 = ans->state->x;
	y0 = ans->state->y;
	setMap(x0, y0, -1);
	showTree(&root);
}
/******************************************************************************
*                   核心算法: 蒙特卡洛树搜索
*蒙特卡洛树搜索 MCTS:
*The process of MCTS is split up into four steps:
*[1]selection,[2]expansion,[3]simulation,[4]backpropagation.
*
******************************************************************************/
/*--------------------------------[ MCTS ]--------------------------------
*蒙特卡洛树搜索 MCTS:
*The process of MCTS is split up into four steps:
*[1]selection,[2]expansion,[3]simulation,[4]backpropagation.
**------------------------------------------------------------------------*/
MCTSNode* Go::MCTS(MCTSNode* node) {
	for (int i = 0; i < MctsBudget; i++) {
		MCTSNode* expandNode = TreePolicy(node);		//[1][2]
		int reward = Simulation(expandNode->state);		//[3]
		Backpropagation(expandNode, reward);			//[4]
	}
	return Select(node, false);							//Ans
}
/*--------------------------------[ TreePolicy ]--------------------------------
  蒙特卡罗树搜索的[1.Selection],[2.Expansion]阶段，
  输入当前开始搜索的节点[node]，
  Selection返回最好的需要expend的节点，注意如果节点是叶子结点直接返回。
  基本策略是先找当前未选择过的子节点，
  如果有多个则随机选。
  如果都选择过就找权衡过exploration/exploitation的UCB值最大的，
  如果UCB值相等则随机选。
**----------------------------------------------------------------------------*/
MCTSNode* Go::TreePolicy(MCTSNode* node) {
	MCTSNode* newnode = NULL;
	while (!judgeWin(node->state->map)) {
		if (node->isFullChildFlag) {
			node = Select(node, true); continue;
		}
		if (Expand(node, &newnode))return newnode;		//尝试扩展子节点，若不行，则该点全满
		node->isFullChildFlag = true;
		node = Select(node, true);
	}
	return node;
}
/*--------------------------------[ [1]Select 选择 ]--------------------------------
*	*输入: [1] 树节点	[2] 探索开关
*	*输出: [1] 树节点下最优子节点
以[UCB公式]为判别标准，
选择[node]节点下，最优子节点作为输出。
若isExploration关闭，则为Exploitation-Only仅利用模式，只按下目前最优的摇臂。
若isExploration开启，有探索尝试的机会。
**----------------------------------------------------------------------------*/
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
/*--------------------------------[ UCB ]--------------------------------
*Upper Confidence Bound (UCB)上置信界算法:
*                              -----------------------
*             W(node)         /   In( N(parentNode) )
*UCT(node) = --------- + c * / ----------------------
*             N(node)       √         N(node)
**-----------------------------------------------------------------------*/
double Go::UCB(MCTSNode* node, double C) {
	double a = (double)node->WinTime / node->VisitTime;
	double b = (double)log(node->parent->VisitTime) / node->VisitTime;
	return a + C * sqrt(b);
}
/*--------------------------------[ [2]Expand 拓展 ]--------------------------------
在[node]节点上拓展一个新节点，
随机选择下一步Action，注意，需要保证新增的节点与其他节点Action不同。
**----------------------------------------------------------------------------*/
bool Go::Expand(MCTSNode* node, MCTSNode** newnode) {
	/*---- New State ----*/
	State* state = new State;
	memcpy(state, node->state, sizeof(State));
	state->player = - node->state->player;
	state->map = (STONE*)malloc(sizeof(STONE) * MapSize * MapSize);
	memcpy(state->map, node->state->map, sizeof(STONE) * MapSize * MapSize);
	/*---- 随机选择动作 ----*/
	if (!nextStateRand(node, state)) {
		free(state);return false;
	}
	downStone(state->map, state->x, state->y, state->player);
	/*---- New Node ----*/
	*newnode = new MCTSNode;
	(*newnode)->state = state;
	(*newnode)->parent = node;
	node->child[node->childCur++] = (*newnode);
	return true;
}
/*--------------------------------[ [4]Backpropagation 回溯 ]--------------------------------
将[3]模拟的得到的[Reward]奖惩值，
从叶[node]往根回溯，更新每个相关的节点。
**----------------------------------------------------------------------------*/
void Go::Backpropagation(MCTSNode* node, int reward)
{
	while (node != NULL) {
		node->VisitTime++;
		if (node->state->player == reward)node->WinTime++;
		else node->WinTime--;
		node = node->parent;
	}
}
/*--------------------------------[ [3]Simulation 模拟 ]--------------------------------
对[node]节点的状态[state]，进行模拟，直到胜利结束。
返回该状态模拟的[Reward]奖惩值。
**----------------------------------------------------------------------------*/
int Go::Simulation(State* state0) {
	State state;
	memcpy(&state, state0, sizeof(State));
	state.map = (STONE*)malloc(sizeof(STONE) * MapSize * MapSize);
	memcpy(state.map, state0->map, sizeof(STONE) * MapSize * MapSize);
	/*---- 开始模拟 ----*/
	int reward = judgeWin(state.map);
	while (reward == 0) {
		state.player = -state.player;
		if(nextStateRand(&state))											//随机选择下一动作
			downStone(state.map, state.x, state.y, state.player);			//落子
		//showMap(state.map);
		//printf("%d,%d\n", state.Jie[0], state.Jie[1]);
		reward = judgeWin(state.map);
	}
	return reward;
}
/*--------------------------------[ nextStateRand ]--------------------------------
基于输入[map]棋盘，在可能的动作下，随机选择一个动作
*	* [a,b)随机整数: (rand() % (b-a))+ a;
**----------------------------------------------------------------------------*/
bool Go::nextStateRand(State* state) {
	STONE map[MapSize * MapSize];
	memcpy(map, state->map, sizeof(STONE) * MapSize * MapSize);
	/*---- 棋块数气 ----*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);
	judgeEye(map, qi, chBlockMap);//眼点标记
	/*---- 劫点标记 ----*/
	if (state->Jie[0] != -1) {
		map[state->Jie[1] * MapSize + state->Jie[0]] = NOTPOINT;
		state->Jie[0] = -1; state->Jie[1] = -1;
	}
	/*---- 随机走子 ----*/
	int xMem[MapSize * MapSize] = { 0 };
	int yMem[MapSize * MapSize] = { 0 };
	int cur = 0;
	for (int z = 0; z < MapSize * MapSize; z++) {
		if (map[z] == 0) {
			xMem[cur] = z % MapSize;
			yMem[cur] = z / MapSize;
			cur++;
		}
	}
	if (cur == 0)return false;
	int curRand = rand() % cur;
	state->x = xMem[curRand];	state->y = yMem[curRand];
	/*---- 劫判定 ----*/
	judgeJie(map, state->player, state->x, state->y, qi, chBlockMap, state->Jie);
	return true;
}
bool Go::nextStateRand(MCTSNode* node, State* state) {
	STONE map[MapSize * MapSize];
	memcpy(map, node->state->map, sizeof(STONE) * MapSize * MapSize);
	/*---- 棋块数气 ----*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);
	judgeEye(map, qi, chBlockMap);//眼点标记
	/*---- 劫点标记 ----*/
	if (state->Jie[0] != -1){
		map[state->Jie[1] * MapSize + state->Jie[0]] = NOTPOINT; 
		state->Jie[0] = -1; state->Jie[1] = -1;
	}
	/*---- 已经走过子 ----*/
	for (int i = 0; i < node->childCur; i++)
		map[node->child[i]->state->y * MapSize + node->child[i]->state->x] = 2;
	/*---- 随机走子 ----*/
	int xMem[MapSize * MapSize] = { 0 };
	int yMem[MapSize * MapSize] = { 0 };
	int cur = 0;
	for (int z = 0; z < MapSize * MapSize; z++) {
		if (map[z] == 0) {
			xMem[cur] = z % MapSize;
			yMem[cur] = z / MapSize;
			cur++;
		}
	}
	if (cur == 0)return false;
	int curRand = rand() % cur;
	state->x = xMem[curRand];	state->y = yMem[curRand];
	/*---- 劫判定 ----*/
	judgeJie(map, state->player, state->x, state->y, qi, chBlockMap, state->Jie);
	return true;
}
/******************************************************************************
*                   围棋规则函数
******************************************************************************/
/*--------------------------------[ [1]:落子提子 ]--------------------------------
[RULE 1]:无气提子
**-------------------------------------------------------------------------------*/
bool Go::downStone(STONE* map, INT8S x0, INT8S y0, STONE stone) {
	map[y0 * MapSize + x0] = stone;
	/*---- 棋块数气 ----*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	
	ComputerQi(map, qi, chBlockMap);
	/*---- 无气提子 ----*/
	for (int x = 0; x < MapSize * MapSize; x++)
		if (qi[chBlockMap[x]] == 0 && chBlockMap[x] != chBlockMap[y0 * MapSize + x0])map[x] = 0;
	return true;
}
/*--------------------------------[ 眼点标记 ]--------------------------------
*	*输入: [1] 棋盘map	[3] 气	[4] 棋块
*	*输出: [1] 被标记眼点的棋盘
*	眼点: 对方的禁入点
*	*判定: [1] 只一空点	[2] 上下左右同一色	[3]上下左右棋块，均非一气
**------------------------------------------------------------------------------*/
void Go::judgeEye(STONE* map, INT8U qi[], INT8U chBlockMap[]) {
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0) {
				bool flag = 1; STONE who = 0;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize) continue;			//过界
					/*---- 核心判断 ----*/
					who += map[yt * MapSize + xt];
					if(map[yt * MapSize + xt] * who <= 0 || qi[chBlockMap[yt * MapSize + xt]] == 1) { flag = 0; break; }
				}
				if (flag)map[y * MapSize + x] = NOTPOINT + (who > 0 ? 1 : -1);
			}
		}
	}
}
/*--------------------------------[ [2]:禁入点标记 ]--------------------------------
*	*输入: [1] 棋盘map	[2] 己方颜色	[3] 气	[4] 棋块
*	*输出: [1] 被标记己方禁入点的棋盘
*	[RULE 2]:非提禁入
*	禁入点: [1]我无气	[2]非杀他
**-------------------------------------------------------------------------------*/
void Go::judgeNotPoint(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]) {
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0) {
				bool flagMe = 1, flagArmy = 0;
				for (int i = 0; i < 4; i++) {
					INT8S xt = x + x_step[i], yt = y + y_step[i];
					if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize)continue;	//过界
					/*---- 核心判断 ----*/
					if (map[yt * MapSize + xt] == 0 || (map[yt * MapSize + xt] == stone && qi[chBlockMap[yt * MapSize + xt]] != 1)) {
						flagMe = 0; break;										//该子有气，非禁入
					}
					if (qi[chBlockMap[yt * MapSize + xt]] == 1) {				//存在一气棋块
						if (map[yt * MapSize + xt] == stone)flagMe = 1;			//杀己，禁入
						else flagArmy = 1;										//杀他，非禁入
					}
				}
				if (flagMe && !flagArmy)map[y * MapSize + x] = NOTPOINT;		//[1]我无气//[2]非杀他
			}
		}
	}
}
/*--------------------------------[ 劫判定 ]--------------------------------
*	*输入: [1] 棋盘map	[2] 己方颜色	[3]预期落子点	[4] 气	[5] 棋块
*	*输出: [1] 是否为劫	[2]对应劫点
*	劫: 只有一种模式: 黑白眼相交错。
		  x o @ x		x o @ x
		  o   o @		o @   @
		  x o @ x		x o @ x
*	*判定: [1] 四周全敌 [2] 有且只有一方，只一气，且只一点
**------------------------------------------------------------------------*/
bool Go::judgeJie(STONE* map, STONE stone, INT8S x0, INT8S y0, INT8U qi[], INT8U chBlockMap[], INT8S Jie0[]) {
	bool Jieflag = 1, JieAramy = 0;
	INT8S Jie[2] = { -1,-1 };
	for (int i = 0; i < 4; i++) {
		INT8S xt = x0 + x_step[i], yt = y0 + y_step[i];
		if (yt < 0 || xt < 0 || yt >= MapSize || xt >= MapSize) continue;			//过界
		/*---- 核心判断 ----*/
		if (map[yt * MapSize + xt] == 0 || map[yt * MapSize + xt] == stone) {		//四周非全敌，非劫
			Jieflag = 0; break;
		}
		if (qi[chBlockMap[yt * MapSize + xt]] == 1) { 		//是敌且只1气
			if (JieAramy == 1) { Jieflag = 0; break; }		//有两个1气敌，非劫
			JieAramy = 1;
			for (int j = 0; j < 4; j++) {					//1气敌只有自己
				INT8S xtt = xt + x_step[j], ytt = yt + y_step[j];
				if (ytt < 0 || xtt < 0 || ytt >= MapSize || xtt >= MapSize || (ytt == y0 && xtt == x0)) continue;			//过界
				if (map[ytt * MapSize + xtt] != stone) { Jieflag = 0; JieAramy = 0; break; }
			}
			if (JieAramy) { Jie[0] = xt; Jie[1] = yt; Jieflag = 1; }
		}
	}
	if (Jieflag) { Jie0[0] = Jie[0]; Jie0[1] = Jie[1]; return true; }
	return false;
}
/*--------------------------------[ [4]:输赢判定(数子法) ]--------------------------------
*	[RULE 4]:局势判定(数子法)
*	无气杀我非杀他，为禁入
**----------------------------------------------------------------------------------------*/
int Go::judgeWin(STONE* map) {										//[RULE 4]:局势判定(数子法)
	STONE mapTemp[MapSize * MapSize] = { 0 };
	memcpy(mapTemp, map, sizeof(STONE) * MapSize * MapSize);
	/*---- 棋块数气 ----*/
	INT8U chBlockMap[MapSize * MapSize] = { 0 }, qi[MapSize * MapSize] = { 0 };
	ComputerQi(map, qi, chBlockMap);
	/*---- 标记眼点 ----*/
	judgeEye(mapTemp, qi, chBlockMap);
	/*---- 是否存在可下子点 ----*//*---- 已经终局，数子法 ----*/
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
/*--------------------------------[ 棋块数气 ]--------------------------------
*	*输入: [1] 棋盘map
*	*输出: [1] 棋盘上棋块chBlockMap [2] 棋块对应的气
*	棋块: 上下左右连通的同一色棋的区域
*	气: 
**--------------------------------------------------------------------------*/
void Go::ComputerQi(STONE* map, INT8U qi[], INT8U chBlockMap[]) {
	INT8S chBlockCur = 1;
	/*---- 棋块标记 ----*/
	INT8S HeadStoneFlag[MapSize * MapSize] = { 0 };			//并查集头节点标记
	for (int y = 0; y < MapSize; y++) {
		for (int x = 0; x < MapSize; x++) {
			if (map[y * MapSize + x] == 0)continue;
			bool flag = 1;
			/*---- STONE UP ----*/
			if (y > 0 && map[(y - 1) * MapSize + x] == map[y * MapSize + x]) {
				chBlockMap[y * MapSize + x] = chBlockMap[(y - 1) * MapSize + x];
				flag = 0;
			}
			/*---- STONE LEFT ----*/
			if (x > 0 && map[y * MapSize + x - 1] == map[y * MapSize + x]) {
				flag = 0;
				if (chBlockMap[y * MapSize + x] == 0)
					chBlockMap[y * MapSize + x] = chBlockMap[y * MapSize + x - 1];
				else if (chBlockMap[y * MapSize + x] != chBlockMap[y * MapSize + x - 1]) {
					/*---- 并查集 更新 ----*/
					int HeadStoneMe = HeadStoneFlag[chBlockMap[y * MapSize + x]];
					while (HeadStoneFlag[HeadStoneMe] != HeadStoneMe)HeadStoneMe = HeadStoneFlag[HeadStoneMe];	//检索头节点
					int HeadStoneLeft = HeadStoneFlag[chBlockMap[y * MapSize + x - 1]];
					while (HeadStoneFlag[HeadStoneLeft] != HeadStoneLeft)HeadStoneLeft = HeadStoneFlag[HeadStoneLeft];	//检索头节点
					if(HeadStoneMe!= HeadStoneLeft)
						HeadStoneFlag[HeadStoneLeft] = HeadStoneMe;
				}
			}
			/*---- No STONE like ----*/
			if (flag) {
				HeadStoneFlag[chBlockCur] = chBlockCur;
				chBlockMap[y * MapSize + x] = chBlockCur++;
			}
		}
	}
	/*---- 完成棋块标记 ----*/
	for (int x = 0; x < MapSize * MapSize; x++) {
		INT8S HeadStone = HeadStoneFlag[chBlockMap[x]];
		while (HeadStoneFlag[HeadStone] != HeadStone)HeadStone = HeadStoneFlag[HeadStone];	//检索头节点
		chBlockMap[x] = HeadStone;
	}
	/*---- 棋块数气 ----*/
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
/******************************************************************************
*                   调试函数
******************************************************************************/
/*--------------------------------[ 显示棋盘 ]--------------------------------
**-------------------------------------------------------------------------*/
void Go::showMap(STONE* map)
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
/*--------------------------------[ 显示每点胜率 ]--------------------------------
**-------------------------------------------------------------------------------*/
void Go::showTree(MCTSNode* root)
{
	for (int i = 0; i < root->childCur; i++) {
		MCTSNode* t = root->child[i];
		printf("(%c,%c) ", t->state->x + 'A', t->state->y + 'A');
		printf("vt:%d,wt:%d,Q:%f", t->VisitTime, t->WinTime, 1.0 * t->WinTime / t->VisitTime);
		printf("\n");
	}
}
