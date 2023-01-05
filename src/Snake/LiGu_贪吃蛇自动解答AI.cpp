#include <Windows.h>
#include <WinUser.h>
#include <iostream>
#include <atlimage.h>
#include <queue>
using namespace std;
unsigned char pic[200][200];
enum {
	LEFT = 0x25, UP, RIGHT, DOWN
};
struct position
{
	unsigned short x, y;
};
unsigned char sleeptime = 100, girdsize = 20;
unsigned short direct = UP, order = direct;
position apple, head;
unsigned short BFS();
void KeyControl(unsigned short order);//模拟按键输入(控制)

int main()
{
	Sleep(4000);
	HWND hWnd = ::FindWindow(NULL, _T("Snake"));
	if (!hWnd) {
		std::cout << "Please Run the Main Program First\n";
		return 0;
	}
	LPRECT rect = new RECT();
	GetWindowRect(hWnd, rect);
	int nScreenWidth = rect->right - rect->left;
	int nScreenHeight = rect->bottom - rect->top;
	HDC hDesktopDC = GetDC(hWnd);
	CImage image;
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//置顶游戏窗口
	while (1) {
		/*:::::::游戏窗口截屏(输入)::::::::*/
		image.Create(nScreenWidth, nScreenHeight, GetDeviceCaps(hDesktopDC, BITSPIXEL));
		BitBlt(image.GetDC(), 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY);
		image.ReleaseDC();
		/*:::::::图像处理(识别)::::::::*/
		for (int i = girdsize / 2; i < nScreenHeight; i += girdsize) {
			for (int j = girdsize / 2; j < nScreenWidth; j += girdsize) {
				COLORREF temp = image.GetPixel(i, j);
				unsigned short R = GetRValue(temp), G = GetGValue(temp), B = GetBValue(temp);
				pic[i / girdsize][j / girdsize] = (R * 30 + G * 59 + B * 11) / 100;
				//printf("%d|", pic[i / girdsize][j / girdsize]);
				switch (pic[i / girdsize][j / girdsize])
				{
				case 76:
					printf("Food:(%d,%d)\n", i / girdsize, j / girdsize);
					apple.x = i / girdsize; apple.y = j / girdsize; break;
				case 150:
					printf("Body:(%d,%d)\n", i / girdsize, j / girdsize); break;
				case 226:
					printf("Head:(%d,%d)\n", i / girdsize, j / girdsize);
					head.x = i / girdsize; head.y = j / girdsize; break;
				default:break;
				}
			}//printf("\n");
		}//image.Save(_T("D://just.jpg"));					//保存游戏截图
		/*:::::::(思考)::::::::*/
		order = BFS();
		printf("Order:%d\n", order);
		/*:::::::模拟按键输入(控制)::::::::*/
		if (direct != order)KeyControl(order);
		direct = order;
		Sleep(sleeptime);
	}return 0;
}
unsigned char BFCMap[200][200];
position BFCBack[200][200];
unsigned char xstep[4] = { 0,0,1,-1 }, ystep[4] = { 1,-1,0,0 };
unsigned short BFS()
{
	memset(BFCMap, 0, sizeof(BFCMap));
	queue<position> Q;
	Q.push({ head.x,head.y });
	BFCMap[head.x][head.y] = 1;
	BFCBack[head.x][head.y] = { head.x ,head.y };
	while (!Q.empty()) {
		bool flag = 0;
		position temp = Q.front();
		Q.pop();
		for (int i = 0; i < 4; i++) {
			unsigned char xt = temp.x + xstep[i], yt = temp.y + ystep[i];
			if (xt<0||yt<0||BFCMap[xt][yt] != 0)continue;
			else if (pic[xt][yt] >= 250) {
				Q.push({ xt,yt });
				BFCMap[xt][yt] = BFCMap[temp.x][temp.y] + 1;
				BFCBack[xt][yt] = { temp.x,temp.y };
			}
			else if (xt == apple.x && yt == apple.y) {
				BFCMap[xt][yt] = BFCMap[temp.x][temp.y] + 1;
				BFCBack[xt][yt] = { temp.x,temp.y };
				printf("Apple Step:%d\n", BFCMap[xt][yt]);
				/*for (int ii = 0;ii < 50;ii++) {
					for (int jj = 0;jj < 50;jj++) {
						printf("%d|", BFCMap[ii][jj]);
					}printf("\n");
				}*/
				position ttemp = { apple.x , apple.y };
				while (BFCBack[ttemp.x][ttemp.y].x != head.x || BFCBack[ttemp.x][ttemp.y].y != head.y) {
					ttemp.x = BFCBack[ttemp.x][ttemp.y].x;
					ttemp.y = BFCBack[ttemp.x][ttemp.y].y;
				}printf("Next Step:(%d,%d)\n", ttemp.x, ttemp.y);
				if (ttemp.x != head.x)return ttemp.x - head.x > 0 ? RIGHT : LEFT;
				if (ttemp.y != head.y)return ttemp.y - head.y > 0 ? DOWN: UP;
			}
		}
	}
}
void KeyControl(unsigned short order) {//模拟按键输入(控制)
	keybd_event(order, 0, 0, 0);
	keybd_event(order, 0, KEYEVENTF_KEYUP, 0);
}