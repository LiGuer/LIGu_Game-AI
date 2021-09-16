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
#ifndef BASIC_MACHINE_LEARNING_H
#define BASIC_MACHINE_LEARNING_H
#include "../../LiGu_AlgorithmLib/Mat.h"
#include <float.h>
#include <algorithm>
#include <vector>
namespace BasicMachineLearning {
/*#############################################################################

						降维

##############################################################################*/
/******************************************************************************
[主成分分析]: Principal Components Analysis 
	[输入/输出]:
		输入: x 原矩阵		输出: y 降维后矩阵
	[目标]:
		数据降维，提取数据的主要特征分量, 满足：
		(1) 最近重构性: 样本点到该超平面的距离都足够近。
		(2) 最大可分性: 样本点在该超平面的投影尽可能分开。
	[优化问题]:
		min_W		tr( W^T x x^T W )
		s.t.		W^T W = I
	[流程]: 
		(1) 数据中心化, Σ \vec x_i = 0
		(2) 计算协方差矩阵 C = X X^T
		(3) 对协方差矩阵C 特征值分解
		(4) 取最大d'个特征值所对应的特征向量{w1,w2,...,wd'},投影矩阵 W = (w1,w2,...,wd')
		(5) 样本点在超平面投影: y_i = W^T x_i
	[原理]:
			分别从目标(1, 2)可以推得同样的结果
		(3)	目标函数: 样本点到超空间投影 y = WT x 尽可能分开
			即.方差最大:max Σ WT x xT W
			协方差矩阵:
				D = 1/m Y Y^T = 1/m (PX) (PX)^T = 1/m P X X^T P^T = 1/m P C P^T
			协方差矩阵对角化
		(4) 优化问题构造:
				min_W		tr( W^T x x^T W )
				s.t.		W^T W = I
		(5) 计算最优点:
			Lagrange函数 L(W,λ) = W^T x x^T W + λ( W^T W - I )
			Lagrange对偶 G(λ) = inf L(W,λ) = inf (W^T x x^T W + λ( W^T W - I ))
			L(W,λ)求导, 当导数为0时, 取得极值
			=>	X X^T ω_i = λ_i ω_i,
			即.对协方差X XT, 特征值求解
		()	取特征值最大的yDim个特征向量, 即目标投影矩阵W
*******************************************************************************/
void PCA(Mat<>& x, Mat<>& y, int yDim) {
	//[1]
	Mat<> mean;
	mean.mul(1.0 / x.cols, x.sum(mean, 1));		//得到均值
	Mat<> x2(x);
	for (int i = 0; i < x.rows; i++)
		for (int j = 0; j < x.cols; j++)
			x2(i, j) -= mean[i];
	//[2]
	Mat<> Cov;
	Cov.mul(x2, x2.transpose(Cov));		//X*XT
	//[3] 
	Mat<> eigVec, eigValue;
	Cov.eig(1e-5, eigVec, eigValue);
	//[4] // sort
	Mat<> eigValueTemp(1, eigValue.cols), W(eigVec.rows, yDim);
	for (int i = 0; i < eigValue.cols; i++)eigValueTemp[i] = eigValue(i, i);
	std::sort(eigValueTemp.data, eigValueTemp.data + eigValueTemp.cols);
	// W
	for (int i = 0; i < yDim; i++) {
		double value = eigValueTemp[eigValueTemp.cols - 1 - i];
		for (int j = 0; j < eigValue.cols; j++) {
			if (eigValue(j, j) == value) {
				for (int k = 0; k < eigVec.rows; k++) W(k, i) = eigVec(k, j);
				eigValue(j, j) = -DBL_MAX;
				break;
			}
		}
	}
	//[5]
	y.mul(W.transpose(y), x);
}

/*#############################################################################

						聚类

##############################################################################*/
/******************************************************************************
[K-Mean 聚类]
	[目标]:
		聚类. 对N维分布的数据点，可以将其聚类在 K 个关键簇内. 
	[步骤]:
		(1) 随机选择 K 个簇心点 Center
		(2) 迭代开始
			(3) 归零 Cluster , Cluster: 簇,记录ith簇内的数据指针。
			(4) 计算每个xi到簇心μj的距离
				(5) 选择距离最小的簇心, 将该点加入其簇内
			(6) 对每个簇,计算其质心 Center'
			(7) Center≠Center' , 则更正Center为 Center'
			(8) 迭代重新开始
		(9) 一轮无更正时，迭代结束
*******************************************************************************/
void K_Mean(Mat<>& x, int K, Mat<>& Center, std::vector<int>* Cluster, int TimeMax = 0x7FFFFFFF) {
	int Dim = x.rows, N = x.cols;
	Center.zero(Dim, K);
	//[1] 随机选择 K 个簇心点 
	for (int i = 0; i < K; i++) {
		int index = rand() % N;
		for (int dim = 0; dim < Dim; dim++) Center(dim, i) = x(dim, index);
	}
	//[2]
	for (int times = 0; times < TimeMax; times++) {
		//[3]
		for (int k = 0; k < K; k++) Cluster[k].clear();
		//[4] 计算每个xi到Center_j的距离
		for (int i = 0; i < N; i++) {
			Mat<> d(1, K);
			for (int k = 0; k < K; k++)
				for (int dim = 0; dim < Dim; dim++)
					d[k] += pow(x(dim, i) - Center(dim, k), 2);
			//[5]
			int index; d.min(index);
			Cluster[index].push_back(i);
		}
		//[6] 对每个簇,计算其质心 Center'
		Mat<> CenterTmp(Dim, K);
		for (int k = 0; k < K; k++) {
			for (int dim = 0; dim < Dim; dim++) {
				for (int j = 0; j < Cluster[k].size(); j++)
					CenterTmp(dim, k) += x(dim, Cluster[k][j]);
				CenterTmp(dim, k) /= Cluster[k].size();
			}
		}
		//[7] 更正簇心//[9]
		if (CenterTmp == Center) return;						
		Center.swap(CenterTmp);
	}
}




/*################################################################################

						回归
	[目标]:
		* 分类，是离散的回归问题.
#################################################################################*/
/******************************************************************************
[最小二乘法]
	[目标]:
		求一条直线，使得所有样本点到该直线的Euclid距离最小.
	[优化问题]:
		min_w	MSE(y~)*n = Σ (y~ - y)² = Σ (w^T x - y)²
	[原理]:
		(1) 直线方程: f(x) = w^T x + b = w^T x'		(x' = [1, x], w_0 = b)
		(2) 均方误差: 
				MSE(y~)*n = Σ (y~ - y)² 
				= Σ (w^T x - y)² = (X w - y)^T (X w - y)
		(3) 优化问题构造: 无约束凸优化问题
				min_W	Σ (w^T x - y)² = (X w - y)^T (X w - y)
		(4) 计算最优点: 求导, 导数为0时取得极值
				∂MSE / ∂W = 2·X^T·(w^T x - y) = 0
			=>	w* = (X^T X)^-1 X^T y
				f(x) = x^T (X^T X)^-1 X^T y
		(5) 一维场景:
			优化问题:
				min_{w,b}	Σ (w^T x_i + b - y_i)²
			=>	∂E/∂w = 2( w^T Σx_i² + Σ(x_i(b - y_i)) )	= 0
				∂E/∂b = 2( n b + Σ(y_i - w x_i) )			= 0
			最优点:
			=>	wΣx_i² = Σ_i x_i y_i - 1/n (Σ_i x_i)(Σ_i y_i) + w/n (Σ_i x_i)²
				w* = (Σ y_i(x_i - x¯)) / (Σ x_i² - 1/n (Σ x_i)²)
				b* = 1/n·Σ(y_i - w x_i)
******************************************************************************/
void OrdinaryLeastSquares(Mat<>& x, Mat<>& y, double& w, double& b) {
	//w 
	double w1 = 0, w2 = 0, xbar = x.mean();
	for (int i = 0; i < x.size(); i++) {
		w1 += y[i] * (x[i] - xbar);
		w2 += x[i] * x[i];
	}
	w = w1 / (w2 - pow(x.sum(), 2) / x.size());
	//b 
	b = 0;
	for (int i = 0; i < x.size(); i++) {
		b += y[i] - w * x[i];
	}
	b /= x.size();
}


/*################################################################################

						插值

#################################################################################*/
/******************************************************************************
[Lagrange插值]
	[原理]:
		f(x) = Σ_(i=1)^n  y_i · f_i(x)
		f_i(x) = Π_(j=1,i≠j)^n  (x - x_j) / (x_i - x_j)
		第N点y = 基函数1×第1点y + 基函数2×第2点y + 基函数3×第3点y
		基函数状态2 = (输入X-第1点x)(输入X-第3点x) / (第2点x-第1点x)(第2点x-第3点x)
******************************************************************************/
double LagrangeInterpolation(double x, double* x0, double* y0, int n) {
	double y = 0, t;
	for (int i = 0; i < n; i++) {
		t = y0[i];
		for (int j = 0; j < n; j++)
			if (i != j)
				t *= (x - x0[j]) / (x0[i] - x0[j]);
		y += t;
	}
	return y;
}
/******************************************************************************
[样条插值]
	[算法]: 过求解三弯矩方程组得出曲线函数组的过程
******************************************************************************/
void CubicSpline(double x, double y) {

}
/*********************************************************************************
[反距离加权插值]: Inverse Distance Weighted
	[目的]:
		空间插值
	[原理]:
		插值点与样本点间的距离为权重进行加权平均，离插值点越近的样本点赋予权重越大。
		f(x) = Σ w_i(x) f(x_i)
		w_i(x) = (1/d_i(x)^α) / (Σ 1/d_i(x)^α)
		d_i(x) = ||x - x_i||₂
*********************************************************************************/
double InvDisWeight(Mat<>& data, double x, double y, int alpha = 2) {
	double z = 0, d0 = 0;
	for (int i = 0; i < data.cols; i++) {
		d0 += pow((pow(x - data(0, i), 2) + pow(y - data(1, i), 2)), -alpha);
	}
	for (int i = 0; i < data.cols; i++) {
		double d = pow((pow(x - data(0, i), 2) + pow(y - data(1, i), 2)), -alpha);
		z += (d / d0) * data(2, i);
	}
	return z;
}
/*********************************************************************************
[普通Kriging插值]: Ordinary Kriging
	[目的]:
		空间插值. 满足假设:
		(1) 空间属性z是均一的. 对空间任意一点，都有相同期望、方差.
	[优化问题]:
		min		var = 2Σ w_i γ_i0 - ΣΣ w_i w_j γ_ij - γ_00
		s.t.	Σ w_i = 1
				γ_ij = σ² - C_ij = E[(Z(x_i) - Z(x_j))²]
	[步骤]:
		(1) 确定半方差函数γ(xi,xj) = E[(Z(x_i) - Z(x_j))²]
			确定半方差函数与两点间距离的函数关系.
		(2) 计算权值
			权值计算方程:
			[w_1]		[ γ(x1,x1)	...	γ(x1,xn)	 1	]^-1	[γ(x1,x*)]
			|...|  =	|	...		...		...		...	|		|	...	  |
			|w_n|		| γ(xn,x1)	...	γ(xn,xn)	 1	|		|γ(x1,x*)|
			[μ ]		[	 1		...		 1		 0	]		[	 1	  ]
		(3) 计算插值点结果
			f(x) = Σ w_i(x) f(x_i)
	[原理]:
		(1) f(x) = Σ w_i(x) f(x_i)
			z~: 估计值	z : 实际值
			求解权重系数: 使其为满足插值点处，估计值与真实值差最小的一组系数.
		(2) 对空间任意一点，都有相同期望、方差. 即:
			E	(z(x,y)) = μ
			Var	(z(x,y)) = σ²
			即. z(x,y) = μ + R(x,y)    Var(R(x,y)) = σ²
		(3) 约束方程 —— 无偏估计条件 E(z~ - z) = 0
			=> E(z~ - z) = E(Σ w_i z_i - z) = μΣ w_i - μ = 0
			=> Σ w_i = 1
		(4)	目标函数 —— 估计误差 var = Var(z~ - z)
			=> var = Var(Σ w_i z_i - z) = ΣΣ w_i w_j Cov(zi,zj) - 2Σ w_i Cov(zi,z) + cov(z,z)
			=> var = ΣΣ w_i w_j C_ij - 2Σ w_i C_i0 + cov_00		(C_ij = Cov(zi - μ,z - μ))
		(5) 定义 半方差函数γ_ij = σ² - C_ij
			=> var = 2Σ w_i γ_i0 - ΣΣ w_i w_j γ_ij - γ_00
		(6) 凸优化问题构建完成:
				min		var = 2Σ w_i γ_i0 - ΣΣ w_i w_j γ_ij - γ_00
				s.t.	Σ w_i = 1
			Lagrange函数	L(w_i, λ) = var + λ(Σ w_i - 1)
			Lagrange对偶	G(λ) = inf L(w_i, λ)
			L(W,λ)求导, 当导数为0时, 取得极值
			即. 得到权值计算方程.
		(7) 半方差函数γ_ij = σ² - C_ij = E[(Z(x_i) - Z(x_j))²]
			∵地理学第一定律: 空间上相近的属性相近.
			∴γ_ij与两点间距离，存在函数关系
			将所有d和γ绘成散点图，寻找最优曲线拟合d与γ，得到函数关系式.
*********************************************************************************/
double KrigingOrdinary(Mat<>& data, double x, double y, double(*variogram)(double x1, double y1, double x2, double y2)) {
	double z = 0;
	int N = data.cols;
	Mat<> weight(N + 1), R0(N + 1, N + 1), R(N + 1);
	//R0 R
	R0 = 1; R0(N, N) = 0;
	R(N) = 1;
	for (int i = 0; i < N; i++) {
		R(i) = variogram(data(0, i), data(1, i), x, y);
		for (int j = 0; j < N; j++) {
			R0(i, j) = variogram(data(0, i), data(1, i), data(0, j), data(1, j));
		}
	}
	//lambda z
	weight.mul(R0.inv(R0), R);
	for (int i = 0; i < data.cols; i++) {
		z += weight[i] * data(2, i);
	}
	z += weight[N];
	return z;
}




/******************************************************************************
*				Mahalanobis Distance
* Mahalanobis Distance
*******************************************************************************/
void MahalanobisDist(Mat<>& x, Mat<>& mahalanobisDistance) {
	mahalanobisDistance.alloc(x.size());
	// mean, diff, cov
	Mat<> mean, diff, covMat, tmp;
	mean.mul(1.0 / x.size(), x.sum(mean, 1));
	covMat.mul(x, x.transpose(covMat));
	// mahalanobis distance
	covMat.inv(covMat);
	Mat<> xt;
	for (int i = 0; i < x.size(); i++) {
		diff.sub(x.getCol(i, xt), mean);
		tmp.mul(tmp.mul(diff.transpose(tmp), covMat), diff);
		mahalanobisDistance[i] = tmp[0];
	}
}
double MahalanobisDist(Mat<>& x1, Mat<>& x2, Mat<>& covMat) {
	double ans;
	Mat<> diff, _covMat, tmp;
	diff.sub(x1, x2);
	covMat.inv(_covMat);
	return ((diff.transpose(tmp) *= _covMat) *= diff)[0];
}
}
#endif