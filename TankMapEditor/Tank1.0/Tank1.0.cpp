// tank.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <TIME.H>
#include <mmsystem.h> //导入声音头文件库   
#pragma comment(lib,"winmm.lib")//导入声音的链接库 

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szWindowClass[MAX_LOADSTRING];		    // The title bar text
//RECT	  rect;
HDC hdc, mhdc, mmhdc;        //贴图
int pre_Time, now_Time;      //消息循环
HBITMAP wall_bmp[11];		 //墙图
HBITMAP background[2];       //背景图
HBITMAP myTank_bmp[2][4][2]; //我的坦克图

int key_pre_time, key_now_time;  //按键时间

struct MapBlock {
	int x, y;                   //坦克坐标和方向
	int pre_time, now_time;
	int kind;                     //坦克种类
	MapBlock() {
		x = y = 0; kind = 1;
	}
}MapBlock;

struct Tank {
	int x, y, d;                   //坦克坐标和方向
}myTank;

int map[25][25];				   //地图，0：空地；1、2：土墙；3：铁墙；4：草地；5：河
int x = 0, y = 0;                          //地图块所在位置
int MapNum;                        //地图块编号
int StartTime = 0;                     //初始化地图用

								   // Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void draw_MapBlock();					  //画所选地图块
void move_MapBlock(int d, int id);		      //移动我的坦克,d为移动的方向,id为移动坦克的标号
void init();							  //每关初始化

void draw_myTank();						  //画我的坦克
void move_myTank(int d, int id);		  //移动我的坦克,d为移动的方向,id为移动坦克的标号

BOOL meet(int x, int y, int d, int belong); //碰撞检测

void SetMap();                            //地图初始化
void myPaint();							  //把贴图函数放在此函数中
void clear_wall();                        //清空地图
void key_down();                          //模拟按键操作
void WriteMap();                          //将地图保存到文档中
//void DrawWall();

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDC_TANK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TANK);

	// Main message loop:
	GetMessage(&msg, NULL, NULL, NULL);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			key_now_time = GetTickCount();
			if (key_now_time - key_pre_time >= 30 ) {
				key_down();
			}
			now_Time = GetTickCount();
			if (now_Time - pre_Time >= 0) {
				myPaint();										 //贴图
			}
		}
	}
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TANK);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCSTR)IDC_TANK;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, "地图编辑器", WS_OVERLAPPEDWINDOW,
		700, 300, 716, 539, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mhdc = CreateCompatibleDC(hdc);
	mmhdc = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, 1300, 680);//1015 680
	SelectObject(mhdc, bmp);
	/***********************************以下为加载位图*********************************************/
	char str[100];
	int i, j, k;
	for (i = 0; i<2; i++) {								//加载我的坦克位图
		for (j = 0; j<4; j++) {
			for (k = 0; k < 2; k++) {
				sprintf_s(str, 100, "Image\\MyTank\\myTank%d%d%d.bmp", i, j, k);
				myTank_bmp[i][j][k] = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 40, 40, LR_LOADFROMFILE);
			}
		}
	}
	for (i = 1; i <= 6; i++) {
		sprintf_s(str, 100, "Image\\Wall\\wall%d.bmp", i);
		wall_bmp[i] = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);//加载墙的位图
	}
	background[0] = (HBITMAP)LoadImage(NULL, "Image\\bj0.bmp", IMAGE_BITMAP, 1015, 680, LR_LOADFROMFILE);//加载背景
	background[1] = (HBITMAP)LoadImage(NULL, "Image\\bj1.bmp", IMAGE_BITMAP, 200, 500, LR_LOADFROMFILE);
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_UP:
			if (MapBlock.y > 0)
				move_MapBlock(0, 1);
			break;
		case VK_DOWN:
			if (MapBlock.y < 465)
				move_MapBlock(2, 1);
			break;
		case VK_RIGHT:
			if (MapBlock.x < 465)
				move_MapBlock(1, 1);
			break;
		case VK_LEFT:
			if (MapBlock.x > 0)
				move_MapBlock(3, 1);
			break;
		case VK_SPACE:
			if (MapBlock.kind == map[MapBlock.y / 20][MapBlock.x / 20]) {
				map[MapBlock.y / 20][MapBlock.x / 20] = 0;
			}
			else {
				map[MapBlock.y / 20][MapBlock.x / 20] = MapBlock.kind;
			}
			break;
		case VK_RETURN:
			break;
		case VK_PRIOR:
			break;
		case VK_NEXT:
			break;
		}
		break;
	case WM_COMMAND://点击菜单， 点击加速键，点击子窗口按钮，点击工具栏按钮，这些时候都有command消息产生
					/*WM_COMMAND消息中有两个参 数，wparam、lparam，定义如下：

					wParam 高两个字节 通知码

					wParam 低两字节 命令ID

					lParam 发送命令消息的子窗体句柄。
					对于菜单 和加速键来说，lParam为0，只有控件此项才非0。
					命令ID也就是资源脚本中定义的菜单项的命令ID或者加速键的命令ID；
					菜单的通知码为0；
					加速键的通知码为1。

					对于Windows菜单中菜单项和加速键，点击后，Windows会向所属的窗体发送WM_SYSCOMMAND，
					而不是WM_COMMAND消息。注 意，WINDOWS菜单是系统菜单，也就是在标题栏点击鼠标左键的时候弹出的菜单。
					我们可以捕获WM_CREATE消息，加入自己的操 作：GetSysMenu获取系统菜单句柄，然后对系统菜单进行操作，
					并且捕获添加菜单项（根据菜单命令ID）ID对应的WM_SYSCOMMAND消息 进行处理。修改系统默认的菜单行为。

					子窗体和父窗体：

					子窗体被触发时，向父窗体发送一个WM_COMMAND消息，父窗体的窗口函数处理这个消息，进行 相关的处理。
					lParam表示子窗口句柄，LOWORD(wParam)表示子窗口ID，HIWORD (wParam)表示通知码（例如单击，双击，SETFOCUS等）。*/
		wmId = LOWORD(wParam); //取高两个字节 通知码
		wmEvent = HIWORD(wParam); //低两字节 命令ID
								  // Parse（从语法上描述或分析） the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		myPaint();
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)//About对话框的对话框处理函数处理About对话框的各种消息
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));//清除一个模态对话框,并使系统中止对对话框的任何处理的函数。
											//BOOL EndDialog(HWND hDlg,int nResult): hDlg：表示要被清除的对话框窗口;NResult：指定从创建对话框函数返回到应用程序的值。
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void SetMap() {
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, "Map\\map.txt", "r");//打开文件成功返回0，失败返回非0，比fopen安全


	int i, j;
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 25; j++)
			fscanf_s(fp, "%d", &map[i][j]);
			//map[i][j] = 0;
	}
	fclose(fp);
	StartTime++;
}

