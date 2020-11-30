# Daiyu-Go	黛玉-Go 
The AI of [Go],[GoBang] named "Daiyu黛玉".
## [1].Go 围棋  
* core: <go.h/cpp>
* 核心思想: "频率估计概率"  
           用一千万次随机下子,估计每一点的胜率,来寻找最优的落子点。  
		   (十万场,平均每场一百步)  
* 核心算法:  
	* 蒙特卡洛树搜索MCTS  
		[1] 选择 | 依据UCB公式,选择节点,兼顾Exploit & Explore  
		[2] 拓展 | 扩展新的树节点  
		[3] 模拟 | 对新增节点进行随机走子,直至终盘,得到一场随机比赛结果  
		[4] 回溯 | 将模拟所得输赢结果,从新增节点开始向根节点回溯(更新节点权值)  
* 围棋规则:  
    [1]无气提子 [2]非提禁入 [3]劫隔一手 [4]数子法判定输赢  
	[other] 真眼活棋  

* 注.目前围棋一子思考时间是27秒以内,暂无游戏体验感,后期进一步优化。  
     五子棋速度可以。  

## [2].GoBang 五子棋  
* core: <gobang.h/cpp>  
* The program of GoBang-AI is in the src/gobang.cpp and gobang.h.   
* The algorithm of GoBang-AI is "Minimax".  
* The UI is built by QT.  
* ------------------  
* core: <gobang.h/cpp>  
* 算法:  
	* 决策树, 深度优先搜索, 遍历4层所有下棋情况
	* 剪枝算法:   
		(1)alpha-beta剪枝算法   
		(2)四周无子格点直接略过。(不像围棋有"飞子")   
		(3)赢盘连五中止搜索。   
	* Policy():决策函数/博弈树(极小化极大值算法)  
	* Evaluate():评价函数，棋局分数评价判断。 

## [3].Chess 国际象棋
* core: <chess.h/cpp>  
* 未完成, 待续

## Image
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/vs%E9%87%8E%E7%8B%90%E4%B8%AD%E4%B8%8B12%E7%BA%A7.png)  
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/20201122182136.png)  
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/20201122183254.png)  

## 核心函数API  
### [MCTS]核心函数  
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
### 围棋规则函数  
```
	bool downStone(State* state);									//[RULE 1]:无气提子 //落子
	void ComputerQi(STONE* map, INT8U qi[], INT8U chBlockMap[]);	//棋块数气
	void judgeNotPoint(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]);	//[RULE 2]:非提禁入 //一方禁入点标记
	bool judgeJie(STONE* map, STONE stone, INT8S x0, INT8S y0,INT8U qi[], INT8U chBlockMap[], INT8S Jie0[]);	//[RULE 3]:劫停一手//判定劫
	void judgeEye(STONE* map, INT8U qi[], INT8U chBlockMap[]);
	void judgeEyeAndNot(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]);
	int judgeWin(State* state);										//[RULE 4]:局势判定(数子法)
```
  
## Other    
###  
* MCTS[3]模拟阶段，随机落子至满盘，  
9x9平均可落120手  
19x19平均可落500手  
  
###   
* 蒙特卡洛树，通过析构函数，实现子节点内存递归释放。  
* free()无法调用释放对象的析构函数，而delete可以。  