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
[Apriori]
	[输入/输出]:
		输入: (1) 初始频繁项集	(2) 最小支持度
		输出: (1) 关联项集		(2) 关联项集支持度
	[目的]: 根据频繁项集，寻找数据集中变量之间的关联规则.
	[概念]:
		* 频繁项集: 经常出现在一块的物品的集合.
		* 关联规则: 两种物品之间可能存在很强的关系.
		* 支持度P(AB): 数据集中包含该项集的记录所占的比例. P(AB) ≌ num(AB) / num(all)
		* 置信度P(A→B) = P(B|A) = P(AB) / P(A)
	[定理]:
		* 项集频繁，则其子集频繁. <=> 项集不频繁，则其超集不频繁.
		* 若规则X→Y−X低于置信度阈值，则对于X子集X',规则X'→Y−X'也低于置信度阈值
		* 频繁项集生成的方法:
			(1) Fk = Fk-1 × F1
			(2) Fk = Fk-1 × Fk-1
		* 频繁项集每一项各不相同， 每一项内部排列有序.
	[过程]:
		(1) 频繁项集生成,对于K项的集合
			(2) 频繁项集子集生成. 生成K项所有可以组合的集合. eg.(frozenset({2, 3}), frozenset({3, 5})) -> (frozenset({2, 3, 5}))
			(3) 保存满足目标支持度P(AB)的集合.
		(4)  关联规则生成, 对不同长度(K)的频繁项集依次分析
			(4.1) 频繁项集只有两个元素{AB}，直接计算置信度P(A→B),P(B→A)
			(4.2) 频繁项集超过两个元素{ABC...}，依次计算置信度P(AC...→B)
			(5) 保存满足目标置信度的关联规则.