void init() {
	MapBlock.x = 0; MapBlock.y = 0;
	MapBlock.kind = 1;
	myTank.x = 130; myTank.y = 460;
	key_pre_time = GetTickCount();
}

void key_down() {
	/*::GetKeyState(VK_SHIFT) > 0 没按下
	::GetKeyState(VK_SHIFT) < 0被按下*/
	if (::GetKeyState(49) < 0) {
		MapBlock.kind = 1;
	}
	else if (::GetKeyState(50) < 0) {
		MapBlock.kind = 2;
	}
	else if (::GetKeyState(51) < 0) {
		MapBlock.kind = 3;
	}
	else if (::GetKeyState(52) < 0) {
		MapBlock.kind = 4;
	}
	else if (::GetKeyState(53) < 0) {
		MapBlock.kind = 5;
	}
	else if (::GetKeyState(54) < 0) {
		MapBlock.kind = 6;
	}
	if (::GetKeyState(87)<0) {
		//87-->W
		myTank.d = 0;
		if (meet(myTank.x, myTank.y, 0, 0))
		move_myTank(0, 0);
	}
	else if (::GetKeyState(68)<0) {
		//68-->D
		myTank.d = 1;
		if (meet(myTank.x, myTank.y, 1, 0))
		move_myTank(1, 0);
	}
	else if (::GetKeyState(83)<0) {
		//83-->S
		myTank.d = 2;
		if (meet(myTank.x, myTank.y, 2, 0))
		move_myTank(2, 0);
	}
	else if (::GetKeyState(65)<0) {
		//65-->A
		myTank.d = 3;
		if (meet(myTank.x, myTank.y, 3, 0))
		move_myTank(3, 0);
	}
	key_pre_time = GetTickCount();
	if (::GetKeyState(67) < 0 && ::GetKeyState(65) < 0) {//A + C
		clear_wall();
	}
	if (::GetKeyState(83) < 0 && ::GetKeyState(17) < 0) { //ctrl + s
		WriteMap();
		int w;
		w = MessageBox(GetForegroundWindow(), "Map has been saved.", "Message", 1);
	}	
}

void draw_MapBlock() {
	SelectObject(mmhdc, wall_bmp[MapBlock.kind]);
	BitBlt(mhdc, MapBlock.x, MapBlock.y, 20, 20, mmhdc, 0, 0, SRCPAINT);

}

void move_MapBlock(int d, int belong) {
	if (d == 0) {					 //UP
		MapBlock.y -= 20;
	}
	else if (d == 1) {				//RIGHT
		MapBlock.x += 20;
	}
	else if (d == 2) {				//DOWN
		MapBlock.y += 20;
	}
	else if (d == 3) {				//LEFT
		MapBlock.x -= 20;
	}
}

