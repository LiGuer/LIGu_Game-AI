* Monte Carlo Tree Search
  - Purprose  
    Evaluate the state formed by possible action.

  - Procedure
    - Selection  
      Starting from the root node, MCTS uses a combination of exploration and exploitation to select the most promising node to expand.

      Exploration: Selecting nodes that have not been visited often. Using Upper Confidence Bound to choose action $A_k$ for state $S_k$

      $$\text{UCD}(S_k) = \frac{N_\text{had paly}(S_k)}{N_{\text{win}}(S_k)} + C \sqrt{\frac{\ln(N_{_\text{had paly}}(S_{k-1}))}{N_\text{had paly}(S_k)}}$$

      Exploitation: Selecting nodes that have led to good results in the past. 

    - Expansion  
      Once a node has been selected for expansion, MCTS adds one or more child nodes to the tree, representing the possible moves that can be made from that state.  

    - Simulation  
      Randomly select action base on the input state, and simulate the game until the termination state is reached.

    - Backpropagation  
      The reward obtained from Simulation is backpropagated from the leaf node to root, and each related node is updated.