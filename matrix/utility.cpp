#include "utility.h"

HRESULT WIC::LoadBitmapFromFile(ID2D1HwndRenderTarget * pRenderTarget, IWICImagingFactory * pIWICFactory, PCWSTR uri, ID2D1Bitmap ** ppBitmap)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;


	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);

	}


	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
		if (SUCCEEDED(hr))
		{

			// Create a Direct2D bitmap from the WIC bitmap.
			hr = pRenderTarget->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				ppBitmap
			);
		}

		SafeRelease(&pDecoder);
		SafeRelease(&pSource);
		SafeRelease(&pStream);
		SafeRelease(&pConverter);

		return hr;
	}
}

void WIC::DrawBitmap(ID2D1RenderTarget * pRenderTarget, ID2D1Bitmap * pBitmap, float x, float y, float width, float height)
{
	if (pBitmap&&pRenderTarget) {
		D2D1_SIZE_F size = pBitmap->GetSize();
		bool a = 0;
		if (width < 0) { width = size.width; a = 1; }
		if (height < 0) {
			height = size.height*width / size.width;
		}
		// Draw a bitmap.
		pRenderTarget->DrawBitmap(
			pBitmap,
			D2D1::RectF(
				x,
				y,
				x + width,
				y + height)
		);
	}
}

void WIC::initWIC(IWICImagingFactory ** pFactoryWIC) {
	// Initialize COM
	CoInitialize(NULL);
	// Create the COM imaging factory
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(pFactoryWIC)
	);
}

HRESULT IDW::CreateDeviceIndependentResources(ID2D1Factory * m_pD2DFactory, IDWriteFactory ** m_pDWriteFactory, IDWriteTextFormat ** m_pTextFormat, float size)
{
	static const WCHAR msc_fontName[] = L"Verdana";
	FLOAT msc_fontSize = size;
	HRESULT hr;
	// Create a DirectWrite factory.
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(*m_pDWriteFactory),
		reinterpret_cast<IUnknown **>(m_pDWriteFactory)
	);

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory[0]->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		m_pTextFormat[0]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		m_pTextFormat[0]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


	}

	return hr;
}

int num::wstrToInt(wchar_t * in) {
	int ans = 0;
	int negative = 1;
	if (in[0] == L'-')negative = -1;
	for (int i = negative < 0; in[i]; i++) {
		if (in[i] > L'9' || in[i] < L'0')break;
		ans *= 10;
		ans += in[i] - L'0';

	}
	return ans * negative;
}

std::wstring num::intToWstr(int in) {
	std::wstring ans;
	bool negative = 0;
	if (!in)return L"0";
	if (in < 0)negative = 1;
	while (in) {
		wchar_t b = ((in % 10) + L'0');
		ans.insert(ans.begin(), b);
		in /= 10;
	}
	if (negative)
		ans.insert(ans.begin(), '-');
	return ans;
}
