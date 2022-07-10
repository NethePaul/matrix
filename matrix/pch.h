#pragma once
#include"coroutines.h"
template<class T> void SafeRelease(T*ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = 0;
	}
}
#include<string.h>
#include<Windows.h>
#include<windowsx.h>
#include<math.h>
#include<d2d1.h>
#include<d2d1_1.h>
#include<d2d1_1helper.h>
#include<time.h>
#include<vector>
#include<wincodec.h>
#include<wincodecsdk.h>
#include<wrl\client.h>
#include<dwrite.h>
#include<fstream>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"windowscodecs.lib")

#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")

#define PI 3.1415926535898

#pragma comment(lib,"d2d1")

#include"VKex.h"
#include"complexnumbers.h"
#include"utility.h"
#include"imageSystem.h"

#define WIDTH 1600
#define HEIGHT 900