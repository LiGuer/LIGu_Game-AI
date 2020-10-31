# Daiyu-Go 黛玉-Go  
* A Go AI of LiGu name "Daiyu-Go 黛玉-Go"  
* 核心:  
    [1] 蒙特卡洛树搜索 MCTS  
    [2] 围棋规则：  
        (1) 无气提子  
        (2) 非提禁入  
        (3) 劫隔一手  
        (4) 数子法判定输赢  
* 关键程序API：  
### [MCTS]核心函数  
```
	MCTSNode* MCTS(MCTSNode* node);									//[MCTS]蒙特卡洛树搜索  
	MCTSNode* Select(MCTSNode* node, bool isExploration);			//[1]选择最优节点  
	bool Expand(MCTSNode* node, MCTSNode** newnode);								//[2]拓展新节点  
	int Simulation(State* state0);									//[3]模拟,快速走子  
	void Backpropagation(MCTSNode* node, int reward);				//[4]回溯  
	double UCB(MCTSNode* node, double C);							//UCB公式  
	MCTSNode* TreePolicy(MCTSNode* node);							//  
	bool nextStateRand(State* state);//随机走子  
```
### 围棋规则函数  
```
	bool downStone(STONE* map, INT8S x0, INT8S y0, STONE stone);	//[RULE 1]:无气提子 //落子  
	void ComputerQi(STONE* map, INT8U qi[], INT8U chBlockMap[]);	//棋块数气  
	void judgeNotPoint(STONE* map, STONE stone, INT8U qi[], INT8U chBlockMap[]);//[RULE 2]:非提禁入 //一方禁入点标记  
	bool judgeJie(STONE* map, STONE stone, INT8S x0, INT8S y0,INT8U qi[], INT8U chBlockMap[], INT8S Jie0[]);	//[RULE 3]:劫停一手//判定劫  
	void judgeEye(STONE* map, INT8U qi[], INT8U chBlockMap[]);  
	int judgeWin(STONE* map);										//[RULE 4]:局势判定(数子法)  
```
  
## 日志  
###  
随机落子至满盘，  
9x9平均可落120手  
19x19平均可落500手  
  
###  
围棋输赢判断规则不具完备性，需要对弈双方主观确认，"双活"在程序设计里是个巨坑....  
理应围棋下到不能下为止，但为了节省时间，才出现了死棋就不再下，胜率不高就投子认输的方法。  
但这就对编程，产生了极大的考验  

### 
【蒙特卡洛树搜索 · 9格】 五子棋测试。成功了。
1.蒙特卡洛的问题，
在于19格无法实现"频率估计概率"，
需要加大搜索步数，耗时可能指数级增长。
2.将五子棋的判赢函数，改成围棋，
即实现9子围棋AI，下一步目标。

###  
通过[任务管理器]监测内存。
Okey，蒙特卡洛树，内存实时释放成功。
注.
free()无法调用释放对象的析构函数，
而delete可以。
