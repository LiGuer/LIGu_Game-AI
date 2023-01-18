#ifndef CHESS_H
#define CHESS_H

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include "C:/Users/29753/Desktop/Library/Math/src/Matrix/Mat.h"

using namespace std;

namespace Chess {
#define BOARDSIZE 8
#define BLACK  1
#define WHITE -1

    // Chess Piece
    enum { KING = 1, QUEEN, BISHOP, ROOK, KNIGHT, PAWN };
    typedef char Piece;

    // step of x, y
    static const int
        x_step[] = { 1,-1, 0, 0, 1,-1, 1,-1 },
        y_step[] = { 0, 0, 1,-1, 1, 1,-1,-1 };

    // State
    struct State {
        short action = -1;
        char  player = -1;

        Mat<Piece> board{ BOARDSIZE , BOARDSIZE };
        unordered_map<int, int> actionSet;

        State* parent = NULL;

        // Special Rule Flag
        int king_move_black = 0,
            king_move_white = 0,
            rook_move_black = 0,
            rook_move_white = 0,
            pawn_move_two = -1;

        State() {
            board.zero(BOARDSIZE, BOARDSIZE);
        }

        State& operator=(State& x) {
            action = x.action;
            player = x.player;

            board = x.board;
            actionSet = x.actionSet;

            parent = x.parent;

            king_move_black = x.king_move_black;
            king_move_white = x.king_move_white;
            rook_move_black = x.rook_move_black;
            rook_move_white = x.rook_move_white;
            pawn_move_two = x.pawn_move_two;

            return *this;
        }
    };

    inline void initBoard(Mat<Piece>& board) {
        board = {
            -ROOK,-KNIGHT,-BISHOP,-QUEEN,-KING,-BISHOP,-KNIGHT,-ROOK,
            -PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,
            PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN,
            ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
        };
    }

