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
#ifndef NEURAL_NETWORKS_H
#define NEURAL_NETWORKS_H
#include <iostream>
#include <vector>
#include "../../LiGu_AlgorithmLib/Mat.h"
#include "../../LiGu_AlgorithmLib/Tensor.h"
/*################################################################################################
核心类:
class NeuralLayer	(int inSize, int outSize)	//神经网络层
class ConvLayer		(int inChannelNum, int outChannelNum,int kernelSize,int padding,int stride)	//卷积层
class PoolLayer		(int kernelSize,   int padding, int stride, int poolType)					//下采样层
class LstmLayer		(int inSize, int outSize)	//长短期记忆层
--------------------------------------------------------------------------------------------------
经典结构:
class BackPropagation_NeuralNetworks()				//反向传播神经网络 : 1986.Rumelhart,McClelland
class LeNet_NeuralNetworks()						//LeNet卷积神经网络 : 1998.Yann LeCun
class Inception()									//Inception模块 : 2014.Google
class GoogLeNet_NeuralNetworks()					//GoogLeNet卷积神经网络 : 2014.Google
class LstmNetwork()									//LSTM长短期记忆网络
################################################################################################*/
/*************************************************************************************************
*							基础函数
*************************************************************************************************/
/*----------------[ ReLU ]----------------*/
double relu		(double x) { return x > 0 ? x   : 0; }
double reluD	(double x) { return x > 0 ? 1.0 : 0; }
/*----------------[ sigmoid ]----------------*/
double sigmoid	(double x) { return 1 / (1 + exp(-x)); }
double sigmoidD	(double x) { return sigmoid(x) * (1 - sigmoid(x)); }
/*************************************************************************************************
*							损失函数
*************************************************************************************************/
double QuadraticLossFunc(Mat<>& y, Mat<>& target) {
	Mat<> t; return t.sub(y, target).dot(t);
}
Mat<>& QuadraticLossFuncD(Mat<>& y, Mat<>& target, Mat<>& error) { return error.mul(-2, error.sub(target, target)); }
/*************************************************************************************************
*							NeuralLayer	神经网络层
*	[核心数据]: 
		[1] weight, bias	[2] actFunc, actDerivFunc 激活函数、导函数
		[3] out 输出
*	[注]:
		[ReLU]: ReLU(x) = x > 0 ? x : 0
		[Sigmoid]: Sigmoid(x) = 1 / (1 + e^-x)
----------------------------------------------------------------------------------
*	正向传播
		y = σ(W×x + b)
		x: 输入  y: 输出    z = Σwi·xi + b: 线性拟合
		σ(): 激活函数, 使线性拟合非线性化, eg. relu(x), Sigmoid(x)
----------------------------------------------------------------------------------
*	误差·损失函数: E_total = Σ(target_i - out_i)²
----------------------------------------------------------------------------------
*	反向传播
	[结论]:
		[1] δl = ((w_l+1)^T·δ_l+1)·σ'(zl)
			δL = ▽aE·σ'(z_outl)		▽aE  = -2(target - y_outl)
		[2] ∂E/∂w_l = δl×x_l^T		wl = wl + lr·∂E/∂wl
			∂E/∂b_l = δl				bl = bl + lr·∂E/∂bl
	[设]: 
		l: lth layer	L:out layer 
	[目的]: 
		w = w + lr·∂E/∂b, 
		b = b + lr·∂E/∂b, 
		求∂E/∂w, ∂E/∂b
	[推导]:
		链式法则: ∂E/∂w_l = ∂z_l/∂w_l·(∂y_l/∂z_l·∂z_l/∂y_(l+1))·(∂y_l/∂z_(l+1)·∂z_l/∂y_(l+2))·...·(∂y_l/∂z_(L-1)·∂z_l/∂y_L)·∂y_l/∂z_L·∂E/∂y_L
		∂y_l/∂z_l		= σ'(z_l)
		∂z_l/∂y_(l+1)	= W_(l+1)^T
		δl = (∂y_l/∂z_l·∂z_l/∂y_(l+1))·(∂y_l/∂z_(l+1)·∂z_l/∂y_(l+2))·...·(∂y_l/∂z_(L-1)·∂z_l/∂y_L)·∂y_l/∂z_L·∂E/∂y_L
		    = (σ'(z_l)·W_(l+1)^T)·...·(σ'(z_(L-1))·W_L^T)·σ'(z_L)·∂E/∂y_L
		∴∂E/∂w_l = δl·∂z_l/∂w_l = δl×x_l^T
		  ∂E/∂b_l = δl·∂z_l/∂b_l = δl
		其中，输出层
		δL = ∂E/∂y_L·σ'(z_L)		∂E/∂y_L = ▽yE = -2(y_target - y_L) : 误差函数
		即可求出 ∂E/∂w_l
----------------------------------------------------------------------------------
*	[Reference]:
[1] Wikipedia. http://www.wikipedia.org/wiki/Backpropagation
*************************************************************************************************/
class NeuralLayer {
public:
	Mat<> weight, bias;
	Mat<> out, z;
	double(*actFunc)     (double);
	double(*actDerivFunc)(double);
	/*----------------[ init ]----------------*/
	NeuralLayer() { ; }
	NeuralLayer(int inSize, int outSize, double(*_actFunc)(double), double(*_actDerivFunc)(double)) { init(inSize, outSize, _actFunc, _actDerivFunc); }
	NeuralLayer(int inSize, int outSize) { init(inSize, outSize, [](double x) { return x; },[](double x) { return 1.0; }); }
	void init  (int inSize, int outSize, double(*_actFunc)(double), double(*_actDerivFunc)(double)) {
		weight.rands(outSize, inSize, -1, 1);
		bias.  rands(outSize, 1,	  -1, 1);
		actFunc		= _actFunc;
		actDerivFunc= _actDerivFunc;
	}
	/*----------------[ forward ]----------------*/
	Mat<>* operator()(Mat<>& in) { return forward(in); }
	Mat<>* forward   (Mat<>& in) {
		return &out.function(z.add(z.mul(weight, in), bias), actFunc);
	}
	/*----------------[ backward ]----------------*/
	void backward(Mat<>& preIn, Mat<>& error, double learnRate) {
		static Mat<> t, delta;
		error.mul(
			weight.transpose(t), 
			delta.elementMul(error, delta.function(z, actDerivFunc))
		);
		weight += t.mul(-learnRate, t.mul(delta, preIn.transpose(t)));
		bias   += t.mul(-learnRate, delta);
	}
	/*----------------[ save/load ]----------------*/
	void save(FILE* file) {
		for (int i = 0; i < weight.size(); i++) fprintf(file, "%f ", weight[i]);
		for (int i = 0; i < bias  .size(); i++) fprintf(file, "%f ", bias  [i]);
		fprintf(file, "\n");
	}
	void load(FILE* file) {
		for (int i = 0; i < weight.size(); i++) fscanf(file, "%lf", &weight[i]);
		for (int i = 0; i < bias  .size(); i++) fscanf(file, "%lf", &bias  [i]);
	}
};
/*************************************************************************************************
*							Convolution Layer	卷积层
*	[输入]: kernel: 卷积核		padding: 加边框宽度		in/outChannelNum: 输入/输出通道数
*	[参数]:
		Height_out = (Height_in - Height_kernel + 2 * padding) / (stride + 1)
		 Width_out = ( Width_in -  Width_kernel + 2 * padding) / (stride + 1)
-------------------------------------------------------------------------------------------------
*	[正向传播]: 卷积操作
		x * k = Σ_(c=1)^(xChannelNum) Σ_(p=1)^H Σ_(q=1)^W  x_{c,i+p-1,j+q-1} × k_cpq
		y_c = x * k[c] + b[c]
		y   = {y_1, y_2, ... , y_n}
-------------------------------------------------------------------------------------------------
*	[反向传播]:
		∂E / ∂y_l = ∂E / ∂y_L·(∂y_L . ∂y_(L-1))·...·(∂y_(l+1) / ∂y_l) 
				  = δ_l = δ_(l+1)·(∂y_(l+1) / ∂x_(l+1)) 
		          = δ_(l-1) * Rot180(w_l)
		∂E / ∂k_l = ∂E / ∂y_l·∂y_l / ∂k_l = δ_l·∂y_l / ∂k_l
		          = δ_l * x_l
		∂E / ∂b_l = δ_l·∂y_l / ∂b_l = Σ_x Σ_y δ_l
*************************************************************************************************/
class ConvLayer {
public:
	Tensor<> kernel, out;
	Mat<> bias; bool isBias = true;
	int padding, stride;
	/*----------------[ init ]----------------*/
	ConvLayer() { ; }
	ConvLayer(int inChannelNum, int outChannelNum, int kernelSize, int _padding, int _stride) {
		init(inChannelNum, outChannelNum, kernelSize, _padding, _stride);
	}
	void init(int inChannelNum, int outChannelNum, int kernelSize, int _padding, int _stride) {
		padding			= _padding, 
		stride			= _stride; 
		kernel.alloc(kernelSize, kernelSize, inChannelNum, outChannelNum).rands(-1, 1);
		if (isBias) bias.rands(outChannelNum, 1, -1, 1);
	}
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward   (Tensor<>& in) {
		out.zero(
			(in.dim[0] - kernel.dim[0] + 2 * padding) / stride + 1,
			(in.dim[1] - kernel.dim[1] + 2 * padding) / stride + 1,
			kernel.dim[3]
		); 
		for (int i = 0; i < out.size(); i++)
			for (int k = 0; k < kernel.size() / kernel.dim[3]; k++) {
				int xi = -padding + out.i2x(i) * stride + kernel.i2x(k),
					yi = -padding + out.i2y(i) * stride + kernel.i2y(k),
					zi =                                  kernel.i2z(k);
				if (xi < 0 || xi >= in.dim[0] || yi < 0 || yi >= in.dim[1]) continue;
				out(i) += in(xi, yi, zi) * kernel(k + i * kernel.size(3));
			}
		if (isBias) for (int i = 0; i < out.size(); i++) out[i] += bias[out.i2z(i)];
		return &out;
	}
	/*----------------[ backward ]----------------*/
	void backward(Tensor<>& preIn, Tensor<>& error, double learnRate) {
		// δ_(l-1)
		static Tensor<> delta; delta = error;
		error.alloc(preIn.dim).zero();
		for (int i = 0; i < error.size(); i++)
			for (int k = 0; k < kernel.size() / kernel.dim[3]; k++) {
				int xi = -padding + error.i2x(i) * stride + kernel.i2x(kernel.size(3) - 1 - k),
					yi = -padding + error.i2y(i) * stride + kernel.i2y(kernel.size(3) - 1 - k),
					zi =                                    kernel.i2z(kernel.size(3) - 1 - k);
				if (xi < 0 || xi >= delta.dim[0] || yi < 0 || yi >= delta.dim[1]) continue;
				error(i) += delta(xi, yi, zi) * kernel(kernel.size(3) - 1 - k + i * kernel.size(3));
			}
		// ∂E / ∂k_l
		static Tensor<> deltaKernel; deltaKernel.alloc(kernel.dim);
		for (int i = 0; i < delta.size(); i++)
			for (int k = 0; k < preIn.size() / preIn.dim[3]; k++) {
				int xi = -padding + delta.i2x(i) * stride + preIn.i2x(k),
					yi = -padding + delta.i2y(i) * stride + preIn.i2y(k),
					zi =                                    preIn.i2z(k);
				if (xi < 0 || xi >= delta.dim[0] || yi < 0 || yi >= delta.dim[1]) continue;
				delta(i) += delta(xi, yi, zi) * preIn(k + i * preIn.size(3));
			}
		kernel += (delta *= learnRate);
		// ∂E / ∂b_l
		static Mat<> deltaBias;  deltaBias.alloc(bias.size());
		for (int i = 0; i < deltaBias.size(); i++) {
			deltaBias[i] = 0;
			for (int j = 0; j < delta.size(2); j++) deltaBias[i] += delta(j + i * delta.size(2));
		} bias += (deltaBias *= learnRate);
	}
	/*----------------[ save / load ]----------------*/
	void save(FILE* file) {
		for (int i = 0; i < kernel.size(); i++)			 fprintf(file, "%f ", kernel[i]);
		if(isBias) for (int i = 0; i < bias.size(); i++) fprintf(file, "%f ", bias  [i]);
		fprintf(file, "\n");
	}
	void load(FILE* file) {
		for (int i = 0; i < kernel.size(); i++)			 fscanf(file, "%lf", &kernel[i]);
		if(isBias) for (int i = 0; i < bias.size(); i++) fscanf(file, "%lf", &bias  [i]);
	}
};
/*************************************************************************************************
*							Pool Layer	采样层
*	[分类]:	[1] AvePool 平均采样层    [2] MaxPool 最大采样层
-------------------------------------------------------------------------------------------------
*	[正向传播]:
		MaxPool: 最大值
		AvePool: 平均值
-------------------------------------------------------------------------------------------------
*	[反向传播]:
		MaxPool: 区域最大值对应的位置误差为δ，而其它位置误差为0
		AvePool: 区域每个位置的误差为采样后误差除以区域大小
*************************************************************************************************/
class PoolLayer {
public:
	Tensor<> out;
	int kernelSize, padding, stride, poolType = 0;
	enum { A, M };
	/*----------------[ init ]----------------*/
	PoolLayer() { ; }
	PoolLayer(int _kernelSize, int _padding, int _stride, int _poolType) {
		init(_kernelSize, _padding, _stride, _poolType);
	}
	void init(int _kernelSize, int _padding, int _stride, int _poolType) {
		kernelSize = _kernelSize, padding = _padding, stride = _stride, poolType = _poolType;
	}
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward   (Tensor<>& in) {
		out.zero(
			(in.dim[0] - kernelSize + 2 * padding) / stride + 1,
			(in.dim[1] - kernelSize + 2 * padding) / stride + 1,
			 in.dim[2]
		);
		for (int i = 0; i < out.size(); i++)
			for (int k = 0; k < kernelSize * kernelSize; k++) {
				int xt = -padding + out.i2x(i) * stride + k % kernelSize,
					yt = -padding + out.i2y(i) * stride + k / kernelSize,
					zt =            out.i2z(i);
				if (xt < 0 || xt >= in.dim[0] || yt < 0 || yt >= in.dim[1]) continue;
				switch (poolType) {
				case A: out(i) += in(xt, yt, zt); break;
				case M: out(i)  = in(xt, yt, zt) > out(i) ? in(xt, yt, zt) : out(i); break;
				}
			}
		if (poolType == A) out *= 1.0 / (kernelSize * kernelSize);
		return &out;
	}
	/*----------------[ backward ]----------------*/
	void backward(Tensor<>& preIn, Tensor<>& error) {
		static Tensor<> delta; delta = error;
		error.alloc(preIn.dim).zero();
		if (poolType == A) {
			for (int i = 0; i < error.size(); i++)
				for (int k = 0; k < kernelSize * kernelSize; k++) {
					int xt = -padding + out.i2x(i) * stride + k % kernelSize,
						yt = -padding + out.i2y(i) * stride + k / kernelSize,
						zt =            out.i2z(i);
					if (xt < 0 || xt >= delta.dim[0] || yt < 0 || yt >= delta.dim[1]) continue;
					error(i) += delta(xt, yt, zt) / (kernelSize * kernelSize);
				}
		}
		if (poolType == M) {
			for (int i = 0; i < delta.size(); i++) {
				double maxn = -DBL_MAX; int maxIndex;
				for (int k = 0; k < kernelSize * kernelSize; k++) {
					int xt = -padding + delta.i2x(i) * stride + k % kernelSize,
						yt = -padding + delta.i2y(i) * stride + k / kernelSize,
						zt =            delta.i2z(i);
					if (xt < 0 || xt >= preIn.dim[0] || yt < 0 || yt >= preIn.dim[1]) continue;
					if (preIn(xt, yt, zt) > maxn) maxn = preIn(xt, yt, zt), maxIndex = xt * yt * zt;
				} error(maxIndex) = delta(i);
			}
		}
	}
};
/*********************************************************************************
						LSTM Layer 长短期记忆层
*	结构: Gate[4]: G, F, I, O 四个并行神经网络层
----------------------------------------------------------------------------------
*	正向传播
		gt = tanh(Wg×[h_(t-1), xt] + bg)
		ft = Sigmoid(Wf×[h_(t-1), xt] + bf)
		it = Sigmoid(Wi×[h_(t-1), xt] + bi)
		ot = Sigmoid(Wo×[h_(t-1), xt] + bo)
		------
		st = ft·s_(t-1) + it·gt
		ht = ot·tanh(Ct)
		------
		W_gfio			(cellNum, cellNum + xDim)
		[h_(t-1), xt]	(cellNum + xDim)
		gfiosh_t		(cellNum)
----------------------------------------------------------------------------------
*	反向传播
		Δg = it	 ·Δs		·tanh'(gt)
		Δf = s_(t-1)·Δs		·Sigmoid'(ft)
		Δi = gt	 ·Δs		·Sigmoid'(it)
		Δo = st	 ·Δh_(t+1)·Sigmoid'(ot)
		------
		Δgfio_W += Δgfio×xc^T
		Δgfio_b += Δgfio
		------
		Δs = ot·Δh_(t+1) + Δs_(t+1)
		Δh = Δxc[Δx, Δh]
		Δxc = ΣW^T×Δ_gfio
*********************************************************************************/
class LstmLayer
{
public:
	Mat<> gate[4], weights[4], bias[4], diffWeights[4], diffBias[4];	// Gate[4]: G, F, I, O
	Mat<> s, h, xc;													// State,out
	std::vector<Mat<>> gateSet[4], prevSSet, prevHSet, xcSet;
	LstmLayer() { ; }
	LstmLayer(int inSize, int outSize) { init(inSize, outSize); }
	void init(int inSize, int outSize) {
		for (int i = 0; i < 4; i++) {
			gate		[i].zero (outSize);
			weights		[i].rands(outSize, outSize + inSize, -0.1, 0.1);
			bias		[i].rands(outSize, 1, -0.1, 0.1);
			diffWeights	[i].zero (weights[i]);
			diffBias	[i].zero (bias	 [i]);
		}
	}
	/*-------------------------------- 正向传播 --------------------------------*/
	std::vector<Mat<>>* operator()(std::vector<Mat<>>& in, Mat<>& s_prev, Mat<>& h_prev) { return forward(in, s_prev, h_prev); }
	std::vector<Mat<>>* forward   (std::vector<Mat<>>& in, Mat<>& s_prev, Mat<>& h_prev) {
		for (int i = 0; i < 4; i++)gateSet[i].clear();
		prevSSet.clear(); prevSSet.push_back(s_prev);
		prevHSet.clear(); prevHSet.push_back(h_prev);
		xcSet.   clear();
		for (int timeStep = 0; timeStep < in.size(); timeStep++) {
			forward(
				in   [timeStep], 
				prevSSet[timeStep], 
				prevHSet[timeStep]
			);
			for (int i = 0; i < 4; i++) 
				gateSet[i].push_back(gate[i]);
			prevSSet.push_back(s);
			prevHSet.push_back(h);
			xcSet.   push_back(xc);
		}
		s_prev = s;
		h_prev = h;
		return &prevHSet;
	}
	Mat<>* forward(Mat<>& in, Mat<>& prevS, Mat<>& prevH) {
		// G F I O
		xc.rowsStack(in, prevH);														//[h_(t-1), xt]
		for (int i = 0; i < 4; i++) { 													
			gate[i].add(gate[i].mul(weights[i], xc), bias[i]);
			i == 0 ?
				gate[i].function([](double x) { return tanh(x); }):						//gt = tanh(Wg×[h_(t-1), xt] + bg) 	
				gate[i].function([](double x) { return 1 / (1 + exp(-x)); });			//Ft, It, Ot = Sigmoid( W_ifo×[h_(t-1), xt] + b_ifo )
		}
		// S H
		Mat<> tmp;
		s.add(tmp.elementMul(gate[0], gate[2]), s.elementMul(prevS, gate[1]));		//st = gt·it + s_(t-1)·ft
		return &h.elementMul(gate[3], s);
		//h.elementMul(gate[3], tmp.function(s, [](double x) { return tanh(x); }));	//ht = ot·tanh(st)
	}
	/*-------------------------------- 反向传播 --------------------------------*/
	void backward(std::vector<Mat<>>& error, double learnRate) {
		Mat<> tmp, 
			  diffH(error[0].rows), 
			  diffS(error[0].rows);
		for (int timeStep = error.size() - 1; timeStep >= 0; timeStep--) {
			for (int i = 0; i < 4; i++) 
				gate[i] = gateSet[i][timeStep];
			s  = prevSSet[timeStep + 1];
			xc = xcSet[timeStep];
			backward(
				prevSSet[timeStep],
				error   [timeStep], 
				diffH, 
				diffS
			);
		}
		//Update
		for (int i = 0; i < 4; i++) {
			weights[i] += (diffWeights[i] *= -learnRate);
			bias   [i] += (diffBias   [i] *= -learnRate);
			diffWeights[i].zero();
			diffBias   [i].zero();
		}
	}
	void backward(Mat<>& prevS, Mat<>& error, Mat<>& diffH, Mat<>& diffS) {
		diffH += error;
		//notice that top_diffS is carried along the constant error carousel
		Mat<> diffGate[4], tmp;
		diffS.add(tmp.elementMul(gate[3], diffH), diffS);							//Δs = ot·Δh_(t+1) + Δs_(t+1) 
		diffGate[0].  elementMul(gate[2], diffS);									//Δg = it·Δs
		diffGate[1].  elementMul(prevS,   diffS);									//Δf = s_(t-1)·Δs
		diffGate[2].  elementMul(gate[0], diffS);									//Δi = gt·Δs
		diffGate[3].  elementMul(s,       diffH);									//Δo = st·Δh_(t+1)
		//arctive Derivative function												//Δgfio = Δgfio·σ'(gfio)
		for (int i = 0; i < 4; i++)
			i == 0 ?
			diffGate[i].elementMul(tmp.function(gate[i], [](double x) { return x * x > 0 ? 0 : 1 - x * x; })) :
			diffGate[i].elementMul(tmp.function(gate[i], [](double x) { return x * (1 - x); }));
		//diffs w.r.t. ins & compute bottom diff
		Mat<> diffXc(xc.rows);												//Δgfio_W += Δgfio×xc^T //Δgfio_b += Δgfio //Δxc = ΣW^T×Δ_gfio
		for (int i = 0; i < 4; i++) {
			diffWeights[i] += tmp.mul(diffGate[i], xc.transpose(tmp));
			diffBias   [i] += diffGate[i];
			diffXc += tmp.mul(weights[i].transpose(tmp), diffGate[i]);
		}
		diffS.elementMul(gate[1]);													//Δs = Δs·ft
		diffXc.block_(diffXc.rows - diffH.rows, diffXc.rows - 1, 0, 0, diffH);		//Δh
	}
	/*----------------[ save / load ]----------------*/
	void save(FILE* file) {
		for (int k = 0; k < 4; k++) {
			for (int i = 0; i < weights[k].size(); i++) fprintf(file, "%f ", weights[k][i]);
			for (int i = 0; i < bias   [k].rows;   i++) fprintf(file, "%f ", bias   [k][i]);
		} fprintf(file, "\n");
	}
	void load(FILE* file) {
		for (int k = 0; k < 4; k++) {
			for (int i = 0; i < weights[k].size(); i++) fscanf(file, "%lf", &weights[k][i]);
			for (int i = 0; i < bias   [k].rows;   i++)	fscanf(file, "%lf", &bias   [k][i]);
		}
	}
};
/*************************************************************************************************
*							BatchNorm
*************************************************************************************************/
class BatchNorm {
public:
	int outChannelNum;
	double eps;
	BatchNorm() { ; }
	BatchNorm(int _outChannelNum, double _eps) { init(_outChannelNum, _eps); }
	void init(int _outChannelNum, double _eps) {
		outChannelNum = _outChannelNum;
		eps = _eps;
	}
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward   (Tensor<>& in) {
	}
	/*----------------[ backward ]----------------*/
};
/*################################################################################################

*						经典神经网络模块

################################################################################################*/
/*************************************************************************************************
*							BasicConv 基础卷积层 模块
*	[组成]:	[1] ConvLayer	[2] BatchNorm	[3] ReLU 
*	[作用]: 每一层数据拉回均值0，方差1的标准正态分布
*************************************************************************************************/
class BasicConv {
public:
	ConvLayer conv;
	BatchNorm bn;
	Tensor<>  out;
	BasicConv() { ; }
	BasicConv(int _inChannelNum, int _outChannelNum, int kernelSize, int _padding, int _stride) {
		init(_inChannelNum, _outChannelNum, kernelSize, _padding, _stride);
	}
	void init(int _inChannelNum, int _outChannelNum, int kernelSize, int _padding, int _stride) {
		conv.init(_inChannelNum, _outChannelNum, kernelSize, _padding, _stride);
		bn	.init(_outChannelNum, 0.001);
	}
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward   (Tensor<>& in) { (out = *bn(*conv(in))).function(relu); }
	/*----------------[ backward ]----------------*/

