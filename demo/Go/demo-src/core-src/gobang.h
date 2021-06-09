/*
Copyright 2020 LiGuer. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/


#ifndef GOBANG_H
#define GOBANG_H
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
typedef signed char INT8S;
typedef INT8S CHESS;
struct position
{
    INT8S x, y;
};
class GoBang {
public:
    /*---------------- 基础参数 ----------------*/
    INT8S MapSize = 15, MaxLevel = 4;           //棋盘尺寸//最大博弈深度
    const INT8S  Black = 1,White = -1;
    CHESS *Map;                                 //棋盘
    /*---------------- 基础函数 ----------------*/
    void init();
    void clearMap();                            //清盘
    bool setMap(INT8S x, INT8S y, CHESS chess);       //下子
    bool judgeOutMap(int x, int y);             //判断该点是否过界
    /*---------------- 核心函数 ----------------*/
    int Policy(INT8S level, CHESS* map, int alpha, int beta, INT8S who);//博弈树(核心:决策函数)
    int Evaluate(CHESS* map);           //棋局分数评价判断(核心:评价函数)
    INT8S judgeLineChess(INT8S x, INT8S y, INT8S dx, INT8S dy, CHESS* map, INT8S& flag);//判断棋子连线
    INT8S judgeWin(CHESS* map);                 //判断是否赢
    /*---------------- 无聊函数 ----------------*/
    position Ans;
    position GoBangAns(INT8S who);              //AI计算结果
};
#endif // MAINWINDOW_H

