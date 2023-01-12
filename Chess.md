* Chess
  - Concept
    - Board  
      Board is a $8 \times 8$ matrix whose value is,

      - Initial Board  
        $$B_0 = \left(\begin{matrix}
          -R & -H & -B & -Q & -K & -B & -H & -R  \\
          -P & -P & -P & -P & -P & -P & -P & -P  \\
          0 & 0 & 0 & 0 & 0 & 0 & 0 & 0  \\
          0 & 0 & 0 & 0 & 0 & 0 & 0 & 0  \\
          0 & 0 & 0 & 0 & 0 & 0 & 0 & 0  \\
          0 & 0 & 0 & 0 & 0 & 0 & 0 & 0  \\
          P & P & P & P & P & P & P & P  \\
          R & H & B & Q & K & B & H & R  \\
        \end{matrix}\right)_{8 \times 8}$$

    - Chess Pieces  
      The chess pieces are divided into black and white, and each side of the game holds one color of chess pieces.    
      $$c \in \{\pm K, \pm Q, \pm R, \pm B, \pm H, \pm P\}$$

      |Symbol|Name|
      |---|---|
      | $K$ | King |
      | $Q$ | Queen |
      | $R$ | Rook |
      | $B$ | Bishop |
      | $N$ | Knight |
      | $P$ | Pawn |
      |||

  - Rule 
    - Movement, Action Set $\mathcal A$
      - $K$  
        $$A^{(K)} = \{(x, y), (x, y \pm 1), (x \pm 1, y), (x \pm 1, y \pm 1), (x \pm 1, y \mp 1)\}$$

        - Castling  
          Castling consists of moving the king two squares towards a rook, then placing the rook on the other side of the king, adjacent to it. Castling is only permissible if all of the following conditions hold
            - The king and rook involved in castling must not have previously moved; 
            - There must be no pieces between the king and the rook; 
            - The king may not currently be under attack, nor may the king pass through or end up in a square that is under attack by an enemy piece (though the rook is permitted to be under attack and to pass over an attacked square); 
            - The castling must be kingside or queenside as shown in the diagram. An unmoved king and an unmoved rook of the same color on the same rank are said to have castling rights.

      - $Q$  
        $$A^{(Q)} = \{(x, y), (x, y + \Delta), (x + \Delta, y), (x + \Delta, y + \Delta), (x - \Delta, y + \Delta) \ |\ \Delta \in \mathbb Z\}$$

      - $R$  
        $$A^{(R)} = \{(x, y), (x, y + \Delta), (x + \Delta, y) \ |\ \Delta \in \mathbb Z\}$$

      - $B$  
        $$A^{(B)} = \{(x, y), (x + \Delta, y + \Delta), (x - \Delta, y + \Delta) \ |\ \Delta \in \mathbb Z\}$$

      - $N$  
        $$A^{(N)} = \{(x, y), (x \pm 1, y \pm 2), (x \pm 2, y \pm 1), (x \mp 1, y \pm 2), (x \mp 2, y \pm 1)$$

      - $P$  
        The movement of $P$ includes only one step forward.
        $$A^{(P)} = \{(x, y), (x + 1, y)\}$$

        if $P$ has never moved,
        $$A^{(P)} = \{(x, y), (x + 1, y), (x + 2, y)\}$$

        - En passant  
          When a pawn advances two squares from its original square and ends the turn adjacent to a pawn of the opponent's on the same rank, it may be captured en passant by that pawn of the opponent's, as if it had moved only one square forward. This capture is only legal on the opponent's next move immediately following the first pawn's advance.

        - Promotion  
          If a player advances a pawn to its eighth rank, the pawn is then promoted (converted) to a queen, rook, bishop, or knight of the same color at the choice of the player (a queen is usually chosen).