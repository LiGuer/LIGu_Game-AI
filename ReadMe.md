# LiGu_AI	黛玉
An AI named 黛玉.
## [0].算法
* 文件:  
<NeuralNetworks.h>			神经网络算法   
<MiniMax.h>				MiniMax算法  
<MontecarloTreeSearch.h> 	Montecarlo树搜索算法  
### <NeuralNetworks.h> 神经网络类
```
核心类:
class NeuralLayer	(int inputSize, int outputSize)	//神经网络层
class ConvLayer		(int inChannelNum, int outChannelNum,int kernelSize,int padding,int stride)	//卷积层
class PoolLayer		(int kernelSize,   int padding, int stride, int poolType)					//下采样层
class LstmLayer		(int inputSize, int outputSize)	//长短期记忆层
--------------------------------------------------------------------------------------------------
经典结构:
class BackPropagation_NeuralNetworks()				//反向传播神经网络 : 1986.Rumelhart,McClelland
class LeNet_NeuralNetworks()						//LeNet卷积神经网络 : 1998.Yann LeCun
class Inception()									//Inception模块 : 2014.Google
class GoogLeNet_NeuralNetworks()					//GoogLeNet卷积神经网络 : 2014.Google
class LstmNetwork()									//LSTM长短期记忆网络
```
### <MiniMax.h> MiniMax算法:  
* 决策树, 深度优先搜索, 遍历K层所有下棋情况  
* alpha-beta剪枝  
* 	[流程]:  
		[0] 到达层数阈值, 返回当前状态的评价(评价函数)  
		[1] 基于当前状态, 生成新状态, 对于每一个可能的新状态  
		[2] 递归决策函数, 深度优先搜索生成&遍历博弈树  
		[3] 收尾  
		[3.1] 单数博弈层(对手层) 取最小  
		[3.2] 双数博弈层(我  层) 取最大  
		[3.3] alpha-beta剪枝  
```
	MCTSNode* MCTS(MCTSNode* node);									//[MCTS]蒙特卡洛树搜索
	MCTSNode* Select(MCTSNode* node, bool isExploration);			//[1]选择最优节点
	bool Expand(MCTSNode* node, MCTSNode** newnode);				//[2]拓展新节点
	int Simulation(State* state0);									//[3]模拟,快速走子
	void Backpropagation(MCTSNode* node, int reward);				//[4]回溯
	double UCB(MCTSNode* node, double C);							//UCB公式
	MCTSNode* TreePolicy(MCTSNode* node);							//[1]+[2]
	bool nextStateRand(State* state);								//随机走子
```
### <MontecarloTreeSearch.h> Montecarlo树搜索:  
* 核心思想: "频率估计概率"  
	用一千万次随机下子,估计每一点的胜率,来寻找最优的落子点。  
	(十万场,平均每场一百步)  
* 核心算法:  
	[1] 选择 | 依据UCB公式,选择节点,兼顾Exploit & Explore  
	[2] 拓展 | 扩展新的树节点  
	[3] 模拟 | 对新增节点进行随机走子,直至终盘,得到一场随机比赛结果  
	[4] 回溯 | 将模拟所得输赢结果,从新增节点开始向根节点回溯(更新节点权值)  
* Other: Montecarlo树，通过析构函数，实现子节点内存递归释放。 free()无法调用释放对象的析构函数，而delete可以。  
## [1].Example
### [1.1].Go 围棋  
* 文件:	<go.h>
* 算法:  Montecarlo树搜索
* 围棋规则:  
	[1]无气提子 
	[2]非提禁入 
	[3]劫隔一手 
	[4]数子法判定输赢  
	[other] 真眼活棋  
* Other: MCTS[3]模拟阶段，随机落子至满盘，9x9平均可落120手, 19x19平均可落500手  
```
	bool downStone(State* state);									//[RULE 1]:无气提子 //落子
	void ComputerQi(STONE* map, INT8U qi[], INT8U chBlockMap[]);	//棋块数气
	void judgeNotPoint(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]);	//[RULE 2]:非提禁入 //一方禁入点标记
	bool judgeJie(STONE* map, STONE stone, INT8S x0, INT8S y0,INT8U qi[], INT8U chBlockMap[], INT8S Jie0[]);	//[RULE 3]:劫停一手//判定劫
	void judgeEye(STONE* map, INT8U qi[], INT8U chBlockMap[]);
	void judgeEyeAndNot(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]);
	int judgeWin(State* state);										//[RULE 4]:局势判定(数子法)
```
### [1.2].GoBang 五子棋  
* 文件:	<gobang.h>  
* 算法:  
	* 决策树, 深度优先搜索, 遍历K层所有下棋情况
	* 剪枝算法:   
		(1)alpha-beta剪枝算法   
		(2)四周无子格点直接略过。(不像围棋有"飞子")   
		(3)赢盘连五中止搜索。   
	* Policy():决策函数/博弈树(极小化极大值算法)  
	* Evaluate():评价函数，棋局分数评价判断。 

### [1.3].Chess 象棋
* 文件:	<chess.h>  
* 算法:  Montecarlo树搜索
## Image
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/vs%E9%87%8E%E7%8B%90%E4%B8%AD%E4%B8%8B12%E7%BA%A7.png)  
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/20201122182136.png)  
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/20201122183254.png)  