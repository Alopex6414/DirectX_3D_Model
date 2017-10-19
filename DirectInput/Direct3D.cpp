#include"Common.h"
#include "Include.h"
#include "Template.h"

/*
** Global Variable
*/
//D3DDevice
LPDIRECT3DDEVICE9 g_pD3D9Device = NULL;//DirectX�豸

//DirectInput
LPDIRECTINPUT8 g_pDirectInput = NULL;//DirectInput����
LPDIRECTINPUTDEVICE8 g_pDirectInputMouseDevice = NULL;//DirectInput�豸:Mouse���
LPDIRECTINPUTDEVICE8 g_pDirectInputKeyBoardDevice = NULL;//DirectInput�豸:KeyBoard����
DIMOUSESTATE g_DiMouseState = { 0 };//Mouse���״̬
char g_pKeyStateBuffer[256] = { 0 };//KeyBoard����״̬

//D3DXMesh
LPD3DXMESH g_pMesh = NULL;//�������
D3DMATERIAL9*g_pMaterial = NULL;//���������Ϣ
LPDIRECT3DTEXTURE9*g_ppTexture = NULL;//����������Ϣ
DWORD g_dwNumMtrl = 0;// ���ʵ���Ŀ 

//D3DXFont
LPD3DXFONT g_pFontFPS = NULL;//FPS��������
LPD3DXFONT g_pFontAdaperName = NULL;//�Կ��ͺ���Ϣ
LPD3DXFONT g_pFontHelp = NULL;//������Ϣ
LPD3DXFONT g_pFontInfo = NULL;//������Ϣ

//FPS
float	g_FPS = 0.0f;//֡����
wchar_t g_sFPS[20];//֡�����ַ�����

//Adapter
wchar_t g_sAdapterName[60] = { 0 };//�Կ������ַ�����

/*
** Function:  Direct3D_Init(HWND hWnd, HINSTANCE hInstance)
** Purpose:  Direct3D��ʼ��
** Return:  HRESULT��ʼ��״̬
*/
HRESULT Direct3D_Init(HWND hWnd, HINSTANCE hInstance)
{
	IDirect3D9 *pD3D9 = NULL;
	D3DCAPS9 Caps;
	D3DPRESENT_PARAMETERS D3Dpp;
	HRESULT Result;
	int Vp;

	//Step1 ��ȡIDirect3D9�ӿ�ָ��
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);//��ȡIDirect3D9�ӿ�ָ��

	if(pD3D9 == NULL)//IDirect3D9�ӿ�ָ��ΪNULL
	{
		return E_FAIL;//FAIL
	}

	//Step2 У��Ӳ����������
	Result = pD3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);//У��Ӳ����������

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	if(Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		Vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//֧��Ӳ����������(Ӳ����������)
	}
	else
	{
		Vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;//��֧��Ӳ����������(�����������)
	}

	//Step3 ���D3DPRESENT_PARAMETERS�ṹ
	D3Dpp.BackBufferWidth = USER_SCREENWIDTH;//��̨���������(Pixel)
	D3Dpp.BackBufferHeight = USER_SCREENHEIGHT;//��̨�������߶�(Pixel)
	D3Dpp.BackBufferFormat = D3DFMT_A8R8G8B8;//��̨�������ظ�ʽ
	D3Dpp.BackBufferCount = 1;//��̨��������(1)
	D3Dpp.MultiSampleType = D3DMULTISAMPLE_NONE;//��̨������ز�������
	D3Dpp.MultiSampleQuality = 0;//��̨������ز�������
	D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//������ҳ���û���ʽ
	D3Dpp.hDeviceWindow = hWnd;//�豸��ش��ھ��
	D3Dpp.Windowed = true;//����ģʽ:true/ȫ��ģʽ:false
	D3Dpp.EnableAutoDepthStencil = true;//Direct3D�Զ�����ά����Ȼ����ģ�建��
	D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//��Ȼ����ģ�建�����ظ�ʽ
	D3Dpp.Flags = 0;//�ޱ�Ǹ�ʽ
	D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//Direct3DĬ��ˢ��Ƶ��
	D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//Direct3D�ύƵ��(Ĭ���ύ)

	//Step4 ����IDirect3DDevice9�ӿ�
	Result = pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, Vp, &D3Dpp, &g_pD3D9Device);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//Step5 ��ȡ�Կ��ͺ�
	D3DADAPTER_IDENTIFIER9 Adapter;
	wchar_t AdapterName[60] = L"Adapter Version:";
	int Length = 0;
	
	pD3D9->GetAdapterIdentifier(0, 0, &Adapter);
	Length = MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, Adapter.Description, -1, g_sAdapterName, Length);
	wcscat_s(AdapterName, g_sAdapterName);
	wcscpy_s(g_sAdapterName, AdapterName);

	//Step6����DirectInput�豸
	//Mouse���
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);//����DirectInput�豸
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pDirectInputMouseDevice, NULL);//Mouse����豸

	g_pDirectInputMouseDevice->SetDataFormat(&c_dfDIMouse);//�������ݸ�ʽ

	g_pDirectInputMouseDevice->Acquire();//��ȡ�豸����Ȩ

	//KeyBoard����
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);//����DirectInput�豸
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pDirectInputKeyBoardDevice, NULL);//KeyBoard�豸
	
	g_pDirectInputKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard);//�������ݸ�ʽ
	g_pDirectInputKeyBoardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);//����Э������

	g_pDirectInputKeyBoardDevice->Acquire();//��ȡ�豸����Ȩ

	SafeRelease<IDirect3D9*>(pD3D9);//�ͷ�ָ��

	return S_OK;
}

