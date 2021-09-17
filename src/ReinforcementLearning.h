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
#include "../../LiGu_AlgorithmLib/Mat.h"
#include <float.h>
#include <algorithm>
#include <vector>
/******************************************************************************
[QLearning]
	[定义]:Q(s,a) = (1 + lr)·Q(s,a) + lr·( R + g·max Q(s',:) )
			s: state	a: action	R: reward	lr: learning rate	g: forget factor
	[原理]:
		选择动作: ε-greedy方法:
			每个状态以ε概率随机选取动作，1-ε概率选择当前最优解
		眼前利益R: 反馈值.
		记忆中的利益 max Q(s',:): 记忆里，新位置s'能给出的最大效用值.
		forget factor越大，越重视以往经验，越小，则只重视眼前利益R.
	[步骤]:
		[1] Init Q table arbitrarily
		[2] Repeat (for each episode), until s is terminal
			[3] Choose a from s using policy derived from Q (eg. ε-greedy)
			[4] Take action a, observe r s'
			[5] Q(s,a) = (1 + lr)·Q(s,a) + lr·( R + g·max Q(s',:) )
				s = s'
	[注]: 可以逐渐降低随机选取动作的概率ε,一开始随机率可达100%,然后随训练次数增加,应逐渐降低随机概率.
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
[支持向量机]: Support Vector Machines 
	[目标]: 找到目标超平面, 使得所有样本点间隔最小值γmin = min γi最大.
	[优化问题]: 凸二次规划问题
			min		||w||² / 2
			s.t.	y_i (w^T x_i + b) ≥ 1
	[步骤]:
		(1) 计算λ*
		(2) 计算 w*, b*
	[原理]:
		(1) 超平面方程: w^T x + b = 0
			点面距: d = |w^T x + b| / ||w||
		(2) 分类
				w^T x_i + b ≥ +1		(y_i = +1)
				w^T x_i + b <= -1		(y_i = -1)
			=>	y_i (w^T x_i + b) ≥ 1
		(3) 间隔: 离超平面最近的2个异类样本点到超平面的距离之和.
				Δ = 2 / ||w||
		(4) 优化问题构造: 使间隔最大化. 凸二次规划问题
				max  2 / ||w||    <=>    min  ||w||² / 2
			=>	min		||w||² / 2
				s.t.	y_i (w^T x_i + b) ≥ 1
		(5) 计算最优点: 
			Lagrange函数	
				L(w, b,λ) = ||w||² / 2 + Σλ_i (1 - y_i (w^T x_i + b))
				L(w, b,λ)求导, 当导数为0时, 取得极值
				=>	w* = Σ λ_i y_i x_i		0 = Σ λ_i y_i
			Lagrange对偶	
				G(λ) = Lx(w*, b*, λ) = Σλ_i - 1/2 Σ_i Σ_j λ_i λ_j y_i y_j x_i^T x_j
			对偶问题: 二次规划问题	
				max		G(λ)
				s.t.	λ≥0		Σ λ_i y_i = 0
				=>	Sequential Minimal Optimization算法求解λ*
			KKT条件
				λ≥0    y_i (w^T x_i + b) - 1 ≥ 0    λ_i(y_i(w^T x_i + b) - 1) = 0
		(6) 核函数:将样本从原始空间映射到更高维特征空间,使得其线性可分.
			=>	超平面方程: w^T Φ(x) + b = 0
				min		||w||² / 2
				s.t.	y_i (w^T Φ(x_i) + b) ≥ 1
				G(λ) = Σλ_i - 1/2 Σ_i Σ_j λ_i λ_j y_i y_j Φ(x_i)^T Φ(x_j)
			=>	设 核函数к(x_i, x_j) = Φ(x_i)^T Φ(x_j)
				f(x) = w*^T x + b = Σ λ_i y_i к(x, x_i) + b
			*	к是核函数 <=> 核矩阵[a_ij = к(x_i, x_j)]总是半正定
		(7) Sequential Minimal Optimization算法
			∵	λ是n-1自由度, 确定前n-1个量, 则第n个由Σ λ_i y_i = 0自动确定.
			∴	每次选2个λ_i λ_j, 固定其他λ_k不变, 优化λ_i λ_j, 更新b
			优化λ_i λ_j:
			=>	λ_i = (-Σ_{k≠i≠j} λ_k y_k)·y_1 - λ_j y_i y_j = ζ y_1 - λ_j y_i y_j
				G(λ_j) = (λ_j + ζ y_1 - λ_j y_i y_j) + C - v_i (ζ-λ_j y_j) - v_j λ_j y_j 
						- 1/2 к_ii (ζ-λ_j y_j)² - 1/2 к_jj λ_j² - к_ij λ_j y_j (ζ-λ_j y_j)²
				其中 v_i = Σ_{k≠i≠j} λ_i y_i к_ki = f(x_i) - λ_i y_i к_ii -λ_j y_j к_ij - b, v_j同理
				∂G/∂λ_j = -(к_ii+к_jj-2к_ij)(λ_j^old - λ_j^new) + y_j(y_j-y_i+f(x_i)-f(x_j)) = 0
			=>	λ_j^new = λ_j^old + y_j (E_i - E_j)/(к_ii+к_jj-2к_ij)		其中E_i = f(x_i) - y_i
			修剪: 使得λ_j* 满足约束条件, λ_j应当∈[L,H]:
				y_i≠y_j时,	下界L = max(0,λ_j^old-λ_i^old)	,上界L = min(C,λ_j^old-λ_i^old + C)
				y_i= y_j时,	下界L = max(0,λ_j^old+λ_i^old - C),上界L = min(C,λ_j^old+λ_i^old)
			更新b:
		(8)	软间隔:
******************************************************************************/
bool SupportVectorMachines_SMO(Mat<>& X, int* y, int i, double& b, Mat<>& lamb, double C, double toler, Mat<>& kernelMat) {
	// error[]
	static Mat<> err(X.cols);
	for (int k = 0; k < err.size(); k++) {
		err[k] = b - y[k];
		for (int h = 0; h < lamb.size(); h++) {
			err[k] += lamb[h] * y[h] * kernelMat(k, h);
		}
	}
	// λ_j, λ_i
	if( (y[i] * err[i] < -toler && lamb[i] < C)
	||	(y[i] * err[i] >  toler && lamb[i] > 0)
	) {
		//1.j, err_i, err_ j
		int j = -1;
		double maxErr = 0;
		for (int k = 0; k < err.size(); k++) {
			if (k == i) continue;
			maxErr = maxErr > abs(err[k] - err[i]) ? maxErr : (j = k, abs(err[k] - err[i]));
		}
		if (j == -1) { while ((j = rand() % X.cols) == i); }
		double lamb_i_old = lamb[i],
			   lamb_j_old = lamb[j];
		//2.к_ii+к_jj-2к_ij, L, H
		double K = 2 * kernelMat(i, j) - kernelMat(i, i) - kernelMat(j, j),
			   L = y[i] != y[j] ? std::max(0.0,lamb[j] - lamb[i])    : std::max(0.0,lamb[j] + lamb[i] - C),
			   H = y[i] != y[j] ? std::min(C,  lamb[j] - lamb[i] + C): std::min(C,  lamb[j] + lamb[i]);
		if (K >= 0 || L == H) return 0;
		//3.λ_j, λ_i
		lamb[j] -= y[j] * (err[i] - err[j]) / K;
		lamb[j] = lamb[j] < H ? (lamb[j] > L ? lamb[j] : L) : H;
		lamb[i] += y[i] * y[j] * (lamb_j_old - lamb[j]);
		if (abs(lamb_j_old - lamb[j]) < 1e-5) return 0;
		//4.b
		double bi = - y[i] * (lamb[i] - lamb_i_old) * kernelMat(i, i)
				    - y[j] * (lamb[j] - lamb_j_old) * kernelMat(i, j) - err[i] + b,
			   bj = - y[i] * (lamb[i] - lamb_i_old) * kernelMat(i, j)
					- y[j] * (lamb[j] - lamb_j_old) * kernelMat(j, j) - err[j] + b;
		b = (lamb[i] > 0 && lamb[i] < C) ? bi : ((lamb[j] > 0 && lamb[j] < C) ? bj : (bi + bj) / 2);
		return 1;
	} return 0;
}
double SupportVectorMachines(Mat<>& X, int* y, Mat<>& w, double C, double toler, double(*kernel)(Mat<>& a, Mat<>& b), int maxIter = 100) {
	int N = X.cols;
	double b;
	// kernel matrix
	Mat<> kernelMat(N, N), tmp1, tmp2; 
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			kernelMat(i, j) = kernel(X.getCol(i, tmp1), X.getCol(j, tmp2));
		}
	}
	// λ*
	Mat<> lamb(X.rows);
	for (int iter = 0; iter < maxIter; iter++) {
		bool lambPairsChanged = 0;
		for (int i = 0; i < N; i++) {
			lambPairsChanged = ~SupportVectorMachines_SMO(X, y, i, b, lamb, C, toler, kernelMat);
		}
		if (lambPairsChanged) break;
	}
	// w*,b*
	w.zero(X.rows);
	for (int i = 0; i < N; i++) {
		for (int dim = 0; dim < X.rows; dim++) {
			w[dim] += X(dim, i) * lamb[dim] * y[dim];
		}
	}

	for (int i = 0; i < w.size(); i++)
		printf("%f ", w[i]);
	printf("\n%f \n", b);
	return b;
};