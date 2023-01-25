* Go
  - Rules
    * Tromp-Taylor Rules
      1. Go is played on a 19x19 square grid of points, by two players called Black and White.
      1. Each point on the grid may be colored black, white or empty.
      1. A point P, not colored C, is said to reach C, if there is a path of (vertically or horizontally) adjacent points of P’s color from P to a point of color C.
      1. Clearing a color is the process of emptying all points of that color that don’t reach empty.
      1. Starting with an empty grid, the players alternate turns, starting with Black.
      1. A turn is either a pass; or a move that doesn’t repeat an earlier grid coloring.  
      1. A move consists of coloring an empty point one’s own color; then clearing the opponent color, and then clearing one’s own color.
      1. The game ends after two consecutive passes.
      1. A player’s score is the number of points of her color, plus the number of empty points that reach only her color.
      1. The player with the higher score at the end of the game is the winner. Equal scores result in a tie.

    * 中国围棋规则  
      1. 围棋的棋具
         - 棋盘。棋盘由纵横各19条等距离、垂直交叉的平行线构成。形成361个交叉点，简称为“点”。
         - 棋子。棋子分黑白两色。
      1. 围棋的下法
         - 对局双方各执一色棋子。
         - 空枰开局。
         - 黑先白后，交替着一子于棋盘的点上。
         - 棋子下定后，不再向其他点移动。
         - 轮流下子是双方的权利，但允许任何一方放弃下子权而使用虚着。
      1. 棋子的气。一个棋子在棋盘上，与它直线紧邻的空点是这个棋子的“气”。直线紧邻的点上如果有同色棋子存在，这些棋子就相互连接成一个不可分割的整体。直线紧邻的点上如果有异色棋子存在，此处的气便不存在。棋子如失去所有的气，就不能在棋盘上存在。
      1. 提子。把无气之子清理出棋盘的手段叫“提子”。提子有二种：
         - 下子后，对方棋子无气，应立即提取对方无气之子。
         - 下子后，双方棋子都呈无气状态，应立即提取对方无气之子。
      1. 禁着点。棋盘上的任何一点，如某方下子后，该子立即呈无气状态，同时又不能提取对方的棋子，这个点叫做“禁着点”。
      1. 禁止全局同形。着子后不得使对方重复面临曾出现过的局面。
      1. 终局
         - 棋局下到双方一致确认着子完毕时，为终局。
         - 对局中有一方中途认输时，为终局。
         - 双方连续使用虚着，为终局。
      1. 活棋与死棋
         - 终局时，经双方确认，不能被提取的棋都是活棋。
         - 终局时，经双方确认，能被提取的棋都是死棋。
      1. 计算胜负。着子完毕的棋局，采用数子法计算胜负。将双方死子清理出盘外后，对任意一方的活棋和活棋围住的点以子为单位进行计数。双方活棋之间的空点各得一半。棋盘总点数的一半180.5点为归本数。一方总得点数超过此数为胜，等于此数为和，小于此数为负。

  - Mathematical Description
    - Concept
      - Board & Color  
        Board is ```a 19x19 square grid of points,```  
        Player: ```by two players called Black and White.```  
        Color: ```Each point on the grid may be colored black, white or empty.```

        $$\boldsymbol B \in \{0, 1, -1\}^{n \times n}, n = 19  \tag{Board}$$
        $$c \in \{1, -1\} \tag{Color}$$

        |Name|Number|
        |---|---|
        |Empty| 0 |
        |Black| 1 |
        |White| -1 |
        |||
        
      - Reach  
        ```A point P, not colored C, is said to reach C, if there is a path of (vertically or horizontally) adjacent points of P’s color from P to a point of color C.```

        $$\text{adj}(x, y) = \{(x+1, y), (x, y+1), (x-1, y), (x, y-1)\}  \tag{Adjacent}$$

        - Stone Block
          Stone Block is consisted of adjecent stones of the same color.

        - Qi
          Qi is a number of empty points that a stone block can reach.
            
    - $\mathcal S$ State 
      $$S = (\boldsymbol B) \tag{State Set}$$

      - $S_0$ Initial State  
        ```Starting with an empty grid.```

        $$\begin{align*}
          \boldsymbol B_0 &= \boldsymbol 0  \tag{空枰开局}
        \end{align*}$$

      - $S_{\text{end}}$  Termination Status  
        ```The game ends after two consecutive passes.```
        $$a_{\text{end-1}} = a_{\text{end}} = \text{PASS}$$

    - $\mathcal A$ Action   
      - ```A turn is either a pass; or a move```
        $$\mathcal A = \{(\text{PASS}, c)\} \cup \{(x, y, c) \ |\ x \in 1:n,\ y \in 1:n,\ c \in \{1, -1\},\ B(x, y) = 0\}  \tag{Action Set}$$

      - $A_0$ Initial Action  
        ```starting with Black.```
        $$A_{0,c} = 1  \tag{黑先白后}$$

      - ```the players alternate turns.```
        $$A_{k,c} = -A_{k-1,c}  \tag{交替落子}$$

      - ```禁止全局同形```, ```that doesn’t repeat an earlier grid coloring.```
        $$\boldsymbol B_k \neq \boldsymbol B_{k-i}  \quad,  \forall 0 < i \le k  \tag{禁全同}$$

    - $S_{k-1} \overset{A_{k-1}}{\rightarrow} S_{k}$ Status update   
      - ```A move consists of coloring an empty point one’s own color;```
        $$\boldsymbol B(a_{k,x}, a_{k,y}) \gets a_{k,c}  \tag{空地落子}$$ 
 
      - ```then clearing the opponent color, and then clearing one’s own color.```, ```Clearing a color is the process of emptying all points of that color that don’t reach empty.```
        $$\boldsymbol B((u_x, u_y) \ |\ Q(U) = 0, U_c \neq a_{k,c}) \gets 0  \tag{无气提子}$$

    - $R(S_{\text{end}})$ Reward, Victory or Defeat  
      - ```A player’s score is the number of points of her color, plus the number of empty points that reach only her color.```
      - In order balance the first-hand advantage, the black gives white 7.5 points. 
      - ```The player with the larger score at the end of the game is the winner.```

        $$R_{\text{Black}}(S) = -R_{\text{White}}(S)  \tag{Zero-Sum Game}$$

        $$R_{\text{Black}}(S) = - \frac{7.5}{2} + \sum_{\begin{matrix}\tiny{(x, y) \in (1:n, 1:n)} \\ \tiny{\boldsymbol B(x, y) = 0}\end{matrix}}(x, y, S)  \tag{数子}$$ 

        $$I_{\text{Black}}(x, y, S) = \left\{\begin{matrix} c & c \in \text{reach}(x, y), -c \notin \text{reach}(x, y) \\ \frac{1}{2} & other.\end{matrix}\right.$$