    inline void createActionSet(State& s) {
        s.actionSet.clear();

        for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
            if (s.board[i] * s.player > 0) {
                int x = i / BOARDSIZE,
                    y = i % BOARDSIZE,
                    p = s.board[i];

                switch (abs(p)) {
                case KING: {
                    for (int j = 0; j < 8; j++) {
                        int xt = x + x_step[j],
                            yt = y + y_step[j];

                        if (xt < 0 || xt >= BOARDSIZE ||
                            yt < 0 || yt >= BOARDSIZE ||
                            s.board(xt, yt) * p > 0)
                            continue;

                        s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;
                    }
                    // Castling  
                    if ((p > 0 && s.king_move_black == 0 && s.rook_move_black == 0) ||
                        (p < 0 && s.king_move_white == 0 && s.rook_move_white == 0)) {
                        if (s.board(x, 5) == 0 && s.board(x, 6) == 0)
                            s.actionSet[i * 64 + x * BOARDSIZE + 6] = -1;

                        if (s.board(x, 1) == 0 && s.board(x, 2) == 0 && s.board(x, 3) == 0)
                            s.actionSet[i * 64 + x * BOARDSIZE + 2] = -1;
                    } 

                } break;
                case QUEEN: case BISHOP: case ROOK: {
                    int j_st = abs(p) == BISHOP ? 4 : 0,
                        j_ed = abs(p) == ROOK   ? 4 : 8;

                    for (int j = j_st; j < j_ed; j++) {
                        int t = 0;

                        while (++t) {
                            int xt = x + t * x_step[j],
                                yt = y + t * y_step[j];

                            if (xt < 0 || xt >= BOARDSIZE ||
                                yt < 0 || yt >= BOARDSIZE ||
                                s.board(xt, yt) * p > 0)
                                break;

                            if (s.board(xt, yt) * p < 0) {
                                s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;
                                break;
                            }

                            s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;
                        }
                    }
                } break;
                case KNIGHT: {
                    for (int j = 4; j < 8; j++) {
                        // (2, 1)
                        int xt = x + x_step[j] * 2,
                            yt = y + y_step[j] * 1;

                        if (!(xt < 0 || xt >= BOARDSIZE ||
                              yt < 0 || yt >= BOARDSIZE || s.board(xt, yt) * p > 0))
                            s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;

                        // (1, 2)
                        xt = x + x_step[j] * 1,
                        yt = y + y_step[j] * 2;

                        if (!(xt < 0 || xt >= BOARDSIZE ||
                              yt < 0 || yt >= BOARDSIZE || s.board(xt, yt) * p > 0))
                            s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;
                    }
                } break;
                case PAWN: {
                    int x_ = x + (p > 0 ? -1 : +1);

                    if (s.board(x_, y) == 0)
                        s.actionSet[i * 64 + x_ * BOARDSIZE + y] = 1;
                    if (y - 1 >= 0 && s.board(x_, y - 1) * p < 0)
                        s.actionSet[i * 64 + x_ * BOARDSIZE + (y - 1)] = 1;
                    if (y + 1 < BOARDSIZE && s.board(x_, y + 1) * p < 0)
                        s.actionSet[i * 64 + x_ * BOARDSIZE + (y + 1)] = 1;

                    if (p > 0) {
                        if (x == BOARDSIZE - 2 && s.board(x - 1, y) == 0 && s.board(x - 2, y) == 0) {
                            s.actionSet[i * 64 + (x - 2) * BOARDSIZE + y] = 1;
                        }
                        else if (x == 3 && abs(s.pawn_move_two - i) == 1) {
                            // En passant
                            s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (s.pawn_move_two % BOARDSIZE)] = -1;
                        }
                        else if (x == 1) {
                            // Promotion
                            if (s.board(x - 1, y) == 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + y] = -1;
                            if (y - 1 >= 0 && s.board(x - 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y - 1)] = -1;
                            if (y + 1 < BOARDSIZE && s.board(x - 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y + 1)] = -1;
                        }
                    }
                    if (p < 0) {
                        if (x == 1 && s.board(x + 1, y) == 0 && s.board(x + 2, y) == 0) {
                            s.actionSet[i * 64 + (x + 2) * BOARDSIZE + y] = 1;
                        }
                        else if (x == 4 && abs(s.pawn_move_two - i) == 1) {
                            // En passant
                            s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (s.pawn_move_two % BOARDSIZE)] = -1;
                        }
                        else if (x == BOARDSIZE - 2) {
                            // Promotion
                            if (s.board(x + 1, y) == 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + y] = -1;
                            if (y - 1 >= 0 && s.board(x + 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y - 1)] = -1;
                            if (y + 1 < BOARDSIZE && s.board(x + 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y + 1)] = -1;
                        }
                    }
                }  break;
                }
            }
        }
    }

    // move chess
    inline bool moveChess(State& s) {
        int a = s.action,
            st = a / 64,
            ed = a % 64;

        if (s.actionSet.find(s.action) == s.actionSet.end())
            return false;

        {   // mark the flag of state
            s.pawn_move_two = -1;
            int p = abs(s.board[st]);

            if (p == KING) {
                if (s.player > 0)
                    s.king_move_black = 1;
                if (s.player < 0)
                    s.king_move_white = 1;
            }
            else if (p == ROOK || (p == KING && s.actionSet[s.action] < 0)) {
                if (s.player > 0)
                    s.rook_move_black = 1;
                if (s.player < 0)
                    s.rook_move_white = 1;
            }
            else if (p == PAWN) {
                if (abs(st / BOARDSIZE - ed / BOARDSIZE) == 2) {
                    s.pawn_move_two = ed;
                }
            }
        }

        if (s.actionSet[s.action] > 0) {
            s.board[ed] = s.board[st];
            s.board[st] = 0;
        }
        else {
            // Special Rules
            if (abs(s.board[st]) == KING) {
                // Castling  
                int x = ed / BOARDSIZE,
                    y = ed % BOARDSIZE;

                swap(s.board[st], s.board[ed]);

                if (y == 6) 
                    swap(s.board(x, 7), s.board(x, 5));
                else if (y == 2)
                    swap(s.board(x, 0), s.board(x, 3));
            }
            if (abs(s.board[st]) == PAWN) {
                int x = ed / BOARDSIZE,
                    y = ed % BOARDSIZE;

                if (x != 0 && x != BOARDSIZE - 1) {
                    // En passant  
                    swap(s.board[st], s.board[ed]);
                    s.board(st / BOARDSIZE, y) = 0;
                }
                else {
                    // Promotion  
                    s.board[st] = 0;
                    s.board[ed] = s.player * QUEEN;
                }
            }
        }

        s.player = -s.player;
        createActionSet(s);

        return true;
    }
}
#endif