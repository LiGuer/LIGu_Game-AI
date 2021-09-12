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
#include "Mat.h"
#include <float.h>
#include <algorithm>
#include <vector>
/******************************************************************************
*								QLearning
*	[定义]:Q(s,a) = (1 + lr)·Q(s,a) + lr·( R + g·max Q(s',:) )
			s: state	a: action	R: reward	lr: learning rate	g: forget factor
*	[原理]:
		选择动作: ε-greedy方法:
			每个状态以ε概率随机选取动作，1-ε概率选择当前最优解
		眼前利益R: 反馈值.
		记忆中的利益 max Q(s',:): 记忆里，新位置s'能给出的最大效用值.
		forget factor越大，越重视以往经验，越小，则只重视眼前利益R.
*	[流程]:
		[1] Init Q table arbitrarily
		[2] Repeat (for each episode), until s is terminal
			[3] Choose a from s using policy derived from Q (eg. ε-greedy)
			[4] Take action a, observe r s'
			[5] Q(s,a) = (1 + lr)·Q(s,a) + lr·( R + g·max Q(s',:) )
				s = s'
*	[Ps]:
		可以逐渐降低随机选取动作的概率ε，一开始随机率可达100%
			然后随训练次数的深入，应当逐渐降低随机概率。
*******************************************************************************/
class QLearning {
public:
	/*---------------- 核心数据 ----------------*/
	Mat<> QTable;
	double learnRate = 0.6,
		forgetRate = 0.8,
		greedy = 0.9;
	/*---------------- 初始化 ----------------*/
	QLearning() { ; }
	QLearning(int _stateNum, int _actionNum) { init(_stateNum, _actionNum); }
	void init(int _stateNum, int _actionNum) { QTable.zero(_stateNum, _actionNum); }
	/*---------------- 选择行为 ----------------
	*	[1] 探索模式(随机)	[2] 利用模式(最优)
	-------------------------------------------*/
	int operator()(int state) { return forward(state); }
	int forward(int state) {
		int action = 0;
		double maxQ = -DBL_MAX,
			minQ = DBL_MAX;
		for (int i = 0; i < QTable.cols; i++) {
			maxQ = QTable(state, i) > maxQ ? action = i, QTable(state, i) : maxQ;
			minQ = QTable(state, i) < minQ ? QTable(state, i) : minQ;
		}
		//[1]
		if ((minQ == 0 && maxQ == 0) || rand() / double(RAND_MAX) < greedy)
			return rand() % QTable.cols;
		//[2]
		return action;
	}
	/*---------------- 反馈学习 ----------------*/
	void backward(int state, int action, double reward, int& preState) {
		double maxQ = -DBL_MAX;
		for (int i = 0; i < QTable.cols; i++)
			maxQ = QTable(state, i) > maxQ ? QTable(state, i) : maxQ;
		QTable(preState, action) = (1 - learnRate) * QTable(preState, action) + learnRate * (reward + forgetRate * maxQ);
		preState = state;
	}
};
/******************************************************************************
*                    Deep Q Network
*	[思想]: Q-Learning + Neural Network
		[Loss Function]: 近似值和真实值的均方差
*	[流程]:
	[1] Initialize replay memory D to capacity N
		Initialize action-value function Q with random weights θ
		Initialize target action-value function Q with weights θ- = θ
	[2] for episode 1,M do
		[] for t= 1,T do
			[] Initialize sequence 81 = {x1 } and preprocessed sequenceΦ1 =Φ(81)
			[] With probability ε select a random action at
			[] otherwise select at = arg maxa Q(Φ(8t),a;0)
			[] Execute action at in the emulator and observe reward rt and image rt+1
			[] Setst+1 = 8t,at,2t+1 and preprocess中+1 =Φ(8+1)
			[] Store experience (φ,at,rt,φ++1)in D
			[] Sample random minibatch of experiences (Φ, aj,rj,Φj+1) from D
			[] Setyj=	{ rj				if episode terminates at stepj+1
					(rj + r maxa' Q(φj+1,a';0-) otherwise
			[] Perform a gradient descent step on (9j - Q(Φj, aj; 0))2 with respect to the weights θ
			[] Every C steps reset Q=Q
******************************************************************************/
class DeepQNetwork {
public:
	double learnRate = 0.6,
		forgetRate = 0.8,
		greedy = 0.9;
	int actionNum = 0,
		stateNum = 0;
	/*---------------- 选择行为 ----------------*/
	int operator()(int state) { return forward(state); }
	int forward(int state) {
		int action = 0;
		if (rand() / double(RAND_MAX) < greedy)
			return action = rand() % actionNum;
		Mat<> ans;// = evalNet(state);
		ans.max(action);
		return action;
	}
	/*---------------- 反馈学习 ----------------*/
	void backward(int state, int action, double reward, int& preState) {
	}
};
/******************************************************************************
*                    Support Vector Machines 支持向量机
*	[定义]: 特征空间上, [间隔最大]的线性分类器.
		[超平面]: y = w x + b
		[间隔]: 样本点, 到超平面的距离.
				γi = 1/||w||·|w x + b|	(点到面距离公式)
		[样本点]: {(xi,yi)} i=1toN    xi∈R_n 实向量    yi∈{-1,1}
*	[目标]: 找到目标超平面, 使得[所有样本点间隔最小值γmin = min γi]最大.
			max_wb( min_i 1/||w||·|w xi + b| )
			st.  w x + b > 0 , yi = +1  and  w x + b < 0 , yi = -1  (分类)
		[推导]:
		=>	|w xi + b| = yi(w xi + b)		//去绝对值
		=>	max_wb( 1/||w||·min_i yi(w xi + b) )    st. yi(w x + b) > 0
		转化为:
			min_wb 1/2·||w||²    st. yi (W x + b) ≥ 1
			凸二次规划问题, 用拉格朗日乘子法, 得其对偶问题.
*	[结论]:
		min 1/2·Σi Σj αi αj yi yj K(xi,xj) - Σi αi	//线性时 K(xi,xj)即内积
		st. Σi αi yi = 0    0≤αi≤C
*	[Kernal Trick]: 升到高维, 实现非线性分类
		[Classical Kernal Function]:
		* 高斯核函数: K(x,z) = exp( -||x - z||² / 2σ² )
*	[二次规划优化问题]:
		[算法]: Sequential Minimal Optimization 算法
		[思路]: 若所有变量解都满足此最优化问题的KKT条件，则得到最优化问题解
		[Karush Kuhn Tucker条件]: 非线性规划最佳解的必要条件
*	[流程]:
		[1] 选择惩罚参数 C > 0, 构造并求解凸二次规划问题, 得到最优解α
		[2] w = Σi αi yi xi    b = yj - Σαi yi (xi·xj)
		[3] 得到分离超平面 w x + b = 0
			分类决策函数: f(x) = sign( w x + b )
******************************************************************************/
//void SupportVectorMachines(Mat<> X, Mat<int> Y) {

//};