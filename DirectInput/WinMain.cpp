#include "Common.h"
#include "Include.h"

/*
** Global Variable
*/
HWND hWND;//���ھ��
HINSTANCE hInst;//��ǰʵ�����ھ��
TCHAR szTitle[MAX_LOADSTRING];//�������ı�
TCHAR szWindowClass[MAX_LOADSTRING];//����������

//User
int g_nUserScreenWidth = 0;//�û����򴰿ڿ��
int g_nUserScreenHeight = 0;//�û����򴰿ڸ߶�

/*
** Function Declaration
*/
ATOM MyRegisterClass(HINSTANCE hInstance);//ע�ᴰ����
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);//��ʼ������������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);//���������ڵ���Ϣ
void ReleaseWindows(void);//�ͷŴ�����Դ
void RePaintWindows(HWND hWnd, HINSTANCE hInstance);//�ػ洰��

/*
** WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nCmdShow)
** Purpose:  ����������
*/
int WINAPI WinMain(IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);//��������������hPrevInstance����
	UNREFERENCED_PARAMETER(lpCmdLine);//��������������lpCmdLine����

	MSG msg;//������Ϣ

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_TITLE, szTitle, MAX_LOADSTRING);//�������Ƶ���szTitle����
	LoadString(hInstance, IDS_CLASS, szWindowClass, MAX_LOADSTRING);//������������szWindowClass����

																	//ע�ᴰ����
	MyRegisterClass(hInstance);

	//ִ��Ӧ�ó����ʼ��
	if(!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// ����Ϣѭ��
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
			Direct3D_Update(hWND);//Direct3D����
			Direct3D_Render(hWND);//Direct3D��Ⱦ
		}
	}

	//�ͷŴ�����Դ
	ReleaseWindows();

	return (int)msg.wParam;
}

/*
** Function:  MyRegisterClass()
** Purpose:  ע�ᴰ����
*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);//WNDCLASSEX���С

	wcex.style = CS_HREDRAW | CS_VREDRAW;//��������ʽ
	wcex.lpfnWndProc = WndProc;//������ص�����
	wcex.cbClsExtra = 0;//������ṹ�����ֽ���
	wcex.cbWndExtra = 0;//����ʵ�������ֽ���
	wcex.hInstance = hInstance;//���ھ��
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));//ͼ����
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//�����
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//������ˢ��ɫ(�Զ�)
	wcex.lpszMenuName = MAKEINTRESOURCE(IDI_ICON);//�˵�ָ��
	wcex.lpszClassName = szWindowClass;//����ָ��
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL));//Сͼ����

	return RegisterClassEx(&wcex);//ע�ᴰ����
}

/*
** Function:  InitInstance(HINSTANCE, int)
** Purpose:  ����ʵ�����������������
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;//���ھ��
	WINDOWINFO Windowinfo;//������Ϣ
	RECT Rect;//������������
	int li_ScreenWidth = 0;//��Ļ���
	int li_ScreenHeight = 0;//��Ļ�߶�
	int li_PosX = 0;//����X����
	int li_PosY = 0;//����Y����

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	//����Ӧ�������
	SetRect(&Rect, 0, 0, USER_SCREENWIDTH, USER_SCREENHEIGHT);
	AdjustWindowRect(&Rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, (Rect.right - Rect.left), (Rect.bottom - Rect.top), NULL, NULL, hInstance, NULL);

	if(!hWnd)
	{
		return FALSE;
	}

	hWND = hWnd;

	//��ȡ��ǰ������Ϣ
	GetWindowInfo(hWnd, &Windowinfo);//��ȡ������Ϣ
	g_nUserScreenWidth = Windowinfo.rcClient.right - Windowinfo.rcClient.left;//�û�������
	g_nUserScreenHeight = Windowinfo.rcClient.bottom - Windowinfo.rcClient.top;//�û�����߶�

	//���㴰�ڳ�ʼ����
	li_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);//��Ļ���
	li_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);//��Ļ�߶�

	//���ô���λ��
	if(li_ScreenWidth > g_nUserScreenWidth && li_ScreenHeight > g_nUserScreenHeight)
	{
		li_PosX = (li_ScreenWidth - g_nUserScreenWidth) >> 1;//����X����
		li_PosY = (li_ScreenHeight - g_nUserScreenHeight) >> 1;//����Y����
		SetWindowPos(hWnd, HWND_TOP, li_PosX, li_PosY, 0, 0, SWP_NOSIZE);//���ô���λ��(ScreenCenter)
	}

	//Direct3D��ʼ��
	if(!(Direct3D_Init(hWnd, hInstance) == S_OK))
	{
		MessageBox(hWnd, _T("Direct3D��ʼ��ʧ��~!"), _T("DirectXProject"), 0);
	}

	//Direct3DӦ�ó����ʼ��
	if(!(Direct3D_SetUp() == S_OK))
	{
		MessageBox(hWnd, _T("Ӧ�ó�ʼ��ʧ��~!"), _T("DirectXProject"), 0);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

/*
** Function:  WndProc(HWND, UINT, WPARAM, LPARAM)
** Purpose:    ���������ڵ���Ϣ
** WM_COMMAND	- ����Ӧ�ó���˵�
** WM_PAINT - ����������
** WM_DESTROY - �����˳���Ϣ������
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch(message)
	{
	case WM_COMMAND://��������
		wmId = LOWORD(wParam);//�ؼ�ID
		wmEvent = HIWORD(wParam);//֪ͨ��
		break;
	case WM_ERASEBKGND://���ڲ���
		break;
	case WM_PAINT://�����ػ�
		hdc = BeginPaint(hWnd, &ps);//��ʼ�ػ�
		Direct3D_Update(hWnd);
		Direct3D_Render(hWnd);
		EndPaint(hWnd, &ps);//�����ػ�
		break;
	case WM_TIMER:
		break;
	case WM_DESTROY://��������
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
** Function:  ReleaseWindows(void)
** Purpose:  �ͷŴ�����Դ
*/
void ReleaseWindows(void)
{
	Direct3D_ClearUp();
}

/*
** Function:  RePaintWindows(void)
** Purpose:  �ػ洰��
*/
void RePaintWindows(HWND hWnd, HINSTANCE hInstance)
{
	HDC MemDC;
	HBITMAP Title;
	BITMAPINFO TitleInfo;

	MemDC = CreateCompatibleDC(GetDC(hWnd));//HDC���������豸
	Title = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE));//Bitmap���ر���ͼƬ
	GetObject(Title, sizeof(TitleInfo), &TitleInfo);//Bitmap��ȡͼƬ��Ϣ
	SelectObject(MemDC, Title);//Bitmap����ͼƬ����

	StretchBlt(GetDC(hWnd), 0, 0, USER_SCREENWIDTH, USER_SCREENHEIGHT, MemDC, 0, 0, TitleInfo.bmiHeader.biWidth, TitleInfo.bmiHeader.biHeight, SRCCOPY);

	DeleteObject(Title);//�ͷ�Bitmap
	DeleteDC(MemDC);//�ͷ�HDC
}