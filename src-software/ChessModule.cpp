#include "ChessModule.h"

ChessModule::ChessModule(QWidget *parent, int _Mode) : QWidget(parent)
{
    Mode = _Mode;
    Mode == 0?go->init():gobang->init();
    MapSize = Mode == 0?go->MapSize:gobang->MapSize;

    resize(1000,1000);
    setAttribute(Qt::WA_TranslucentBackground, true);
    ChessWarn->setStyleSheet("QLabel{background:#FF0000;}");
    ChessWarn->setGeometry(0,0,0,0);
    QFont font("Times New Roman", 100, 50, true);//字体，大小，粗细（50正常），是否斜体
    WinLable->setGeometry(0,0,0,0);
    WinLable->setFont(font);
    WinLable->setText("");
    for(int i=0;i<361;i++)Chess[i]=new QLabel(this);
}
void ChessModule::mousePressEvent(QMouseEvent *e)
{
    signed char x, y;
    if(e->button()==Qt::LeftButton){    //左键按下
        //======人黑子======
        x =(e->x()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        y =(e->y()-MapModule::BoardMargin+MapModule::GridSize/2)/MapModule::GridSize;
        if(Mode == 0){
            if(!go->setMap(x,y,go->Black))return;
            PrintChess(go->MainState.map);
        }
        else{
            if(!gobang->setMap(x,y,gobang->Black))return;
            PrintChess(gobang->Map);
        }

    }
    else if(e->button()==Qt::MidButton){
    }
    //======若赢======
    if(Mode == 0 && go->judgeWin(&go->MainState)==go->Black){PrintWin(1);return;}
    if(Mode == 1 && gobang->judgeWin(gobang->Map)==gobang->Black){PrintWin(1);return;}
    //======电脑白子======
    if(Mode == 0){
        go->GoAI(x,y,go->White);
        if(!go->setMap(x,y,go->White))return;
        PrintChess(go->MainState.map);
    }
    else {
        position a = gobang->GoBangAns(gobang->White);
        x = a.x; y = a.y;
        PrintChess(gobang->Map);
    }
    ChessWarn->setGeometry(MapModule::BoardMargin+x*MapModule::GridSize-ChessSize/2,MapModule::BoardMargin+y*MapModule::GridSize-ChessSize/2,3,3);
    //======若赢======
    if(Mode == 0 && go->judgeWin(&go->MainState)==go->White){PrintWin(0);return;}
    if(Mode == 1 && gobang->judgeWin(gobang->Map)==gobang->White){PrintWin(0);return;}
}
void ChessModule::PrintWin(bool win)
{
    WinLable->setGeometry(0,0,1000,1000);
    if(win)WinLable->setText("You Win");
    else WinLable->setText("You Lose");
}
void ChessModule::PrintChess(INT8S* map)
{
    int ChessCur = 0;
    for(int z=0;z<MapSize*MapSize;z++){
        if(map[z]!=0){
            int x = z%MapSize,y=z/MapSize;
            x = MapModule::BoardMargin+x*MapModule::GridSize-ChessSize/2;
            y = MapModule::BoardMargin+y*MapModule::GridSize-ChessSize/2;

            Chess[ChessCur]->setGeometry(x,y,ChessSize,ChessSize);
            if(map[z]==go->Black)Chess[ChessCur]->setStyleSheet("QLabel{background:#000000;border-radius: 12px;border:2px solid black;}");
            else Chess[ChessCur]->setStyleSheet("QLabel{background:#EEFF0000;border-radius: 12px;border:3px solid #EEFF0000;}");
            Chess[ChessCur]->show();
            ChessCur++;
        }
    }
    for(int i=ChessCur;i<MapSize*MapSize;i++)Chess[i]->hide();
}
