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

        State() {
            board.zero(BOARDSIZE, BOARDSIZE);
        }

        State& operator=(State& x) {
            action = x.action;
            player = x.player;
            board = x.board;
            parent = x.parent;
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

                } break;
                case QUEEN: case BISHOP: case ROOK: {
                    int j_st = 0,
                        j_ed = 8;

                    if (abs(p) == BISHOP)
                        j_st = 4;
                    else if (abs(p) == ROOK)
                        j_ed = 4;

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

                        if (xt < 0 || xt >= BOARDSIZE ||
                            yt < 0 || yt >= BOARDSIZE ||
                            s.board(xt, yt) * p > 0)
                            continue;

                        s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;

                        // (1, 2)
                        xt = x + x_step[j] * 1,
                        yt = y + y_step[j] * 2;

                        if (xt < 0 || xt >= BOARDSIZE ||
                            yt < 0 || yt >= BOARDSIZE ||
                            s.board(xt, yt) * p > 0)
                            continue;

                        s.actionSet[i * 64 + xt * BOARDSIZE + yt] = 1;
                    }
                } break;
                case PAWN: {
                    if (p > 0) {
                        if (x == BOARDSIZE - 2) {
                            s.actionSet[i * 64 + (x - 1) * BOARDSIZE + y] = 1;
                            s.actionSet[i * 64 + (x - 2) * BOARDSIZE + y] = 1;

                            if (s.board(x - 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y - 1)] = 1;
                            if (s.board(x - 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y + 1)] = 1;
                        }
                        else if (x > 1) {
                            s.actionSet[i * 64 + (x - 1) * BOARDSIZE + y] = 1;

                            if(s.board(x - 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y - 1)] = 1;
                            if (s.board(x - 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x - 1) * BOARDSIZE + (y + 1)] = 1;
                        }
                        else if (x == 1) {
                            // Promotion
                            s.actionSet[i * 64 + (x - 1) * BOARDSIZE + y] = 1;
                        }
                    }
                    if (p < 0) {
                        if (x == 1) {
                            s.actionSet[i * 64 + (x + 1) * BOARDSIZE + y] = 1;
                            s.actionSet[i * 64 + (x + 2) * BOARDSIZE + y] = 1;

                            if (s.board(x + 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y - 1)] = 1;
                            if (s.board(x + 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y + 1)] = 1;
                        }
                        else if (x < BOARDSIZE - 2) {
                            s.actionSet[i * 64 + (x + 1) * BOARDSIZE + y] = 1;

                            if (s.board(x + 1, y - 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y - 1)] = 1;
                            if (s.board(x + 1, y + 1) * p < 0)
                                s.actionSet[i * 64 + (x + 1) * BOARDSIZE + (y + 1)] = 1;
                        }
                        else if (x == BOARDSIZE - 2) {
                            // Promotion
                            s.actionSet[i * 64 + (x + 1) * BOARDSIZE + y] = 1;
                        }
                    }
                }  break;
                }
            }
        }
    }

    // move chess
    inline bool moveChess(State& s) {
        int a = abs(s.action),
            st = a / 64,
            ed = a % 64;

        if (s.actionSet.find(s.action) == s.actionSet.end())
            return false;

        if (s.action > 0) {
            s.board[ed] = s.board[st];
            s.board[st] = 0;
        }
        else {
            if (abs(s.board[st]) == KING) {
                //
            }
            if (abs(s.board[st]) == PAWN) {
                //
            }
        }

        s.player = -s.player;
        createActionSet(s);

        return true;
    }
}
#endif