/*
** Function:  Direct3D_SetUp()
** Purpose:  Direct3DӦ�ó����ʼ��
** Return:  HRESULT��ʼ��״̬
*/
HRESULT Direct3D_SetUp(void)
{
	//��������
	HRESULT Result;

	//FPS֡��������
	Result = D3DXCreateFont(g_pD3D9Device, 12, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("����"), &g_pFontFPS);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//�Կ��ͺ���Ϣ����
	Result = D3DXCreateFont(g_pD3D9Device, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontAdaperName);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//������Ϣ����
	Result = D3DXCreateFont(g_pD3D9Device, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontHelp);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//������Ϣ����
	Result = D3DXCreateFont(g_pD3D9Device, 24, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("Calibri"), &g_pFontInfo);

	if(FAILED(Result))
	{
		return E_FAIL;//FAIL
	}

	//X�ļ�������������
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pD3D9Device, &pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrl, &g_pMesh);

	//��ȡ������������
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

	//������Ⱦ״̬
	g_pD3D9Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//������������
	g_pD3D9Device->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//���û�����

	g_pD3D9Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//����Ϊ�����������
	g_pD3D9Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

/*
** Function:  Direct3D_Update(HWND hWnd)
** Purpose:  Direct3D����
*/
void Direct3D_Update(HWND hWnd)
{
	if(g_pKeyStateBuffer[DIK_1] & 0x80)
	{
		g_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);//ʵ�����
	}
	if(g_pKeyStateBuffer[DIK_2] & 0x80)
	{
		g_pD3D9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//�߿����
	}

	// ��ȡ�������
	ZeroMemory(&g_DiMouseState, sizeof(g_DiMouseState));
	Device_Read(g_pDirectInputMouseDevice, (LPVOID)&g_DiMouseState, sizeof(g_DiMouseState));

	// ��ȡ��������
	ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pDirectInputKeyBoardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));

	// ��ס����Ҽ����϶���Ϊƽ�Ʋ���
	D3DXMATRIX WorldMatrix;
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if(g_DiMouseState.rgbButtons[1] & 0x80)
	{
		fPosX += g_DiMouseState.lX *  0.5f;
		fPosY += g_DiMouseState.lY * -0.5f;
	}

	//�����֣�Ϊ�۲����������
	fPosZ -= g_DiMouseState.lZ * 0.02f;

	// ƽ������
	if(g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.5f;
	if(g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.5f;
	if(g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.5f;
	if(g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.5f;

	D3DXMatrixTranslation(&WorldMatrix, fPosX, fPosY, fPosZ);

	// ��ס���������϶���Ϊ��ת����
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI;
	if(g_DiMouseState.rgbButtons[0] & 0x80)
	{
		fAngleX -= g_DiMouseState.lY * -0.01f;
		fAngleY += g_DiMouseState.lX * -0.01f;
	}
	// ��ת����
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
** Purpose:  Direct3D��Ⱦ
*/
void Direct3D_Render(HWND hWnd)
{
	RECT FormatRect;

	//Step1 Clear���
	g_pD3D9Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	GetClientRect(hWnd, &FormatRect);

	//Step2 ��ʼ��Ⱦ
	g_pD3D9Device->BeginScene();

	//Step3 ��Ⱦ����
	// ��������
	for(DWORD i = 0; i < g_dwNumMtrl; i++)
	{
		g_pD3D9Device->SetMaterial(&g_pMaterial[i]);
		g_pD3D9Device->SetTexture(0, g_ppTexture[i]);
		g_pMesh->DrawSubset(i);
	}

	//��ʾ�Կ�������
	g_pFontAdaperName->DrawText(NULL, g_sAdapterName, -1, &FormatRect, DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));

	//���ư�����Ϣ
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

	//֡����
	int CharCount = swprintf_s(g_sFPS, 20, _T("%0.1ffps"), Direct3D_FPS());
	g_pFontFPS->DrawText(NULL, g_sFPS, CharCount, &FormatRect, DT_BOTTOM | DT_RIGHT, D3DXCOLOR(0.96f, 0.5f, 0.5f, 1.0f));

	//Step4 ������Ⱦ
	g_pD3D9Device->EndScene();

	//Step5 Display��ʾ
	g_pD3D9Device->Present(NULL, NULL, NULL, NULL);
}

/*
** Function:  Direct3D_ClearUp()
** Purpose:  Direct3D�ͷ���Դ
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
** Purpose:  Direct3D����fps
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
** Purpose:  Direct3D����任
*/
void MatrixSet(void)
{
	//ViewSpaceȡ������ϵ
	D3DXMATRIX MatrixView;
	D3DXVECTOR3 Vector_Eye(0.0f, 0.0f, -250.0f);
	D3DXVECTOR3 Vector_At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Vector_Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&MatrixView, &Vector_Eye, &Vector_At, &Vector_Up);
	g_pD3D9Device->SetTransform(D3DTS_VIEW, &MatrixView);

	//PerspectiveProjectionͶӰ����ϵ
	D3DXMATRIX MatrixProject;
	D3DXMatrixPerspectiveFovLH(&MatrixProject, D3DX_PI / 4.0f, (float)USER_SCREENWIDTH / (float)USER_SCREENHEIGHT, 1.0f, 1000.0f);
	g_pD3D9Device->SetTransform(D3DTS_PROJECTION, &MatrixProject);

	//ViewPortTransform�ӿ�����ϵ
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
		pDIDevice->Poll();// ��ѯ�豸
		pDIDevice->Acquire();// ��ȡ�豸�Ŀ���Ȩ
		if(SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if(hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if(FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}