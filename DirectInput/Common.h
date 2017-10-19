#pragma once

#ifndef __COMMON_H_
#define __COMMON_H_

#define WIN32_LEAN_AND_MEAN		//从 Windows 头文件中排除极少使用的信息

//Windows 头文件: 
#include <windows.h>

//C 运行时头文件
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <tchar.h>
#include <time.h>
#include <mmsystem.h>

//包含DirectX文件
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <dinput.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"WinMM.lib")

//包括 SDKDDKVer.h 将定义可用的最高版本的 Windows 平台
#include <SDKDDKVer.h>

//包含资源文件
#include "resource.h"

#endif // !__COMMON_H_
