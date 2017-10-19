#include"Common.h"
#include "Include.h"
#include "Template.h"

/*
** Global Variable
*/
//D3DDevice
LPDIRECT3DDEVICE9 g_pD3D9Device = NULL;//DirectX设备

//DirectInput
LPDIRECTINPUT8 g_pDirectInput = NULL;//DirectInput输入
LPDIRECTINPUTDEVICE8 g_pDirectInputMouseDevice = NULL;//DirectInput设备:Mouse鼠标
LPDIRECTINPUTDEVICE8 g_pDirectInputKeyBoardDevice = NULL;//DirectInput设备:KeyBoard键盘
DIMOUSESTATE g_DiMouseState = { 0 };//Mouse鼠标状态
char g_pKeyStateBuffer[256] = { 0 };//KeyBoard键盘状态

//D3DXMesh
LPD3DXMESH g_pMesh = NULL;//网格对象
D3DMATERIAL9*g_pMaterial = NULL;//网格材质信息
LPDIRECT3DTEXTURE9*g_ppTexture = NULL;//网格纹理信息
DWORD g_dwNumMtrl = 0;// 材质的数目 

//D3DXFont
LPD3DXFONT g_pFontFPS = NULL;//FPS速率字体
LPD3DXFONT g_pFontAdaperName = NULL;//显卡型号信息
LPD3DXFONT g_pFontHelp = NULL;//帮助信息
LPD3DXFONT g_pFontInfo = NULL;//绘制信息

//FPS
float	g_FPS = 0.0f;//帧速率
wchar_t g_sFPS[20];//帧速率字符数组

//Adapter
wchar_t g_sAdapterName[60] = { 0 };//显卡名称字符数组

/*
** Function:  Direct3D_Init(HWND hWnd, HINSTANCE hInstance)
** Purpose:  Direct3D初始化
** Return:  HRESULT初始化状态
*/
HRESULT Direct3D_Init(HWND hWnd, HINSTANCE hInstance)
{
	IDirect3D9 *pD3D9 = NULL;
	D3DCAPS9 Caps;
	D3DPRESENT_PARAMETERS D3Dpp;
	HRESULT Result;
	int Vp;

	//Step1 获取IDirect3D9接口指针
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);//获取IDirect3D9接口指针

	if(pD3D9 == NULL)//IDirect3D9接口指针为NULL
	{
		return E_FAIL;//FAIL
	}

	//Step2 校验硬件顶点运算
	Result = pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);//校验硬件顶点运算

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	if(Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		Vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//支持硬件顶点运算(硬件顶点运算)
	}
	else
	{
		Vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;//不支持硬件顶点运算(软件顶点运算)
	}

	//Step3 填充D3DPRESENT_PARAMETERS结构
	D3Dpp.BackBufferWidth = USER_SCREENWIDTH;//后台缓冲表面宽度(Pixel)
	D3Dpp.BackBufferHeight = USER_SCREENHEIGHT;//后台缓冲表面高度(Pixel)
	D3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;//后台缓冲像素格式
	D3Dpp.BackBufferCount = 1;//后台缓冲数量(1)
	D3Dpp.MultiSampleType = D3DMULTISAMPLE_NONE;//后台缓冲多重采样类型
	D3Dpp.MultiSampleQuality = 0;//后台缓冲多重采样质量
	D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//交换链页面置换方式
	D3Dpp.hDeviceWindow = hWnd;//设备相关窗口句柄
	D3Dpp.Windowed = true;//窗口模式:true/全屏模式:false
	D3Dpp.EnableAutoDepthStencil = true;//Direct3D自动创建维护深度缓冲和模板缓冲
	D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//深度缓冲和模板缓冲像素格式
	D3Dpp.Flags = 0;//无标记格式
	D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//Direct3D默认刷新频率
	D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//Direct3D提交频率(默认提交)

	//Step4 创建IDirect3DDevice9接口
	Result = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, Vp, &D3Dpp, &g_pD3D9Device);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//Step5 获取显卡型号
	D3DADAPTER_IDENTIFIER9 Adapter;
	wchar_t AdapterName[60] = L"Adapter Version:";
	int Length = 0;
	
	pD3D9->GetAdapterIdentifier(0, 0, &Adapter);
	Length = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_sAdapterName, Length);
	wcscat_s(AdapterName, g_sAdapterName);
	wcscpy_s(g_sAdapterName, AdapterName);

	//Step6创建DirectInput设备
	//Mouse鼠标
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);//创建DirectInput设备
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pDirectInputMouseDevice, NULL);//Mouse鼠标设备

	g_pDirectInputMouseDevice->SetDataFormat(&c_dfDIMouse);//设置数据格式

	g_pDirectInputMouseDevice->Acquire();//获取设备控制权

	//KeyBoard键盘
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);//创建DirectInput设备
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pDirectInputKeyBoardDevice, NULL);//KeyBoard设备
	
	g_pDirectInputKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard);//设置数据格式
	g_pDirectInputKeyBoardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);//设置协作级别

	g_pDirectInputKeyBoardDevice->Acquire();//获取设备控制权

	SafeRelease<IDirect3D9*>(pD3D9);//释放指针

	return S_OK;
}

