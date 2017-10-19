#include "Common.h"
#include "Include.h"

/*
** Global Variable
*/
HWND hWND;//窗口句柄
HINSTANCE hInst;//当前实例窗口句柄
TCHAR szTitle[MAX_LOADSTRING];//标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];//主窗口类名

//User
int g_nUserScreenWidth = 0;//用户区域窗口宽度
int g_nUserScreenHeight = 0;//用户区域窗口高度

/*
** Function Declaration
*/
ATOM MyRegisterClass(HINSTANCE hInstance);//注册窗口类
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);//初始化创建主窗口
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//处理主窗口的消息
void ReleaseWindows(void);//释放窗口资源
void RePaintWindows(HWND hWnd, HINSTANCE hInstance);//重绘窗口

/*
** WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nCmdShow)
** Purpose:  窗口主函数
*/
int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);//编译器不检测变量hPrevInstance警告
	UNREFERENCED_PARAMETER(lpCmdLine);//编译器不检测变量lpCmdLine警告

	MSG msg;//窗口消息

	// 初始化全局字符串
	LoadString(hInstance, IDS_TITLE, szTitle, MAX_LOADSTRING);//窗口名称导入szTitle数组
	LoadString(hInstance, IDS_CLASS, szWindowClass, MAX_LOADSTRING);//窗口类名导入szWindowClass数组

																	//注册窗口类
	MyRegisterClass(hInstance);

	//执行应用程序初始化
	if(!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// 主消息循环
	ZeroMemory(&msg, sizeof(msg));
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Direct3D_Update(hWND);//Direct3D更新
			Direct3D_Render(hWND);//Direct3D渲染
		}
	}

	//释放窗口资源
	ReleaseWindows();

	return (int)msg.wParam;
}

/*
** Function:  MyRegisterClass()
** Purpose:  注册窗口类
*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);//WNDCLASSEX类大小

	wcex.style = CS_HREDRAW | CS_VREDRAW;//窗口类样式
	wcex.lpfnWndProc = WndProc;//窗口类回调函数
	wcex.cbClsExtra = 0;//窗口类结构附加字节数
	wcex.cbWndExtra = 0;//窗口实例附加字节数
	wcex.hInstance = hInstance;//窗口句柄
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));//图标句柄
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//光标句柄
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//背景画刷颜色(自动)
	wcex.lpszMenuName = MAKEINTRESOURCE(IDI_ICON);//菜单指针
	wcex.lpszClassName = szWindowClass;//类名指针
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL));//小图标句柄

	return RegisterClassEx(&wcex);//注册窗口类
}

/*
** Function:  InitInstance(HINSTANCE, int)
** Purpose:  保存实例句柄并创建主窗口
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;//窗口句柄
	WINDOWINFO Windowinfo;//窗口信息
	RECT Rect;//创建矩形区域
	int li_ScreenWidth = 0;//屏幕宽度
	int li_ScreenHeight = 0;//屏幕高度
	int li_PosX = 0;//窗口X坐标
	int li_PosY = 0;//窗口Y坐标

	hInst = hInstance; // 将实例句柄存储在全局变量中

	//自适应窗体界面
	SetRect(&Rect, 0, 0, USER_SCREENWIDTH, USER_SCREENHEIGHT);
	AdjustWindowRect(&Rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, (Rect.right - Rect.left), (Rect.bottom - Rect.top), NULL, NULL, hInstance, NULL);

	if(!hWnd)
	{
		return FALSE;
	}

	hWND = hWnd;

	//获取当前窗口信息
	GetWindowInfo(hWnd, &Windowinfo);//获取窗口信息
	g_nUserScreenWidth = Windowinfo.rcClient.right - Windowinfo.rcClient.left;//用户区域宽度
	g_nUserScreenHeight = Windowinfo.rcClient.bottom - Windowinfo.rcClient.top;//用户区域高度

	//计算窗口初始坐标
	li_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);//屏幕宽度
	li_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);//屏幕高度

	//设置窗口位置
	if(li_ScreenWidth > g_nUserScreenWidth && li_ScreenHeight > g_nUserScreenHeight)
	{
		li_PosX = (li_ScreenWidth - g_nUserScreenWidth) >> 1;//窗口X坐标
		li_PosY = (li_ScreenHeight - g_nUserScreenHeight) >> 1;//窗口Y坐标
		SetWindowPos(hWnd, HWND_TOP, li_PosX, li_PosY, 0, 0, SWP_NOSIZE);//设置窗口位置(ScreenCenter)
	}

	//Direct3D初始化
	if(!(Direct3D_Init(hWnd, hInstance) == S_OK))
	{
		MessageBox(hWnd, _T("Direct3D初始化失败~!"), _T("DirectXProject"), 0);
	}

	//Direct3D应用程序初始化
	if(!(Direct3D_SetUp() == S_OK))
	{
		MessageBox(hWnd, _T("应用初始化失败~!"), _T("DirectXProject"), 0);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

/*
** Function:  WndProc(HWND, UINT, WPARAM, LPARAM)
** Purpose:    处理主窗口的消息
** WM_COMMAND	- 处理应用程序菜单
** WM_PAINT - 绘制主窗口
** WM_DESTROY - 发送退出消息并返回
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch(message)
	{
	case WM_COMMAND://窗口命令
		wmId = LOWORD(wParam);//控件ID
		wmEvent = HIWORD(wParam);//通知码
		break;
	case WM_ERASEBKGND://窗口擦除
		break;
	case WM_PAINT://窗口重绘
		hdc = BeginPaint(hWnd, &ps);//开始重绘
		Direct3D_Update(hWnd);
		Direct3D_Render(hWnd);
		EndPaint(hWnd, &ps);//结束重绘
		break;
	case WM_TIMER:
		break;
	case WM_DESTROY://窗口销毁
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
** Function:  ReleaseWindows(void)
** Purpose:  释放窗口资源
*/
void ReleaseWindows(void)
{
	Direct3D_ClearUp();
}

/*
** Function:  RePaintWindows(void)
** Purpose:  重绘窗口
*/
void RePaintWindows(HWND hWnd, HINSTANCE hInstance)
{
	HDC MemDC;
	HBITMAP Title;
	BITMAPINFO TitleInfo;

	MemDC = CreateCompatibleDC(GetDC(hWnd));//HDC创建窗口设备
	Title = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE));//Bitmap加载标题图片
	GetObject(Title, sizeof(TitleInfo), &TitleInfo);//Bitmap获取图片信息
	SelectObject(MemDC, Title);//Bitmap绘制图片缓冲

	StretchBlt(GetDC(hWnd), 0, 0, USER_SCREENWIDTH, USER_SCREENHEIGHT, MemDC, 0, 0, TitleInfo.bmiHeader.biWidth, TitleInfo.bmiHeader.biHeight, SRCCOPY);

	DeleteObject(Title);//释放Bitmap
	DeleteDC(MemDC);//释放HDC
}