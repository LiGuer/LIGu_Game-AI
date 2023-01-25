# AlphaGo-Zero

## Reference
1. *Silver D, Schrittwieser J, Simonyan K, et al. Mastering the game of go without human knowledge[J]. nature, 2017, 550(7676): 354-359.*

## Innovation
1. trained solely by self-play reinforcement learning, without human data
2. only board with stone as input features
3. single neural network (NN)
4. simpler tree search without Monte Carlo rollouts

## System & Algorithm
- neural network 
  $$(\boldsymbol p, v) = f_\theta(s)$$
  - input
    - state, position an its history through $19 \times 19 \times 17$ binary feature planes.
      - $8$ planes indicating currrent player's board
      - $8$ planes indicating opponent's board
      - $1$ plane represent the color to play ($\boldsymbol 1$: Black, $\boldsymbol 0$: White)

  - output $(\boldsymbol p, v)$
    - move probilities $\boldsymbol p = (\mathbb P(a | s))$
    - estimated value of probability of the current player winning from position $s$.
 
  - Architecture: ResNet

- reinforcement learning 
  - state set, position an its history
  - action set, including pass

- MCTS
  - Select
    $$a_t = \arg\max_a (Q(s_t, a) + U(s_t, a))$$
    $$U(s, a) = c_{\text{puct}} P(s, a) \frac{\sqrt{\sum\limits_b N(s, b)}}{1 + N(s, a)}$$ 

    |Symbol|Meaning|
    |---|---|
    |$N(s, a)$| visit count |
    |$W(s, a)$| total action value |
    |$Q(s, a)$| mean action value |
    |$P(s, a)$| prior probability of selecting that edge |
    |$U(s, a)$| upper confidence bound |
    |||
    
    - maximum action value $Q$
    - upper confidence bound $U$

  - Expand & Evaluate
    - expand a new leaf node
      $$\begin{align*}
        N(s_L, a) \gets 0  \\ 
        W(s_L, a) \gets 0  \\
        Q(s_L, a) \gets 0  \\
        P(s_L, a) \gets p_a
      \end{align*}$$ 

    - evaluate by neural network
      $$(d(\boldsymbol p), v) = f_\theta(d_i(s_L))$$

  - Backup
    $$\begin{align*}
      N(s_t, a_t) \gets& N(s_t, a_t) + 1  \\
      W(s_t, a_t) \gets& W(s_t, a_t) + v  \\
      Q(s_t, a_t) \gets& \frac{W(s_t, a_t)}{N(s_t, a_t)}  \\
    \end{align*}$$ 

  - Play
    $$\pi(a | s_0)$$