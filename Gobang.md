* Gobang
  - Concept
    - 棋盘  
      A matrix whose value is $\{0, 1, -1\}$.
      $$\boldsymbol B \in \{0, 1, -1\}^{n \times n}, n = 15$$

    - 棋子  
      棋子分黑白两色，对局双方各执一色棋子。
      $$s = (x, y, c) \in (1:n, 1:n, \{1, -1\})  \tag{棋分黑白}$$

    - 连子  
      是由紧邻的同色棋子，构成的线形的连通结构。

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

    - 输赢判定  
      落子后，当一方存在连子数$\ge 5$时，获胜。