	/*----------------[ save/load ]----------------*/
	void save(FILE* file) { conv.save(file); }
	void load(FILE* file) { conv.load(file); }
};


/*************************************************************************************************
*							Inception  模块
*	[Author]: Google.Going deeper with convolutions.2014.
*************************************************************************************************/
class Inception {
public:
	BasicConv b1, b2_1x1_a, b2_3x3_b, b3_1x1_a, b3_3x3_b, b3_3x3_c, b4_1x1;
	PoolLayer b4_pool;
	Tensor<> out;
	/*----------------[ init ]----------------*/
	Inception() { ; }
	Inception(int inChannelNum, int n1x1, int n3x3red, int n3x3, int n5x5red, int n5x5, int poolChannelNum) {
		init(inChannelNum, n1x1, n3x3red, n3x3, n5x5red, n5x5, poolChannelNum);
	}
	void init(int inChannelNum, int n1x1, int n3x3red, int n3x3, int n5x5red, int n5x5, int poolChannelNum) {
		b1.		 init(inChannelNum, n1x1,			1, 0, 1);	// 1x1 conv branch
		b2_1x1_a.init(inChannelNum, n3x3red,		1, 0, 1);	// 1x1 conv -> 3x3 conv branch
		b2_3x3_b.init(n3x3red,		n3x3,			3, 1, 1);
		b3_1x1_a.init(inChannelNum, n5x5red,		1, 0, 1);	// 1x1 conv -> 3x3 conv -> 3x3 conv branch
		b3_3x3_b.init(n5x5red,		n5x5,			3, 1, 1);
		b3_3x3_c.init(n5x5,			n5x5,			3, 1, 1);
		b4_pool. init(3, 0, 3, b4_pool.M);						// x3 pool -> 1x1 conv branch
		b4_1x1.  init(inChannelNum, poolChannelNum, 1, 0, 1);
	};
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward(Tensor<>& in) {
		Tensor<>* y1 = b1								(in);
		Tensor<>* y2 = b2_3x3_b(*b2_1x1_a				(in));
		Tensor<>* y3 = b3_3x3_c(*b3_3x3_b(*b3_1x1_a	(in)));
		Tensor<>* y4 = b4_1x1  (*b4_pool				(in));
		Tensor<>* t[]{ y1, y2, y3, y4 };
		return &out.merge(t, 4, 1);
	}
	/*----------------[ backward ]----------------*/

