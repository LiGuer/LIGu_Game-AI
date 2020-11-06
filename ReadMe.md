# Daiyu-Go	黛玉-Go 
The AI of [Go],[GoBang] named "Daiyu黛玉".
## [1].Go 围棋  
* 核心程序: <go.h/cpp>
* 核心算法:  
    [1] 蒙特卡洛树搜索MCTS  
		[1]选择 [2]拓展 [3]模拟 [4]回溯
    [2] 围棋规则：  
        [1]无气提子 [2]非提禁入 [3]劫隔一手 [4]数子法判定输赢
		[other] 真眼活棋 
* 关键程序API：  
### [MCTS]核心函数  
```
	MCTSNode* MCTS(MCTSNode* node);									//[MCTS]蒙特卡洛树搜索  
	MCTSNode* Select(MCTSNode* node, bool isExploration);			//[1]选择最优节点  
	MCTSNode* Expand(MCTSNode* node);								//[2]拓展新节点  
	int Simulation(State* state0);									//[3]模拟,快速走子  
	void Backpropagation(MCTSNode* node, int reward);				//[4]回溯  
    double UCB(MCTSNode* node, double C);							//UCB公式  
	MCTSNode* TreePolicy(MCTSNode* node);							//  
	bool nextStateRand(CHESS* map, INT8S& x0, INT8S& y0, CHESS chess);//随机走子  
```
### 围棋规则函数  
```
	bool downChess(CHESS* map, INT8S x0, INT8S y0, CHESS chess);	//[RULE 1]:无气提子//落子
	void judgeNotPoint(CHESS* map, CHESS chess);					//[RULE 2]:非提禁入
	int judgeWin(CHESS* map);										//[RULE 4]:局势判定(数子法)
	void ComputerQi(CHESS* map, INT8U qi[], INT8U chBlockMap[]);	//棋块数气
	void GoAI(CHESS* map, INT8S player, INT8S& x0, INT8S& y0);  
```
## [2].GoBang 五子棋  
* core: <gobang.h/cpp>
* The program of GoBang-AI is in the src/gobang.cpp and gobang.h.  
* The algorithm of GoBang-AI is "Minimax".
* The UI is built by QT.  
* ------------------
* 核心: <gobang.h/cpp>
* 原理: 本质是深度优先搜索。遍历4层所有下棋情况。 
* 剪枝算法：   
	(1)alpha-beta剪枝算法   
	(2)四周无子格点直接略过。(不像围棋有"飞子")   
	(3)赢盘连五中止搜索。   
* 核心程序：   
	[1]Policy():决策函数/博弈树(极小化极大值算法)  
	[2]Evaluate():评价函数，棋局分数评价判断。 
  
## Image
![Image text](https://github.com/LiGuer/Daiyu-Go/blob/master/contest/vs%E9%87%8E%E7%8B%90%E4%B8%AD%E4%B8%8B12%E7%BA%A7.png)  

## Other    
###  
* MCTS[3]模拟阶段，随机落子至满盘，  
9x9平均可落120手  
19x19平均可落500手  

###  
蒙特卡洛树，通过析构函数，实现子节点内存递归释放。  
* free()无法调用释放对象的析构函数，而delete可以。  