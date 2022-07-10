#pragma once
#include"pch.h"

namespace WIC {
	 HRESULT LoadBitmapFromFile(
		ID2D1HwndRenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR uri,
		ID2D1Bitmap **ppBitmap
	 );
	 void DrawBitmap(ID2D1RenderTarget *pRenderTarget, ID2D1Bitmap*pBitmap, float x, float y, float width = -1, float height = -1);
	 void initWIC(IWICImagingFactory**pFactoryWIC);
}

namespace IDW {
	HRESULT CreateDeviceIndependentResources(ID2D1Factory*m_pD2DFactory, IDWriteFactory**m_pDWriteFactory, IDWriteTextFormat**m_pTextFormat, float size);
}
namespace num {
	int wstrToInt(wchar_t*in);
	std::wstring intToWstr(int in);
	template<class T>
	bool ca(T z, T r) {
		if (z <= r&&z >= r*(-1))return 1;
		return 0;
	}
	template<class T1, class T2>void swap(T1&a, T2&b) {
		auto c = a;
		a = b;
		b = c;
	}
}
#define in_if(con,then,el)((con)?(then):(el))