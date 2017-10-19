#pragma once

#ifndef __DIRECT3D_H_
#define __DIRECT3D_H_

#include "Common.h"

//Define
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }      //自定义一个SAFE_RELEASE()宏,便于COM资源的释放
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }

//Struct Declaration
struct Vertex
{
	float X, Y, Z;
	float Nx, Ny, Nz;
};
#define D3DFVF_VERTEX	(D3DFVF_XYZ|D3DFVF_NORMAL)

//Variable Declaration

//Function Declaration
extern HRESULT Direct3D_Init(HWND hWnd, HINSTANCE hInstance);
extern HRESULT Direct3D_SetUp(void);
extern void Direct3D_Update(HWND hWnd);
extern void Direct3D_Render(HWND hWnd);
extern void Direct3D_ClearUp(void);
extern float Direct3D_FPS(void);
extern void MatrixSet(void);
extern BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);

#endif // !__DIRECT3D_H_
