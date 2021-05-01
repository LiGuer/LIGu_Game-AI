#ifndef MONTECARLO_SEARCH_TREE
#define MONTECARLO_SEARCH_TREE
#include <vector>
/******************************************************************************
*					Montecarlo Search Tree 蒙特卡洛树搜索
*	[算法]:
		[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
*	[注]:
		生成新[State]状态的地方只有两个, Expand 和 Simulation
		生成新[State]，必由上一State的复制
		[State]的内存管理，只能由其构造,析构函数完成
******************************************************************************/
template<class State>
class MontecarloSearchTree {
public:
	/*--------------------------------[ Montecarlo 树节点 ]--------------------------------*/
	struct TreeNode {
		int   winTime = 0, 
			visitTime = 0;
		State*    state  = NULL;
		TreeNode* parent = NULL;
		std::vector<TreeNode*> child;
		// 析构函数
		~TreeNode() {
			delete state;
			for (int i = 0; i < child.size(); i++)
				delete child[i];
		}
	};
	/*--------------------------------[ 核心数据 ]--------------------------------*/
	const int maxSearchTimes = 100000;
	State* (*newStateRandFunc)	(State*);					//生成新状态
	State* (*judgeWin)	        (State*);					//判断输赢
	/*--------------------------------[ 构造函数 ]--------------------------------*/
	MontecarloSearchTree() { ; }
	/*----------------------------------------------------------------------------
				Montecarlo Search Tree
	*The process of MCTS is split up into four steps:
	*	[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
	*---------------------------------------------------------------------------*/
	TreeNode* run(State* state) {
		TreeNode root; 
		root.state = state;
		for (int i = 0; i < maxSearchTimes; i++) {
			TreeNode* expandNode = TreePolicy(root);		//[1][2]
			if (expandNode == NULL) break;
			Backpropagation(								//[4]
				expandNode, 
				Simulation(expandNode->state)				//[3]
			);
		}
		return Select(root, false);							//Ans
	}
	/*------------------------------------------------------------------------------
						TreePolicy
		[1.Selection],[2.Expansion]阶段，
		输入当前开始搜索的节点[node]，
		Selection返回最好的需要expend的节点，注意如果节点是叶子结点直接返回。
		基本策略是先找当前未选择过的子节点，如果有多个则随机选。
		如果都选择过就找权衡过exploration/exploitation的UCB值最大的，如果UCB值相等则随机选。
	**----------------------------------------------------------------------------*/
	TreeNode* TreePolicy(TreeNode* node) {
		TreeNode* newNode = NULL;
		while (node != NULL && !judgeWin(node->state)) {
			if (node->isFullChildFlag) {
				node = Select(node, true); continue;
			}
			if (Expand(node, &newNode))return newNode;		//尝试扩展子节点，若不行，则该点全满
			node->isFullChildFlag = true;
			node = Select(node, true);
		}
		return node;
	}
	/*--------------------------------[ [1]Select 选择 ]--------------------------------
	*	[输入]: [1] 树节点	[2] 探索开关
	*	[输出]: [1] 树节点下最优子节点
		以[UCB公式]为判别标准，
		选择[node]节点下，最优子节点作为输出。
		若isExploration关闭，则为Exploitation-Only仅利用模式，只按下目前最优的摇臂。
		若isExploration开启，有探索尝试的机会
	**----------------------------------------------------------------------------*/
	TreeNode* Select(TreeNode* node, bool isExplore) {
		double UcbConst = isExplore == true ? sqrt(2) : 0,
			   maxScore = -DBL_MAX;
		TreeNode* bestNode = NULL;
		for (int i = 0; i < node->child.size(); i++) {
			double t = UpperConfidenceBound(node->child[i], UcbConst);
			if (maxScore < t) {
				maxScore = t;
				bestNode = node->child[i];
			}
		}
		return bestNode;
	}
	/*--------------------------------[ UCB ]--------------------------------
	*Upper Confidence Bound (UCB)上置信界算法:
	*                              -----------------------
	*             W(node)         /   In( N(parentNode) )
	*UCT(node) = --------- + c * / ----------------------
	*             N(node)       √         N(node)
	**-----------------------------------------------------------------------*/
	double UpperConfidenceBound(TreeNode* node, double C) {
		double a = (double)	   node->winTime			/ node->visitTime,
			   b = (double)log(node->parent->visitTime) / node->visitTime;
		return a + C * sqrt(b);
	}
	/*--------------------------------[ [2]Expand 拓展 ]--------------------------------
	*	在[node]节点上拓展一个新节点，随机选择下一步Action
	*	[注]: 需保证新增节点与其他节点Action不同。
	**----------------------------------------------------------------------------*/
	bool Expand(TreeNode* node, TreeNode*& newNode, bool(*nextStateRand)(TreeNode*, State*)) {
		/*---- New State ----*/
		State* newState;
		if ((newState = nextStateRand(node, state)) != NULL) return false;
		/*---- New Node ----*/
		newNode = new TreeNode;
		newNode->state  = newState;
		newNode->parent = node;
		node   ->child.push_back(*newNode);
		return true;
	}
	/*--------------------------------[ [3]Simulation 模拟 ]--------------------------------
	对[node]节点的状态[state]，进行模拟，直到胜利结束。
	返回该状态模拟的[Reward]奖惩值。
	**----------------------------------------------------------------------------*/
	int Simulation(State* state0) {
		State* newState;
		/*---- 开始模拟 ----*/
		int reward = judgeWin(&state);
		while ((reward = judgeWin(newState)) == 0) {
			if ((newState = newStateRandFunc(&state)) != NULL) break;		//随机选择下一动作
		}
		return reward;
	}
	/*--------------------------------[ [4]Backpropagation 回溯 ]--------------------------------
	将[3]模拟的得到的[Reward]奖惩值，
	从叶[node]往根回溯，更新每个相关的节点。
	**----------------------------------------------------------------------------*/
	void Backpropagation(TreeNode* node, int reward) {
		while (node != NULL) {
			node->visitTime++;
			node->  winTime += node->state->player == reward ? 1 : -1;
			node = node->parent;
		}
	}
};
#endif