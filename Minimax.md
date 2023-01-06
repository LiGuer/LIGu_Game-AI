* Minimax
  - Purpose  
    Traverse the decision tree for a zero-sum game by Depth-First search, and find the optimal action $A_0^*$ of current state $S_0$.
    $$A_0^* = \arg\max_{A_0} V_{\text{me}}(S_1) \quad S_0 \overset{A_0}{\rightarrow}  S_1$$
    $$V_{\text{me}}(S) + V_{\text{opp}}(S) = 0  \tag{zero-sum game}$$

  - Algorithm  
    $$A_{k}^* = \left\{\begin{matrix} 
      \arg\max\limits_{A_k}\ \  V_{\text{me}}(S_{k+1}) & ,k \% 2 = 0 \\ \\
      \arg\min\limits_{A_k}\ \  V_{\text{me}}(S_{k+1}) & ,k \% 2 = 1 \\
      = \arg\max\limits_{A_k}\ \  -V_{\text{me}}(S_{k+1}) \\
      = \arg\max\limits_{A_k}\ \  V_{\text{opp}}(S_{k+1}) 
    \end{matrix}\right.$$  

    * alpha-beta Prune  
      - Principle  
        $\tilde v_k$ is estimated value of the optimal $v_k^* = \max\limits_{A_k}/\min\limits_{A_k}\ V(S_{k+1})$.
        
        $$\left\{\begin{matrix} 
          \tilde v_k \le v_k^* & \Rightarrow \tilde v_k \nearrow & ,k \% 2 = 0   \\
          \tilde v_k \ge v_k^* & \Rightarrow \tilde v_k \searrow & ,k \% 2 = 1
        \end{matrix}\right.$$

        $$\Rightarrow \left\{\begin{matrix} 
          \tilde v_k \text{ Update } \Rightarrow \tilde v_k < v_{\text{new\ sampling}}& ,k \% 2 = 0   \\
          \tilde v_k \text{ Update } \Rightarrow \tilde v_k > v_{\text{new\ sampling}} & ,k \% 2 = 1
        \end{matrix}\right.$$

      - Procedure  
        $\alpha$ is the upper bound and $\beta$ is the lower bound of the range of possiable adoption of the evaluaion value. Meanwhile, $\alpha$ represent the best evaluaion value of maximizing player, and $\beta$ represent the best evaluaion value of the minimizing player.

        $$\begin{align*}
          \alpha_0 &\gets +\infty \tag{Initial}\\
          \beta_0 &\gets -\infty  \\
          \alpha_k &\gets \alpha_{k-1}  \\
          \beta_k &\gets \beta_{k-1}  \\
          \alpha_k &< \alpha_{k-i} \quad, i \in 1: k \tag{Constraint}\\
          \beta_k &> \beta_{k-i} \quad, i \in 1: k  \\
          \alpha_k &\gets \tilde v_k \quad,k \% 2 = 0  \tag{Update}\\
          \beta_k &\gets \tilde v_k \quad,k \% 2 = 1
        \end{align*}$$

        if $\alpha_k < \beta_k$, it means that there is no possiable acceptable evaluaion value for this state, and we can directly exit the search.