	/*----------------[ save/load ]----------------*/
	void save(FILE* file) {
		b1.		 save(file);
		b2_1x1_a.save(file); b2_3x3_b.save(file);
		b3_1x1_a.save(file); b3_3x3_b.save(file); b3_3x3_c.save(file);
		b4_1x1.  save(file);
	}
	void load(FILE* file) {
		b1.		 load(file);
		b2_1x1_a.load(file); b2_3x3_b.load(file);
		b3_1x1_a.load(file); b3_3x3_b.load(file); b3_3x3_c.load(file);
		b4_1x1.  load(file);
	}
};
/*********************************************************************************
						ResBlock 残差模块
*	正向传播:
		xList, nodeList: 保持运行过程数据, 在反向传播中使用
*********************************************************************************/
class ResBlock {
public:
	ConvLayer conv1, conv2;
	BatchNorm bn1 ,bn2;
	Tensor<>  out;
	ResBlock() { ; }
	ResBlock (int inChannelNum, int ChannelNum, int stride) { init(inChannelNum, ChannelNum, stride); }
	void init(int inChannelNum, int ChannelNum, int stride) {
		conv1.init(inChannelNum, ChannelNum, 3, 1, stride);
		conv2.init(  ChannelNum, ChannelNum, 3, 1, 1);
		bn1  .init(ChannelNum, 0.001);
		bn2  .init(ChannelNum, 0.001);
	}
	/*----------------[ forward ]----------------*/
	Tensor<>* operator()(Tensor<>& in) { return forward(in); }
	Tensor<>* forward   (Tensor<>& in) {
	   (out = *bn1(*conv1(in))).function(relu);
		out = *bn2(*conv2(in));
		return &((out += in).function(relu));
	}
	/*----------------[ backward ]----------------*/