/*
** Function:  Direct3D_SetUp()
** Purpose:  Direct3D应用程序初始化
** Return:  HRESULT初始化状态
*/
HRESULT Direct3D_SetUp(void)
{
	//创建字体
	HRESULT Result;

	//FPS帧速率字体
	Result = D3DXCreateFont(g_pD3D9Device, 12, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("等线"), &g_pFontFPS);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//显卡型号信息字体
	Result = D3DXCreateFont(g_pD3D9Device, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontAdaperName);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//帮助信息字体
	Result = D3DXCreateFont(g_pD3D9Device, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontHelp);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//绘制信息字体
	Result = D3DXCreateFont(g_pD3D9Device, 24, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontInfo);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//X文件加载网格数据
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pD3D9Device, &pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrl, &g_pMesh);

	//读取材质纹理数据
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	g_pMaterial = new D3DMATERIAL9[g_dwNumMtrl];
	g_ppTexture = new LPDIRECT3DTEXTURE9[g_dwNumMtrl];

	for(DWORD i = 0; i < g_dwNumMtrl; i++)
	{
		g_pMaterial[i] = pMtrls[i].MatD3D;
		g_pMaterial[i].Ambient = g_pMaterial[i].Diffuse;
		g_ppTexture[i] = NULL;
		D3DXCreateTextureFromFileA(g_pD3D9Device, pMtrls[i].pTextureFilename, &g_ppTexture[i]);
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();

	//设置渲染状态
	g_pD3D9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//开启背面消隐
	g_pD3D9Device->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//设置环境光

	g_pD3D9Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//设置为线性纹理过滤
	g_pD3D9Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

/*
** Function:  Direct3D_Update(HWND hWnd)
** Purpose:  Direct3D更新
*/
void Direct3D_Update(HWND hWnd)
{
	if(g_pKeyStateBuffer[DIK_1] & 0x80)
	{
		g_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);//实体填充
	}
	if(g_pKeyStateBuffer[DIK_2] & 0x80)
	{
		g_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//线框填充
	}

	// 读取鼠标输入
	ZeroMemory(&g_DiMouseState, sizeof(g_DiMouseState));
	Device_Read(g_pDirectInputMouseDevice, (LPVOID)&g_DiMouseState, sizeof(g_DiMouseState));

	// 读取键盘输入
	ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pDirectInputKeyBoardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));

	// 按住鼠标右键并拖动，为平移操作
	D3DXMATRIX WorldMatrix;
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if(g_DiMouseState.rgbButtons[1] & 0x80)
	{
		fPosX += g_DiMouseState.lX *  0.5f;
		fPosY += g_DiMouseState.lY * -0.5f;
	}

	//鼠标滚轮，为观察点收缩操作
	fPosZ -= g_DiMouseState.lZ * 0.02f;

	// 平移物体
	if(g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.5f;
	if(g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.5f;
	if(g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.5f;
	if(g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.5f;

	D3DXMatrixTranslation(&WorldMatrix, fPosX, fPosY, fPosZ);

	// 按住鼠标左键并拖动，为旋转操作
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI;
	if(g_DiMouseState.rgbButtons[0] & 0x80)
	{
		fAngleX -= g_DiMouseState.lY * -0.01f;
		fAngleY += g_DiMouseState.lX * -0.01f;
	}
	// 旋转物体
	if(g_pKeyStateBuffer[DIK_UP] & 0x80) fAngleX -= 0.05f;
	if(g_pKeyStateBuffer[DIK_DOWN] & 0x80) fAngleX += 0.05f;
	if(g_pKeyStateBuffer[DIK_LEFT] & 0x80) fAngleY -= 0.05f;
	if(g_pKeyStateBuffer[DIK_RIGHT] & 0x80) fAngleY += 0.05f;

	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	WorldMatrix = Rx * Ry * WorldMatrix;
	g_pD3D9Device->SetTransform(D3DTS_WORLD, &WorldMatrix);

	MatrixSet();
}

/*
** Function:  Direct3D_Render(HWND hWnd)
** Purpose:  Direct3D渲染
*/
void Direct3D_Render(HWND hWnd)
{
	RECT FormatRect;

	//Step1 Clear清除
	g_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	GetClientRect(hWnd, &FormatRect);

	//Step2 开始渲染
	g_pD3D9Device->BeginScene();

	//Step3 渲染代码
	// 绘制网格
	for(DWORD i = 0; i < g_dwNumMtrl; i++)
	{
		g_pD3D9Device->SetMaterial(&g_pMaterial[i]);
		g_pD3D9Device->SetTexture(0, g_ppTexture[i]);
		g_pMesh->DrawSubset(i);
	}

	//显示显卡类型名
	g_pFontAdaperName->DrawText(NULL, g_sAdapterName, -1, &FormatRect, DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));

	//绘制帮助信息
	RECT Rect = FormatRect;

	Rect.top = 312; 
	Rect.left = 0;
	g_pFontInfo->DrawText(NULL, L"Controls:", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	Rect.left += 24;
	g_pFontHelp->DrawText(NULL, L"Rolate Model: Left Mouse Button.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Rolate Model: Direction Key.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Translate Model: Right Mouse Button.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Translate Model: KeyW,KeyA,KeyS,KeyD.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Rolate Camera: Middle Mouse Button.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Change Fill Mode: Key Number 1/2.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));
	Rect.top += 24;
	g_pFontHelp->DrawText(NULL, L"Quit Program: Key ESC.", -1, &Rect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DXCOLOR(1.0, 0.5f, 0.5f, 1.0f));

	//帧速率
	int CharCount = swprintf_s(g_sFPS, 20, _T("%0.1ffps"), Direct3D_FPS());
	g_pFontFPS->DrawText(NULL, g_sFPS, CharCount, &FormatRect, DT_BOTTOM | DT_RIGHT, D3DXCOLOR(0.96f, 0.5f, 0.5f, 1.0f));

	//Step4 结束渲染
	g_pD3D9Device->EndScene();

	//Step5 Display显示
	g_pD3D9Device->Present(NULL, NULL, NULL, NULL);
}

/*
** Function:  Direct3D_ClearUp()
** Purpose:  Direct3D释放资源
*/
void Direct3D_ClearUp(void)
{
	g_pDirectInputMouseDevice->Unacquire();
	g_pDirectInputKeyBoardDevice->Unacquire();
	for(DWORD i = 0; i<g_dwNumMtrl; i++)
		SAFE_RELEASE(g_ppTexture[i]);
	SAFE_DELETE(g_ppTexture)
	SAFE_DELETE(g_pMaterial)
	SAFE_RELEASE(g_pMesh)
	SafeRelease<LPD3DXFONT>(g_pFontFPS);
	SafeRelease<LPDIRECT3DDEVICE9>(g_pD3D9Device);
}

/*
** Function:  Direct3D_FPS()
** Purpose:  Direct3D计算fps
** Return:  float fps
*/
float Direct3D_FPS(void)
{
	static int FrameCount = 0;
	float CurrentTime = 0.0f;
	static float LastTime = 0.0f;
	static float fps = 0.0f;
	float TimeDelta = 0.0f;

	FrameCount++;
	CurrentTime = (float)timeGetTime();
	TimeDelta = (CurrentTime - LastTime) * 0.001f;

	if(TimeDelta >= 1.0f)
	{
		fps = (float)(FrameCount * 1.0f / TimeDelta);
		LastTime = CurrentTime;
		FrameCount = 0;
	}

	return fps;
}

/*
** Function:  MatrixSet()
** Purpose:  Direct3D坐标变换
*/
void MatrixSet(void)
{
	//ViewSpace取景坐标系
	D3DXMATRIX MatrixView;
	D3DXVECTOR3 Vector_Eye(0.0f, 0.0f, -250.0f);
	D3DXVECTOR3 Vector_At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Vector_Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&MatrixView, &Vector_Eye, &Vector_At, &Vector_Up);
	g_pD3D9Device->SetTransform(D3DTS_VIEW, &MatrixView);

	//PerspectiveProjection投影坐标系
	D3DXMATRIX MatrixProject;
	D3DXMatrixPerspectiveFovLH(&MatrixProject, D3DX_PI / 4.0f, (float)USER_SCREENWIDTH / (float)USER_SCREENHEIGHT, 1.0f, 1000.0f);
	g_pD3D9Device->SetTransform(D3DTS_PROJECTION, &MatrixProject);

	//ViewPortTransform视口坐标系
	D3DVIEWPORT9 ViewPort;
	ViewPort.X = 0;
	ViewPort.Y = 0;
	ViewPort.Width = USER_SCREENWIDTH;
	ViewPort.Height = USER_SCREENHEIGHT;
	ViewPort.MinZ = 0.0f;
	ViewPort.MaxZ = 1.0f;
	g_pD3D9Device->SetViewport(&ViewPort);

}

BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize)
{
	HRESULT hr;
	while(true)
	{
		pDIDevice->Poll();// 轮询设备
		pDIDevice->Acquire();// 获取设备的控制权
		if(SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if(hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if(FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}