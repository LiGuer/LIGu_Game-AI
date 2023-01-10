* Monte Carlo Tree Search
  - Purprose  
    Evaluate the state formed by possible action.

    $$S_{k} \overset{A_k}{\rightarrow} S_{k+1}$$

  - Procedure
    - Select  
      Starting from the root node, MCTS uses a combination of exploration and exploitation to select the most promising node to expand.

      Exploration: Selecting nodes that have not been visited often. Using Upper Confidence Bound to choose action $A_k$ for state $S_k$
      $$\tilde V(S_k) = \frac{\sum R_{\text{simulate}}(S_k)}{N_{\text{visit}}(S_k)}$$

      $$\text{UCD}(S_k) = \tilde V(S_k) + C \sqrt{\frac{\ln(N_{_\text{visit}}(S_{k-1}))}{N_\text{visit}(S_k)}}$$

      Exploitation: Selecting nodes that have led to good results in the past. 
      $$\tilde{S_k^*} = \arg\max_{S_k \in \mathcal S} \tilde V(S_k)$$

    - Expand  
      Once a node has been selected for expansion, MCTS adds one or more child nodes to the tree, representing the possible moves that can be made from that state.  

    - Simulate  
      Randomly select action base on the input state, and simulate the game until the termination state is reached.

      ```cpp
      while(!isTerminate(s))
        s = nextStateRand(s)

      reward = evaluate(s)
      ```

    - Backpropage  
      The reward obtained from Simulation is backpropagated from the leaf node to root, and each related node is updated.

      ```cpp
      while (node != NULL) {
        node->value += reward;
        node->visit += 1;
        node = node->parent;
      }
      ```