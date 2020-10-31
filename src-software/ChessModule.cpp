#include "ChessModule.h"
#include "MapModule.h"
#include<QDebug>
ChessModule::ChessModule(QWidget *parent) : QWidget(parent)
{
    go->init();
    resize(1000,1000);
    setAttribute(Qt::WA_TranslucentBackground, true);
    ChessWarn->setStyleSheet("QLabel{background:#FF0000;}");
    ChessWarn->setGeometry(0,0,0,0);
    QFont font("Times New Roman", 100, 50, true);//字体，大小，粗细（50正常），是否斜体
    WinLable->setGeometry(0,0,0,0);
    WinLable->setFont(font);
    WinLable->setText("");
    for(int i=0;i<go->MapSize*go->MapSize;i++)
        Chess[i]=new QLabel(this);
}
void ChessModule::mousePressEvent(QMouseEvent *e)
{
    signed char x, y;
    if(e->button()==Qt::LeftButton){    //左键按下
        //======人黑子======
        x =(e->x()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        y =(e->y()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        if(!go->setMap(x,y,go->Black))return;
        PrintChess(go->Map);
    }
    else if(e->button()==Qt::RightButton){   //右键按下
        x =(e->x()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        y =(e->y()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        if(!go->setMap(x,y,go->White))return;
        PrintChess(go->Map);
    }
    else if(e->button()==Qt::MidButton){//中键按下
        //======电脑黑子======
        go->GoAI(go->Map,go->Black,x,y);
        PrintChess(go->Map);
    }
    //======若赢======
    if(go->judgeWin(go->Map)==1){
        WinLable->setGeometry(0,0,1000,1000);
        WinLable->setText("You Win");
        return;
    }
    //======电脑白子======
    go->GoAI(go->Map,go->White,x,y);
    PrintChess(go->Map);
    ChessWarn->setGeometry(MapModule::BoardMargin+x*MapModule::GridSize-ChessSize/2,MapModule::BoardMargin+y*MapModule::GridSize-ChessSize/2,3,3);
    //======若赢======
    if(go->judgeWin(go->Map)==-1){
        WinLable->setGeometry(0,0,1000,1000);
        WinLable->setText("You Lose");
        return;
    }
}
void ChessModule::PrintChess(INT8S* map)
{
    int ChessCur = 0;
    for(int z=0;z<go->MapSize*go->MapSize;z++){
        if(map[z]!=0){
            int x = z%go->MapSize,y=z/go->MapSize;
            x = MapModule::BoardMargin+x*MapModule::GridSize-ChessSize/2;
            y = MapModule::BoardMargin+y*MapModule::GridSize-ChessSize/2;

            Chess[ChessCur]->setGeometry(x,y,ChessSize,ChessSize);
            if(map[z]==go->Black)Chess[ChessCur]->setStyleSheet("QLabel{background:#000000;border-radius: 12px;border:2px solid black;}");
            else Chess[ChessCur]->setStyleSheet("QLabel{background:#EEFF0000;border-radius: 12px;border:3px solid #EEFF0000;}");
            Chess[ChessCur]->show();
            ChessCur++;
        }
    }
    for(int i=ChessCur;i<go->MapSize*go->MapSize;i++)Chess[i]->hide();
}