	/*----------------[ save ]----------------*/
	void save(FILE* file) { conv1.save(file); conv2.save(file); }
	void load(FILE* file) { conv1.load(file); conv2.load(file); }
};

/*################################################################################################

*						Some Classical NeuralNetworks  经典神经网络结构

################################################################################################*/

/*************************************************************************************************
*							Back Propagation NeuralNetworks  反向传播神经网络
*	[Author]: 1986.Rumelhart,McClelland
*************************************************************************************************/
#include <vector>
class BackPropagationNeuralNetworks {
public:
	std::vector<NeuralLayer*> layer;
	Mat<> preIn;
	Mat<>&(*lossFunc)(Mat<>& y, Mat<>& target, Mat<>& error) = QuadraticLossFuncD;
	/*----------------[ set Layer ]----------------*/
	void addLayer(int inSize, int outSize) { layer.push_back(new NeuralLayer(inSize, outSize, sigmoid, sigmoidD)); }
	void setLayer(int index, int inSize, int outSize) {
		if (index >= layer.size()) exit(-1);
		delete layer[index];
		layer[index] = new NeuralLayer(inSize, outSize, sigmoid, sigmoidD);
	}
	/*----------------[ forward ]----------------*/
	Mat<>& operator()(Mat<>& in, Mat<>& out) { return forward(in, out); }
	Mat<>& forward   (Mat<>& in, Mat<>& out) {
		Mat<>* y = (*layer[0])(in);
		for (int i = 1; i < layer.size(); i++) y = (*layer[i])(*y);
		preIn = in;
		return out = *y;
	}
	/*----------------[ backward ]----------------*/
	void backward(Mat<>& target, double learnRate = 0.01) {
		Mat<> error; lossFunc(layer.back()->out, target, error);
		for (int i = layer.size() - 1; i >= 0; i--)
			layer[i]->backward(
				i == 0 ? preIn : layer[i - 1]->out, 
				error,
				learnRate
			);
	}
	/*----------------[ save/load ]----------------*/
	void save(const char* saveFile) {
		FILE* file = fopen(saveFile, "w");
		for (int i = 0; i < layer.size(); i++) layer[i]->save(file);
		fclose(file);
	}
	void load(const char* loadFile) {
		FILE* file = fopen(loadFile, "r");
		for (int i = 0; i < layer.size(); i++) layer[i]->load(file);
		fclose(file);
	}
};
/*************************************************************************************************
*							LeNet NeuralNetworks  LeNet卷积神经网络
*	[Author]: 1998.Yann LeCun
*************************************************************************************************/
class LeNet {
public:
	ConvLayer 
		Conv_1{  1, 16, 5, 2, 1 }, 
		Conv_2{ 16, 32, 5, 2, 1 };
	PoolLayer 
		MaxPool_1{ 2, 0, 2, MaxPool_1.M }, 
		MaxPool_2{ 2, 0, 2, MaxPool_2.M };
	NeuralLayer 
		FullConnect_1{ 32 * 7 * 7, 128 }, 
		FullConnect_2{ 128, 64 }, 
		FullConnect_3{  64, 10 };
	Mat<>& (*lossFunc)(Mat<>& y, Mat<>& target, Mat<>& error) = QuadraticLossFuncD;
	Tensor<> preIn;
	LeNet() { ; }
	/*----------------[ forward ]----------------*/
	Mat<>& operator()(Tensor<>& in, Mat<>& out) { return forward(in, out); }
	Mat<>& forward   (Tensor<>& in, Mat<>& out) {
		preIn = in;
		Tensor<>* y; 
		y = MaxPool_1(*Conv_1(in)); 
		y = MaxPool_2(*Conv_2(*y)); 
		Tensor<> t = *y; 
		Mat<> t2(t.size()); t2.data = t.data; t.data = NULL;
		Mat<>* maty = &t2;
		maty = FullConnect_1(*maty);
		maty = FullConnect_2(*maty);
		maty = FullConnect_3(*maty);
		return out = *maty;
	}
	/*----------------[ backward ]----------------*/
	void backward(Mat<>& target, double learnRate = 0.01) {
		Mat<> error;
		lossFunc(FullConnect_3.out, target, error);
		FullConnect_3.backward(FullConnect_2.out, error, learnRate);
		FullConnect_2.backward(FullConnect_1.out, error, learnRate);
		FullConnect_1.backward(FullConnect_1.out, error, learnRate);
		printf("<>");
		Tensor<> error2(7, 7, 32); free(error2.data); error2.data = error.data; error.data = NULL; error.zero(1);
		printf("<>");
		MaxPool_2.backward(Conv_2.out, error2);	Conv_2.backward(MaxPool_1.out, error2, learnRate);
		MaxPool_1.backward(Conv_1.out, error2);	Conv_1.backward(preIn,         error2, learnRate);
	}
	/*----------------[ save/load ]----------------*/
	void save(const char* saveFile) {
		FILE* file = fopen(saveFile, "w");
		Conv_1.		  save(file);
		Conv_2.		  save(file);
		FullConnect_1.save(file);
		FullConnect_2.save(file);
		FullConnect_3.save(file);
		fclose(file);
	}
	void load(const char* loadFile) {
		FILE* file = fopen(loadFile, "r");
		Conv_1.		  load(file);
		Conv_2.		  load(file);
		FullConnect_1.load(file);
		FullConnect_2.load(file);
		FullConnect_3.load(file);
		fclose(file);
	}
};
/*************************************************************************************************
*							GoogLeNet  卷积神经网络
*	[Author]: Google.Going deeper with convolutions.2014.
*************************************************************************************************/
class GoogLeNet {
public:
	Inception a3, b3, a4, b4, c4, d4, e4, a5, b5;
	ConvLayer preLayers;
	PoolLayer maxpool, avgpool;
	NeuralLayer linear;
	GoogLeNet() {
		preLayers.init(1, 64, 5, 1, 2);
		a3.init( 64,  64,  96, 128, 16,  32,  32);
		b3.init(256, 128, 128, 192, 32,  96,  64);
		maxpool.init(3, 2, 1, avgpool.M);
		a4.init(480, 192,  96, 208, 16,  48,  64);
		b4.init(512, 160, 112, 224, 24,  64,  64);
		c4.init(512, 128, 128, 256, 24,  64,  64);
		d4.init(512, 112, 144, 288, 32,  64,  64);
		e4.init(528, 256, 160, 320, 32, 128, 128);
		a5.init(832, 256, 160, 320, 32, 128, 128);
		b5.init(832, 384, 192, 384, 48, 128, 128);
		avgpool.init(8, 8, 0, avgpool.A);
		linear. init(1024 * 5 * 5, 7, relu, reluD);
	}
	/*----------------[ forward ]----------------*/
	Mat<>& operator()(Tensor<>& in, Mat<>& out) { return forward(in, out); }
	Mat<>& forward   (Tensor<>& in, Mat<>& out) {
		Tensor<>* y;
		y = preLayers(in);
		y = b3(*a3 (*y));
		y = maxpool(*y);
		y = e4(*d4(*c4(*b4(*a4(*y)))));
		y = maxpool(*y);
		y = b5(*a5 (*y));
		y = avgpool(*y);
		Tensor<> t = *y;
		Mat<> t2(t.dim.product()); t2.data = t.data; t.data = NULL;
		Mat<>* maty = &t2;
		maty = linear(*maty);
		return out = *maty;
	}
	/*----------------[ save/load ]----------------*/
	void save(const char* saveFile) {
		FILE* file = fopen(saveFile, "w");
		preLayers.save(file);
		a3.save(file); b3.save(file);
		a4.save(file); b4.save(file); c4.save(file); d4.save(file); e4.save(file);
		a5.save(file); b5.save(file);
		linear.save(file);
		fclose(file);
	}
	void load(const char* loadFile) {
		FILE* file = fopen(loadFile, "r");
		preLayers.load(file);
		a3.load(file); b3.load(file);
		a4.load(file); b4.load(file); c4.load(file); d4.load(file); e4.load(file);
		a5.load(file); b5.load(file);
		linear.load(file);
		fclose(file);
	}
};
/*********************************************************************************
						LSTM 长短期记忆网络
*	正向传播:
		xList, nodeList: 保持运行过程数据, 在反向传播中使用
*********************************************************************************/
class LstmNetwork
{
public:
	LstmLayer lstm;
	NeuralLayer nn;
	Mat<> prevLstmS, prevLstmH;
	std::vector<Mat<>> out;
	double learnRate = 0.005;
	LstmNetwork(int inSize, int LstmOutputSize, int outSize) {
		lstm.init(inSize, LstmOutputSize);
		nn.  init(LstmOutputSize, outSize, [](double x) { return x; }, [](double x) { return 1.0; });
		prevLstmS.zero(LstmOutputSize);
		prevLstmH.zero(LstmOutputSize);
	}
	/*-------------------------------- 正向传播 --------------------------------*/
	std::vector<Mat<>>& operator()(std::vector<Mat<>>& in) { return forward(in); }
	std::vector<Mat<>>& forward   (std::vector<Mat<>>& in) {
		*lstm(in, prevLstmS, prevLstmH);
		out.clear();
		for (int i = 0; i < in.size(); i++)
			out.push_back(*nn(lstm.prevHSet[i + 1]));
		return out;
	}
	/*-------------------------------- 反向传播 --------------------------------*/
	double backward(std::vector<Mat<>>& target) {
		std::vector<Mat<>> errorSet;
		Mat<> error;
		for (int i = 0; i < target.size(); i++) {
			nn.backward(
				lstm.prevHSet[i + 1],
				error.mul(-2, error.sub(target[i], out[i])),
				learnRate
			);
			errorSet.push_back(error);
		}
		lstm.backward(errorSet, learnRate);
	}
	/*----------------[ save/load ]----------------*/
	void save(const char* saveFile) {
		FILE* file = fopen(saveFile, "w");
		lstm.save(file);
		nn.  save(file);
		fclose(file);
	}
	void load(const char* loadFile) {
		FILE* file = fopen(loadFile, "r");
		lstm.load(file);
		nn.  load(file);
		fclose(file);
	}
};
#endif