void DrawBasicWall() {
	int i, j;
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 25; j++) {
			if (map[i][j]) {
					SelectObject(mmhdc, wall_bmp[map[i][j]]);
					BitBlt(mhdc, j * 20, i * 20, 20, 20, mmhdc, 0, 0, SRCCOPY);
			}
		}
	}
}

void WriteMap() {
	FILE *fp;
	errno_t err;
	err = fopen_s(&fp, "Map\\map.txt", "w");//打开文件成功返回0，失败返回非0，比fopen安全

	int i, j;
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 25; j++) {
			if (j != 24) {
				fprintf_s(fp, "%d ", map[i][j]);
			}
			else {
				fprintf_s(fp, "%d\n", map[i][j]);
			}
		}
	}
	fclose(fp);
	StartTime++;
}
void clear_wall() {
	for(int i = 0; i < 25; i++)
		for (int j = 0; j < 25; j++) {
			map[i][j] = 0;
		}
}
void draw_myTank() {
	int i = 0;
		SelectObject(mmhdc, myTank_bmp[i][myTank.d][0]);
		BitBlt(mhdc, myTank.x, myTank.y, 40, 40, mmhdc, 0, 0, SRCAND);
		SelectObject(mmhdc, myTank_bmp[i][myTank.d][1]);
		BitBlt(mhdc, myTank.x, myTank.y, 40, 40, mmhdc, 0, 0, SRCPAINT);
}
void move_myTank(int d, int belong) {
	if (d == 0) {					 //UP
		myTank.y -= 5;
		myTank.d = 0;
	}
	else if (d == 1) {				//RIGHT
		myTank.x += 5;
		myTank.d = 1;
	}
	else if (d == 2) {				//DOWN
		myTank.y += 5;
		myTank.d = 2;
	}
	else if (d == 3) {				//LEFT
		myTank.x -= 5;
		myTank.d = 3;
	}
}

BOOL meet(int x, int y, int d, int belong) {
	//0：空地；1、2：土墙；3：铁墙；4：草地；5：河
	if (d == 0) {
		if (y - 5<0 
			|| map[(y - 5) / 20][(x + 30) / 20] >= 3 && map[(y - 5) / 20][(x + 30) / 20] <= 6
			|| map[(y - 5) / 20][(x + 5) / 20] >= 3 && map[(y - 5) / 20][(x + 5) / 20] <= 6
			|| map[(y - 5) / 20][(x + 15) / 20] >= 3 && map[(y - 5) / 20][(x + 15) / 20] <= 6) return false;
	}
	else if (d == 1) {
		if (x + 5 > 465 
			|| map[(y + 30) / 20][(x + 35) / 20] >= 3 && map[(y + 30) / 20][(x + 35) / 20] <= 6
			|| map[(y + 5) / 20][(x + 35) / 20] >= 3 && map[(y + 5) / 20][(x + 35) / 20] <= 6
			|| map[(y + 15) / 20][(x + 35) / 20] >= 3 && map[(y + 15) / 20][(x + 35) / 20] <= 6) return false;
	}
	else if (d == 2) {
		if (y + 5 > 465 
			|| map[(y + 35) / 20][(x + 7) / 20] >= 3 && map[(y + 35) / 20][(x + 7) / 20] <= 6
			|| map[(y + 35) / 20][(x + 33) / 20] >= 3 && map[(y + 35) / 20][(x + 33) / 20] <= 6
			|| map[(y + 35) / 20][(x + 15) / 20] >= 3 && map[(y + 35) / 20][(x + 15) / 20] <= 6) return false;
	}
	else {
		if (x - 5<0 
			|| map[(y + 30) / 20][(x) / 20] >= 3 && map[(y + 30) / 20][(x) / 20] <= 6
			|| map[(y + 8) / 20][(x) / 20] >= 3 && map[(y + 8) / 20][(x) / 20] <= 6
			|| map[(y + 20) / 20][(x) / 20] >= 3 && map[(y + 20) / 20][(x) / 20] <= 6) return false;
	}
	return true;
}

void myPaint() {
	if (StartTime == 0) {
		SetMap();
		init();
	}
	SelectObject(mmhdc, background[0]);
	BitBlt(mhdc, 0, 0, 1015, 680, mmhdc, 0, 0, SRCCOPY);
	SelectObject(mmhdc, background[1]);
	BitBlt(hdc, 500, 0, 200, 500, mmhdc, 0, 0, SRCCOPY);
	//char chInput[512];
	//sprintf_s(chInput, 512, "x:%d y:%d\n", MapBlock.x, MapBlock.y);
	//OutputDebugString(chInput);
	DrawBasicWall();
	draw_MapBlock();
	draw_myTank();
	BitBlt(hdc, 0, 0, 500, 500, mhdc, 0, 0, SRCCOPY);
	pre_Time = GetTickCount();
}