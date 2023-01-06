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
#ifndef MINIMAX_H
#define MINIMAX_H
#include <vector>
template<class State>
class MiniMax {
public:
	int MaxLevel = 5;
	int  (*Evaluate)		(State&);					//评价函数
	bool (*newStateFunc)	(State&, State&);			//生成新状态
	char (*judgeWin)		(State&);					//判断输赢
	void (*endingState)		(State&);					//状态收尾处理
	std::vector<int> ansScoreSet;
	State maxScoreState;
	/*----------------[ 构造/析构函数 ]----------------*/
	MiniMax(
		int (*_Evaluate)	(State&), 
		bool(*_newStateFunc)(State&, State&), 
		char(*_judgeWin)	(State&),
		void(*_endingState) (State&),
		int _MaxLevel = 5
	) {
		Evaluate     = _Evaluate;
		newStateFunc = _newStateFunc;
		judgeWin	 = _judgeWin;
		endingState  = _endingState;
		MaxLevel	 = _MaxLevel;
	}
	/******************************************************************************
	*                    决策函数  博弈树
	*	[流程]:
			[0] 到达层数阈值, 返回当前状态的评价(评价函数)
			[1] 基于当前状态, 生成新状态, 对于每一个可能的新状态
				[2] 递归决策函数, 深度优先搜索生成&遍历博弈树
				[3] 收尾
					[3.1] 单数博弈层(对手层) 取最小
					[3.2] 双数博弈层(我  层) 取最大
					[3.3] alpha-beta剪枝
	******************************************************************************/
	int Policy(char level, State& state, int alpha, int beta) {
		//[0]
		if (level > MaxLevel || judgeWin(state) != 0)
			return Evaluate(state);
		int max = -0x7fffffff, 
			min =  0x7fffffff;
		//[1]
		State newState;
		while (newStateFunc(state, newState)) {
			//[2]
			int score = Policy(level + 1, newState, alpha, beta);
			//[3]
			if (level == 0) {
				ansScoreSet.push_back(score);
				if (score > max) maxScoreState = newState;
			}
			if (level % 2 == 0)
				 max = max >= score ? max : score, alpha = alpha >= score ? alpha : score;
			else min = min <= score ? min : score,  beta =  beta <= score ?  beta : score;
			if (alpha >= beta) break;
		}
		endingState(newState);
		return level % 2 == 0 ? max : min;
	}
};
#endif