/*
Copyright 2020,2021 LiGuer. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef MONTECARLO_TREE_SEARCH
#define MONTECARLO_TREE_SEARCH
#include <vector>
/******************************************************************************
*					Montecarlo Tree Search 蒙特卡洛树搜索
*	[算法]:
		[1] selection,
		[2] expansion,
		[3] simulation,
		[4] backpropagation.
******************************************************************************/
template<class State>
class MonteCarloTreeSearch {
public:
	/*--------------------------------------------------------------------------------
						Montecarlo 树节点
	*	[数据]: [1]状态		[2]胜利次数		[3]失败次数
				[4]父节点	[5]子节点列表	[6]子节点已满标志
	*	[注]: root析构时，其连接的整棵树也会自动依次析构并释放内存.
	--------------------------------------------------------------------------------*/
	struct TreeNode {
		int rewardSum = 0, 
			visitTime = 0;
		State*    state  = NULL;
		TreeNode* parent = NULL;
		bool isChildFull = false;
		std::vector<TreeNode*> child;
		// 析构函数
		~TreeNode() {
			delete state;
			for (int i = 0; i < child.size(); i++)
				delete child[i];
		}
	};

	/*--------------------------------------------------------------------------------
						核心数据
	*	[数据]: [1]根节点		[2]最大搜索次数
				[3]随机生成新状态函数指针	[4]判断输赢函数指针
	--------------------------------------------------------------------------------*/
	TreeNode root;
	int  maxSearchTimes;
	bool(*newStateRandFunc)	(State&, State&, bool);			//随机生成新状态
	char(*judgeWin)	        (State&);						//判断输赢

	/*--------------------------------------------------------------------------------
						构造函数
	*	[输入]: 
			[1]随机生成新状态函数指针	(状态, 新状态, 是否模拟标志)
			[2]判断输赢函数指针			(状态)
	--------------------------------------------------------------------------------*/
	MonteCarloTreeSearch(
		bool(*_newStateRandFunc)(State&, State&, bool),
		char(*_judgeWin)		(State&),
		int _maxSearchTimes = 1E5
	) :
		newStateRandFunc(_newStateRandFunc),
		judgeWin		(_judgeWin),
		maxSearchTimes	(_maxSearchTimes)
	{ ; }

	/*--------------------------------------------------------------------------------
				Montecarlo Search Tree
	*[算法]:
		[1] 选择
		[2] 拓展
		[3] 模拟
		[4] 回溯
	--------------------------------------------------------------------------------*/
	State* run(State* state) {
		root.state = state;
		for (int i = 0; i < maxSearchTimes; i++) {
			TreeNode* expandNode = TreePolicy(&root);		//[1][2]
			if (expandNode == NULL) 
				break;

			Backpropagation(								//[4]
				expandNode, 
				Simulation(expandNode->state)				//[3]
			);
		}
		root.state = NULL;
		return Select(&root, false)->state;					//Ans
	}

	/*--------------------------------------------------------------------------------
						TreePolicy
	*	[1.选择],[2.拓展]
	*	[输入]: 根节点
		[输出]: 拓展的新子节点
	*	[流程]:
			[1] 尝试[拓展]该节点的子节点, 并返回子节点
			[2] 若失败, 则标记该节点已满,[选择]该节点的一个子节点作为运算对象, 回到[1]
	--------------------------------------------------------------------------------*/
	TreeNode* TreePolicy(TreeNode* node) {
		TreeNode* newNode = NULL;
		while ( node != NULL && !judgeWin(*node->state)) {
			if (node->isChildFull) {
				node = Select(node, true); 
				continue;
			}
			if (Expand(node, newNode))
				return newNode;
			else {
				node->isChildFull = true;
				node = Select(node, true);
			}
		}
		return node;
	}

	/*--------------------------------[ [1]Select 选择 ]--------------------------------
	*	[输入]: [1] 节点	[2] 是否探索
		[输出]: [1] 最优子节点
	*	[算法]: UCB公式
			分为[探索]和[不探索]模式.
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

	/*-------------------------[ Upper Confidence Bound ]-----------------------------
	*Upper Confidence Bound 上置信界算法:
	*                              ---------------------
	*             W(node)         / In( N(parentNode) )
	*UCB(node) = --------- + C * / --------------------
	*             N(node)      √         N(node)
	**-----------------------------------------------------------------------*/
	double UpperConfidenceBound(TreeNode* node, double C) {
		double a = (double)	   node->rewardSum			/ node->visitTime,
			   b = (double)log(node->parent->visitTime) / node->visitTime;
		return a + C * sqrt(b);
	}

	/*--------------------------------[ [2]Expand 拓展 ]--------------------------------
	*	[算法]: 
			基于输入节点, 随机选择下一步[动作], 作为一个新子节点并返回.
	*	[注]: 需保证新增节点与其他节点[动作]不同.
	**----------------------------------------------------------------------------*/
	bool Expand(TreeNode* node, TreeNode*& newNode) {
		//New State
		State* newState = new State;
		if (!newStateRandFunc(*(node->state), *newState, false)) {
			delete newState; 
			return false;
		}
		//New Node
		newNode = new TreeNode;
		newNode->state  = newState;
		newNode->parent = node;
		node   ->child.push_back(newNode);
		return true;
	}

	/*--------------------------------[ [3]Simulation 模拟 ]--------------------------------
	*	[算法]: 对输入节点的[状态], 进行[模拟]对战, 直到胜利为止.
	*	[输出]: 该状态模拟的[奖惩]值.
	**----------------------------------------------------------------------------*/
	int Simulation(State* state) {
		static State* newState = new State;
		*newState = *state;
		int reward = 0;
		while ((reward = judgeWin(*newState)) == 0)
			newStateRandFunc(*newState, *newState, true);
		return reward;
	}

	/*--------------------------------[ [4]Backpropagation 回溯 ]--------------------------------
	*	[算法]: 将[3]模拟的[奖惩]值, 从叶节点往根回溯, 更新每个相关节点.
	**----------------------------------------------------------------------------*/
	void Backpropagation(TreeNode* node, int reward) {
		while (node != NULL) {
			node->visitTime++;
			node->rewardSum += node->state->player == reward ? 1 : -1;
			node = node->parent;
		}
	}

};
#endif