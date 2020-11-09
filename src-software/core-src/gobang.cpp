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


#include "gobang.h"
int PolicyWalkSum = 0;                      //调试:决策树走过节点数计数
/******************************************************************************
*                    基础函数
******************************************************************************/
//----------------初始化----------------
void GoBang::init(){
    Map = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);
    clearMap();
}
//----------------清空棋盘----------------
void GoBang::clearMap(){
    memset(Map, 0, sizeof(CHESS) * MapSize * MapSize);
}
//----------------下子----------------
bool GoBang::setMap(INT8S x, INT8S y, CHESS chess){
    if (Map[y * MapSize + x] != 0)return false;
    Map[y * MapSize + x] = chess;
    return true;
}
//----------------判断该点是否过界----------------
bool GoBang::judgeOutMap(int x, int y) {
    return (x >= 0 && x < MapSize && y >= 0 && y < MapSize) ? true : false;
}
/******************************************************************************
*                    核心算法
******************************************************************************/
//----------------博弈树(极小化极大值算法)(核心:决策函数)----------------
int GoBang::Policy(INT8S level, CHESS* map, int alpha, int beta, INT8S who) {
    PolicyWalkSum++;
    if (level > MaxLevel || judgeWin(map) != 0) {		//达到定义的最大博弈深度(递归终点)//已赢(连五)(递归终点、剪枝)//==-who时会出现极其诡异bug
        return (who==-1?-1:1) * Evaluate(map);
    }
    //------最大最小算法，alpha-beta剪枝------
    int max = -0x7fffffff, min = 0x7fffffff;
    const static INT8S step_x[] = { 0,0,1,-1,1,-1,1,-1 };
    const static INT8S step_y[] = { 1,-1,0,0,1,-1,-1,1 };
    CHESS* flagMap = (CHESS*)malloc(sizeof(CHESS) * MapSize * MapSize);	//标记
    memset(flagMap,0,sizeof(CHESS) * MapSize * MapSize);
    for (INT8S y = 0; y < MapSize; y++) {
        for (INT8S x = 0; x < MapSize; x++) {                           //对每个点进行思考
            if(map[y * MapSize + x] != 0){                              //默认五子棋连通，不考虑"飞子"
                for (INT8S i = 0; i < 8; i++) {
                    INT8S xt = x + step_x[i], yt = y + step_y[i];
                    if (xt < 0 || xt >= MapSize || yt < 0 || yt >= MapSize)continue; //过界
                    if (map[yt * MapSize + xt] == 0 && flagMap[yt * MapSize + xt] == 0){    //无子且未下过
                        flagMap[yt * MapSize + xt] = 1;
                        map[yt * MapSize + xt] = level % 2 == 0 ? who : -who;		//虚拟下子
                        int score = Policy(level + 1, map, alpha, beta, who);       //递归	//score:该子评估值
                        map[yt * MapSize + xt] = 0;                                 //还原下子
                        if(level ==0)printf("[%c,%c:%d].",(char)(xt+'A'),(char)(yt+'A'),score);
                        if (level % 2 == 1) {                                       //单数博弈层(对手层)
                            min = min < score ? min : score;                        //取最小
                            beta = beta < score ? beta : score;
                        }
                        else {                                                      //双数博弈层(我层)
                            if (level == 0 && max < score) Ans = { xt, yt };
                            max = max > score ? max : score;                        //取最大
                            alpha = alpha > score ? alpha : score;
                        }
                        if (alpha >= beta)return level % 2 == 0 ? max : min;        //alpha-beta剪枝
                    }
                }
            }
        }
    }
    free(flagMap);
    return level % 2 == 0 ? max : min;
}
/*----------------棋局分数评价判断(核心:评价函数)----------------
 * 黑正白负
* --------------------------------*/
int GoBang::Evaluate(CHESS* map)
{
    int score = 0;
    const static INT8S step_x[4] = { 0,1,1,1 }, step_y[4] = { 1,0,1,-1 };
    for (INT8S y = 0; y < MapSize; y++) {
        for (INT8S x = 0; x < MapSize; x++) {
            if (map[y * MapSize + x] == 0) continue;
            INT8S mark = map[y * MapSize + x] == 1 ? 1 : -1;
            INT8S flag, num;                                                //flag:首末是否能下
            for (INT8S k = 0; k < 4; k++) {
                //------ 评价打分 ------
                num = judgeLineChess(x, y, step_x[k], step_y[k], map, flag);//是否连子//num:连子数
                if (num == 0)continue;                                      //不连子
                else if (num >= 5)score += mark * pow(10, 5);               //连五
                else if (flag == 2)continue;                                //首末封死
                else score += mark * pow(10, num - flag);                   //活四,冲四,活三...
            }
        }
    }return score;
}
//----------------判断棋子连线----------------
INT8S GoBang::judgeLineChess(INT8S x, INT8S y, INT8S dx, INT8S dy, CHESS* map, INT8S& flag) {
    INT8S num = 1;
    flag = 0;
    /*------ 前子 ------*/
    if (judgeOutMap(x - dx, y - dy)) {                                      //前子不碰墙
        if (map[(y - dy) * MapSize + (x - dx)] == map[y * MapSize + x])return 0;//已算的连子
        else if (map[(y - dy) * MapSize + (x - dx)] != 0) flag = 1;         //前子异子
    }
    else flag = 1;                                                          //前子碰墙
    /*------ 连线 ------*/
    while (judgeOutMap(x + num * dx, y + num * dy)
        && map[(y + num * dy) * MapSize + (x + num * dx)] == map[y * MapSize + x])//连子+1
        num++;
    /*------ 后子 ------*/
    if (!judgeOutMap(x + num * dx, y + num * dy)
        || map[(y + num * dy) * MapSize + (x + num * dx)] != 0)flag++;     //后子碰墙or异子
    return num;
}
//----------------判断是否赢----------------
INT8S GoBang::judgeWin(CHESS* map)
{
    const static INT8S step_x[4] = { 0,1,1,1 }, step_y[4] = { 1,0,1,-1 };
    for (INT8S y = 0; y < MapSize; y++) {
        for (INT8S x = 0; x < MapSize; x++) {
            if (map[y * MapSize + x] == 0) continue;
            for (INT8S k = 0; k < 4; k++) {
                if (judgeOutMap(x - step_x[k], y - step_y[k])
                    &&map[(y - step_y[k]) * MapSize + (x - step_x[k])] == map[y * MapSize + x])continue;//已算的连子
                INT8S num = 1,xt = x,yt = y;
                while (judgeOutMap(xt + step_x[k], yt + step_y[k])
                    && map[(yt + step_y[k]) * MapSize + (xt + step_x[k])] == map[y * MapSize + x])//连子+1
                    {num++;xt+=step_x[k];yt+=step_y[k];}
                if (num >= 5)return map[y * MapSize + x];                   //连五
            }
        }
    }return 0;
}
/******************************************************************************
*                    其他函数
******************************************************************************/
//----------------给出AI下棋结果----------------
position GoBang::GoBangAns(INT8S who) {
    PolicyWalkSum = 0;
    Policy(0, Map, -0x7fffffff, 0x7fffffff, who);
    printf("%d\n\n",PolicyWalkSum);
    setMap(Ans.x, Ans.y, who);
    return Ans;
}
