// Based on Dynamic DPI sample from https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/DynamicDPI

#pragma once

#include "Graphics.h"

#define NOMINMAX
#include <windows.h> // for Win32 API

#include <wrl.h> // for ComPtr
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXMath.h>

namespace graphics::dx
{

namespace wrl = Microsoft::WRL;

struct CoInit
{
	CoInit();
	~CoInit();
};

class Device
{
private:
	friend class DeviceContext;

	CoInit _coInit;

	// Direct3D
	wrl::ComPtr<ID3D11Device2>        _d3dDevice;
	wrl::ComPtr<ID3D11DeviceContext2> _d3dContext;
	D3D_FEATURE_LEVEL                 _d3dFeatureLevel;

	// Direct2D
	wrl::ComPtr<ID2D1Factory2>       _d2dFactory;
	wrl::ComPtr<ID2D1Device1>        _d2dDevice;

	// DirectWrite + Windows Imaging Component
	wrl::ComPtr<IDWriteFactory2>     _dwriteFactory;
	wrl::ComPtr<IWICImagingFactory2> _wicFactory;

	void CreateIndependent();
	void CreateDevice();
public:
	Device();
	~Device();
};

class DeviceContext
{
public:
	Device&								_device;
	// DXGI
	wrl::ComPtr<IDXGISwapChain1>		_swapChain;

	// Direct3D
	wrl::ComPtr<ID3D11RenderTargetView> _d3dRenderTargetView;
	wrl::ComPtr<ID3D11DepthStencilView> _d3dDepthStencilView;
	D3D11_VIEWPORT                      _viewport;
	Size                                _d3dRenderTargetSize;

	// Direct2D
	wrl::ComPtr<ID2D1DeviceContext1>	_d2dContext;
	wrl::ComPtr<ID2D1Bitmap1>			_d2dTargetBitmap;

	HWND _hwnd;

	bool Present();
public:
	DeviceContext( Device& device );
	~DeviceContext();

	void Resize( HWND hwnd );
	void BeginDraw();
	void EndDraw();
};

} // namespace graphics::dx
