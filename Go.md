* Go
  - Concept
    - 棋盘  
      A matrix whose value is $\{0, 1, -1\}$.
      $$\boldsymbol B \in \{0, 1, -1\}^{n \times n}, n = 19$$

    - 棋子  
      棋子分黑白两色，对局双方各执一色棋子。
      $$s = (x, y, c) \in (1:n, 1:n, \{1, -1\})  \tag{棋分黑白}$$

    - 棋块  
      棋块，是由直线紧邻的同色棋子$s, s'$，相互连接成一个连通的整体$S$。
      $$\text{adj}(x, y) = \{(x+1, y), (x, y+1), (x-1, y), (x, y-1)\}  \tag{紧邻}$$
      $$(s'_x, s'_y) \in \text{adj}(s_x, s_y),  s'_c = s_c \quad \Rightarrow \quad s \in S(s')  \tag{棋块 S}$$

      - Algorithm : 棋块 Detection  
        Using Union-Find Set to computer connected blocks.

    - 气  
      气，是与棋块直线紧邻的空点，一个空点只算一次。  
      $$Q(S) = \sum_{(x, y) \in (1:n, 1:n)} I(x, y, S)$$
      $$I(x, y, S) = \left\{\begin{matrix} 1 & B[x, y] = 0, (x, y) \in \text{adj}(s_x, s_y), s \in S \\ 0 & other.\end{matrix}\right.$$

      - Algorithm : 数气  
        对于棋盘上的每一个空点，将其紧邻的棋块的气 +1.

  - Rule 
    - 空枰开局，交替落子  
      空枰开局，黑先白后，交替着一子于棋盘的点上。
      棋子下定后，不再向其他点移动。
      $$\begin{align*}
        s_{1, c} &= 1  \tag{黑先白后}\\
        \boldsymbol B_0 &= \boldsymbol 0  \tag{空枰开局}\\
        s_{k,c} &= -s_{k-1, c}  \tag{交替落子}\\
        \boldsymbol B_{k} &= \boldsymbol B_{k-1} + (s_{k, c})_{s_{k, x}, s_{k, y}}  \\
        \boldsymbol B_{k-1}[s_{k, x}, s_{k, y}] &= 0
      \end{align*}$$

    - 无气提子  
      下子后，对方棋子无气，应立即提取对方无气之子。  
      下子后，双方棋子都呈无气状态，应立即提取对方无气之子。
      $$\boldsymbol B[S \ |\ Q(S) = 0, S_c \neq s_{k,c}] \gets 0$$

    - 禁着点  
      棋盘上的空点，若下子后，该子立即无气，同时又不能提对方棋子，则该点不允许落子。

      - Algorithm : 禁着点 Determine
        禁入点判定: [1]上下左右若是我，只一气；若是敌，必不只一气；上下左右不为空
        眼点判定:   [1]上下左右皆我，且均非一气 (眼点一定是对方禁入点)

    - 禁止全局同形
      $$\boldsymbol B_k \neq \boldsymbol B_{k-i}  \quad,  \forall 0 \le i < k$$

    - 棋局输赢
      数子法, 占子最多一方获胜。