*******************************************************************************/
//[2] 生成K项所有可以组合的集合.
void Apriori_GenCandidate(std::vector<Mat<int>>& frozenSet, int K, std::vector<Mat<int>>& newfrozenSet) {
	std::vector<Mat<int>> newfrozenSet_Tmp;
	Mat<int> tmp(K);
	for (int i = 0; i < frozenSet.size(); i++) {
		for (int j = i + 1; j < frozenSet.size(); j++) {
			bool flag = true;
			for (int k = 0; k < K - 2; k++) {
				tmp[k] = frozenSet[i][k];
				if (frozenSet[i][k] != frozenSet[j][k]) { flag = false; break; }
			}
			if (flag) {
				tmp[K - 2] = frozenSet[i][K - 2];
				tmp[K - 1] = frozenSet[j][K - 2];
				//频繁项集每一项内排列有序
				std::sort(tmp.data, tmp.data + tmp.size());
				//频繁项集每一项各不相同
				bool flag2 = true;
				for (int k = 0; k < newfrozenSet_Tmp.size(); k++)
					if (tmp == newfrozenSet_Tmp[k]) { flag2 = false; break; }
				if (flag2) newfrozenSet_Tmp.push_back(tmp);
			}
		}
	}newfrozenSet = newfrozenSet_Tmp;
}
//[3] 过滤小于支持度的集合.
void Apriori_Filter(std::vector<Mat<int>>& dataSet, std::vector<Mat<int>>& frozenSet, double minSupport, std::vector<double>& frozenSet_Support) {
	//[3.1]计算支持度P(AB) ≌ num(AB) / num(all)
	frozenSet_Support.clear();
	Mat<> frozenSet_Count(frozenSet.size());
	for (int i = 0; i < frozenSet.size(); i++) {
		for (int j = 0; j < dataSet.size(); j++) {
			// if frozenSet[i] is dataSet[j]'s subset
			int cur = 0;
			for (int k = 0; k < dataSet[j].size(); k++) {
				if (frozenSet[i][cur] == dataSet[j][k]) cur++;
				if (cur == frozenSet[i].size()) { frozenSet_Count[i]++; break; }
			}
		}
	}
	frozenSet_Count *= 1.0 / dataSet.size();
	//[3.2]删除小于支持度的集合.
	std::vector<Mat<int>> frozenSet_Tmp;
	for (int i = 0; i < frozenSet.size(); i++) {
		if (frozenSet_Count[i] > minSupport) {
			frozenSet_Tmp.push_back(frozenSet[i]);
			frozenSet_Support.push_back(frozenSet_Count[i]);
		}
	}frozenSet = frozenSet_Tmp;
}
// 对关系规则进行评估 获得满足最小可信度的关联规则
void Apriori_RulesFromConseqence(std::vector < std::vector<Mat<int>>>& frozenSet, std::vector<std::vector<double>>& frozenSet_Support, int K, std::vector<Mat<int>>& BSet,
	double minConfidence, std::vector<Mat<int>>& RuleSet_A, std::vector<Mat<int>>& RuleSet_B, std::vector<double>& RuleSet_confidence
) {
	if (K == 2) {
		// 关系评估
		Mat<int> A(K - BSet[0].size());
		for (int i = 0; i < frozenSet[K].size(); i++) {
			for (int j = 0; j < BSet.size(); j++) {
				// 计算可信度P(A→B) = P(B|A) = P(AB) / P(A)// 构建 A
				int B_cur = 0, A_cur = 0;
				for (int k = 0; k < K; k++) {
					if (B_cur < BSet[j].size() && frozenSet[K][i][k] == BSet[j][B_cur]) B_cur++;
					else if (A_cur < A.size()) A[A_cur++] = frozenSet[K][i][k];
				}
				if (A_cur + B_cur != K) continue;
				// 查找 P(A)
				int A_index = -1;
				for (int k = 0; k < frozenSet[A.size()].size(); k++)
					if (frozenSet[A.size()][k] == A) { A_index = k; break; }
				if (A_index == -1) continue;
				double confidence = frozenSet_Support[K][i] / frozenSet_Support[A.size()][A_index];
				// 保存满足可信度的关联规则
				if (confidence >= minConfidence) {
					RuleSet_A.push_back(A);
					RuleSet_B.push_back(BSet[j]);
					RuleSet_confidence.push_back(confidence);
				}
			}
		} return;
	}
	if (K > BSet[0].size()) {
		// 组合得到 new B 集
		Apriori_GenCandidate(BSet, BSet[0].size() + 1, BSet);
		std::vector<Mat<int>> BSet_Tmp;
		// 关系评估
		Mat<int> A(K - BSet[0].size());
		for (int i = 0; i < frozenSet[K].size(); i++) {
			for (int j = 0; j < BSet.size(); j++) {
				// 计算可信度P(A→B) = P(B|A) = P(AB) / P(A) // 构建 A
				int B_cur = 0, A_cur = 0;
				for (int k = 0; k < K; k++) {
					if (B_cur < BSet[j].size() && frozenSet[K][i][k] == BSet[j][B_cur]) B_cur++;
					else if (A_cur < A.size()) A[A_cur++] = frozenSet[K][i][k];
				}
				if (A_cur + B_cur != K) continue;
				// 查找 P(A)
				int A_index = -1;
				for (int k = 0; k < frozenSet[A.size()].size(); k++)
					if (frozenSet[A.size()][k] == A) { A_index = k; break; }
				if (A_index == -1) continue;
				// 保存满足可信度的关联规则
				double confidence = frozenSet_Support[K][i] / frozenSet_Support[A.size()][A_index];
				if (confidence >= minConfidence) {
					RuleSet_A.push_back(A);
					RuleSet_B.push_back(BSet[j]);
					RuleSet_confidence.push_back(confidence);
					BSet_Tmp.push_back(BSet[j]);
				}
			}
		}
		// 可以进一步合并BSet
		BSet = BSet_Tmp;
		if (BSet.size() > 1) Apriori_RulesFromConseqence(
			frozenSet,
			frozenSet_Support,
			K, BSet, minConfidence,
			RuleSet_A,
			RuleSet_B,
			RuleSet_confidence
		);
	}
}
//Main
void Apriori(std::vector<Mat<int>>& dataSet, double minSupport, double minConfidence,
	std::vector<Mat<int>>& RuleSet_A, std::vector<Mat<int>>& RuleSet_B, std::vector<double>& RuleSet_confidence
) {
	std::vector<std::vector<Mat<int>>>	frozenSet;
	std::vector<std::vector<double>>	frozenSet_Support;
	std::vector<Mat<int>>				frozenSet_K;
	std::vector<double>					frozenSet_Support_K;
	frozenSet.push_back(frozenSet_K);
	frozenSet_Support.push_back(frozenSet_Support_K);	//故意填占 frozenSet[0]位置
	Mat<int> tmp;
	{//[2] 初始一个元素的频繁项集，Frozen Set[{ 1 }, { 2 }, { 3 }, { 4 }, { 5 }]
		tmp.zero(1);
		for (int i = 0; i < dataSet.size(); i++) {
			for (int j = 0; j < dataSet[i].size(); j++) {
				int item = dataSet[i][j];
				bool flag = true;
				for (int k = 0; k < frozenSet_K.size(); k++)
					if (item == frozenSet_K[k][0]) { flag = false; break; }
				if (flag) frozenSet_K.push_back(tmp.get(item));
			}
		}
	}
	// [1]
	for (int k = 1; frozenSet_K.size() > 0; k++) {
		//[3]
		Apriori_Filter(dataSet, frozenSet_K, minSupport, frozenSet_Support_K);
		frozenSet.push_back(frozenSet_K);
		frozenSet_Support.push_back(frozenSet_Support_K);
		//[2]
		Apriori_GenCandidate(frozenSet_K, k + 1, frozenSet_K);
	}
	//[4]
	//for (int k = 2; k < frozenSet.size(); k++) {
	for (int k = 2; k < 5; k++) {
		std::vector<Mat<int>> OneElementSet;
		{
			tmp.zero(1);
			for (int i = 0; i < frozenSet[k].size(); i++) {
				for (int j = 0; j < frozenSet[k][i].size(); j++) {
					int item = frozenSet[k][i][j];
					bool flag = true;
					for (int u = 0; u < OneElementSet.size(); u++)
						if (item == OneElementSet[u][0]) { flag = false; break; }
					if (flag) OneElementSet.push_back(tmp.get(item));
				}
			}
		}
		Apriori_RulesFromConseqence(
			frozenSet,
			frozenSet_Support,
			k, OneElementSet, minConfidence,
			RuleSet_A,
			RuleSet_B,
			RuleSet_confidence
		);
	}
}